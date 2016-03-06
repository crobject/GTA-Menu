#pragma once
#include "stdafx.h"
#include "MenuItemObject.h"
#include <functional>

class MenuItemToggle : public MenuItemObject
{
public:
	MenuItemToggle(void);
	MenuItemToggle(std::string name, bool* toggle, void (*action)(Client* client, void* param) = nullptr);
	MenuItemToggle(std::string name, std::function<bool (Client* client)> getToggle, void (*action)(Client* client, void* param));
	~MenuItemToggle(void);

	void OnClick(Client* client, void* param);
	void Render();
	std::string GetName();
private:
	bool* m_toggle;
	std::function<bool (Client* client)> m_getToggle;
};
