#include "MenuItemList.h"
#include "stdafx.h"

MenuItemList::MenuItemList(void)
{
}

MenuItemList::MenuItemList(std::string name, void (*action)(Client* client, void* param), std::vector<std::string>& items, bool callbyIndex) : MenuItemObject(name, action)
{
	m_items = items;
	m_callByIndex = callbyIndex;
	m_currentListItem = m_items.begin();
}

MenuItemList::~MenuItemList(void)
{
}

void MenuItemList::OnScrollLeft()
{
	if(m_currentListItem == m_items.begin())
	{
		m_currentListItem = m_items.end() - 1;
	}
	else
	{
		m_currentListItem--;
	}
	Render();
}

void MenuItemList::OnScrollRight()
{
	if(m_currentListItem == m_items.end() - 1)
	{
		m_currentListItem = m_items.begin();
	}
	else
	{
		m_currentListItem++;
	}
	Render();
}

void MenuItemList::OnClick(Client* client, void* param)
{
	if(m_action)
	{
		if(m_callByIndex)
		{
			int index = std::distance(m_items.begin(), m_currentListItem);
			m_action(client, &index);
		}
		else
		{
			m_action(client, &(*m_currentListItem));
		}
	}
}

void MenuItemList::Render()
{
	std::string fmt_option = m_name + (std::string)"["  + (*m_currentListItem) + (std::string)"]";
	if(!m_elem)
		m_elem = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
	if(m_elem)
		m_elem->SetText(fmt_option, HE_FONT_OBJECTIVE, 1.1, MENU_X_POSITION + m_parent->GetOwner()->MenuModX(), START_Y_POSITION + (ITEM_DIFFERENCE * (float)(GetItemPosition() % MAX_PAGE_ITEM)) + m_parent->GetOwner()->MenuModY(),
			DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0xFFFFFFFF));
}