#include "stdafx.h"
#include "UIMenu.h"
#include <algorithm>
#include "UIItem.h"

UIMenu::UIMenu()
{
}

UIMenu::UIMenu(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose)
{
	m_pageNum = 0;
	m_container = UIContainer(position, size, Color_t(0, 0, 0, 120));
	auto p = position;
	p.m_y += 40;
	m_scrollbar = UIRectangle(p, Size_t(size.m_width, 30), Color_t(255, 255, 255, 255));
	m_title = title;
	m_caption = caption;
	m_position = position;
	m_size = size;
	m_onOpen = onOpen;
	m_onClose = onClose;
}

void UIMenu::Draw()
{
	m_scrollbar.Draw();
	m_title.Draw();
	m_caption.Draw();
	m_container.Draw(m_currentItem, m_filter);
}

void UIMenu::Call()
{
	if (*m_currentItem)
		(*m_currentItem)->OnClick();
}

void UIMenu::ScrollDown()
{
	if (m_currentItem != m_container.GetItems().end() - 1)
		m_currentItem++;
	else
		m_currentItem = m_container.GetItems().begin();
	auto dist = std::distance(m_container.GetItems().begin(), m_currentItem);
	if (dist > 14)
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * (14 + 1))));
	else
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * (dist + 1))));
}

void UIMenu::ScrollUp()
{
	if (m_currentItem != m_container.GetItems().begin())
		m_currentItem--;
	else
		m_currentItem = m_container.GetItems().end() - 1;
	auto dist = std::distance(m_container.GetItems().begin(), m_currentItem);
	if (dist > 14)
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * (14 + 1))));
	else
		m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * (dist + 1))));

}

void UIMenu::ScrollRight()
{
	(*m_currentItem)->OnRightScroll();
}

void UIMenu::ScrollLeft()
{
	(*m_currentItem)->OnLeftScroll();
}
void UIMenu::Add(UIItem* elem)
{
	elem->SetParent(this);
	m_container.AddItem(elem);
	m_currentItem = m_container.GetItems().begin();//stupid hack 
}

void UIMenu::Search(std::string filter)
{
	m_filter = filter;
}

UIMenu::~UIMenu()
{
	for each(auto i in m_container.GetItems())
	{
		if(i)
			delete i;
	}
}