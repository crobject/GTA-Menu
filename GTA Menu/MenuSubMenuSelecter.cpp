#include "MenuSubMenuSelecter.h"
#include "stdafx.h"

MenuSubMenuSelecter::MenuSubMenuSelecter(void)
{
}

MenuSubMenuSelecter::MenuSubMenuSelecter(std::string name, std::function<Menu* (Client* client)> loadSubMenu) : MenuItemObject(name, nullptr)
{
	m_loadSubMenu = loadSubMenu;
	m_menu = nullptr;
}

MenuSubMenuSelecter::MenuSubMenuSelecter(std::string name, Menu* menu) : MenuItemObject(name, nullptr)
{
	m_menu = menu;
	m_loadSubMenu = nullptr;
}


MenuSubMenuSelecter::~MenuSubMenuSelecter(void)
{
}

void MenuSubMenuSelecter::OnClick(Client* client, void* param)
{
	if(m_menu)
		client->RenderSubMenu(m_menu);
	else if(m_loadSubMenu)
	{
		auto old_menu = client->CurrentMenu();
		auto m = m_loadSubMenu(client);
		if(m)
		{
			client->RenderSubMenu(m);
			//delete old_menu;
		}
	}
}