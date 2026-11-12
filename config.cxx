#include "config.hxx"

#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <unistd.h>

// 32 is enough to fit even "light goldenrod yellow" comfortably
#define COLOR_LEN 32

DynamicConfig DynamicConfig::config;

struct DynamicConfigImpl
{
	char options[1000];	// Old options-string
	char path[1000];	// Path to Options-Link
	char focus; 	// 1 = Click , 2 = Raise, 4 = Autoraise
	char disable;	// 0 = New Window option, 1 = no New
	char rightBt;	// 0 = disable, 1 = circulate, 2 = lower, 4 = height
	char passfocusclick;// 0 = no, 1 = yes
};  

DynamicConfig::DynamicConfig() : m_impl(new DynamicConfigImpl)
{
	strcpy(m_impl->options, "");

	char *home = getenv("HOME");
	char *wmxdir = getenv("WMXDIR");

	if (!wmxdir) {

	if (!home) m_impl->path[0] = 0;
	else {
		strcpy(m_impl->path, getenv("HOME"));  // Path to Options-Link
		strcat(m_impl->path, "/");
		strcat(m_impl->path, CONFIG_COMMAND_MENU);
		strcat(m_impl->path, "/options");
	}

	} else {

	if (*wmxdir == '/') strcpy(m_impl->path, wmxdir);
	else {
		strcpy(m_impl->path, home);
		strcat(m_impl->path, "/");
		strcat(m_impl->path, wmxdir);
	}
	strcat(m_impl->path, "/options");
	}

	m_impl->focus = 0;	// 1 = Click , 2 = Raise, 4 = Autoraise
	m_impl->disable = 0;        // 0 = allow New window, 1 = don't
	m_impl->rightBt = 1;	// 0 = disable, 1 = circulate, 2 = lower
	m_impl->passfocusclick = 1;

	scan(1);
}

DynamicConfig::~DynamicConfig()
{
	delete m_impl;
}

char DynamicConfig::clickFocus() { return m_impl->focus & 1; }
char DynamicConfig::raiseFocus() { return m_impl->focus & 2; }
char DynamicConfig::autoRaiseFocus() { return m_impl->focus & 4; }
char DynamicConfig::disableNew() { return m_impl->disable & 1; }
char DynamicConfig::rightCirculate() { return m_impl->rightBt & 1; }
char DynamicConfig::rightLower() { return m_impl->rightBt & 2; }
char DynamicConfig::rightToggleHeight() { return m_impl->rightBt & 4; }
char DynamicConfig::passFocusClick() { return m_impl->passfocusclick; }

void DynamicConfig::scan(char startup)
{
	char temp[1000];
	memset(temp, 0, 1000);

	if (m_impl->path[0] && readlink(m_impl->path, temp, 999) > 0) {
	if (strcmp(temp, m_impl->options) != 0) { // Did it change ?
		strcpy(m_impl->options, temp);
		update(temp);
	}
	} else if (startup) {
	}
}


void DynamicConfig::update(char *string)
{
	char *s;

#define OPTION(x) ( (!strncasecmp(s, x, strlen(x))) && (s += strlen(x)) )


	s = strtok(string, "/");
	do {

	if (OPTION("new:")) {
		if (OPTION("on")) m_impl->disable = 0;
		else if (OPTION("off")) m_impl->disable = 1;
	}

	if (OPTION("passclick:")) {
		if (OPTION("on")) m_impl->passfocusclick = 1;
		else if (OPTION("off")) m_impl->passfocusclick = 0;
	}

	if (OPTION("focus:")) {
		if (OPTION("click")) m_impl->focus = 3;
		else if (OPTION("raise")) m_impl->focus = 2;
		else if (OPTION("delay-raise")) {
		m_impl->focus = 4;
		} else if (OPTION("follow")) m_impl->focus = 0;
	}

	if (OPTION("right:")) {
		if (OPTION("off")) m_impl->rightBt = 0;
		else if (OPTION("circulate")) m_impl->rightBt = 1;
		else if (OPTION("lower")) m_impl->rightBt = 2;
		else if (OPTION("toggleheight")) m_impl->rightBt = 4;
	}

	if (*s != '\0') {
		fprintf(stderr, "\nwmx: Dynamic configuration error: "
			"`%s' @ position %d", s, (int)(string - s));
	}

	} while ((s = strtok(nullptr, "/")));

	fprintf(stderr, "\n");

#undef OPTION
}

