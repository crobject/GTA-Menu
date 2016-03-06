#pragma once
#include "MenuItemObject.h"
#include <functional>
class MenuSubMenuSelecter : public MenuItemObject
{
public:
	MenuSubMenuSelecter(void);
	MenuSubMenuSelecter(std::string name, std::function<Menu* (Client* client)> loadSubMenu);
	MenuSubMenuSelecter(std::string name, Menu* menu);
	void OnClick(Client* client, void* param);
	~MenuSubMenuSelecter(void);
private:
	Menu* m_menu;
	std::function<Menu* (Client* client)> m_loadSubMenu;
};
