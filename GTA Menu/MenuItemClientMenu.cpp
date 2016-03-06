#include "MenuItemClientMenu.h"
#include "stdafx.h"

MenuItemClientMenu::MenuItemClientMenu(std::string name, void (*action)(Client* client, void* param)) : MenuItemSelect(name, action)
{

}

MenuItemClientMenu::MenuItemClientMenu(void)
{
}


MenuItemClientMenu::~MenuItemClientMenu(void)
{

}

void MenuItemClientMenu::OnClick(Client* client, void* param)
{
	if(m_action && client->SelectedClient())
		m_action(client->SelectedClient(), param);
}