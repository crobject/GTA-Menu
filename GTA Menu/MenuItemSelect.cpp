#include "MenuItemSelect.h"
#include "stdafx.h"

MenuItemSelect::MenuItemSelect(void)
{
}

MenuItemSelect::MenuItemSelect(std::string name, void (*action)(Client* client, void* param)) : MenuItemObject(name, action)
{
}

MenuItemSelect::~MenuItemSelect(void)
{
}

void MenuItemSelect::OnHoverEnter()
{
}

void MenuItemSelect::OnHoverLeave()
{
}

void MenuItemSelect::OnClick(Client* client, void* param)
{
	if(m_action)
		m_action(client, param);
}