#include "manager.hxx"
#include "client.hxx"
#include "menu.hxx"
#include <X11/keysym.h>
#include <sys/time.h>

#if CONFIG_WANT_SUNKEYS
#if CONFIG_WANT_SUNPOWERKEY
#include <X11/Sunkeysym.h>
#endif
#endif

void WindowManager::eventButton(XButtonEvent *e, XEvent *ev)
{
	enum {Vertical, Maximum, Horizontal};
	setScreenFromPointer();
	Client *c = windowToClient(e->window);

//            (int)m_channelChangeTime);

	// We shouldn't be getting button events for non-focusable clients,
	// but we'd better check just in case.
	if (CONFIG_PASS_FOCUS_CLICK || c->isNonFocusable())
		XAllowEvents(display(), ReplayPointer, e->time);            
	else
		XAllowEvents(display(), SyncPointer, e->time);

	bool furniture = (c && ((c->type() == DesktopClient) ||
							(c->type() == DockClient)));


	if (!furniture) {
		if (e->button == CONFIG_CIRCULATE_BUTTON && m_channelChangeTime == 0) {
			if (DynamicConfig::config.rightCirculate())
				circulate(e->window == e->root);
			else if (DynamicConfig::config.rightLower())
			{
				if (e->window != e->root && c) c->lower();
			}
			else if (DynamicConfig::config.rightToggleHeight())
			{
				if (e->window != e->root && c) {
					if (c->isFullHeight()) {
						c->unmaximise(Vertical);
					} else {
						c->maximise(Vertical);
					}
				}
			}
			return;
		}
	}

	if (!furniture) {
		if (e->button == CONFIG_NEXTCHANNEL_BUTTON && CONFIG_CHANNEL_SURF) {
			// wheel "up" - increase channel
			flipChannel(false, false, true, c);
			return ;
		} else if (e->button == CONFIG_PREVCHANNEL_BUTTON && CONFIG_CHANNEL_SURF) {
			// wheel "down" - decrease channel
			flipChannel(false, true, true, c);
			return ;
		}
	}

	bool effectiveRoot = false;
	if (e->window == e->root) effectiveRoot = true;
	else if (c && c->type() == DesktopClient) {
		if (e->button == CONFIG_CLIENTMENU_BUTTON) {
			if (e->state & m_altModMask) {
				effectiveRoot = true;
			} else {
				c->eventButton(e);
				return;
			}
		} else {
			effectiveRoot = true;
		}
	}


	if (effectiveRoot) {


	if (e->button == CONFIG_CLIENTMENU_BUTTON && m_channelChangeTime == 0) {

		ClientMenu menu(this, (XEvent *)e);

	} else if (e->x > DisplayWidth(display(), screen()) -
		   CONFIG_CHANNEL_CLICK_SIZE &&
		   e->y < CONFIG_CHANNEL_CLICK_SIZE) {

		if (e->button == CONFIG_COMMANDMENU_BUTTON) {
#if CONFIG_USE_CHANNEL_MENU	
				ChannelMenu(this, (XEvent *)e);
#else
				if (m_channelChangeTime == 0) flipChannel(true, false, false, 0);
				else flipChannel(false, false, false, 0);

			} else if (e->button == CONFIG_CLIENTMENU_BUTTON && m_channelChangeTime != 0) {

				flipChannel(false, true, false, 0);
#endif
		}

	} else if (e->button == CONFIG_COMMANDMENU_BUTTON && m_channelChangeTime == 0) {
		DynamicConfig::config.scan();
		CommandMenu menu(this, (XEvent *)e);
	}

	} else if (c && !furniture) {

	if (e->button == CONFIG_COMMANDMENU_BUTTON && CONFIG_CHANNEL_SURF) {
#if CONFIG_USE_CHANNEL_MENU	
		ChannelMenu menu(this, (XEvent *)e);
#else
			if (m_channelChangeTime == 0) flipChannel(true, false, false, 0);
			else flipChannel(false, false, false, c);
#endif
		return;
	} else if (e->button == CONFIG_CLIENTMENU_BUTTON && m_channelChangeTime != 0) {
		// allow left-button to push down a channel --cc 19991001
		flipChannel(false, true, false, c);
		return;
     }

	c->eventButton(e);

		return;
	} 
}


void WindowManager::circulate(bool activeFirst)
{
	Client *c = 0;
	if (m_clients.count() == 0) return;
	if (activeFirst) c = m_activeClient;

	// cc 2000/05/11: change direction each time we release Alt

	static int direction = 1;
	if (!m_altStateRetained) direction = -direction;
	m_altStateRetained = true;

	if (!c) {

	int i, j;

	if (!m_activeClient) {
			i = (direction > 0 ? -1 : m_clients.count());
		} else {
		for (i = 0; i < m_clients.count(); ++i) {
		if (m_clients.item(i)->channel() != channel()) continue;
		if (m_clients.item(i) == m_activeClient) break;
		}

		if (i >= m_clients.count() - 1 && direction > 0) i = -1;
			else if (i == 0 && direction < 0) i = m_clients.count();
	}

	for (j = i + direction;
			 (!m_clients.item(j)->isNormal() ||
			   m_clients.item(j)->isTransient() ||
			   m_clients.item(j)->channel() != channel() ||
			   m_clients.item(j)->skipsFocus());
			 j += direction) {

		if (direction > 0 && j >= m_clients.count() - 1) j = -1;
			if (direction < 0 && j <= 0) j = m_clients.count();

		if (j == i) return; // no suitable clients
	}

	c = m_clients.item(j);
	}

	c->activateAndWarp();
}

void WindowManager::eventKeyPress(XKeyEvent *ev)
{
	enum {Vertical, Maximum, Horizontal};
	KeySym key = XKeycodeToKeysym(display(), ev->keycode, 0);

	if (CONFIG_USE_KEYBOARD) {

	Client *c = windowToClient(ev->window);

		if (key == CONFIG_ALT_KEY) {
			m_altPressed = true;
			m_altStateRetained = false;
		}

#if CONFIG_WANT_SUNKEYS
#ifdef CONFIG_QUICKRAISE_KEY
	if (key == CONFIG_QUICKRAISE_KEY && c) {

		if (isTop(c) && (CONFIG_QUICKRAISE_ALSO_LOWERS == true)) {
		c->lower();
		} else {
		c->mapRaised();
		}

	} else
#endif // CONFIG_QUICKRAISE_KEY
#ifdef CONFIG_QUICKHIDE_KEY
	if (key == CONFIG_QUICKHIDE_KEY && c) {
		c->hide();

	} else 
#endif // CONFIG_QUICKHIDE_KEY
#ifdef CONFIG_QUICKCLOSE_KEY
	if (key == CONFIG_QUICKCLOSE_KEY && c) {
		c->kill();

	} else
#endif // CONFIG_QUICKCLOSE_KEY
#ifdef CONFIG_QUICKHEIGHT_KEY
	if (key == CONFIG_QUICKHEIGHT_KEY && c) {

		if (c->isFullHeight()) {
		c->unmaximise(Vertical);
		} else {
		c->maximise(Vertical);
		}

	} else
#endif // CONFIG_QUICKHEIGHT_KEY
#if CONFIG_WANT_SUNPOWERKEY
	  if (key == SunXK_PowerSwitch) {
		   pid_t pid = fork();
		   if(pid == 0)
		   {
			   execl(CONFIG_SUNPOWER_EXEC,CONFIG_SUNPOWER_EXEC,
					 CONFIG_SUNPOWER_OPTIONS, nullptr);
		   }

	   } else if (key == SunXK_PowerSwitchShift) {
		   pid_t pid = fork();
		   if(pid == 0)
		   {
			   execl(CONFIG_SUNPOWER_EXEC,CONFIG_SUNPOWER_EXEC,
					 CONFIG_SUNPOWER_SHIFTOPTIONS, nullptr);
		   }
	   } else
#endif // CONFIG_WANT_SUNPOWERKEY
#endif // CONFIG_WANT_SUNKEYS

	   if (ev->state & m_altModMask) {

		   if (!m_altPressed) {
			   // oops! bug
			   m_altPressed = true;
			   m_altStateRetained = false;
//                (long)ev->state, (long)m_altModMask);
		   }

		   if (key >= XK_F1 && key <= XK_F12 &&
		CONFIG_CHANNEL_SURF && CONFIG_USE_CHANNEL_KEYS) {

		int channel = key - XK_F1 + 1;

		gotoChannel(channel, 0);

		} else {

		// These key names also appear in Client::manage(), so
		// when adding a key it must be added in both places

		switch (key) {

		case CONFIG_FLIP_DOWN_KEY:
			flipChannel(false, true, true, 0);
			break;

		case CONFIG_FLIP_UP_KEY:
			flipChannel(false, false, true, 0);
			break;

		case CONFIG_CIRCULATE_KEY:
			circulate(false);
			break;

		case CONFIG_HIDE_KEY:
			if (c) c->hide();
			break;

		case CONFIG_DESTROY_KEY:
			if (c) c->kill();
			break;

		case CONFIG_RAISE_KEY:
			if (c) c->mapRaised();
			break;

		case CONFIG_LOWER_KEY:
			if (c) c->lower();
			break;

		case CONFIG_FULLHEIGHT_KEY:
			if (c) c->maximise(Vertical);
			break;

		case CONFIG_NORMALHEIGHT_KEY:
			if (c && !CONFIG_SAME_KEY_MAX_UNMAX)
						c->unmaximise(Vertical);
			break;

		case CONFIG_FULLWIDTH_KEY:
			if (c) c->maximise(Horizontal);
			break;

		case CONFIG_NORMALWIDTH_KEY:
			if (c && !CONFIG_SAME_KEY_MAX_UNMAX)
						c->unmaximise(Horizontal);
			break;

		case CONFIG_MAXIMISE_KEY:
			if (c) c->maximise(Maximum);
			break;

		case CONFIG_UNMAXIMISE_KEY:
			if (c && !CONFIG_SAME_KEY_MAX_UNMAX)
						c->unmaximise(Maximum);
			break;

		case CONFIG_STICKY_KEY:
			if (c) c->setSticky(!(c->isSticky()));
			break;

		case CONFIG_CLIENT_MENU_KEY:
			if constexpr (CONFIG_WANT_KEYBOARD_MENU) {
			ClientMenu menu(this, (XEvent *)ev);
			break;
			}
		case CONFIG_COMMAND_MENU_KEY:
			if constexpr (CONFIG_WANT_KEYBOARD_MENU) {
			CommandMenu menu(this, (XEvent *)ev);
			break;
			}

		default:
			return;
		}
		}
	}

	XSync(display(), false);
	XUngrabKeyboard(display(), CurrentTime);
	}

	return;
}

void WindowManager::eventKeyRelease(XKeyEvent *ev)
{
	KeySym key = XKeycodeToKeysym(display(), ev->keycode, 0);

	if (key == CONFIG_ALT_KEY) {
		m_altPressed = false;
		m_altStateRetained = false;

//                (long)ev->state, (long)m_altModMask);
	}
	return;
}


void Client::activateAndWarp()
{
	mapRaised();
	ensureVisible();
	warpPointer();
	activate();
}

void Client::eventButton(XButtonEvent *e)
{
	if (e->type != ButtonPress) return;

	bool wasTop = windowManager()->isTop(this);

	if(!wasTop)
		mapRaised();

	if (e->button == CONFIG_CLIENTMENU_BUTTON) {
	if (m_border->hasWindow(e->window)) {
		m_border->eventButton(e);
	} else {
		e->x += m_border->xIndent();
		e->y += m_border->yIndent();
		move(e);
	}
	}

	if constexpr (CONFIG_RAISELOWER_ON_CLICK && wasTop && !m_doSomething) {
	lower();
	}

	if (!isNormal() || isActive() || e->send_event) return;
	activate();
}


int WindowManager::attemptGrab(Window w, Window constrain, int mask, int t)
{
	int status;
	if (t == 0) t = timestamp(false);
	status = XGrabPointer(display(), w, false, mask, GrabModeAsync,
			  GrabModeAsync, constrain, None, t);
	return status;
}


int WindowManager::attemptGrabKey(Window w, int t)
{
	if (t == 0) t = timestamp(false);
	return XGrabKeyboard(display(), w, false, GrabModeAsync, GrabModeAsync, t);
}


void WindowManager::releaseGrab(XButtonEvent *e)
{
	XEvent ev;
	if (!nobuttons(e)) {
	for (;;) {
		XMaskEvent(display(), ButtonMask | ButtonMotionMask, &ev);
		if (ev.type == MotionNotify) continue;
		e = &ev.xbutton;
		if (nobuttons(e)) break;
	}
	}

	XUngrabPointer(display(), e->time);
	m_currentTime = e->time;
}


void WindowManager::releaseGrabKeyMode(XButtonEvent *e) 
{
	XEvent ev;
	if (!nobuttons(e)) {
	for (;;) {
		XMaskEvent(display(), ButtonMask | ButtonMotionMask, &ev);
		if (ev.type == MotionNotify) continue;
		e = &ev.xbutton;
		if (nobuttons(e)) break;
	}
	}

	XUngrabPointer(display(), e->time);
	m_currentTime = e->time;

	XUngrabKeyboard(display(), e->time);
	m_currentTime = e->time;
}


void WindowManager::releaseGrabKeyMode(XKeyEvent* e)
{
	XUngrabPointer(display(), e->time);
	XUngrabKeyboard(display(), e->time);
	m_currentTime = e->time;
}


void Client::move(XButtonEvent *e)
{
	int x = -1, y = -1;
	bool done = false;
	ShowGeometry geometry(m_windowManager, (XEvent *)e);

	if(!isMovable())
		return;

	if (m_windowManager->attemptGrab
	(root(), None, DragMask, e->time) != GrabSuccess) {
	return;
	}

	int mx = DisplayWidth (display(), windowManager()->screen()) - 1;
	int my = DisplayHeight(display(), windowManager()->screen()) - 1;
	int xi = m_border->xIndent();
	int yi = m_border->yIndent();
	int ft = CONFIG_FRAME_THICKNESS;

	XEvent event;
	bool found;
	struct timeval sleepval;

#ifdef CONFIG_BUMP_EVERYWHERE
	EdgeRectList edges;
	for (int i = 0; i < m_windowManager->clients().count(); ++i) {
		m_windowManager->clients().item(i)->appendEdges(edges);
	}
#endif

	m_doSomething = false;
	while (!done) {

	found = false;

	while (XCheckMaskEvent(display(), DragMask | ExposureMask, &event)) {
		found = true;
		if (event.type != MotionNotify) break;
	}

	if (!found) {
		sleepval.tv_sec = 0;
		sleepval.tv_usec = 1000;
		select(0, 0, 0, 0, &sleepval);
		continue;
	}

	switch (event.type) {

	default:
		break;

	case Expose:
		m_windowManager->dispatchEvent(&event);
		break;

	case ButtonPress:
		// don't like this
		XUngrabPointer(display(), event.xbutton.time);
		m_doSomething = false;
		done = true;
		break;

	case ButtonRelease:

		if (!nobuttons(&event.xbutton)) m_doSomething = false;

		m_windowManager->releaseGrab(&event.xbutton);
		done = true;
		break;

	case MotionNotify:

		int nx = event.xbutton.x - e->x;
		int ny = event.xbutton.y - e->y;

		if (nx != x || ny != y) {

		if (m_doSomething) { // so x,y have sensible values already

			// bumping!

					int bumpedh = 0, bumpedv = 0;
					int bd = CONFIG_BUMP_DISTANCE;

			if (nx < x && nx <= 0 && nx > -bd) {
						nx = 0;
						bumpedh = 1;
					}

			if (ny < y && ny <= 0 && ny > -bd) {
						ny = 0;
						bumpedv = 1;
					}

			if (nx > x && nx >= mx - m_w - xi &&
			nx < mx - m_w - xi + bd) {
			nx = mx - m_w - xi;
						bumpedh = 1;
					}

			if (ny > y && ny >= my - m_h - yi &&
			ny < my - m_h - yi + bd) {
			ny = my - m_h - yi;
						bumpedv = 1;
					}

#ifdef CONFIG_BUMP_EVERYWHERE
					if (!isTransient()) {

						if (!bumpedh) {
							for (int i = 0; i < edges.count(); ++i) {
								if (nx < x &&
									nx <= edges.item(i).right - xi + ft &&
									nx  > edges.item(i).right - xi + ft - bd) {
									nx  = edges.item(i).right - xi + ft;
									bumpedh = 1;
								}
								if (nx > x &&
									nx >= edges.item(i).left - m_w - xi &&
									nx  < edges.item(i).left - m_w - xi + bd) {
									nx  = edges.item(i).left - m_w - xi;
									bumpedh = 1;
								}
							}
						}

						if (!bumpedv) {
							for (int i = 0; i < edges.count(); ++i) {
								if (ny < y &&
									ny <= edges.item(i).bottom &&
									ny  > edges.item(i).bottom - bd) {
									ny  = edges.item(i).bottom;
									bumpedv = 1;
								}
								if (ny > y &&
									ny >= edges.item(i).top - m_h - yi &&
									ny  < edges.item(i).top - m_h - yi + bd) {
									ny  = edges.item(i).top - m_h - yi;
									bumpedv = 1;
								}
							}
						}
					}
#endif
				}

		x = nx;
		y = ny;

		geometry.update(x, y);
		m_border->moveTo(x + xi, y + yi);
		m_doSomething = true;
		}
		break;
	}
	}

	geometry.remove();

	if (m_doSomething) {
	m_x = x + xi;
	m_y = y + yi;
	}

	if (CONFIG_CLICK_TO_FOCUS || isFocusOnClick()) 
		activate();
	m_border->moveTo(m_x, m_y);
	sendConfigureNotify();
}


void Client::fixResizeDimensions(int &w, int &h, int &dw, int &dh)
{
	if (w < 50) w = 50;
	if (h < 50) h = 50;

	if (m_sizeHints.flags & PResizeInc) {
	w = m_minWidth  + (((w - m_minWidth) / m_sizeHints.width_inc) *
			   m_sizeHints.width_inc);
	h = m_minHeight + (((h - m_minHeight) / m_sizeHints.height_inc) *
			   m_sizeHints.height_inc);

	dw = (w - m_minWidth)  / m_sizeHints.width_inc;
	dh = (h - m_minHeight) / m_sizeHints.height_inc;
	} else {
	dw = w; dh = h;
	}

	if (m_sizeHints.flags & PMaxSize) {
	if (w > m_sizeHints.max_width)  w = m_sizeHints.max_width;
	if (h > m_sizeHints.max_height) h = m_sizeHints.max_height;
	}

	if (w < m_minWidth)  w = m_minWidth;
	if (h < m_minHeight) h = m_minHeight;
}


void Client::resize(XButtonEvent *e, bool horizontal, bool vertical)
{
	if (isFixedSize()) return;
	ShowGeometry geometry(m_windowManager, (XEvent *)e);

	if (m_windowManager->attemptGrab
	(root(), None, DragMask, e->time) != GrabSuccess) {
	return;
	}

	// the resize increments may have changed since the window was
	// created -- re-read those ones but leave the rest unchanged in
	// case we adjusted them on construction
	XSizeHints currentHints;
	long s = 0;
	if (XGetWMNormalHints(display(), m_window, &currentHints, &s) == 0) {
		m_sizeHints.width_inc = currentHints.width_inc;
		m_sizeHints.height_inc = currentHints.height_inc;
	}

	if (vertical && horizontal)
	m_windowManager->installCursor(WindowManager::DownrightCursor);
	else if (vertical)
	m_windowManager->installCursor(WindowManager::DownCursor);
	else
	m_windowManager->installCursor(WindowManager::RightCursor);

	Window dummy;
	XTranslateCoordinates(display(), e->window, parent(),
			  e->x, e->y, &e->x, &e->y, &dummy);

	int xorig = e->x;
	int yorig = e->y;
	int x = xorig;
	int y = yorig;
	int w = m_w, h = m_h;
	int prevW, prevH;
	int dw, dh;

	XEvent event;
	bool found;
	bool done = false;
	struct timeval sleepval;

	m_doSomething = false;
	while (!done) {

	found = false;

	while (XCheckMaskEvent(display(), DragMask | ExposureMask, &event)) {
		found = true;
		if (event.type != MotionNotify) break;
	}

	if (!found) {
		sleepval.tv_sec = 0;
		sleepval.tv_usec = 10000;
		select(0, 0, 0, 0, &sleepval);
		continue;
	}

	switch (event.type) {

	default:
		break;

	case Expose:
		m_windowManager->dispatchEvent(&event);
		break;

	case ButtonPress:
		// don't like this
		XUngrabPointer(display(), event.xbutton.time);
		done = true;
		break;

	case ButtonRelease:

		x = event.xbutton.x; y = event.xbutton.y;

		if (!nobuttons(&event.xbutton)) x = -1;
		m_windowManager->releaseGrab(&event.xbutton);

		done = true;
		break;

	case MotionNotify:
		x = event.xbutton.x; y = event.xbutton.y;

		if (vertical && horizontal) {
		prevH = h; h = y - m_y;
		prevW = w; w = x - m_x;
		fixResizeDimensions(w, h, dw, dh);
		if (h == prevH && w == prevW) break;
		m_border->configure(m_x, m_y, w, h, CWWidth | CWHeight, 0);
		if constexpr (CONFIG_RESIZE_UPDATE)
			XResizeWindow(display(), m_window, w, h);
		geometry.update(dw, dh);
		m_doSomething = true;

		} else if (vertical) {
		prevH = h; h = y - m_y;
		fixResizeDimensions(w, h, dw, dh);
		if (h == prevH) break;
		m_border->configure(m_x, m_y, w, h, CWHeight, 0);
		if constexpr (CONFIG_RESIZE_UPDATE)
			XResizeWindow(display(), m_window, w, h);
		geometry.update(dw, dh);
		m_doSomething = true;

		} else {
		prevW = w; w = x - m_x;
		fixResizeDimensions(w, h, dw, dh);
		if (w == prevW) break;
		m_border->configure(m_x, m_y, w, h, CWWidth, 0);
		if constexpr (CONFIG_RESIZE_UPDATE)
			XResizeWindow(display(), m_window, w, h);
		geometry.update(dw, dh);
		m_doSomething = true;
		}

		break;
	}
	}

	if (m_doSomething) {

	geometry.remove();

	if (vertical && horizontal) {
		m_w = x - m_x;
		m_h = y - m_y;
		fixResizeDimensions(m_w, m_h, dw, dh);
		m_border->configure(m_x, m_y, m_w, m_h, CWWidth|CWHeight, 0, true);
	} else if (vertical) {
		m_h = y - m_y;
		fixResizeDimensions(m_w, m_h, dw, dh);
		m_border->configure(m_x, m_y, m_w, m_h, CWHeight, 0, true);
	} else {
		m_w = x - m_x;
		fixResizeDimensions(m_w, m_h, dw, dh);
		m_border->configure(m_x, m_y, m_w, m_h, CWWidth, 0, true);
	}

	XMoveResizeWindow(display(), m_window,
			  m_border->xIndent(), m_border->yIndent(), m_w, m_h);

	if (vertical && horizontal) {
		makeThisNormalHeight();
		makeThisNormalWidth();
	} else if (vertical)
		makeThisNormalHeight(); // in case it was full-height
	else if (horizontal)
		makeThisNormalWidth();

	sendConfigureNotify();
	}

	m_windowManager->installCursor(WindowManager::NormalCursor);
}


void Client::moveOrResize(XButtonEvent *e)
{
	if (e->x < m_border->xIndent() && e->y > m_h) {
	resize(e, false, true);
	} else if (e->y < m_border->yIndent() &&
		   e->x > m_w + m_border->xIndent() - m_border->yIndent()) { //hack
	resize(e, true, false);
	} else {
	move(e);
	}
}


void Border::eventButton(XButtonEvent *e)
{
	if (e->window == m_parent) {

	if (!m_client->isActive()) return;
	if (isTransient()) {
		if (e->x >= xIndent() && e->y >= yIndent()) {
		return;
		} else {
		m_client->move(e);
		return;
		}
	}

	m_client->moveOrResize(e);
	return;

	} else if (e->window == m_tab) {
	m_client->move(e);
	return;
	}

	if (e->window == m_resize) {
	m_client->resize(e, true, true);
	return;
	}

	if (e->window != m_button || e->type == ButtonRelease) return;

	if (windowManager()->attemptGrab(m_button, None, MenuGrabMask, e->time)
	!= GrabSuccess) {
	return;
	}

	XEvent event;
	bool found;
	bool done = false;
	struct timeval sleepval;
	unsigned long tdiff = 0L;
	int x = e->x;
	int y = e->y;
	int action = 1;
	int buttonSize = m_tabWidth[screen()] - TAB_TOP_HEIGHT*2 - 4;

	XFillRectangle(display(), m_button, m_drawGC[screen()],
		   0, 0, buttonSize, buttonSize);

	while (!done) {

	found = false;

	if (tdiff > CONFIG_DESTROY_WINDOW_DELAY && action == 1) {
		windowManager()->installCursor(WindowManager::DeleteCursor);
		action = 2;
	}

	while (XCheckMaskEvent(display(), MenuMask, &event)) {
		found = true;
		if (event.type != MotionNotify) break;
	}

	if (!found) {
		sleepval.tv_sec = 0;
		sleepval.tv_usec = 50000;
		select(0, 0, 0, 0, &sleepval);
		tdiff += 50;
		continue;
	}

	switch (event.type) {

	default:
		break;

	case Expose:
		windowManager()->dispatchEvent(&event);
		break;

	case ButtonPress:
		break;

	case ButtonRelease:

		if (!nobuttons(&event.xbutton)) {
		action = 0;
		}

		if (x < 0 || y < 0 || x >= buttonSize || y >= buttonSize) {
		action = 0;
		}

		windowManager()->releaseGrab(&event.xbutton);
		done = true;
		break;

	case MotionNotify:
		tdiff = event.xmotion.time - e->time;
		if (tdiff > 5000L) tdiff = 5001L; // in case of overflow!

		x = event.xmotion.x;
		y = event.xmotion.y;

		if (action == 0 || action == 2) {
		if (x < 0 || y < 0 || x >= buttonSize || y >= buttonSize) {
			windowManager()->installCursor(WindowManager::NormalCursor);
			action = 0;
		} else {
			windowManager()->installCursor(WindowManager::DeleteCursor);
			action = 2;
		}
		}

		break;
	}
	}

	XClearWindow(display(), m_button);
	windowManager()->installCursor(WindowManager::NormalCursor);

	if (tdiff > 5000L) {	// do nothing, they dithered too long
	return;
	}

	if (action == 1) m_client->hide();
	else if (action == 2) m_client->kill();
}


