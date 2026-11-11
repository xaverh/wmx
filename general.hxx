#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <cstdio>
#include <csignal>
#include <cerrno>
#include <cstdlib>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <X11/extensions/shape.h>


#include "config.hxx"

#include "listmacro.hxx"

#define NewString(x) (strcpy((char *)malloc(strlen(x)+1),(x)))

#include "config.hxx"

#include <ft2build.h>
#include FT_FREETYPE_H 
#include FT_OUTLINE_H
#include FT_GLYPH_H
#include <X11/Xft/Xft.h>


class Atoms {
public:
	static Atom wm_state;
	static Atom wm_changeState;
	static Atom wm_protocols;
	static Atom wm_delete;
	static Atom wm_takeFocus;
	static Atom wm_colormaps;
	static Atom wmx_running;

	static Atom netwm_supportingWmCheck;
	static Atom netwm_wmName;
	static Atom netwm_supported;
	static Atom netwm_clientList;
	static Atom netwm_clientListStacking;
	static Atom netwm_desktop;
	static Atom netwm_desktopCount;
	static Atom netwm_desktopNames;
	static Atom netwm_activeWindow;

	static Atom netwm_winState;



	static Atom netwm_winDesktop;

	static Atom netwm_winType;

	static Atom netwm_winType_desktop; // desktop active background window
	static Atom netwm_winType_dock;    // dock or panel to remain on top
	static Atom netwm_winType_toolbar; // managed torn-off toolbar window
	static Atom netwm_winType_menu;    // managed torn-off menu window
	static Atom netwm_winType_utility; // small persistent palette or similar
	static Atom netwm_winType_splash;  // splash screen
	static Atom netwm_winType_dialog;  // dialog; default for managed transient
	static Atom netwm_winType_dropdown;// menu window (override-redirect)
	static Atom netwm_winType_popup;   // menu window (override-redirect)
	static Atom netwm_winType_tooltip; // tooltip (override-redirect)
	static Atom netwm_winType_notify;  // e.g. battery low (override-redirect)
	static Atom netwm_winType_combo;   // combobox menu (override-redirect)
	static Atom netwm_winType_dnd;     // dragged object (override-redirect)
	static Atom netwm_winType_normal;  // normal top-level window
};

/* These are the netwm window types that we actually care about. */

enum ClientType 
{
	/* netwm_winType_normal,
	   or no hint and either override-redirect or no transient: */
	NormalClient,

	/* netwm_winType_dialog,
	   or no hint and transient: */
	DialogClient,

	DesktopClient,  // netwm_winType_desktop
	DockClient,     // netwm_winType_dock
	ToolbarClient,  // netwm_winType_toolbar
	MenuClient,     // netwm_winType_menu
	UtilityClient,  // netwm_winType_utility
	SplashClient,   // netwm_winType_splash
	NotifyClient    // netwm_winType_notify
};

declareList(AtomList, Atom);

constexpr int DESKTOP_LAYER = 0;

constexpr int FIRST_FOCUS_FOLLOWS_LAYER = 3; // above this it's focus-on-click only
constexpr int FIRST_DECORATED_LAYER = 3;     // above this, windows are borderless

constexpr int NORMAL_LAYER = 4;
constexpr int UTILITY_LAYER = 6;
constexpr int DIALOG_LAYER = 7;

constexpr int LAST_FOCUSABLE_LAYER = 7; // below this, windows do not receive focus

constexpr int TOOLBAR_LAYER = 8;

constexpr int LAST_DECORATED_LAYER = 8; // below this, windows are borderless

constexpr int DOCK_LAYER = 9;
constexpr int FULLSCREEN_LAYER = 12;

constexpr int MAX_LAYER = 13;

extern bool ignoreBadWindowErrors; // tidiness hack

#define AllButtonMask	( Button1Mask | Button2Mask | Button3Mask \
			| Button4Mask | Button5Mask )
#define ButtonMask	( ButtonPressMask | ButtonReleaseMask )
#define DragMask        ( ButtonMask | ButtonMotionMask )
#define MenuMask	( ButtonMask | ButtonMotionMask | ExposureMask )
#define MenuGrabMask	( ButtonMask | ButtonMotionMask | StructureNotifyMask \
			  | SubstructureNotifyMask )

#endif
