#pragma once
#include <string>
#include <vector>
#include <functional>

#include "UIItem.h"

class UIMenu
{
public:
	UIMenu();
	UIMenu(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose);
	~UIMenu();
	void Draw();
	void Add(UIItem* elem);
private:
	UIText m_title;
	UIText m_caption;
	UIContainer m_container;
	Point m_position;
	Size_t m_size;

	std::function<void()> m_onOpen;
	std::function<void()> m_onClose;
};

