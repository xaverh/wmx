#include <cstdio>
#include <cstring>
#include <filesystem>
#include <vector>

#include "keybinding.hxx"

int Keybinding::scan_directory(char *dir_name, bool with_modifier)
{
	namespace fs = std::filesystem;
	table.clear();

	std::error_code ec;
	if (!fs::is_directory(dir_name, ec))
		return 0;

	for (auto const &ent : fs::directory_iterator(dir_name, ec)) {
		if (ec || !ent.is_regular_file())
			continue;
		auto perms = ent.status().permissions();
		if ((perms & fs::perms::owner_exec) == fs::perms::none)
			continue;
		auto name = ent.path().filename().string();
		KeySym sym = XStringToKeysym(name.c_str());
		if (sym == 0)
			continue;
		table.push_back({sym, ent.path().string()});
	}
	return table.size();
}

Keybinding::Keybinding(WindowManager *const w_mgr, char *dir,
			   bool with_modifier)
{
	namespace fs = std::filesystem;
	wm = w_mgr;
	char const *keys_name = with_modifier ? ".mkeys" : ".keys";
	fs::path base = dir ? fs::path(dir)
			    : fs::path(getenv("HOME") ? getenv("HOME") : "")
			      / CONFIG_COMMAND_MENU;
	scan_directory(const_cast<char *>((base / keys_name).c_str()), with_modifier);
}

Keybinding::~Keybinding()
{}


char const *Keybinding::command_for_keysym(KeySym sym)
{
	for (auto const &e : table)
		if (e.sym == sym)
			return e.command.c_str();
	return nullptr;
}


void Keybinding::spawn(char *command, int windowid)
{
	char client_id[1024];
	sprintf(client_id, "0x%x", windowid);

	wm->spawn(command, command, client_id);

}

void Keybinding::grab_each_key(Display *display, Window window, int mask) {
	int i;
	int keycode;

	for (auto const &e : table) {
	keycode = XKeysymToKeycode(display, e.sym);
	XGrabKey(display, keycode,
		 mask,
		 window, false,
		 GrabModeAsync, GrabModeAsync);
	XGrabKey(display, keycode,
		 mask|LockMask|Mod2Mask,
		 window, false,
		 GrabModeAsync, GrabModeAsync);
	XGrabKey(display, keycode,
		 mask|LockMask,
		 window, false,
		 GrabModeAsync, GrabModeAsync);
	XGrabKey(display, keycode,
		 mask|Mod2Mask,
		 window, false,
		 GrabModeAsync, GrabModeAsync);
	}
}
