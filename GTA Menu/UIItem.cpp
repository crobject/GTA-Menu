#include "stdafx.h"
#include "UIItem.h"

UIItem::UIItem(std::string title, std::string description, OptionCallback fn) : UIText(title, Point(0, 0), DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255))
{
	m_fn = fn;
}

void UIItem::OnClick()
{
	if (m_fn)
		m_fn((void*)this);
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

UIItemSubMenu::UIItemSubMenu(std::string title, std::string description, Client* client, std::function<UIMenu*()> createMenu) : UIItem(title, description, []{})
{
	m_client = client;
	m_createMenu = createMenu;
}

void UIItemSubMenu::OnClick()
{
	if(m_client && m_createMenu)
	{
		auto menu = m_createMenu();
		m_menu->SetParent(parent);
		m_client->SetMenu(menu);
	}
}
