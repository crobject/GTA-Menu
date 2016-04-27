#include "stdafx.h"
#include "UIMenu.h"
#include <algorithm>
#include "UIItem.h"

UIMenu::UIMenu()
{
}

UIMenu::UIMenu(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose)
{
	m_currentIndex = 0;
	m_container = UIContainer(position, size, Color_t(0, 0, 0, 155));
	auto p = position;
	p.m_y += m_container.GetLogo().GetSize().m_height + m_container.GetDescriptionBar().GetSize().m_height;
	m_scrollbar = UIRectangle(p, Size_t(size.m_width, 30), Color_t(255, 255, 255, 255));
	m_title = title;
	m_caption = caption;
	m_position = position;
	m_size = size;
	m_onOpen = onOpen;
	m_onClose = onClose;
	//remove previous filter
	Search("");
}

void UIMenu::Draw()
{
	if (m_currentIndex > m_container.GetItems().size())
		m_currentIndex = 0;
	m_scrollbar.Draw();
	m_title.Draw();
	m_caption.Draw();
	m_container.Draw(GetCurrentItem(), m_filter);
}

void UIMenu::Call()
{
	if (*GetCurrentItem())
		(*GetCurrentItem())->OnClick();
}

void UIMenu::ScrollDown()
{
	if (GetCurrentItem() != m_container.GetItems().end() - 1)
		m_currentIndex++;
	else
		m_currentIndex = 0;
	auto dist = std::distance(m_container.GetItems().begin(), GetCurrentItem());
	auto sizeOffset = m_container.GetLogo().GetSize().m_height + m_container.GetDescriptionBar().GetSize().m_height;
	if (dist > 14)
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + sizeOffset + (m_ItemSize * (14))));
	else
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + sizeOffset +  (m_ItemSize * (dist))));
}

void UIMenu::ScrollUp()
{
	auto sizeOffset = m_container.GetLogo().GetSize().m_height + m_container.GetDescriptionBar().GetSize().m_height;

	if (GetCurrentItem() != m_container.GetItems().begin())
		m_currentIndex--;
	else
		m_currentIndex = m_container.GetItems().size();
	auto dist = std::distance(m_container.GetItems().begin(), GetCurrentItem());
	if (dist > 14)
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + sizeOffset + (m_ItemSize * (14 ))));
	else
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + sizeOffset + (m_ItemSize * (dist ))));
}

void UIMenu::ScrollRight()
{
	(*GetCurrentItem())->OnRightScroll();
}

void UIMenu::ScrollLeft()
{
	(*GetCurrentItem())->OnLeftScroll();
}
void UIMenu::Add(UIItem* elem)
{
	elem->SetParent(this);
	m_container.AddItem(elem);	
}

void UIMenu::Search(std::string filter)
{
	m_container.SetFilter(filter);
}

UIMenu::~UIMenu()
{
	for each(auto i in m_container.GetItems())
	{
		if (i)
			delete i;
	}
}