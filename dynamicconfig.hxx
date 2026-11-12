#ifndef _DYNAMICCONFIG_H_
#define _DYNAMICCONFIG_H_

// Runtime configuration.  Anything in config.hxx that is
// #defined to DynamicConfig::config.something() takes its
// default from config.cxx.

struct DynamicConfigImpl;

class DynamicConfig
{
public:
	DynamicConfig();
	~DynamicConfig();
	void scan(char startup = 0);

	char clickFocus();
	char raiseFocus();
	char autoRaiseFocus();
	char disableNew();
	char rightCirculate();
	char rightLower();
	char rightToggleHeight();
	char passFocusClick();

	static DynamicConfig config;

private:
	DynamicConfigImpl *m_impl;
	void update(char *);
};

#endif
