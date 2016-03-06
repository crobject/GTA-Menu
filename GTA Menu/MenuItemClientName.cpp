#include "MenuItemClientName.h"
#include "stdafx.h"

MenuItemClientName::MenuItemClientName(void)
{
}

MenuItemClientName::MenuItemClientName(int clientNum, std::function<Menu* (Client* client)> loadSubMenu) : MenuItemObject("", NULL)
{
	m_loadSubMenu= loadSubMenu;
	m_clientNum = clientNum;
}

MenuItemClientName::~MenuItemClientName(void)
{
}

void MenuItemClientName::Render()
{
	m_elem = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
	if (m_elem)
		m_elem->SetPlayerNameString(m_clientNum, HE_FONT_OBJECTIVE, 1.1, MENU_X_POSITION + m_parent->GetOwner()->MenuModX(), START_Y_POSITION + (ITEM_DIFFERENCE * (float)(GetItemPosition() % MAX_PAGE_ITEM)) + m_parent->GetOwner()->MenuModY(), DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0xFFFFFFFF));
}

void MenuItemClientName::OnClick(Client* client, void* param)
{
	if(m_loadSubMenu)
	{
		Client* selectedClient = g_clients[m_clientNum];
		if(!selectedClient)
			return;
		Menu* m = m_loadSubMenu(client);
		if(m)
		{
			client->RenderSubMenu(m);
			client->CurrentMenu()->GetTitle()->SetPlayerNameString(m_clientNum, HE_FONT_BIG, 2.0, MENU_X_POSITION, START_Y_POSITION + MENU_TITLE_DISTANCE, DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0xFFFFFFFF));
			client->SelectedClient(selectedClient);
		}
	}
}