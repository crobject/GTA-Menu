#pragma once
#include "MenuOption.h"
class MenuOptionSubmenu :
	public MenuOption
{
public:
	MenuOptionSubmenu(std::string name);
	void Render();
	void Call();
	~MenuOptionSubmenu();
};
