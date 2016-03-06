#include "MenuItemIndexGetter.h"
#include "stdafx.h"

MenuItemIndexGetter::MenuItemIndexGetter(void)
{
}

MenuItemIndexGetter::~MenuItemIndexGetter(void)
{
}

MenuItemIndexGetter::MenuItemIndexGetter(void (*action)(Client* client, void* param), std::string (*getter)(Client* client, int index), bool passbyIndex) : MenuItemObject("", action)
{
	m_getter = getter;
	m_passbyIndex = passbyIndex;
}

void MenuItemIndexGetter::Render()
{
	m_elem = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
	if(m_elem && m_getter)
		m_elem->SetText(m_getter(m_parent->GetOwner(), GetItemPosition()), HE_FONT_OBJECTIVE, 1.1, MENU_X_POSITION + m_parent->GetOwner()->MenuModX(), START_Y_POSITION + (ITEM_DIFFERENCE * (float)(GetItemPosition() % MAX_PAGE_ITEM)) + m_parent->GetOwner()->MenuModY(), DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN,hudelem_color_t(0xFFFFFFFF));
}
void MenuItemIndexGetter::OnClick(Client* client, void* param)
{
	if(m_action)
	{
		if(m_passbyIndex)
			m_action(client, (void*)GetItemPosition());
		else
			m_action(client, (void*)m_getter(client, GetItemPosition()).c_str());
	}
}