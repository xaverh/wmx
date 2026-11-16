#ifndef __KEYBINDING_H_
#define __KEYBINDING_H_

#include "manager.hxx"
#include <string>
#include <vector>

struct key_entry {
	KeySym sym;
	std::string command;
};

class WindowManager;
class Keybinding {
 public:
	Keybinding(WindowManager *const, char *, bool);
	~Keybinding();
	char const *command_for_keysym(KeySym sym);
	void spawn(char *command, int);
	void grab_each_key(Display *display, Window window, int mask);

 private:
	WindowManager *wm;
	int scan_directory(char *dir_name, bool with_modifier);
	std::vector<key_entry> table;
};

#endif
