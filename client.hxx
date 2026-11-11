#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "general.hxx"
#include "manager.hxx"
#include "border.hxx"

class EdgeRect {
public:
	EdgeRect() : left(0), right(0), top(0), bottom(0) { }
	int left, right, top, bottom;
};

using EdgeRectList = std::vector<EdgeRect>;

class Client {
public:
	Client(WindowManager *const, Window, bool);
	void release();

	/* for call from WindowManager: */

	void activate();		/* active() */
	void deactivate();		/* setactive(0) */
	void gravitate(bool invert);
	void installColormap();
	void unreparent();
	void withdraw(bool changeState = true);
	void unwithdraw();          // when map event received w/o map request
	void hide();
	void unhide(bool map);
	void rename();
	void kill();
	void mapRaised();		// without activating
	void lower();
	void raiseOrLower();	// without activating, raise if it isn't top.
				// otherwise lower.

	void move(XButtonEvent *);		// event for grab timestamp & coords
	void resize(XButtonEvent *, bool, bool);
	void moveOrResize(XButtonEvent *);
	void ensureVisible();	// make sure x, y are on-screen

	// These are the only accepted calls for feedback: the Manager
	// should *not* call directly into the Border
	void showFeedback();
	void raiseFeedbackLevel();
	void removeFeedback(bool mapped);

	void manage(bool mapped);
	bool hasWindow(Window);

	Client *revertTo() const { return m_revert; }
	void setRevertTo(Client *c) { m_revert = c; }

	bool isHidden()      const { return (m_state == IconicState);    }
	bool isWithdrawn()   const { return (m_state == WithdrawnState); }
	bool isNormal()      const { return (m_state == NormalState);    }
	bool isKilled()      const { return (m_window == None);          }
	bool isTransient()   const { return ((m_transient != None) || (m_shaped)); }
	bool isSticky()      const { return m_sticky; }
	bool skipsFocus()    const { return m_skipFocus || isNonFocusable(); } //!!!merge
	bool isFocusOnClick() const { return (m_focusOnClick ||
									  (layer() < FIRST_FOCUS_FOLLOWS_LAYER)); }
	bool isMovable()     const { return m_movable; }
	Window  transientFor()  const { return m_transient; }
	bool isFixedSize()   const { return m_fixedSize; }
	bool isShaped()      const { return m_shaped; }

	// Client is borderless (like things on desktop or at 'dock' layer
	// or above.
	bool isBorderless()  const { return ((layer() < FIRST_DECORATED_LAYER) ||
									  (layer() > LAST_DECORATED_LAYER)); }

	// Client should not receive focus (panel, desktop etc.)
	bool isNonFocusable(){ return ((layer() > LAST_FOCUSABLE_LAYER)); }

	char const *label()     const { return m_label;    }
	char const *name()      const { return m_name;     }
	char const *iconName()  const { return m_iconName; }

	int layer()             const { return m_layer; }

	ClientType type()       const { return m_type; }

	int channel() const { return m_channel;  }
	void flipChannel(bool leaving, int newChannel);
	bool isNormalButElsewhere() const { return isNormal()||m_unmappedForChannel; }
	void setChannel(int channel) { m_channel = channel; netwmUpdateChannel(); }
	void setSticky(bool sticky);
	void setSkipFocus(bool);
	void setFocusOnClick(bool);
	void setMovable(bool);
	void setLayer(int newLayer);

	void sendMessage(Atom, long);
	void sendConfigureNotify();

	void warpPointer();
	void activateAndWarp();
	void focusIfAppropriate(bool);
	void selectOnMotion(Window, bool);

	void gotoClient(); // when activated from e.g. menu or netwm task bar

	void maximise(int);
	void unmaximise(int);
	bool isFullHeight() const { return m_isFullHeight; }
	void makeThisNormalHeight() { m_isFullHeight = false; }
	void makeThisNormalWidth() { m_isFullWidth = false; }

	/* for call from within: */

	void fatal(char const *m)    { m_windowManager->fatal(m);              }
	Display *display()     const { return m_windowManager->display();      }
	int screen();
	Window parent()        const { return m_border->parent();              }
	Window root();
	Client *activeClient() const { return m_windowManager->activeClient(); }
	bool isActive()     const { return (activeClient() == this);        }

	WindowManager *windowManager() const { return m_windowManager; }

	// for call from equivalent wm functions in Events.C:

	void eventButton(XButtonEvent *);
	void eventMapRequest(XMapRequestEvent *);
	void eventConfigureRequest(XConfigureRequestEvent *);
	void eventMap(XMapEvent *);
	void eventUnmap(XUnmapEvent *);
	void eventColormap(XColormapEvent *);
	void eventProperty(XPropertyEvent *);
	void eventEnter(XCrossingEvent *);
	void eventFocusIn(XFocusInEvent *);
	void eventExposure(XExposeEvent *);
	void eventClient(XClientMessageEvent *);


	void netwmUpdateChannel();
	Window window()        const { return m_window; }

	void appendEdges(EdgeRectList &);

protected:      // cravenly submitting to gcc's warnings
	~Client();

private:

	Window m_window;
	Window m_transient;
	Border *m_border;

	bool m_shaped;

	Client *m_revert;

	int m_x;
	int m_y;
	int m_w;
	int m_h;
	int m_bw;
	Window m_wroot;
	int m_screen;
	bool m_doSomething;	// Become true if move() or resize() made
				// effect to this client.

	int m_channel;
	bool m_unmappedForChannel;
	bool m_sticky;
	bool m_skipFocus;
	bool m_focusOnClick;
	int m_layer;
	ClientType m_type;

//#if CONFIG_MAD_FEEDBACK != 0
	bool m_levelRaised;
	bool m_speculating;
//#endif

	XSizeHints m_sizeHints;
	bool m_fixedSize;
	bool m_movable;
	int m_minWidth;
	int m_minHeight;
	void fixResizeDimensions(int &, int &, int &, int &);
	bool coordsInHole(int, int);

	int m_state;
	int m_protocol;
	bool m_managed;
	bool m_reparenting;
	bool m_stubborn;		// keeps popping itself to the front
	Time m_lastPopTime;

	bool m_isFullHeight;
	bool m_isFullWidth;
	int m_normalH;
	int m_normalY;
	int m_normalW;
	int m_normalX;

	char *m_name;
	char *m_iconName;
	char const *m_label;	// alias: one of (instance,class,name,iconName)
	static char const *const m_defaultLabel;

	Colormap m_colormap;
	int m_colormapWinCount;
	Window *m_colormapWindows;
	Colormap *m_windowColormaps;

	WindowManager *const m_windowManager;

	char *getProperty(Atom name);
	char *getProperty(Atom name, Atom requiredType, int &length);

	// accessors 
	bool getState(int *);
	void setState(int);
	void setNetwmProperty(Atom, unsigned char, bool);


	// internal instantiation requests
	bool setLabel(void);	// returns true if changed
	void getColormaps(void);
	void getProtocols(void);
	void getTransient(void);
	void getClientType(void);
	void getChannel(void);

	void decorate(bool active);
};

constexpr int Pdelete = 1;
constexpr int PtakeFocus = 2;

#endif

