#include "stdafx.h"
#include "UIItem.h"
#include "UIMenu.h"

UIItem::UIItem(std::string title, std::string description, OptionCallback fn) : UIText(title, Point(0, 0), DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255))
{
	m_fn = fn;
}

void UIItem::OnClick()
{
	if (m_fn)
		m_fn((void*)this);
}

void UIItem::SetParent(UIMenu* parent) 
{ 
	m_parent = parent; 
}

UIMenu* UIItem::GetParent() 
{
	return m_parent; 
}

UIItem::~UIItem()
{
}

UIItemToggle::UIItemToggle(std::string title, std::string description, OptionCallback fn, bool* toggle) : UIItem(title, description, fn)
{
	m_toggle = toggle;
}

void UIItemToggle::OnClick()
{
	if (m_toggle)
		*m_toggle = !*m_toggle;
	UIItem::OnClick();
}

UIItemToggle::~UIItemToggle()
{
}

UIItemSubMenu::UIItemSubMenu(std::string title, std::string description, Client* client, MenuCallback createMenu) : UIItem(title, description, [](void*){})
{
	m_client = client;
	m_createMenu = createMenu;
}

void UIItemSubMenu::OnClick()
{
	if(m_client && m_createMenu)
	{
		auto menu = m_createMenu();
		menu->SetParent(m_parent);
		m_client->SetMenu(menu);
	}
}

UIItemSubMenu::~UIItemSubMenu()
{

}

UIItemSuperSelect::UIItemSuperSelect(std::string title, std::string description) : UIItem(title, description, [](void*) {})
{
	m_selected = false;
}
void UIItemSuperSelect::OnClick()
{
	m_selected = !m_selected;
	for each (auto i in m_parent->GetContainer().GetItems())
	{
		if (UIItemToggle* toggleItem = dynamic_cast< UIItemToggle* >(i))
		{
			*(toggleItem->GetToggle()) = m_selected;
		}
	}
}
UIItemSuperSelect::~UIItemSuperSelect()
{

}