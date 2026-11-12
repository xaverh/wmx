#include "manager.hxx"
#include "client.hxx"

int WindowManager::loop()
{
	XEvent ev;
	m_looping = true;

	while (m_looping) {

	nextEvent(&ev);
	dispatchEvent(&ev);
	}

	release();
	return m_returnCode;
}

void WindowManager::dispatchEvent(XEvent *ev)
{
	m_currentTime = CurrentTime;

//            (int)ev->type, (void *)((XUnmapEvent *)ev)->window);

	switch (ev->type) {

	case ButtonPress:
	eventButton(&ev->xbutton, ev);
	break;

	case ButtonRelease:
	break;

	case KeyPress:
	eventKeyPress(&ev->xkey); // in Buttons.C
	break;

	case KeyRelease:
	eventKeyRelease(&ev->xkey); // in Buttons.C
	break;

	case MapRequest:
	eventMapRequest(&ev->xmaprequest);
	break;

	case ConfigureRequest:
	eventConfigureRequest(&ev->xconfigurerequest);
	break;

	case UnmapNotify:
	eventUnmap(&ev->xunmap);
	break;

	case CreateNotify:
	break;

	case DestroyNotify:
	eventDestroy(&ev->xdestroywindow);
	break;

	case ClientMessage:
	eventClient(&ev->xclient);
	break;

	case ColormapNotify:
	eventColormap(&ev->xcolormap);
	break;

	case PropertyNotify:
	eventProperty(&ev->xproperty);
	break;

	case SelectionClear:
	break;

	case SelectionNotify:
	break;

	case SelectionRequest:
	break;

	case EnterNotify:
	case LeaveNotify:
	eventEnter(&ev->xcrossing);
	break;

	case ReparentNotify:
	break;

	case FocusIn:
	eventFocusIn(&ev->xfocus);
	break;

	case Expose:		// might be wm tab
	eventExposure(&ev->xexpose);
	break;

	case MotionNotify:
	break;

	case MapNotify:
		eventMap(&ev->xmap);
		break;

	case FocusOut:
	case ConfigureNotify:
	case MappingNotify:
	case NoExpose:
	break;

	default:
	if (ev->type == m_shapeEvent) {
		fprintf(stderr, "wmx: shaped windows are not supported\n");
	} else {
		fprintf(stderr, "wmx: unsupported event type %d\n", ev->type);
	}
	break;
	}
}


void WindowManager::nextEvent(XEvent *e)
{
	int fd;
	fd_set rfds;
	struct timeval t;
	int r;

	if (!m_signalled) {

	waiting:

	if (m_channelChangeTime > 0) {

		Time t = timestamp(true);

		if (m_channelChangeTime > 0 && t >= m_channelChangeTime) {
		instateChannel();
		}
	}

	if (QLength(m_display) > 0) {
		XNextEvent(m_display, e);
		return;
	}

	fd = ConnectionNumber(m_display);
	memset((void *)&rfds, 0, sizeof(fd_set)); // SGI's FD_ZERO is fucked
	FD_SET(fd, &rfds);
	t.tv_sec = t.tv_usec = 0;


		int nfds = fd + 1;

	if (select(nfds, &rfds, nullptr, nullptr, &t) > 0) {

		//!!! This two-select structure is getting disgusting;
		// a marginal improvement would be to put this body in
		// another function, but it'd be better to go back and
		// think hard about why the code is like this at all

		if (FD_ISSET(fd, &rfds)) {
		XNextEvent(m_display, e);
		return;
		}

	}

	XFlush(m_display);

	memcpy((void *)&rfds, (void *)&active_fds, sizeof(rfds));
	t.tv_sec = 0; t.tv_usec = 20000;


	if ((r = select(max_fd, &rfds, nullptr, nullptr,
			(m_channelChangeTime > 0) ? &t :
			(struct timeval *)nullptr)) > 0) {

		int f;
		for (f=0; f < max_fd; f++) {
		  if (FD_ISSET(f, &rfds)) {
			if (f == fd) {
				  XNextEvent(m_display, e);
				  return;
			}
		  }
		}

	} // if select found something

	if (r == 0) goto waiting;

	if (errno != EINTR || !m_signalled) {
		perror("wmx: select failed");
		m_looping = false;
	}
	}

	fprintf(stderr, "wmx: signal caught, exiting\n");
	ignoreBadWindowErrors = true;
	m_looping = false;
	m_returnCode = 0;
}


void WindowManager::setScreenFromPointer()
{
	Window rw, cw;
	int rx, ry, cx, cy;
	unsigned int k;

	XQueryPointer(display(), root(), &rw, &cw, &rx, &ry, &cx, &cy, &k);
	setScreenFromRoot(rw);
}

void WindowManager::eventConfigureRequest(XConfigureRequestEvent *e)
{
	XWindowChanges wc;
	Client *c = windowToClient(e->window);

	e->value_mask &= ~CWSibling;
	if (c) c->eventConfigureRequest(e);
	else {

	wc.x = e->x;
	wc.y = e->y;
	wc.width  = e->width;
	wc.height = e->height;
	wc.border_width = 0;
	wc.sibling = None;
	wc.stack_mode = Above;
	e->value_mask &= ~CWStackMode;
	e->value_mask |= CWBorderWidth;

	XConfigureWindow(display(), e->window, e->value_mask, &wc);
	}
}


void Client::eventConfigureRequest(XConfigureRequestEvent *e)
{
	XWindowChanges wc;
	bool raise = false;

	e->value_mask &= ~CWSibling;

	gravitate(true);

	if (e->value_mask & CWX)      m_x = e->x;
	if (e->value_mask & CWY)      m_y = e->y;
	if (e->value_mask & CWWidth)  m_w = e->width;
	if (e->value_mask & CWHeight) m_h = e->height;
	if (e->value_mask & CWBorderWidth) m_bw = e->border_width;

	gravitate(false);

	if (e->value_mask & CWStackMode) {
	if (e->detail == Above) raise = true;
	e->value_mask &= ~CWStackMode;
	}

	if (parent() != root() && m_window == e->window) {
	m_border->configure(m_x, m_y, m_w, m_h, e->value_mask, e->detail);
	sendConfigureNotify();
	}

	if (m_managed) {
	wc.x = m_border->xIndent();
	wc.y = m_border->yIndent();
	} else {
	wc.x = e->x;
	wc.y = e->y;
	}

	wc.width = e->width;
	wc.height = e->height;
	wc.border_width = 0;
	wc.sibling = None;
	wc.stack_mode = Above;
	e->value_mask &= ~CWStackMode;
	e->value_mask |= CWBorderWidth;

	XConfigureWindow(display(), e->window, e->value_mask, &wc);

	if (raise && parent() != root()) {

	if (!m_stubborn) { // outstubborn stubborn windows
		Time popTime = windowManager()->timestamp(true);

		if (m_lastPopTime > 0L &&
			popTime > m_lastPopTime &&
			popTime - m_lastPopTime < 2000) { // 2 pops in 2 seconds
			m_stubborn = true;
			m_lastPopTime = 0L;

		} else {
			m_lastPopTime = popTime;
		}

		mapRaised();
		}
	} else {
		mapRaised();
		if (CONFIG_CLICK_TO_FOCUS || isFocusOnClick())
				 activate();
	}
	}
}


void WindowManager::eventMapRequest(XMapRequestEvent *e)
{
	Client *c = windowToClient(e->window);

	// JG
	if (!c) {
	c = windowToClient(e->window, true);
	if (c) {
		c->eventMapRequest(e);
		c->sendConfigureNotify();
	}
	} else {
	c->eventMapRequest(e);
	}

	updateStackingOrder();

	// some stuff for multi-screen fuckups here, omitted

//    else {
//    }
}


void Client::eventMapRequest(XMapRequestEvent *)
{
	switch(m_state) {

	case WithdrawnState:
	if (parent() == root()) {
		manage(false);
		return;
	}

	m_border->reparent();

	XAddToSaveSet(display(), m_window);
	if (m_channel == windowManager()->channel()) {
		XMapWindow(display(), m_window);
	}
	mapRaised();
	setState(NormalState);
	if (CONFIG_CLICK_TO_FOCUS || isFocusOnClick()) activate();
	break;

	case NormalState:
	if (m_channel == windowManager()->channel()) {
		XMapWindow(display(), m_window);
	}
	mapRaised();
	if (CONFIG_CLICK_TO_FOCUS || isFocusOnClick()) activate();
	break;

	case IconicState:
	unhide(true);
	break;
	}
}


void WindowManager::eventUnmap(XUnmapEvent *e)
{
	Client *c = windowToClient(e->window);
	if (c) c->eventUnmap(e);
}


void Client::eventUnmap(XUnmapEvent *e)
{
	if (e->window != m_window) return;


	if (m_unmappedForChannel) {
	setState(NormalState);
     return;
	}

	switch (m_state) {

	case IconicState:
	if (e->send_event) {
		unhide(false);
		withdraw();
	}
	break;

	case NormalState:
	if (isActive()) m_windowManager->clearFocus();
	if (!m_reparenting && !m_unmappedForChannel) withdraw();
	break;
	}

	// When unmapped transient window.  Should change a focus to not
	// transient window.
	if (transientFor()) {

	// gotta take it off the m_ordered list 
	// (if we don't do this, sometimes we see transient windows when we aren't
	// meant to - for an example, open netscape's find window and click close.
	// If this line isn't here, the window will stay, but be blank.
	m_windowManager->removeFromOrderedList(this);

	Client* c = windowManager()->windowToClient(transientFor());
	if (c && !c->isActive() && !CONFIG_CLICK_TO_FOCUS && !c->isFocusOnClick()) {
		c->activate();
		if (CONFIG_RAISE_ON_FOCUS) {
		c->mapRaised();
		}
	}
	}

	m_stubborn = false;
}


void Client::eventMap(XMapEvent *e)
{
	if (e->window != m_window) return;


	if (m_reparenting) {
	} else if (isWithdrawn()) {
		unwithdraw();
	}
}

void WindowManager::eventCreate(XCreateWindowEvent *e)
{
	// not currently used! windows created on first MapRequest instead
}


void WindowManager::eventDestroy(XDestroyWindowEvent *e)
{
	Client *c = windowToClient(e->window);

	if (c) {


	for (int i = m_clients.size()-1; i >= 0; --i) {
		if (m_clients[i] == c) {
		m_clients.erase(m_clients.begin() + i);
		break;
		}
	}

		int there = -1;


	checkChannel(c->channel());
	c->release();

	ignoreBadWindowErrors = true;
	XSync(display(), false
				);
	ignoreBadWindowErrors = false;
	}
}


void WindowManager::eventClient(XClientMessageEvent *e)
{
	if (e->message_type == Atoms::netwm_desktop) {

	int channel = (int)e->data.l[0] + 1;

	// netwm is not up-to-date and asked us to flip to a 
	// non-existing channel
	if (channel > m_channels) {
		netwmUpdateChannelList();
		return;
	}

		// gotoChannel always does something -- even if we're already
		// on that channel, it will flash up the channel indicator.
		// We don't want that if we're just responding to a NETWM

		if (channel != m_currentChannel) {
			gotoChannel(channel, 0);
		}

	return;
	}

	Client *c = windowToClient(e->window);
	if (c)
		c->eventClient(e);
}

void Client::eventClient(XClientMessageEvent *e)
{
	if (e->message_type == Atoms::netwm_activeWindow) {
		gotoClient();
		return;
	}

	if (e->message_type == Atoms::wm_changeState) {
		if (e->format == 32 && e->data.l[0] == IconicState) {
			if (isNormal()) hide();
			else if (isHidden()) gotoClient();
			return;
		} else if (e->format == 32 && e->data.l[0] == NormalState) {
			gotoClient();
			return;
		}
	}
}


void WindowManager::eventColormap(XColormapEvent *e)
{
	Client *c = windowToClient(e->window);
	int i;

	if (e->c_new) {  // this field is called "new" in the old C++-unaware Xlib

	if (c) c->eventColormap(e);
	else {
		for (i = 0; i < m_clients.size(); ++i) {
		m_clients[i]->eventColormap(e);
		}
	}
	}
}


void Client::eventColormap(XColormapEvent *e)
{
	if (e->window == m_window || e->window == parent()) {

	m_colormap = e->colormap;
	if (isActive()) installColormap();

	} else {

	for (int i = 0; i < m_colormapWinCount; ++i) {
		if (m_colormapWindows[i] == e->window) {
		m_windowColormaps[i] = e->colormap;
		if (isActive()) installColormap();
		return;
		}
	}
	}
}


void WindowManager::eventProperty(XPropertyEvent *e)
{
	Client *c = windowToClient(e->window);
	if (c) c->eventProperty(e);
}


void Client::eventProperty(XPropertyEvent *e)
{
	Atom a = e->atom;
	bool shouldDelete = (e->state == PropertyDelete);

	switch (a) {

	case XA_WM_ICON_NAME:
	if (m_iconName) XFree((char *)m_iconName);
	m_iconName = shouldDelete ? 0 : getProperty(a);
	if (setLabel()) rename();
	return;

	case XA_WM_NAME:
	if (m_name) XFree((char *)m_name);
	m_name = shouldDelete ? 0 : getProperty(a);
	if (setLabel()) rename();
	return;

	case XA_WM_TRANSIENT_FOR:
	getTransient();
	return;
	}

	if (a == Atoms::wm_colormaps) {
	getColormaps();
	if (isActive()) installColormap();
	}
}


void WindowManager::eventReparent(XReparentEvent *e)
{
	// not currently used! map events used instead, only
//    (void)windowToClient(e->window, true); // create if absent

	// odd screen complications, omitted
}


void WindowManager::eventMap(XMapEvent *e)
{
	Client *c = windowToClient(e->window);
	if (c) {
		c->eventMap(e);
	}
}


void WindowManager::eventEnter(XCrossingEvent *e)
{
	// quick hack for multi-screen stuff; although it still only
	// manages one screen, it will now allow focus on others.
	// thanks to Johan Danielsson

	if (e->type != EnterNotify) {
	if (e->same_screen == 0) {
		if (m_activeClient) m_activeClient->deactivate();
		m_activeClient = 0;
		XSetInputFocus(m_display, PointerRoot, None, timestamp(false));
	}
	return;
	}

	while (XCheckMaskEvent(m_display, EnterWindowMask, (XEvent *)e));
	m_currentTime = e->time;	// not CurrentTime

	Client *c = windowToClient(e->window);
	if (c) c->eventEnter(e);
}


void Client::eventEnter(XCrossingEvent *e)
{
	// first, big checks so as not to allow focus to change "through"
	// the hole in the tab

	if (!isActive() && activeClient() && activeClient()->isNormal() &&
	!activeClient()->isTransient()) {

	int x, y;
	Window c;

	XTranslateCoordinates
		(display(), activeClient()->parent(), e->window, 0, 0, &x, &y, &c);

	if (activeClient()->coordsInHole(e->x - x, e->y - y)) return;
	}
/*********************************/
	if (e->type == EnterNotify) {
	if (!isActive() && !CONFIG_CLICK_TO_FOCUS && !isFocusOnClick()) {
		activate();
		if (CONFIG_RAISE_ON_FOCUS) {
		mapRaised();
		}
	}
	}
}


bool Client::coordsInHole(int x, int y)	// relative to parent
{
	return m_border->coordsInHole(x, y);
}


bool Border::coordsInHole(int x, int y)	// this is all a bit of a hack
{
	return (x > 1 && x < m_tabWidth[screen()]-1 &&
		y > 1 && y < m_tabWidth[screen()]-1);
}


void WindowManager::eventFocusIn(XFocusInEvent *e)
{
	if (e->detail != NotifyNonlinearVirtual) return;
	Client *c = windowToClient(e->window);

	if (c) c->eventFocusIn(e);
}


void Client::eventFocusIn(XFocusInEvent *e)
{
	if (m_window == e->window && !isActive()) {
	activate();
	mapRaised();
	}
}


void WindowManager::eventExposure(XExposeEvent *e)
{
	if (e->count != 0) return;
	Client *c = windowToClient(e->window);
	if (c) c->eventExposure(e);
}


void Client::eventExposure(XExposeEvent *e)
{
	if (m_border->hasWindow(e->window)) {
	m_border->expose(e);
	}
}


