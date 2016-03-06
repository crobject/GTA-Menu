#pragma once
#include "MenuItemObject.h"
const int SCROLLBAR_ITEM_WIDTH = 50;
template <class T>
class MenuItemScrollbar : public MenuItemObject
{
public:
	MenuItemScrollbar(void){}
	MenuItemScrollbar(std::string name, void (*action)(Client* client, void* param), T* data, T incrementSize, T max, T min) : MenuItemObject(name, action)
	{
		m_incrementSize = incrementSize;
		m_data = data;
		m_max = max;
		m_min = min;
	}
	~MenuItemScrollbar(void){}
	void FreeHuds()
	{
		MenuItemObject::FreeHuds();
		if(m_counter)
			delete m_counter;
		if(m_counterBar)
			delete m_counterBar;
		if(m_counterScroller)
			delete m_counterScroller;
	}
	void OnClick(Client* client, void* param)
	{
		if(m_action)
			m_action(client, param);
	}
	void OnScrollRight()
	{
		if(*m_data + m_incrementSize <= m_max)
		{
			*m_data += m_incrementSize;
			T currentData = *(T*)m_data;
			m_counterScroller->Setvalue((float)currentData);
			m_counterBar->Setx(m_counterBar->Getx() + m_incrementSize * (SCROLLBAR_ITEM_WIDTH / m_max));
		}
	}
	void OnScrollLeft()
	{
		if(*m_data - m_incrementSize > m_min)
		{
			*m_data -= m_incrementSize;
			T currentData = *(T*)m_data;
			m_counterScroller->Setvalue((float)currentData);
			m_counterBar->Setx(m_counterBar->Getx() - m_incrementSize * (SCROLLBAR_ITEM_WIDTH / m_max));
		}
	}
	void Render()
	{
		m_counter = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
		m_counterBar = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
		m_counterScroller = new HudElem(m_parent->GetOwner()->GetClientNum(), 0);
		MenuItemObject::Render();
		if(m_counter)
			m_counter->SetMaterial("black", 2, SCROLLBAR_ITEM_WIDTH, m_elem->Getx() + 40 + MENU_WIDTH, m_elem->Gety(), DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0x000000FF));
		if(m_counterBar)
			m_counterBar->SetMaterial("white", 2, 1, m_elem->Getx() + 40 + MENU_WIDTH, m_elem->Gety(), DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0xFFFFFFFF));
		if(m_counterScroller)
		{
			T currentData = *(T*)m_data;
			m_counterScroller->SetValue((float)currentData , m_elem->Getx() + 45 + MENU_WIDTH + (SCROLLBAR_ITEM_WIDTH), m_elem->Gety(), DEFAULT_VERT_ALIGN, DEFAULT_HORZ_ALIGN, hudelem_color_t(0x000000FF));
		}
	}
private:
	T m_incrementSize;
	T m_max;
	T m_min;
	T* m_data;
	HudElem* m_counter;
	HudElem* m_counterBar;
	HudElem* m_counterScroller;
};
