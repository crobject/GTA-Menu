#pragma once
#include "stdafx.h"
#include "MenuItemObject.h"
class MenuItemIndexGetter : public MenuItemObject
{
public:
	MenuItemIndexGetter(void);
	MenuItemIndexGetter(void (*action)(Client* client, void* param), std::string (*getter)(Client* client, int index), bool passbyIndex = false);
	~MenuItemIndexGetter(void);
	void Render();
	void OnClick(Client* client, void* param);
private:
	std::string (*m_getter)(Client* client, int index);
	bool m_passbyIndex;
};