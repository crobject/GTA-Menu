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
	m_container = UIPagedContainer(position, size, Color_t(0, 0, 0, 120));
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
	m_container.Draw(m_pageNum);
}

void UIMenu::ScrollDown()
{
	if (m_currentItem != m_container.GetItems().end() - 1)
		m_currentItem++;
	else
		m_currentItem = m_container.GetItems().begin();
	auto dist = std::distance(m_container.GetItems().begin(), m_currentItem);
	m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * (dist + 1))));
}

void UIMenu::ScrollUp()
{
	if (m_currentItem != m_container.GetItems().begin())
		m_currentItem--;
	else
		m_currentItem = m_container.GetItems().end() - 1;
	auto dist = std::distance(m_container.GetItems().begin(), m_currentItem);
	m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * (dist + 1))));

}

void UIMenu::PageRight()
{
	if (m_pageNum < m_container.GetPageCount())
	{
		m_pageNum++;
		auto begin = m_container.GetItems().begin() + (m_pageNum * m_container.GetPageSize());
		m_currentItem = begin;
	}

}
void UIMenu::PageLeft()
{
	if (m_pageNum > 0)
	{
		m_pageNum--;
		auto begin = m_container.GetItems().begin() + (m_pageNum * m_container.GetPageSize());
		m_currentItem = begin;
	}
}

void UIMenu::Add(UIItem* elem)
{
	elem->SetPosition(Point(m_position.m_x, m_position.m_y + 10 + (m_ItemSize * ((m_container.GetItems().size() % m_container.GetPageSize()) + 1))));
	elem->SetParent(this);
	m_container.AddItem(elem);
	m_currentItem = m_container.GetItems().begin();//stupid hack 
}

UIMenu::~UIMenu()
{
}