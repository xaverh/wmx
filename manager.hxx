#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "general.hxx"
#include "keybinding.hxx"

class Client;

using ClientList = std::vector<Client *>;


class WindowManager {
public:
	WindowManager(int argc, char **argv);
	~WindowManager();

	void fatal(std::string_view);

	// for call from Client and within:

	Client *windowToClient(Window, bool create = false);
	Client *activeClient() const { return m_activeClient; }
	bool raiseTransients(Client *); // true if raised any
	Time timestamp(bool reset);
	void clearFocus();

	void setActiveClient(Client *const c);

	void addToHiddenList(Client *);
	void removeFromHiddenList(Client *);
	void skipInRevert(Client *, Client *);

	Display *display() const { return m_display; }
	Window root() const { return m_root[screen()]; }
	Window mroot(int i) const { return m_root[i]; }
	int screen() const { return m_screenNumber; }
	int screensTotal() const { return m_screensTotal; }
	void setScreen(int i) { m_screenNumber = i; }
	void setScreenFromRoot(Window);
	void setScreenFromPointer();

	int altModMask() const { return m_altModMask; }

	enum RootCursor {
	NormalCursor, DeleteCursor, DownCursor, RightCursor, DownrightCursor
	};


	void installCursor(RootCursor);
	void installCursorOnWindow(RootCursor, Window);
	void installColormap(Colormap);
	unsigned long allocateColour(int, char const *, char const *);

	void considerFocusChange(Client *, Window, Time timestamp);
	void stopConsideringFocus();

	// shouldn't really be public
	int attemptGrab(Window, Window, int, int);
	int attemptGrabKey(Window, int);
	void releaseGrab(XButtonEvent *);
	void releaseGrabKeyMode(XButtonEvent *);
	void releaseGrabKeyMode(XKeyEvent *);
	void spawn(char *, char *, char * = nullptr);

	int channel() const { return m_currentChannel; }
	int channels() const { return m_channels; }
	void setSignalled() { m_looping = false; } // ...
	void gotoChannel(int channel, Client *push);
	void ensureChannelExists(int channel);

	ClientList &clients() { return m_clients; }
	ClientList &hiddenClients() { return m_hiddenClients; }

	void hoistToTop(Client *);
	void hoistToBottom(Client *);
	void removeFromOrderedList(Client *);
	bool isTop(Client *);

	// Instruct the X-Server to reorder the windows to match our ordering.
	// Takes account of layering.
	void updateStackingOrder();    

	// for exposures during client grab, and window map/unmap/destroy
	// during menu display:
	void dispatchEvent(XEvent *);


	void netwmUpdateWindowList();
	void netwmUpdateStackingOrder();
	void netwmUpdateActiveClient();
	void netwmUpdateChannelList();
	void netwmUpdateCurrentChannel();

	// Stupid little helper function
	static int numdigits(int);

	void sort_clients(void);

	class Keybinding *key_binding_with_mod;
	class Keybinding *key_binding_without_mod;

	void print_clients(int);
	void add_fd_to_watch(int);
	void remove_fd_from_watch(int fd);

private:
	int loop();
	void release();

	Display *m_display;
	int m_screenNumber;
	int m_screensTotal;

	Window *m_root;

	Colormap *m_defaultColormap;
//    int *m_minimumColormaps;

	Cursor m_cursor;
	Cursor m_xCursor;
	Cursor m_vCursor;
	Cursor m_hCursor;
	Cursor m_vhCursor;

	char *m_terminal;

	ClientList m_clients;
	ClientList m_hiddenClients;

	ClientList m_orderedClients[MAX_LAYER + 1]; 
									// One list for each netwm/MWM layer
									// Layer 4 (NORMAL_LAYER) is the default.
	Client *m_activeClient;


	int m_shapeEvent;
	int m_currentTime;

	int m_channels;
	int m_currentChannel;	// from 1 to ...
	void flipChannel(bool statusOnly, bool flipDown,
			 bool quickFlip, Client *push); // bleah!
	Window createNumberWindow(int screen, char const *colour);
	int shapeNumberWindow(Window w, int number, int minDigits); // returns width in pixels
	void instateChannel();
	void createNewChannel();
	void checkChannel(int);
	Time m_channelChangeTime;
	Window *m_channelWindow;

	void updateClock();
	Window *m_clockWindow;
	Time m_clockUpdateTime;

	bool m_looping;
	int m_returnCode;

	static bool m_initialising;
	static int errorHandler(Display *, XErrorEvent *);
	static void sigHandler(int);
	static int m_signalled;
	static int m_restart;

	void initialiseScreen();
	void scanInitialWindows();

	void circulate(bool activeFirst);

	bool m_focusChanging;	// checking times for focus change
	Client *m_focusCandidate;
	Window  m_focusCandidateWindow;
	Time    m_focusTimestamp;	// time of last crossing event
	bool m_focusPointerMoved;
	bool m_focusPointerNowStill;
	void checkDelaysForFocus();


	void nextEvent(XEvent *);	// return

	void eventButton(XButtonEvent *, XEvent *);
	void eventKeyRelease(XKeyEvent *);
	void eventMapRequest(XMapRequestEvent *);
	void eventMap(XMapEvent *);
	void eventConfigureRequest(XConfigureRequestEvent *);
	void eventUnmap(XUnmapEvent *);
	void eventCreate(XCreateWindowEvent *);
	void eventDestroy(XDestroyWindowEvent *);
	void eventClient(XClientMessageEvent *);
	void eventColormap(XColormapEvent *);
	void eventProperty(XPropertyEvent *);
	void eventEnter(XCrossingEvent *);
	void eventReparent(XReparentEvent *);
	void eventFocusIn(XFocusInEvent *);
	void eventExposure(XExposeEvent *);

	bool m_altPressed;
	bool m_altStateRetained;
	void eventKeyPress(XKeyEvent *);

	void netwmInitialiseCompliance();
	Window m_netwmCheckWin;

	int m_altModMask;
	fd_set active_fds;
	int max_fd;

};

#endif

