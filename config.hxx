#ifndef _CONFIG_H_
#define _CONFIG_H_

// ============================
// Configuration header for wmx
// ============================
// 
// To configure: change the relevant bit of the file, and rebuild the
// manager.  Make sure all necessary source files are built (by
// running "make depend" before you start).
// 
// This file is in four sections.  (This might imply that it's getting
// too long.)  The sections are:
// 
// I.   Straightforward operational parameters
// II.  Key bindings
// III. Colours and fonts
// IV.  Flashy stuff: channels, pixmaps, skeletal feedback &c.
// 
// All timing values are in milliseconds, but accuracy depends on the
// minimum timer value available to select, so they should be taken
// with a pinch of salt.  On the machine I'm using now, I mentally
// double all the given values.
// 
// -- Chris Cannam, January 1998


#include "dynamicconfig.hxx"
#include <optional>
#include <X11/X.h>
#include <X11/keysym.h>


// =================================================
// Section I. Straightforward operational parameters
// =================================================

// List visible as well as hidden clients on the root menu?  (Visible
// ones will be towards the bottom of the menu, flush-right.)
#define CONFIG_EVERYTHING_ON_ROOT_MENU (DynamicConfig::config.fullMenu())

// What to run to get a new window (from the "New" menu option)
constexpr char const *CONFIG_NEW_WINDOW_LABEL = "New";
constexpr char const *CONFIG_NEW_WINDOW_COMMAND = "x-terminal-emulator";
// Null-terminated argument list for the New command.
// Example with flags: { "-ls", "-sb", "-sl", "1024", nullptr }
constexpr char const *CONFIG_NEW_WINDOW_COMMAND_OPTIONS[] = {
	nullptr
};
// alternatively,
#define CONFIG_DISABLE_NEW_WINDOW_COMMAND (DynamicConfig::config.disableNew())

// Area where [exit wmx] is added (0 -> everywhere -# -> px from other side)
constexpr int CONFIG_EXIT_CLICK_SIZE_X = 0;
constexpr int CONFIG_EXIT_CLICK_SIZE_Y = -3;

// Directory under $HOME in which to look for commands for the
// middle-button menu
constexpr char const *CONFIG_COMMAND_MENU = ".wmx";
// append screennumber to COMMAND_MENU directory;
// use non screen style as fallback
constexpr bool CONFIG_ADD_SCREEN_TO_COMMAND_MENU = false;

// Focus possibilities.
// 
// You can't have CLICK_TO_FOCUS without RAISE_ON_FOCUS, but the other
// combinations should be okay.  If you set AUTO_RAISE you must leave
// the other two false; you'll then get focus-follows, auto-raise, and
// a delay on auto-raise as configured in the DELAY settings below.

#define CONFIG_CLICK_TO_FOCUS     (DynamicConfig::config.clickFocus())
#define CONFIG_RAISE_ON_FOCUS     (DynamicConfig::config.raiseFocus())
#define CONFIG_AUTO_RAISE         (DynamicConfig::config.autoRaiseFocus())

#define CONFIG_PASS_FOCUS_CLICK   (DynamicConfig::config.passFocusClick())

// Delays when using AUTO_RAISE focus method
// 
// In theory these only apply when using AUTO_RAISE, not when just
// using RAISE_ON_FOCUS without CLICK_TO_FOCUS.  First of these is the
// usual delay before raising; second is the delay after the pointer
// has stopped moving (only when over simple X windows such as xvt).

#define CONFIG_AUTO_RAISE_DELAY       (DynamicConfig::config.raiseDelay())
constexpr int CONFIG_POINTER_STOPPED_DELAY = 80;
constexpr int CONFIG_DESTROY_WINDOW_DELAY = 600;

// Number of pixels off the screen you have to push a window
// before the manager notices the window is off-screen (the higher
// the value, the easier it is to place windows at the screen edges)

constexpr int CONFIG_BUMP_DISTANCE = 16;

// If CONFIG_BUMP_EVERYWHERE is defined, windows will "bump" against
// other window edges as well as the edges of the screen

constexpr bool CONFIG_BUMP_EVERYWHERE = true;

// If RESIZE_UPDATE is true, windows will opaque-resize "correctly";
// if false, behaviour will be as in wm2 (stretching the background
// image only).

constexpr bool CONFIG_RESIZE_UPDATE = true;

// If RAISELOWER_ON_CLICK is true, clicking on the title of the
// topmost window will lower instead of raising it (patch due to
// Kazushi (Jam) Marukawa)

constexpr bool CONFIG_RAISELOWER_ON_CLICK = false;

// Specify the maximum length of an entry in the client menu or the command
// menu. Set this to zero if you want no limitation

constexpr int MENU_ENTRY_MAXLENGTH = 80;


// ========================
// Section II. Key bindings
// ========================

// Allow keyboard control?
#define CONFIG_USE_KEYBOARD       (DynamicConfig::config.useKeyboard())

// This is the key for wm controls: e.g. Alt/Left and Alt/Right to
// flip channels, and Alt/Tab to switch windows.  (On my 105-key
// PC keyboard, Meta_L corresponds to the left Windows key.)

constexpr std::optional<KeySym> CONFIG_ALT_KEY = XK_Super_L;

// And these define the rest of the keyboard controls, when the above
// modifier is pressed; they're keysyms as defined in <X11/keysym.h>
// and <X11/keysymdef.h>

constexpr std::optional<KeySym> CONFIG_FLIP_UP_KEY = XK_Right;
constexpr std::optional<KeySym> CONFIG_FLIP_DOWN_KEY = XK_Left;
constexpr std::optional<KeySym> CONFIG_HIDE_KEY = XK_Return;
constexpr std::optional<KeySym> CONFIG_STICKY_KEY = XK_Pause;
constexpr std::optional<KeySym> CONFIG_RAISE_KEY = XK_Up;
constexpr std::optional<KeySym> CONFIG_LOWER_KEY = XK_Down;
// Prior and Next should be the same as Page_Up and Page_Down in R6
constexpr std::optional<KeySym> CONFIG_FULLHEIGHT_KEY = XK_Prior;
constexpr std::optional<KeySym> CONFIG_NORMALHEIGHT_KEY = XK_Next;
constexpr std::optional<KeySym> CONFIG_FULLWIDTH_KEY = XK_KP_Add;
constexpr std::optional<KeySym> CONFIG_NORMALWIDTH_KEY = XK_KP_Subtract;
constexpr std::optional<KeySym> CONFIG_MAXIMISE_KEY = XK_Home;
constexpr std::optional<KeySym> CONFIG_UNMAXIMISE_KEY = XK_End;
constexpr bool CONFIG_SAME_KEY_MAX_UNMAX = false;

// The next two may clash badly with Emacs, if you use Alt as the
// modifier.  The commented variants might work better for some.
constexpr std::optional<KeySym> CONFIG_CIRCULATE_KEY = XK_Tab;
//constexpr std::optional<KeySym> CONFIG_CIRCULATE_KEY = XK_grave;
//constexpr std::optional<KeySym> CONFIG_CIRCULATE_KEY = XK_section;
constexpr std::optional<KeySym> CONFIG_DESTROY_KEY = XK_BackSpace;
//constexpr std::optional<KeySym> CONFIG_DESTROY_KEY = XK_Delete;
//constexpr std::optional<KeySym> CONFIG_DESTROY_KEY = XK_Insert;

// If WANT_KEYBOARD_MENU is true, then the MENU_KEY, when pressed with
// the modifier, will call up a client menu with keyboard navigation
constexpr bool CONFIG_WANT_KEYBOARD_MENU = true;
constexpr std::optional<KeySym> CONFIG_CLIENT_MENU_KEY = XK_Menu;
constexpr std::optional<KeySym> CONFIG_COMMAND_MENU_KEY = XK_Multi_key;
constexpr bool CONFIG_EXIT_ON_KBD_MENU = true;
// these are for navigating on the menu; they don't require a modifier
constexpr std::optional<KeySym> CONFIG_MENU_UP_KEY = XK_Up;
constexpr std::optional<KeySym> CONFIG_MENU_DOWN_KEY = XK_Down;
constexpr std::optional<KeySym> CONFIG_MENU_SELECT_KEY = XK_Return;
constexpr std::optional<KeySym> CONFIG_MENU_CANCEL_KEY = XK_Escape;

// Useful for fortunate people with Sun Type-5 keyboards.  These don't
// require the modifier to be pressed.
constexpr std::optional<KeySym> CONFIG_QUICKRAISE_KEY = XK_F15;
constexpr std::optional<KeySym> CONFIG_QUICKHIDE_KEY = XK_F17;
constexpr std::optional<KeySym> CONFIG_QUICKHEIGHT_KEY = XK_F13;
constexpr std::optional<KeySym> CONFIG_QUICKCLOSE_KEY = XK_F11;
constexpr bool CONFIG_QUICKRAISE_ALSO_LOWERS = true;

// Mouse Configuration
// Use this section to remap your mouse button actions.
//   Button1 = LMB, Button2 = MMB, Button3 = RMB 
//   Button4 = WheelUp, Button5 = WheelDown 
// To prevent one or more of these from being supported
// at all, define it to CONFIG_NO_BUTTON.
constexpr int CONFIG_NO_BUTTON = 999;
constexpr unsigned int CONFIG_CLIENTMENU_BUTTON = Button1;
constexpr unsigned int CONFIG_COMMANDMENU_BUTTON = Button2;
constexpr unsigned int CONFIG_CIRCULATE_BUTTON = Button3; // switch window, when over frame
constexpr unsigned int CONFIG_PREVCHANNEL_BUTTON = Button5; // flip channel, when over frame
constexpr unsigned int CONFIG_NEXTCHANNEL_BUTTON = Button4; // flip channel, when over frame


// ==============================
// Section III. Colours and fonts
// ==============================

// Fonts used all over the place.  FRAME_FONT is a comma-separated
// list of font names to use for the frame text (the first available
// font in the list is used).  MENU_FONT is likewise for menu text.
// The SIZE values are in pixels.
// Fonts are drawn with Xft.

//!!! no proper way to handle italic/bold yet

constexpr char const *CONFIG_FRAME_FONT = "Bitstream Vera Sans,Lucida Sans Unicode";
constexpr int CONFIG_FRAME_FONT_SIZE = 12;

constexpr char const *CONFIG_MENU_FONT = "Bitstream Vera Sans,Lucida Sans Unicode";
constexpr int CONFIG_MENU_FONT_SIZE = 12;

// CONFIG_TAB_MARGIN defines the size of the gap on the left and right of the
// text in the tab.

#define CONFIG_TAB_MARGIN   (DynamicConfig::config.tabMargin())


// If USE_PLAIN_X_CURSORS is true, wmx will use cursors from the
// standard X cursor font; otherwise it will install its own.  You may
// wish to set this if your X client and server are on different
// endian machines, or if you've aliased the cursor font to something
// else you like better, or if you just prefer the plain X cursors

constexpr bool CONFIG_USE_PLAIN_X_CURSORS = true;

// Colours for window decorations.  The BORDERS one is for the
// one-pixel border around the edge of each piece of decoration, not
// for the whole decoration

#define CONFIG_TAB_FOREGROUND     (DynamicConfig::config.tabForeground())
#define CONFIG_TAB_BACKGROUND     (DynamicConfig::config.tabBackground())
#define CONFIG_FRAME_BACKGROUND   (DynamicConfig::config.frameBackground())
#define CONFIG_BUTTON_BACKGROUND  (DynamicConfig::config.frameBackground())
constexpr char const *CONFIG_BORDERS = "black";
constexpr char const *CONFIG_CHANNEL_NUMBER = "green";

#define CONFIG_MENU_FOREGROUND    (DynamicConfig::config.tabForeground())
#define CONFIG_MENU_BACKGROUND    (DynamicConfig::config.tabBackground())
constexpr char const *CONFIG_MENU_BORDERS = "black";

// Pixel width for the bit of frame to the left of the window and the
// sum of the two bits at the top

constexpr int CONFIG_FRAME_THICKNESS = 8;


// ========================
// Section IV. Flashy stuff
// ========================

// If USE_PIXMAPS is true the frames will be decorated with the pixmap
// in ./background.xpm; if USE_PIXMAP_MENUS is also true, the menus
// will be too.  The latter screws up in palette-based visuals, but
// should be okay in true-colour.

constexpr bool CONFIG_USE_PIXMAPS = true;
constexpr bool CONFIG_USE_PIXMAP_MENUS = true;

// Set CHANNEL_SURF for multi-channel switching; CHANNEL_CLICK_SIZE is
// how close you have to middle-button-click to the top-right corner
// of the root window before the channel change happens.  Set
// USE_CHANNEL_KEYS if you want Alt-F1, Alt-F2 etc for quick channel
// changes, provided USE_KEYBOARD is also true.  Set USE_CHANNEL_MENU
// if you want to change channels via a keyboard-controlled menu
// instead of linearly up and down one at a time like TV.

constexpr bool CONFIG_CHANNEL_SURF = true;
constexpr int CONFIG_CHANNEL_CLICK_SIZE = 120;
constexpr int CONFIG_CHANNEL_NUMBER_SIZE = 20;
constexpr bool CONFIG_USE_CHANNEL_KEYS = true;
constexpr bool CONFIG_USE_CHANNEL_MENU = false;

// FLIP_DELAY is the length of time the big green number stays in the
// top-right when flipping channels, before the windows reappear.
// QUICK_FLIP_DELAY is the equivalent figure used when flipping with
// the Alt-Fn keys or mouse wheel.  Milliseconds.

constexpr int CONFIG_FLIP_DELAY = 1000;
constexpr int CONFIG_QUICK_FLIP_DELAY = 500;

// Set MAD_FEEDBACK for skeletal representations of windows when
// flicking through the client menu and changing channels.  The DELAY
// is how long skeletal feedback has to persist before wmx decides to
// post the entire window contents instead; if it's negative, the
// contents will never be posted.  Experiment with these -- if your
// machine is fast and you're really hyper, you might even like a
// delay of 0ms.

#define CONFIG_MAD_FEEDBACK       (DynamicConfig::config.useFeedback())
#define CONFIG_FEEDBACK_DELAY     (DynamicConfig::config.feedbackDelay())

// Position of the geometry window:
// X < 0 left, X > 0 right,  X = 0 center
// Y < 0 top,  Y > 0 bottom, Y = 0 center
constexpr int CONFIG_GEOMETRY_X_POS = 0;
constexpr int CONFIG_GEOMETRY_Y_POS = 0;

#endif

