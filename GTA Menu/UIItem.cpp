#include "stdafx.h"
#include "UIItem.h"


UIItem::UIItem(std::string title, std::string description, std::function<void(void*)> fn)
{
	Caption = title;
	m_description = description;
	m_fn = fn;
	Color = Color_t(255, 255, 255, 255);
}

void UIItem::OnClick()
{
	if (m_fn)
		m_fn(this);
}


UIItem::~UIItem()
{
}

UIItemToggle::UIItemToggle(std::string title, std::string description, std::function<void(void*)> fn, bool* toggle) : UIItem(title, description, fn)
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