#ifndef _BORDER_H_
#define _BORDER_H_

#include "general.hxx"

class Client;
class WindowManager;

// These distances exclude the 1-pixel borders.  You could probably
// change these a certain amount before breaking the shoddy code in
// the rest of this file.

constexpr int TAB_TOP_HEIGHT = 2;
constexpr int FRAME_WIDTH = CONFIG_FRAME_THICKNESS;
constexpr int TRANSIENT_FRAME_WIDTH = 4;
// NB frameTopHeight = frameHeight-tabTopHeight

class Border {			// friend of client
public:
	Border(Client *const, Window child);
	~Border();

	void map();
	void unmap();
	//void lower();
	//void mapRaised();
	void decorate(bool active, int w, int h);
	void reparent();
	void configure(int x, int y, int w, int h, unsigned long mask, int detail,
		   bool force = false);
	void moveTo(int x, int y);

//#if CONFIG_MAD_FEEDBACK != 0
	// For call from Client only, please

	void showFeedback(int x, int y, int w, int h);
	void removeFeedback();
	void toggleFeedback(int x, int y, int w, int h);
//#endif

	WindowManager *windowManager(); // calls into Client
	bool isTransient();	// calls into Client
	bool isFixedSize();	// calls into Client
	Window parent() const { return m_parent; }
	bool hasWindow(Window);

	Display *display();
	int screen();
	Window root();

	void expose(XExposeEvent *);
	void eventButton(XButtonEvent *); // in Buttons.C

	int yIndent();
	int xIndent();

	bool coordsInHole(int, int); // in Events.C of all places
	static Pixmap backgroundPixmap(WindowManager *);
	static GC drawGC(WindowManager *,int);

private:
	Client *m_client;

	Window m_parent;
	Window m_tab;
	Window m_child;
	Window m_button;
	Window m_resize;

//#if CONFIG_MAD_FEEDBACK != 0
	Window m_feedback;
	bool m_fedback;
//#endif

	void fatal(std::string_view);

	std::string m_label;

	XftDraw *m_xftDraw;

	void fixTabHeight(int);
	int getRotatedTextWidth(std::string_view);
	void drawLabel();

	void setFrameVisibility(bool, int, int);
	void setTransientFrameVisibility(bool, int, int);
	void shapeParent(int, int);
	void shapeTransientParent(int, int);
	void shapeTab(int, int);
	void resizeTab(int);	// for rename without changing window size
	void shapeResize();

	int m_prevW;
	int m_prevH;

private:
	int        m_tabHeight;	// depends on the label
	static int *m_tabWidth;	// depends on the label font
	static XftFont *m_tabFont;
	static XftColor *m_xftColour;
	static GC  *m_drawGC;
	static unsigned long *m_foregroundPixel;
	static unsigned long *m_backgroundPixel;
	static unsigned long *m_frameBackgroundPixel;
	static unsigned long *m_buttonBackgroundPixel;
	static unsigned long *m_borderPixel;
	static Pixmap m_backgroundPixmap;

	static void initialiseStatics(WindowManager *);
};


#endif

