#include "menu.hxx"
#include "manager.hxx"
#include "client.hxx"

#include <sys/types.h>
#include <filesystem>
#include <vector>
#include <X11/keysym.h>

bool       Menu::m_initialised = false;
GC           *Menu::m_menuGC;
XftFont      *Menu::m_font;
XftColor     *Menu::m_xftColour;
XftDraw     **Menu::m_xftDraw;
unsigned long Menu::m_foreground;
unsigned long Menu::m_background;
unsigned long Menu::m_border;
Window       *Menu::m_window;

#if MENU_ENTRY_MAXLENGTH > 0
#define STRLEN_MITEMS(i) ((strlen(m_items[(i)]) > MENU_ENTRY_MAXLENGTH) ? \
						  MENU_ENTRY_MAXLENGTH : strlen(m_items[(i)]))
#else
#define STRLEN_MITEMS(i) (strlen(m_items[(i)]))
#endif


Menu::Menu(WindowManager *manager, XEvent *e)
	: m_items(0), m_nItems(0), m_nHidden(0),
	  m_hasSubmenus(false),
	  m_windowManager(manager),
	  m_event(e)
{
	if (!m_initialised)
	{
	XGCValues *values;
	XSetWindowAttributes *attr;

		m_menuGC = (GC *) malloc(m_windowManager->screensTotal() * sizeof(GC));
		m_window = (Window *) malloc(m_windowManager->screensTotal() *
					 sizeof(Window));
	char *fi = strdup(CONFIG_MENU_FONT);
	char *ffi = fi, *tokstr = fi;
	while ((fi = strtok(tokstr, ","))) {

		tokstr = 0;

		FcPattern *pattern = FcPatternCreate();
		FcPatternAddString(pattern, FC_FAMILY, (FcChar8 *)fi);
		FcPatternAddInteger(pattern, FC_SLANT, FC_SLANT_ROMAN);

		FcPatternAddInteger(pattern, FC_WEIGHT, FC_WEIGHT_REGULAR);
		FcPatternAddInteger(pattern, FC_PIXEL_SIZE, CONFIG_MENU_FONT_SIZE);
		FcConfigSubstitute(FcConfigGetCurrent(), pattern, FcMatchPattern);

		FcResult result = FcResultMatch;
		FcPattern *match = FcFontMatch(FcConfigGetCurrent(), pattern, &result);
		FcPatternDestroy(pattern);

		if (!match || result != FcResultMatch) {
		if (match) FcPatternDestroy(match);
		continue;
		}

		m_font = XftFontOpenPattern(display(), match);
		if (m_font) break;
		FcPatternDestroy(match);
	}
	free(ffi);
	if (!m_font) {
		m_windowManager->fatal("couldn't load menu Xft font");
	}
	m_xftColour = (XftColor *) malloc(m_windowManager->screensTotal() *
					  sizeof(XftColor));
	m_xftDraw = (XftDraw **) malloc(m_windowManager->screensTotal() *
					sizeof(XftDraw *));
	values = (XGCValues *) malloc(m_windowManager->screensTotal() *
					  sizeof(XGCValues));
	attr = (XSetWindowAttributes *) malloc(m_windowManager->screensTotal() *
					  sizeof(XSetWindowAttributes));

		for (int i = 0; i < m_windowManager->screensTotal(); i++)
	{
		m_foreground = m_windowManager->allocateColour
		  (i, CONFIG_MENU_FOREGROUND, "menu foreground");
		m_background = m_windowManager->allocateColour
		  (i, CONFIG_MENU_BACKGROUND, "menu background");
		m_border = m_windowManager->allocateColour
		  (i, CONFIG_MENU_BORDERS, "menu border");


		values[i].background = m_background;
		values[i].foreground = m_foreground ^ m_background;
		values[i].function = GXxor;
		values[i].line_width = 0;
		values[i].subwindow_mode = IncludeInferiors;
		m_menuGC[i] = XCreateGC
		  (display(), m_windowManager->mroot(i),
		   GCForeground | GCBackground | GCFunction |
		   GCLineWidth | GCSubwindowMode, &values[i]);


		m_window[i] = XCreateSimpleWindow
		  (display(), m_windowManager->mroot(i), 0, 0, 1, 1, 1,
		   m_border, m_background);


#if ( CONFIG_USE_PIXMAPS != false ) && ( CONFIG_USE_PIXMAP_MENUS != false )
		attr[i].background_pixmap = Border::backgroundPixmap(manager);
#endif
		attr[i].save_under =
		  (DoesSaveUnders(ScreenOfDisplay(display(), i)) ?
		 true : false);

#if ( CONFIG_USE_PIXMAPS != false ) && ( CONFIG_USE_PIXMAP_MENUS != false )
		XChangeWindowAttributes
		  (display(), m_window[i], CWBackPixmap, &attr[i]);
#endif
		XChangeWindowAttributes
		  (display(), m_window[i], CWSaveUnder, &attr[i]);

		XftColorAllocName
		(display(),
		 XDefaultVisual(display(), i),
		 XDefaultColormap(display(), i),
		 CONFIG_MENU_FOREGROUND,
		 &m_xftColour[i]);

		m_xftDraw[i] = XftDrawCreate(display(), m_window[i],
					 XDefaultVisual(display(), i),
					 XDefaultColormap(display(), i));
	}
	m_initialised = true;
	}
}

Menu::~Menu()
{
	XUnmapWindow(display(), m_window[screen()]);
}

int Menu::getTextWidth(char *text, unsigned int len)
{
	XGlyphInfo extents;
	XftTextExtentsUtf8(display(), m_font, (FcChar8 *)text, len, &extents);
	return extents.width;
}

void Menu::cleanup(WindowManager *const wm)
{
	if (m_initialised) { // fix due to Eric Marsden
	XftFontClose(wm->display(), m_font);
		for (int i = 0; i < wm->screensTotal(); i++) {
		XftDrawDestroy(m_xftDraw[i]);
		XFreeGC(wm->display(), m_menuGC[i]);
	}
	}
}

int nobuttons(XButtonEvent *e) // straight outta 9wm
{
	int state;
	state = (e->state & AllButtonMask);
	return (e->type == ButtonRelease) && (state & (state - 1)) == 0;
}

int Menu::getSelection()
{
	m_items = getItems(&m_nItems, &m_nHidden);
	XButtonEvent *xbev = (XButtonEvent *)m_event; // KeyEvent is similar enough

	if (xbev->window == m_window[screen()] || m_nItems == 0) return -1;

	int width, maxWidth = 10;
	for (int i = 0; i < m_nItems; i++) {
	width = getTextWidth(m_items[i], STRLEN_MITEMS(i));
	if (width > maxWidth) maxWidth = width;
	}
	maxWidth += 32;

	bool isKeyboardMenu = isKeyboardMenuEvent(m_event);
	int selecting = isKeyboardMenu ? 0 : -1, prev = -1;
	int entryHeight = m_font->ascent + m_font->descent + 4;
	int totalHeight = entryHeight * m_nItems + 13;

	int mx = DisplayWidth (display(), screen()) - 1;
	int my = DisplayHeight(display(), screen()) - 1;

	int x, y;

	if (isKeyboardMenu) {
	x = mx / 2 - maxWidth / 2;
	y = my / 2 - totalHeight / 2;
	} else {

	x = xbev->x - maxWidth/2;
	y = xbev->y - 2;

	bool warp = false;

	if (x < 0) {
		xbev->x -= x;
		x = 0;
		warp = true;
	} else if (x + maxWidth >= mx) {
		xbev->x -= x + maxWidth - mx;
		x = mx - maxWidth;
		warp = true;
	}

	if (y < 0) {
		xbev->y -= y;
		y = 0;
		warp = true;
	} else if (y + totalHeight >= my) {
		xbev->y -= y + totalHeight - my;
		y = my - totalHeight;
		warp = true;
	}

	if (warp) XWarpPointer(display(), None, root(), None, None,
				   None, None, xbev->x, xbev->y);
	}

	XMoveResizeWindow(display(), m_window[screen()], x, y, maxWidth, totalHeight);
	XSelectInput(display(), m_window[screen()], MenuMask);
	XMapRaised(display(), m_window[screen()]);

	if (m_windowManager->attemptGrab(m_window[screen()], None,
					 MenuGrabMask, xbev->time)
	!= GrabSuccess) {
	XUnmapWindow(display(), m_window[screen()]);
	return -1;
	}

	if (isKeyboardMenu) {
	if (m_windowManager->attemptGrabKey(m_window[screen()], xbev->time)
		!= GrabSuccess) {
		XUnmapWindow(display(), m_window[screen()]);
		return -1;
	}
	}	    

	bool done = false;
	bool drawn = false;
	XEvent event;
	struct timeval sleepval;
	unsigned long tdiff = 0L;
	bool speculating = false;
	bool foundEvent;

	while (!done)
	{
	int i;
	foundEvent = false;

	if (CONFIG_FEEDBACK_DELAY >= 0 &&
		tdiff > (unsigned long)CONFIG_FEEDBACK_DELAY &&
		!isKeyboardMenu && // removeFeedback didn't seem to work for it
		!speculating) {

		if (selecting >= 0 && selecting < m_nItems) {
		raiseFeedbackLevel(selecting);
		XRaiseWindow(display(), m_window[screen()]);
		}

		speculating = true;
	}

	//!!! MenuMask | ??? suggests MenuMask is wrong
	while (XCheckMaskEvent
		   (display(), MenuMask | StructureNotifyMask |
		KeyPressMask | KeyReleaseMask, &event)) {
		foundEvent = true;
		if (event.type != MotionNotify) break;
	}

	if (!foundEvent) {
		sleepval.tv_sec = 0;
		sleepval.tv_usec = 10000;
		select(0, 0, 0, 0, &sleepval);
		tdiff += 10;
		continue;
	}

	switch (event.type)
	{
	case ButtonPress:
		break;

	case ButtonRelease:
		if (isKeyboardMenu) break;

		if (drawn) {

		if (event.xbutton.button != xbev->button) break;
		x = event.xbutton.x;
		y = event.xbutton.y - 11;
		i = y / entryHeight;

		if (selecting >= 0 && y >= selecting * entryHeight - 3 &&
			y <= (selecting + 1) * entryHeight - 3) i = selecting;

		if (m_hasSubmenus && (i >= 0 && i < m_nHidden)) i = -i;

		if (x < 0 || x > maxWidth || y < -3) i = -1;
		else if (i < 0 || i >= m_nItems) i = -1;

		} else {
		i = -1;
		}

		if (!nobuttons(&event.xbutton)) i = -1;
		m_windowManager->releaseGrab(&event.xbutton);
		XUnmapWindow(display(), m_window[screen()]);
		selecting = i;
		done = true;
		break;

	case MotionNotify:
		if (!drawn || isKeyboardMenu) break;

		x = event.xbutton.x;
		y = event.xbutton.y - 11;
		prev = selecting;
		selecting = y / entryHeight;

		if (prev >= 0 && y >= prev * entryHeight - 3 &&
		y <= (prev+1) * entryHeight - 3) selecting = prev;

		if (m_hasSubmenus && (selecting >= 0 && selecting < m_nHidden) &&
		x >= maxWidth-32 && x < maxWidth)
		{
		xbev->x += event.xbutton.x - 32;
		xbev->y += event.xbutton.y;

		createSubmenu ((XEvent *)xbev, selecting);
		done = true;
		break;
		}

		if (x < 0 || x > maxWidth || y < -3) selecting = -1;
		else if (selecting < 0 || selecting > m_nItems) selecting = -1;

		if (selecting == prev) break;
		tdiff = 0; speculating = false;

		if (prev >= 0 && prev < m_nItems) {
		removeFeedback(prev, speculating);
		XFillRectangle(display(), m_window[screen()], m_menuGC[screen()],
				   4, prev * entryHeight + 9,
				   maxWidth - 8, entryHeight);
		}

		if (selecting >= 0 && selecting < m_nItems) {
		showFeedback(selecting);
		XRaiseWindow(display(), m_window[screen()]);
		XFillRectangle(display(), m_window[screen()], m_menuGC[screen()],
				   4, selecting * entryHeight + 9,
				   maxWidth - 8, entryHeight);
		}

		break;

	case Expose:

		if (CONFIG_MAD_FEEDBACK && event.xexpose.window != m_window[screen()]) {
		m_windowManager->dispatchEvent(&event);
		break;
		}

		XClearWindow(display(), m_window[screen()]);

		XDrawRectangle(display(), m_window[screen()], m_menuGC[screen()], 2, 7,
			   maxWidth - 5, totalHeight - 10);

		for (i = 0; i < m_nItems; i++) {

		int dx = getTextWidth(m_items[i], STRLEN_MITEMS(i));
		int dy = i * entryHeight + m_font->ascent + 10;

		if (i >= m_nHidden) {
			XftDrawStringUtf8(m_xftDraw[screen()],
					  &m_xftColour[screen()],
					  m_font,
					  maxWidth - 8 - dx, dy,
					  (FcChar8 *)m_items[i], STRLEN_MITEMS(i));
		} else {
			XftDrawStringUtf8(m_xftDraw[screen()],
					  &m_xftColour[screen()],
					  m_font,
					  8, dy,
					  (FcChar8 *)m_items[i], STRLEN_MITEMS(i));
		}
		}

		if (selecting >= 0 && selecting < m_nItems) {
		XFillRectangle(display(), m_window[screen()], m_menuGC[screen()],
				   4, selecting * entryHeight + 9,
				   maxWidth - 8, entryHeight);
		}

		drawn = true;
		break;

	case KeyPress:
	{
		if (!isKeyboardMenu) break;

		KeySym key = XKeycodeToKeysym(display(), event.xkey.keycode, 0);

		if constexpr (CONFIG_MENU_SELECT_KEY) if (key == *CONFIG_MENU_SELECT_KEY) {

		if (!drawn) selecting = -1;

		if (m_hasSubmenus && selecting >= 0 && selecting < m_nHidden)
		{
			createSubmenu((XEvent *)xbev, selecting);
			selecting = -1;
		}
		m_windowManager->releaseGrabKeyMode(&event.xkey);
		XUnmapWindow(display(), m_window[screen()]);
		done = true;
		break;

		} else if constexpr (CONFIG_MENU_CANCEL_KEY) if (key == *CONFIG_MENU_CANCEL_KEY) {

		m_windowManager->releaseGrabKeyMode(&event.xkey);
		XUnmapWindow(display(), m_window[screen()]);
		if (selecting >= 0) removeFeedback(selecting, speculating);
		selecting = -1;
		done = true;
		break;

		} else if ((!( CONFIG_MENU_UP_KEY) || key != *CONFIG_MENU_UP_KEY) &&
			   (!( CONFIG_MENU_DOWN_KEY) || key != *CONFIG_MENU_DOWN_KEY)) {
		break;
		}

		if (!drawn) break;
		prev = selecting;

		if constexpr (CONFIG_MENU_UP_KEY) if (key == *CONFIG_MENU_UP_KEY) {

		if (prev <= 0) selecting = m_nItems - 1;
		else selecting = prev - 1;

		} else {

		if (prev == m_nItems - 1 || prev < 0) selecting = 0;
		else selecting = prev + 1;
		}

		tdiff = 0; speculating = false;

		if (prev >= 0 && prev < m_nItems) {
			removeFeedback(prev, speculating);
		XFillRectangle(display(), m_window[screen()], m_menuGC[screen()],
				   4, prev * entryHeight + 9,
				   maxWidth - 8, entryHeight);
		}

		if (selecting >= 0 && selecting < m_nItems) {
		showFeedback(selecting);
		XRaiseWindow(display(), m_window[screen()]);
		XFillRectangle(display(), m_window[screen()], m_menuGC[screen()],
				   4, selecting * entryHeight + 9,
				   maxWidth - 8, entryHeight);
		}

		break;
	}

	case KeyRelease:
		break;

	default:
		if (event.xmap.window == m_window[screen()]) break;
		m_windowManager->dispatchEvent(&event);
	}
	}

	if (selecting >= 0) removeFeedback(selecting, speculating);
	return selecting;
}


ClientMenu::ClientMenu(WindowManager *manager, XEvent *e)
	: Menu(manager, e), m_allowExit(false)
{
	//!!! this is effectively calling a pure virtual from a constructor,
	// which is a bit gross
	int selecting = getSelection();

	if (selecting == m_nItems-1 && m_allowExit) { // getItems sets m_allowExit
	m_windowManager->setSignalled();
	return;
	}

	if (!CONFIG_NEW_WINDOW[0] && selecting >= 0) {
	++selecting; // ha ha ha!
	++m_nHidden;
	}

	if (selecting == 0) {
	m_windowManager->spawn((char *)CONFIG_NEW_WINDOW[0], nullptr);
	return;
	}

	if (selecting > 0) {

	Client *cl = m_clients[selecting - 1];

	if (selecting < m_nHidden) cl->unhide(true);
	else if (selecting < m_nItems) {
			cl->gotoClient();
	}

	if constexpr (CONFIG_WANT_KEYBOARD_MENU) if (e->type == KeyPress)
		cl->activateAndWarp();
	}

	--m_nHidden; // just in case
}

ClientMenu::~ClientMenu()
{
	m_clients.clear();
	free(m_items);
}

char **ClientMenu::getItems(int *niR, int *nhR)
{
	int i;
	XButtonEvent *xbev = (XButtonEvent *)m_event; // KeyEvent is similar enough

	for (i = 0; i < m_windowManager->hiddenClients().size(); ++i) {
		Client *client = m_windowManager->hiddenClients()[i];
	if (client->root() == m_windowManager->root() &&
		client->channel() == m_windowManager->channel() &&
			client->type() != DockClient) {
		m_clients.push_back(client);
	}
	}

	int nh = m_clients.size();
	if (CONFIG_NEW_WINDOW[0]) ++nh;

	if (CONFIG_EVERYTHING_ON_ROOT_MENU) {
	for (i = 0; i < m_windowManager->clients().size(); ++i) {
			Client *client = m_windowManager->clients()[i];
		if (client->isNormal() &&
		client->root() == m_windowManager->root() &&
		client->channel() == m_windowManager->channel() &&
				client->type() != DockClient) {
		m_clients.push_back(client);
		}
	}
	}

	int n = m_clients.size();
	if (CONFIG_NEW_WINDOW[0]) ++n;

	int mx = DisplayWidth (display(), screen()) - 1;
	int my = DisplayHeight(display(), screen()) - 1;

	m_allowExit =
	((CONFIG_EXIT_CLICK_SIZE_X != 0 ? 
	  (CONFIG_EXIT_CLICK_SIZE_X > 0 ? 
	   (xbev->x < CONFIG_EXIT_CLICK_SIZE_X) : 
	   (xbev->x > mx + CONFIG_EXIT_CLICK_SIZE_X)) : 1) &&
	 (CONFIG_EXIT_CLICK_SIZE_Y != 0 ? 
	  (CONFIG_EXIT_CLICK_SIZE_Y > 0 ? 
	   (xbev->y < CONFIG_EXIT_CLICK_SIZE_Y) : 
	   (xbev->y > my + CONFIG_EXIT_CLICK_SIZE_Y)) : 1));

	m_allowExit = m_allowExit ||
	(isKeyboardMenuEvent(m_event) && CONFIG_EXIT_ON_KBD_MENU);

	if (m_allowExit) ++n;

	char const **items = (char const **)malloc(n * sizeof(char *));

	for (i = 0; i < n; ++i) {

	if (CONFIG_NEW_WINDOW[0]) {

		if (i == 0) items[i] = CONFIG_NEW_WINDOW_LABEL;
		else if (m_allowExit && i > m_clients.size()) items[i] = "[Exit wmx]";
		else items[i] = m_clients[i-1]->label();

	} else {

		if (m_allowExit && i >= m_clients.size()) items[i] = "[Exit wmx]";
		else items[i] = m_clients[i]->label();
	}
	}

	*niR = n;
	*nhR = nh;

	return (char **)items;
}


Client *ClientMenu::checkFeedback(int item)
{
	if (CONFIG_MAD_FEEDBACK == false) return nullptr;

	if (CONFIG_NEW_WINDOW[0]) {

	if (item <= 0 || item > m_clients.size()+1) return nullptr;
	if (m_allowExit && item == m_clients.size() + 1) return nullptr;
	return m_clients[item - 1];

	} else {

	if (item < 0 || item > m_clients.size()) return nullptr;
	if (m_allowExit && item == m_clients.size()) return nullptr;
	return m_clients[item];
	}
}

void ClientMenu::showFeedback(int item)
{
	if (Client *c = checkFeedback(item)) c->showFeedback();
}

void ClientMenu::removeFeedback(int item, bool mapped)
{
	if (Client *c = checkFeedback(item)) c->removeFeedback(mapped);
}

void ClientMenu::raiseFeedbackLevel(int item)
{
	if (Client *c = checkFeedback(item)) c->raiseFeedbackLevel();
}


CommandMenu::CommandMenu(WindowManager *manager, XEvent *e, char* otherdir)
	: Menu(manager, e)
{
	char const *home = getenv("HOME");
	char const *wmxdir = getenv("WMXDIR");

	m_commandDir = nullptr;
	m_hasSubmenus = true;

	if (otherdir == nullptr) {
		namespace fs = std::filesystem;
		fs::path p;
		if (wmxdir == nullptr) {
			if (home == nullptr) return;
			p = fs::path(home) / CONFIG_COMMAND_MENU;
		} else if (wmxdir[0] == '/') {
			p = wmxdir;
		} else {
			if (home == nullptr) return;
			p = fs::path(home) / wmxdir;
		}
		auto s = p.string();
		m_commandDir = strdup(s.c_str());
	} else {
		m_commandDir = strdup(otherdir);
	}

	int i = getSelection();

	if (i >= 0 && i < m_nHidden) {
	// This should never happen.
	}

	if (i >= m_nHidden && i < m_nItems)
	{
	char *commandFile =
		(char *)malloc(strlen(m_commandDir) + STRLEN_MITEMS(i) + 2);

	sprintf(commandFile, "%s/%s", m_commandDir, m_items[i]);
	m_windowManager->spawn(m_items[i], commandFile);
	free(commandFile);
	}
}

CommandMenu::~CommandMenu()
{
	free(m_commandDir);
	for (int i = 0; i < m_nItems; i++) {
	free(m_items[i]);
	}
	free(m_items);
}

static int sortstrs(void const *va, void const *vb)
{
	char **a = (char **)va;
	char **b = (char **)vb;    
	return strcmp(*a, *b);
}

void CommandMenu::createSubmenu (XEvent *e, int i)
{
	char *new_directory;
	int dirlen = strlen (m_commandDir);

	new_directory = (char *)malloc (dirlen + STRLEN_MITEMS(i) + 2);
	strcpy (new_directory, m_commandDir);
	new_directory[dirlen] = '/';
	strcpy (new_directory + dirlen + 1, m_items[i]);

	CommandMenu menu (m_windowManager, e, new_directory);
	free(new_directory);
}


char **CommandMenu::getItems(int *niR, int *nhR)
{
	namespace fs = std::filesystem;
	*niR = *nhR = 0;

	if (!getenv("HOME")) return nullptr;

	std::error_code ec;
	fs::path dir = m_commandDir;
	if (!fs::is_directory(dir, ec)) {
		free(m_commandDir);
#ifndef CONFIG_SYSTEM_COMMAND_MENU
#define CONFIG_SYSTEM_COMMAND_MENU "/usr/local/lib/wmx/menu"
#endif
		m_commandDir = strdup(CONFIG_SYSTEM_COMMAND_MENU);
		dir = m_commandDir;
		if (!fs::is_directory(dir, ec))
			return nullptr;
	}

	std::vector<std::string> dirs, files;
	for (auto const &ent : fs::directory_iterator(dir, ec)) {
		if (ec) break;
		auto name = ent.path().filename().string();
		if (name.empty() || name[0] == '.') continue;
		if (ent.is_directory() &&
		    (ent.status().permissions() & fs::perms::owner_read) != fs::perms::none)
			dirs.push_back(name);
		else if (ent.is_regular_file() &&
			 (ent.status().permissions() & fs::perms::owner_exec) != fs::perms::none)
			files.push_back(name);
	}

	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());

	*nhR = dirs.size();
	*niR = dirs.size() + files.size();
	if (*niR == 0) return nullptr;

	char **items = static_cast<char **>(malloc(*niR * sizeof(char *)));
	int i = 0;
	for (auto const &s : dirs)  items[i++] = strdup(s.c_str());
	for (auto const &s : files) items[i++] = strdup(s.c_str());
	return items;
}


// Fake geometry as a "menu" to save on code

ShowGeometry::ShowGeometry(WindowManager *manager, XEvent *e)
	: Menu(manager, e)
{
	// empty
}

void ShowGeometry::update(int x, int y)
{
	char string[20];

	sprintf(string, "%d %d", x, y);
	int width = getTextWidth(string, strlen(string)) + 8;
	int height = m_font->ascent + m_font->descent + 8;
	int mx = DisplayWidth (display(), screen()) - 1;
	int my = DisplayHeight(display(), screen()) - 1;

	XMoveResizeWindow(display(), m_window[screen()],

#if CONFIG_GEOMETRY_X_POS < 0
			  0
#elif CONFIG_GEOMETRY_X_POS > 0
			  (mx - width)
#else
			  (mx - width) / 2
#endif
			  ,
#if CONFIG_GEOMETRY_X_POS < 0
			  0
#elif CONFIG_GEOMETRY_X_POS > 0
			  (my - height)
#else
					  (my - height) / 2
#endif
			  , width, height);

	XClearWindow(display(), m_window[screen()]);
	XMapRaised(display(), m_window[screen()]);

	XftDrawStringUtf8(m_xftDraw[screen()],
			  &m_xftColour[screen()],
			  m_font,
			  4,
			  4 + m_font->ascent,
			  (FcChar8 *)string,
			  strlen(string));
}

void ShowGeometry::remove()
{
	XUnmapWindow(display(), m_window[screen()]);
}

ShowGeometry::~ShowGeometry()
{
	// empty
}

char **ShowGeometry::getItems(int *niR, int *nhR)
{
	niR = nhR = 0;
	return nullptr;
}


