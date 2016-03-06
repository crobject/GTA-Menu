#include "MenuItemToggle.h"
#include "stdafx.h"

MenuItemToggle::MenuItemToggle(void)
{
}

MenuItemToggle::MenuItemToggle(std::string name, bool* toggle, void (*action)(Client* client, void* param)) : MenuItemObject(name, action)
{
	m_toggle = toggle;
	m_getToggle = nullptr;
}

MenuItemToggle::MenuItemToggle(std::string name, std::function<bool (Client* client)> getToggle, void (*action)(Client* client, void* param)) : MenuItemObject(name, action)
{
	m_getToggle = getToggle;
	m_toggle = nullptr;
}

MenuItemToggle::~MenuItemToggle(void)
{
}

void MenuItemToggle::Render()
{
	m_elem = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
	if(m_elem)
		m_elem->SetText(GetName(), HE_FONT_OBJECTIVE, 1.1, MENU_X_POSITION + m_parent->GetOwner()->MenuModX(), START_Y_POSITION + (ITEM_DIFFERENCE * (float)(GetItemPosition() % MAX_PAGE_ITEM)) + m_parent->GetOwner()->MenuModY(),
			DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0xFFFFFFFF));
}
void MenuItemToggle::OnClick(Client* client, void* param)
{
	if(m_toggle)
		*m_toggle = !*m_toggle;
	if(m_action)
		m_action(client, param);
	FreeHuds();
	Render();
}

std::string MenuItemToggle::GetName()
{
	if(m_getToggle)
	{
		return m_name + std::string(" [") + (m_getToggle(m_parent->GetOwner()) ? "^3On" : "^1Off") + std::string("^7]");
	}
	else if(m_toggle)
		return m_name + std::string(" [") + (*m_toggle ? "^3On" : "^1Off") + std::string("^7]");
	else
		return "";
}