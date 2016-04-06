#include "stdafx.h"
#include "UIMenu.h"
#include <algorithm>

UIMenu::UIMenu()
{
}

UIMenu::UIMenu(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose)
{
	m_container = UIContainer(position, size, Color_t(0, 0, 0, 120));
	m_scrollbar = UIRectangle(position, Size_t(size.m_width, 30), Color_t(255, 0, 0, 200));
	m_title = title;
	m_caption = caption;
	m_position = position;
	m_size = size;
	m_onOpen = onOpen;
	m_onClose = onClose;
}

void UIMenu::Draw()
{
	m_title.Draw();
	m_caption.Draw();
	m_scrollbar.Draw();
	m_container.Draw();
}

void UIMenu::ScrollDown()
{
	if (m_currentItem != m_container.GetItems().end())
		m_currentItem++;
	else
		m_currentItem = m_container.GetItems().begin();
	auto dist = std::distance(m_container.GetItems().begin(), m_currentItem);
	m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + (30 * dist + 1)));
}

void UIMenu::ScrollUp()
{
	if (m_currentItem != m_container.GetItems().begin())
		m_currentItem--;
	else
		m_currentItem = m_container.GetItems().end();
	auto dist = std::distance(m_container.GetItems().begin(), m_currentItem);
	m_scrollbar.SetPosition(Point(m_position.m_x, m_position.m_y + (30 * dist + 1)));
}

void UIMenu::Add(UIItem* elem)
{
	elem->SetPosition(Point(m_position.m_x, m_position.m_y + (30 * (m_container.GetItems().size() + 1))));
	m_container.AddItem(elem);
	m_currentItem = m_container.GetItems().begin();//stupid hack 
}

UIMenu::~UIMenu()
{
}