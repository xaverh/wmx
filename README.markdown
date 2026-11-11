# wmx — another window manager

wmx is a window manager for X. It is based on wm2 and provides a similarly unusual style of window decoration (side tabs, shaped windows); in place of wm2’s minimal functionality it offers many of the features of more conventional managers, often in the most simplistic implementations imaginable.

## Current status

The classic “eighth release” dates from November 2014. Since then the code has received:

- 2011: optional clock
- 2009: Composite redirects, `.desktop` file
- 2012: fontconfig, focus-revert crash fix
- 2014: notify window type, various warning fixes
- 2019: further compiler-warning fixes
- 2024: explicit licence (`COPYING`)
- 2026: integration of the long-standing rsharman patches (client names on menus, arbitrary keybindings, remote control) and licence-text updates

## Building wmx

You need a Unix machine, X libraries (R4 or newer) with the Shape extension, and a C++ compiler. A three-button mouse is strongly recommended. Xpm is needed only if you want frame background pixmaps.

wmx makes relatively heavy demands on the plain 2D performance of your X server because of shaped windows.

Edit `Config.h` (everything useful is documented there), then:

```sh
./configure
make
```

## Using wmx

Make sure no other window manager is running, `DISPLAY` is set, and the X server is up, then run the `wmx` binary.

### Command-line options

- `-r` — enable remote control on the default port (6999)
- `-p <port>` — enable remote control on the given port

There are still no X resources and no startup file beyond the directories and the options symlink described below.

### Window manipulations

- **Focus**: depends on the policy set in `Config.h` (see below).
- **Raise**: click on the tab or frame (unless auto-raise is on).
- **Move**: drag the tab, or Alt-drag anywhere on the window.
- **Hide**: click the button at the top of the tab.
- **Restore a hidden window**: left-button root menu.
- **New terminal**: first item on the left-button root menu (“New”), unless disabled in `Config.h`.
- **Delete**: click the tab button, hold ≥ 1.5 s until the cursor becomes a cross, then release (delay is configurable).
- **Resize**: drag the bottom-right corner; constrained resize on the other corners.
- **Circulate windows**: right-click on root, frame or tab.
- **Channels** (virtual desktops): middle-click near the top-right of the root window (big green number appears); click again to step. Middle-click on a frame moves that window to another channel. Wheel mouse also changes channels (root) or moves a window (tab).
- **Launch an application**: middle-click on the root (not top-right). Executables in `$HOME/.wmx` (or the directory named by `CONFIG_COMMAND_MENU`) appear on the menu. You can add/remove them while wmx is running. Note that `~/.wmx` is a directory, not a file.
- **Exit**: move to the extreme lower-right corner and left-click for the root menu; choose “Exit wmx”.

All move and resize operations are opaque.

## Keyboard controls

Hard-coded bindings (modifier is configurable via `CONFIG_ALT_KEY` in `Config.h`; default is now `Super_L`):

- Alt/Up, Alt/Down — raise / lower focused window
- Alt/Tab — circulate
- Alt/Return — hide
- Alt/BackSpace — delete
- Alt/PageUp, Alt/PageDown — full height / restore height
- Alt/Home, Alt/End — maximise / unmaximise
- Alt/KP_Add, Alt/KP_Subtract — full width / restore width
- `CONFIG_SAME_KEY_MAX_UNMAX` — make maximise a toggle
- Alt/Left, Alt/Right — change channel
- Alt/Fn — jump to channel *n*
- Alt/Pause — sticky toggle
- Alt/Menu — keyboard client menu
- Alt/Multi_Key — keyboard application menu

Some of these can be disabled in `Config.h`.

### Arbitrary keybindings

In addition to the hard-coded bindings, wmx scans two directories under the command-menu directory (normally `~/.wmx`):

- `.keys` — fire with no modifier
- `.mkeys` — fire with the wmx-modifier (`CONFIG_ALT_KEY`)

Filenames must be valid KeySym names (`F1`, `KP_Enter`, `comma`, `period`, …). Executable files are bound to that key. When the key is pressed the script is spawned with one argument: the window ID of the currently focused client. NumLock and CapsLock are ignored.

Example `~/.wmx/.mkeys/comma`:

```sh
#!/bin/sh
xwit -id $1 -rmove -50 0
```

(`xwit` or similar tools are useful here.) See also the remote-control section for combining the two features.

## Wheel mouse

Wheel on the root changes channel; wheel on a window tab moves that window to another channel.

## Focus policy

Controlled in `Config.h` (and overridable at runtime via the options symlink):

- `CONFIG_CLICK_TO_FOCUS`
- `CONFIG_RAISE_ON_FOCUS`
- `CONFIG_AUTO_RAISE` (only meaningful when the first two are false; adds a delay)

Also “passclick” (whether the focusing click is also delivered to the client) when click-to-focus is used.

## Skeletal feedback

`CONFIG_MAD_FEEDBACK` draws a half-frame for the menu item under the pointer. `CONFIG_FEEDBACK_DELAY` can briefly show the real window.

## Dynamic configuration

A symbolic link `~/.wmx/options` (or in the command-menu directory) whose target is a string of the form

```
menu:full/new:off/focus:delay-raise,100/class:on/sort:on
```

sets runtime options. Recognised tokens include `menu`, `new`, `keyboard`, `feedback`, `focus` (click/raise/delay-raise/follow), `passclick`, `class`, `sort`, and others. Defaults live in `Config.C`.

## Client menu enhancements

With `class:on` the menu shows `[class] name` instead of just the label. With `sort:on` the entries are sorted (so all windows of the same class appear together). Both can be set in `Config.h` or via the options symlink (or the remote-control `options` command).

## Remote control

Start wmx with `-r` (port 6999) or `-p <port>`. A simple line-oriented protocol is accepted on the socket. Replies always end with a blank line.

Example:

```
$ telnet localhost 6999
Welcome to wmx remote control.  Type help for commands
channel 2
OK

```

Useful with the `socket` program (or any equivalent) from keybinding scripts. Typical commands: `channel`, `wchannel`, `hide`, `warp`, `clients`, `options`, `help`, `quit`, …

## Pixmaps

Set `CONFIG_USE_PIXMAPS` and supply `background.xpm` (or `border.xpm` in the `.wmx` directory) to texture the frames.

## Optional clock

`CONFIG_CLOCK` (default false) draws a permanent clock in the top-left of screen 0, reusing the shaped-number code used for the channel indicator. Colour is `CONFIG_CLOCK_NUMBER`.

## Composite

`CONFIG_USE_COMPOSITE` (default true) enables `CompositeRedirectAutomatic` on every screen root when the Composite extension is present. This can improve performance on modern cards; appearance and behaviour are otherwise unchanged.

## NETWM and desktop environments

Partial support for the NETWM extended window-manager hints is present (enough for basic use with GNOME/KDE-style environments). Known gaps (still open):

- full-screen windows
- struts (reserved screen edges for panels)
- always-on-current-desktop and “reveal desktop”
- some window-type handling (tear-off menus, splash screens)
- a few property-update and key-binding edge cases

Patches welcome.

## Other notes

- Some old xterm/rxvt versions refresh slowly; try a different terminal or put the scrollbar on the right and make it thick enough not to collide with the resize handle.
- xnodecor (external) can give completely borderless sticky windows.

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
- Keybindings, remote control, menu class/sort: Richard Sharman
- Many bug fixes and small features from Eric Marsden, Lasse Rasinen, Bill Spitzak, Jacques Garrigue, Martin Andrews, Glyn Faulkner, Zvezdan Petkovic, Damion Yates, Teemu Voipio, Ben Stern and others.

Feel free to modify and redistribute, retaining the original copyrights as appropriate. See the `COPYING` file for the licence.

(The old 42.org mailing list and its archive are historical.)
