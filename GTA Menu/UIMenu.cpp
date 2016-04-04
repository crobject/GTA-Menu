#include "stdafx.h"
#include "UIMenu.h"

UIMenu::UIMenu()
{
}

UIMenu::UIMenu(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose)
{
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
	m_container.Draw();
}
void UIMenu::Add(UIItem* elem) 
{
	elem->Position.m_x = m_position.m_x;
	elem->Position.m_y = m_position.m_y + (20 * (m_container.Items.size() + 1));
	m_container.AddItem(elem);
}

UIMenu::~UIMenu()
{
}
