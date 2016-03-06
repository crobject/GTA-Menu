#pragma once
#include "stdafx.h"
#include "MenuItemObject.h"
#include <functional>
class MenuItemClientName : public MenuItemObject
{
public:
	MenuItemClientName(void);
	MenuItemClientName(int clientNum, std::function<Menu* (Client* client)> loadSubMenu);
	~MenuItemClientName(void);
	void OnClick(Client* client, void* param);
	void Render();
private:
	int m_clientNum;
	std::function<Menu* (Client* client)> m_loadSubMenu;
};