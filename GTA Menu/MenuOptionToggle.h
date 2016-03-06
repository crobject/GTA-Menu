#pragma once
#include "MenuOption.h"
class MenuOptionToggle :
	public MenuOption
{
public:
	MenuOptionToggle(std::string name, bool* toggle, OptionCallback cb);
	void Render();
	void Call();
	~MenuOptionToggle();
private:
	bool* m_toggle;
};

