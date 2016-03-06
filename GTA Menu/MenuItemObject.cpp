#include "MenuItemObject.h"
#include "stdafx.h"

MenuItemObject::MenuItemObject(void)
{
	m_elem = nullptr;
}

MenuItemObject::MenuItemObject(std::string name, void (*action)(Client* client, void* param))
{
	m_name = name;
	m_action = action;
	m_elem = nullptr;
}

MenuItemObject::~MenuItemObject(void)
{
}

std::string MenuItemObject::GetName()
{
	return m_name;
}

void MenuItemObject::FreeHuds()
{
	delete m_elem;
	m_elem = nullptr;
}

void MenuItemObject::Render()
{
	m_elem = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
	if(m_elem)
		m_elem->SetText(m_name, m_parent->GetOwner()->TextFont(), 1.1, m_parent->GetOwner()->MenuModX() + MENU_X_POSITION, m_parent->GetOwner()->MenuModY() + (ITEM_DIFFERENCE * (float)(GetItemPosition() % MAX_PAGE_ITEM)) + START_Y_POSITION, DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0xFFFFFFFF));
} 

int MenuItemObject::GetItemPosition() const
{
	if(m_parent)
	{
		int count = 0;
		MenuItemObject** object = m_parent->GetItems();
		while(*object)
		{
			if(*object == this)
			{
				return count;
			}
			else
				count++;
			object++;
		}
	}
	return -1;
}

void MenuItemObject::SetParent(Menu* parent)
{
	m_parent = parent;
}

HudElem* MenuItemObject::GetElem()
{
	return m_elem;
}

void MenuItemObject::OnClick(Client* client, void* param)
{
	if(m_action)
		m_action(client, param);
}