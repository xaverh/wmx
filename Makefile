# POSIX.1-2024 (IEEE Std 1003.1-2024)
.POSIX:

# wmx - another window manager
# See COPYING for licence details.

CXX         = c++
PKG_CONFIG  = pkg-config

PREFIX      = /usr/local
MANPREFIX   = $(PREFIX)/share/man

CXXFLAGS    = -Wall -O2
LDFLAGS     = -lX11 -lXext -lXmu -lm

# Xft is required for fonts.
XFT_CFLAGS != $(PKG_CONFIG) --cflags xft fontconfig freetype2
XFT_LIBS   != $(PKG_CONFIG) --libs   xft fontconfig freetype2

CXXFLAGS += $(XFT_CFLAGS) -DCONFIG_SYSTEM_COMMAND_MENU=\"$(PREFIX)/lib/wmx/menu\"
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
      menu.cxx

OBJ = border.o \
      buttons.o \
      channel.o \
      client.o \
      config.o \
      events.o \
      keybinding.o \
      main.o \
      manager.o \
      menu.o

all: wmx

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $<

wmx: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

# Header dependencies
border.o: border.cxx background.xpm border.hxx client.hxx config.hxx dynamicconfig.hxx \
	general.hxx keybinding.hxx manager.hxx
buttons.o: buttons.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx general.hxx \
	keybinding.hxx manager.hxx menu.hxx
channel.o: channel.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx general.hxx \
	keybinding.hxx manager.hxx
client.o: client.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx general.hxx \
	keybinding.hxx manager.hxx
config.o: config.cxx config.hxx dynamicconfig.hxx
events.o: events.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx general.hxx \
	keybinding.hxx manager.hxx
keybinding.o: keybinding.cxx config.hxx dynamicconfig.hxx general.hxx keybinding.hxx \
	manager.hxx
main.o: main.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx general.hxx \
	keybinding.hxx manager.hxx
manager.o: manager.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx cursors.hxx \
	general.hxx keybinding.hxx manager.hxx menu.hxx
menu.o: menu.cxx border.hxx client.hxx config.hxx dynamicconfig.hxx general.hxx \
	keybinding.hxx manager.hxx menu.hxx

wmx.desktop: wmx.desktop.in
	sed 's|@PREFIX@|$(PREFIX)|g' wmx.desktop.in > wmx.desktop

install: wmx wmx.desktop
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f wmx $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/wmx
	mkdir -p $(DESTDIR)$(PREFIX)/share/xsessions
	cp -f wmx.desktop $(DESTDIR)$(PREFIX)/share/xsessions/wmx.desktop

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/wmx
	rm -f $(DESTDIR)$(PREFIX)/share/xsessions/wmx.desktop

clean:
	rm -f wmx $(OBJ) wmx.desktop

.PHONY: all clean install uninstall
