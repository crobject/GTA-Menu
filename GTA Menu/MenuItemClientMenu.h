#pragma once
#include "stdafx.h"
#include "MenuItemSelect.h"
class MenuItemClientMenu : public MenuItemSelect
{
public:
	MenuItemClientMenu(void);
	MenuItemClientMenu(std::string name, void (*action)(Client* client, void* param));
	~MenuItemClientMenu(void);
	void OnClick(Client* client, void* param);
};

