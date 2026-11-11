# POSIX.1-2024 (IEEE Std 1003.1-2024)
.POSIX:

# wmx - another window manager
# See COPYING for licence details.

CXX         = c++
PKG_CONFIG  = pkg-config

PREFIX      = /usr/local
MANPREFIX   = $(PREFIX)/share/man

CXXFLAGS    = -Wall -O2
LDFLAGS     = -lX11 -lXext -lXmu -lSM -lICE -lm

# Optional libraries. Empty if pkg-config or the packages are missing.
XFT_CFLAGS != $(PKG_CONFIG) --cflags xft fontconfig freetype2 2>/dev/null || true
XFT_LIBS   != $(PKG_CONFIG) --libs   xft fontconfig freetype2 2>/dev/null || true

CXXFLAGS += $(XFT_CFLAGS)
LDFLAGS  += $(XFT_LIBS) -lXpm -lXcomposite

SRC = border.cxx \
      buttons.cxx \
      channel.cxx \
      client.cxx \
      config.cxx \
      events.cxx \
      keybinding.cxx \
      main.cxx \
      manager.cxx \
      menu.cxx \
      remote.cxx \
      rotated.cxx \
      session.cxx

OBJ = border.o \
      buttons.o \
      channel.o \
      client.o \
      config.o \
      events.o \
      keybinding.o \
      main.o \
      manager.o \
      menu.o \
      remote.o \
      rotated.o \
      session.o

all: wmx

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $<

wmx: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

# Header dependencies
border.o: border.cxx background.xpm border.hxx client.hxx config.hxx \
	general.hxx keybinding.hxx listmacro.hxx manager.hxx remote.hxx \
	rotated.hxx
buttons.o: buttons.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx menu.hxx remote.hxx \
	rotated.hxx
channel.o: channel.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx remote.hxx rotated.hxx
client.o: client.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx remote.hxx rotated.hxx
config.o: config.cxx config.hxx
events.o: events.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx remote.hxx rotated.hxx
keybinding.o: keybinding.cxx config.hxx general.hxx keybinding.hxx \
	listmacro.hxx manager.hxx remote.hxx
main.o: main.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx remote.hxx rotated.hxx
manager.o: manager.cxx border.hxx client.hxx config.hxx cursors.hxx \
	general.hxx keybinding.hxx listmacro.hxx manager.hxx menu.hxx \
	remote.hxx rotated.hxx
menu.o: menu.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx menu.hxx remote.hxx \
	rotated.hxx
remote.o: remote.cxx border.hxx client.hxx config.hxx general.hxx \
	keybinding.hxx listmacro.hxx manager.hxx remote.hxx rotated.hxx
rotated.o: rotated.cxx config.hxx rotated.hxx
session.o: session.cxx config.hxx general.hxx keybinding.hxx \
	listmacro.hxx manager.hxx remote.hxx

clean:
	rm -f wmx $(OBJ)

install: wmx
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f wmx $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/wmx

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/wmx

.PHONY: all clean install uninstall
