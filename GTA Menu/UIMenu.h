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
	virtual void Call();
	void Draw();
	void ScrollDown();
	void ScrollUp();
	void ScrollRight();
	void ScrollLeft();
	void Add(UIItem* elem);
	UIContainer& GetContainer() { return m_container; }
	std::vector<UIItem*>::iterator GetCurrentItem() { return m_container.GetItems().begin() + m_currentIndex; }
	void SetParent(UIMenu* parent) { m_parent = parent; }
	UIMenu* GetParent() { return m_parent; }
	void Search(std::string filter);
protected:
	const uint32_t m_ItemSize = 30;
	UIText m_title;
	UIText m_caption;
	UIContainer m_container;
	UIRectangle m_scrollbar;
	Point m_position;
	Size_t m_size;
	//std::vector<UIItem*>::iterator m_currentItem;
	uint32_t m_currentIndex;
	std::function<void()> m_onOpen;
	std::function<void()> m_onClose;
	UIMenu* m_parent;
	uint32_t m_pageNum;
	std::string m_filter;
};

template <class T>
class UIMenuStorage : public UIMenu
{
public:
	UIMenuStorage(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void()> onOpen, std::function<void()> onClose, T item) : UIMenu(title, caption, position, size, onOpen, onClose)
	{
		m_item = item;
	}
	T GetItem() { return m_item; }
private:
	T m_item;
};

class UIMenuRGB : public UIMenu
{
public:
	UIMenuRGB(const UIText& title, const UIText& caption, Point position, Size_t size, std::function<void(uint32_t* param)> onUpdate, std::function<void()> onOpen, std::function<void()> onClose, uint32_t* start);
	void Draw();
private:
	uint32_t m_color[3];
	std::function<void(uint32_t* param)> m_onUpdate;
};