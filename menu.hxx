#ifndef _MENU_H_
#define _MENU_H_

#include "general.hxx"
#include "manager.hxx"
#include <dirent.h>

class Menu 
{
public:
	Menu(WindowManager *manager, XEvent *e);
	virtual ~Menu();

	virtual int getSelection();
	static void cleanup(WindowManager *const);
	int screen() { return m_windowManager->screen(); }

 protected:
	static Window *m_window;

	static bool m_initialised;
	static GC *m_menuGC;
	static XftFont *m_font;
	static XftColor *m_xftColour;
	static XftDraw **m_xftDraw;
	static unsigned long m_foreground;
	static unsigned long m_background;
	static unsigned long m_border;

	int getTextWidth(char *text, unsigned int len);

	char **m_items;
	int m_nItems;
	int m_nHidden;

	bool m_hasSubmenus;
	virtual void createSubmenu (XEvent *e, int i) {};

	WindowManager *m_windowManager;
	XEvent *m_event;

	virtual char **getItems(int *nitems, int *nhidden) = 0;

	Display *display() { return m_windowManager->display(); }
	Window root()      { return m_windowManager->root();    }

	bool isKeyboardMenuEvent (XEvent *e) {
		return (CONFIG_WANT_KEYBOARD_MENU && (e->type == KeyPress));
	}

	virtual void showFeedback(int) { }
	virtual void removeFeedback(int, bool) { }
	virtual void raiseFeedbackLevel(int) { }

};

class ClientMenu : public Menu
{
public:
	ClientMenu(WindowManager *, XEvent *e);
	virtual ~ClientMenu();

private:
	virtual char **getItems(int *, int *);
	ClientList m_clients;
	bool m_allowExit;

	Client *checkFeedback(int);
	virtual void showFeedback(int);
	virtual void removeFeedback(int, bool);
	virtual void raiseFeedbackLevel(int);
};

class ChannelMenu : public Menu
{
 public:
	ChannelMenu(WindowManager *, XEvent *e);
	virtual ~ChannelMenu();

 private:
	virtual char **getItems(int *, int *);
	int m_channel;
	int m_channels;
	char **m_items;
};

class CommandMenu : public Menu
{
public:
	CommandMenu(WindowManager *, XEvent *e, char *otherdir = nullptr);
	virtual ~CommandMenu();

private:
	void createSubmenu(XEvent *, int i);
	virtual char **getItems(int *, int *);
	char *m_commandDir;

};

class ShowGeometry : public Menu
{
public:
	ShowGeometry(WindowManager *, XEvent *);
	virtual ~ShowGeometry();

	void update(int x, int y);
	void remove();

private:
	virtual char **getItems(int *, int *);
};

int nobuttons(XButtonEvent *e);

#endif /* _MENU_H_ */
