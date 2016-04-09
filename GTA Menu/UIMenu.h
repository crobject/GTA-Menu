#pragma once
#include <string>
#include <vector>
#include <functional>

#include "UI.h"

class UIItem;
class UIMenu
{
public:
	UIMenu();
	UIMenu(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose);
	~UIMenu();
	void Draw();
	void ScrollDown();
	void ScrollUp();
	void PageRight();
	void PageLeft();
	void Add(UIItem* elem);
	UIPagedContainer& GetContainer() { return m_container; }
	std::vector<UIElement*>::iterator GetCurrentItem() { return m_currentItem; }
	void SetParent(UIMenu* parent) { m_parent = parent; }
private:
	const uint32_t m_ItemSize = 30;
	UIText m_title;
	UIText m_caption;
	UIPagedContainer m_container;
	UIRectangle m_scrollbar;
	Point m_position;
	Size_t m_size;
	std::vector<UIElement*>::iterator m_currentItem;
	std::function<void()> m_onOpen;
	std::function<void()> m_onClose;
	UIMenu* m_parent;
	uint32_t m_pageNum;
};
