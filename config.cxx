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
	char rightBt;	// 0 = disable, 1 = circulate, 2 = lower, 4 = height
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

	m_impl->rightBt = 1;	// 0 = disable, 1 = circulate, 2 = lower

	scan(1);
}

DynamicConfig::~DynamicConfig()
{
	delete m_impl;
}

char DynamicConfig::rightCirculate() { return m_impl->rightBt & 1; }
char DynamicConfig::rightLower() { return m_impl->rightBt & 2; }
char DynamicConfig::rightToggleHeight() { return m_impl->rightBt & 4; }

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

