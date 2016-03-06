#pragma once
#include "stdafx.h"
#include "MenuItemObject.h"
class MenuItemSelect : public MenuItemObject
{
public:
	MenuItemSelect(void);
	MenuItemSelect(std::string name, void (*action)(Client* client, void* param));
	~MenuItemSelect(void);
	void OnHoverEnter();
	void OnHoverLeave();
	virtual void OnClick(Client* client, void* param);
};