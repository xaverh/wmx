
#ifndef __KEYBINDING_H_
#define __KEYBINDING_H_

#include "Manager.h"

struct key_entry {
      KeySym	sym;
      char	*command;
} ;

class WindowManager;
class Keybinding {
 public:
    Keybinding::Keybinding(WindowManager *const, char *, bool);
    ~Keybinding();
    void Keybinding::setup(void);
    char * Keybinding::command_for_keysym(KeySym sym);
    void Keybinding::spawn(char *command, int);
    void Keybinding::grab_each_key(Display *display, Window window, int mask);


 private:
    WindowManager *wm;
    int Keybinding::scan_directory(char *dir_name, bool with_modifier);
    int table_count;
    struct key_entry *table;

};

#endif
