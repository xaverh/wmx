# wmx — another window manager

wmx is a window manager for X. It is based on wm2 and provides a similarly unusual style of window decoration (side tabs, shaped windows); in place of wm2’s minimal functionality it offers many of the features of more conventional managers, often in the most simplistic implementations imaginable.

## Building

You need a Unix machine, X libraries (R4 or newer) with the Shape extension, and a C++ compiler. A three-button mouse is strongly recommended. Xpm is needed for frame background pixmaps.

```sh
make
```

Configuration is compile-time. Edit `config.hxx` and rebuild. There is no runtime options file and no X resources.

## Using wmx

Make sure no other window manager is running, `DISPLAY` is set, and the X server is up, then run the `wmx` binary.

### Window manipulations

- **Focus**: see Focus policy below.
- **Raise**: click on the tab or frame.
- **Move**: drag the tab, or Alt-drag anywhere on the window.
- **Hide**: click the button at the top of the tab.
- **Restore a hidden window**: left-button root menu.
- **New terminal**: first item on the left-button root menu (“New”), unless `CONFIG_NEW_WINDOW` is empty.
- **Delete**: click the tab button, hold until the cursor becomes a cross, then release (`CONFIG_DESTROY_WINDOW_DELAY`, default 600 ms).
- **Resize**: drag the bottom-right corner; constrained resize on the other corners.
- **Circulate windows**: right-click on root, frame or tab (configurable).
- **Channels** (virtual desktops): middle-click near the top-right of the root window (big green number appears); click again to step. Middle-click on a frame moves that window to another channel. Wheel mouse also changes channels (root) or moves a window (tab).
- **Launch an application**: middle-click on the root (not top-right). Executables in `$HOME/.wmx` appear on the menu. You can add or remove them while wmx is running. Note that `~/.wmx` is a directory, not a file.
- **Exit**: move to the extreme lower-right corner and left-click for the root menu; choose “Exit wmx”.

All move and resize operations are opaque.

## Keyboard controls

The modifier is `CONFIG_ALT_KEY` (default `Super_L`). Bindings are `std::optional<KeySym>` in `config.hxx`; set one to `std::nullopt` to disable it.

| Key | Action |
|-----|--------|
| Alt/Up, Alt/Down | Raise / lower focused window |
| Alt/Tab | Circulate |
| Alt/Return | Hide |
| Alt/BackSpace | Delete |
| Alt/PageUp, Alt/PageDown | Full height / restore height |
| Alt/Home, Alt/End | Maximise / unmaximise |
| Alt/KP_Add, Alt/KP_Subtract | Full width / restore width |
| Alt/Left, Alt/Right | Change channel |
| Alt/Fn | Jump to channel *n* |
| Alt/Pause | Sticky toggle |
| Alt/Menu | Keyboard client menu |
| Alt/Multi_Key | Keyboard application menu |

`CONFIG_SAME_KEY_MAX_UNMAX` makes maximise a toggle. Channel keys (`Alt-F1`…`F12`) are controlled by `CONFIG_USE_CHANNEL_KEYS`.

### Arbitrary keybindings

wmx also scans two directories under the command-menu directory (normally `~/.wmx`):

- `.keys` — fire with no modifier
- `.mkeys` — fire with the wmx modifier

Filenames must be valid KeySym names (`F1`, `KP_Enter`, `comma`, …). Executable files are bound to that key. When the key is pressed the script is spawned with one argument: the window ID of the currently focused client. NumLock and CapsLock are ignored. Either directory may be absent or empty.

## Focus policy

Two independent flags in `config.hxx`:

| `CONFIG_CLICK_TO_FOCUS` | `CONFIG_RAISE_ON_FOCUS` | Behaviour |
|-------------------------|-------------------------|-----------|
| false | false | Focus follows pointer, no raise (default) |
| false | true | Focus follows, raise on focus |
| true | false | Click to focus, stacking unchanged |
| true | true | Click to focus and raise |

## Channels

`CONFIG_CHANNEL_SURF` (default true) enables the middle-click channel switcher. `CONFIG_CHANNEL_CLICK_SIZE` is how close to the top-right corner the click must be. `CONFIG_USE_CHANNEL_MENU` (default false) replaces the surf gesture with a menu.

## Command menu

Executables and subdirectories in `~/.wmx` (or the directory named by `CONFIG_COMMAND_MENU`, or `$WMXDIR`) appear on the middle-button menu. Subdirectories become submenus.

`CONFIG_NEW_WINDOW` is a null-terminated list. The first element is the command for the “New” entry; further elements are arguments. An empty list omits the entry.

## Pixmaps

`CONFIG_USE_PIXMAPS` (default true) textures the frames with the compiled-in `background.xpm`. `CONFIG_USE_PIXMAP_MENUS` does the same for menus.

## Configuration reference

Everything is set in `config.hxx`. Rebuild after changing it.

| Option | Default | Meaning |
|--------|---------|---------|
| `CONFIG_CLICK_TO_FOCUS` | false | Click to focus |
| `CONFIG_RAISE_ON_FOCUS` | false | Raise on focus |
| `CONFIG_RIGHT_BUTTON` | Circulate | Right-click action: Off, Circulate, Lower, ToggleHeight |
| `CONFIG_NEW_WINDOW` | `x-terminal-emulator` | “New” menu command and arguments |
| `CONFIG_EVERYTHING_ON_ROOT_MENU` | true | Show visible clients on the root menu too |
| `CONFIG_COMMAND_MENU` | `.wmx` | Directory for the command menu |
| `CONFIG_DESTROY_WINDOW_DELAY` | 600 | Hold time (ms) before delete |
| `CONFIG_BUMP_DISTANCE` | 16 | Edge resistance in pixels |
| `CONFIG_BUMP_EVERYWHERE` | true | Resist other windows, not only screen edges |
| `CONFIG_RESIZE_UPDATE` | true | Opaque resize |
| `CONFIG_RAISELOWER_ON_CLICK` | false | Click on the top window lowers it |
| `CONFIG_USE_PIXMAPS` | true | Frame background image |
| `CONFIG_USE_PIXMAP_MENUS` | true | Menu background image |
| `CONFIG_CHANNEL_SURF` | true | Middle-click channel switcher |
| `CONFIG_USE_CHANNEL_KEYS` | true | Alt-F1…F12 jump to channel |
| `CONFIG_USE_CHANNEL_MENU` | false | Channel menu instead of surf |
| `CONFIG_MAD_FEEDBACK` | true | Skeletal frame for the menu item under the pointer |
| `CONFIG_FEEDBACK_DELAY` | 300 | Delay (ms) before showing the real window |
| `CONFIG_TAB_MARGIN` | 2 | Gap beside the tab label |
| `CONFIG_FRAME_THICKNESS` | 8 | Frame width |
| Colours, fonts | see file | `CONFIG_TAB_*`, `CONFIG_FRAME_*`, `CONFIG_MENU_*` |

Key bindings are the `CONFIG_*_KEY` optionals in the same file.

## NETWM

Partial support for the NETWM extended window-manager hints is present (enough for basic use with GNOME/KDE-style environments). Known gaps:

- full-screen windows
- struts (reserved screen edges for panels)
- always-on-current-desktop and “reveal desktop”
- some window-type handling

## Credits

wmx was written by Chris Cannam, recycling a lot of code and structure from 9wm by David Hogan.

- Sideways tabs: Andy Green
- Font rotation: Alan Richardson’s xvertext
- I18N: Kazushi (Jam) Marukawa
- Original application menu: Jeremy Fitzhardinge
- Dynamic configuration: Stefan “Sec” Zehl
- Multi-head: Sven Oliver “SvOlli” Moll
- NETWM: based on a 2000 patch by James Montgomery
- Shaped clients / early GNOME: Henri Naccache
- Keybindings, menu class/sort: Richard Sharman
- Many bug fixes and small features from Eric Marsden, Lasse Rasinen, Bill Spitzak, Jacques Garrigue, Martin Andrews, Glyn Faulkner, Zvezdan Petkovic, Damion Yates, Teemu Voipio, Ben Stern and others.

Feel free to modify and redistribute, retaining the original copyrights as appropriate. See the `COPYING` file for the licence.
