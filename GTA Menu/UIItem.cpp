#include "stdafx.h"
#include "UIItem.h"
#include "UIMenu.h"
#include "Menu Utils.h"

UIItem::UIItem(std::string title, std::string description, OptionCallback fn) : UIText(title, Point(0, 0), DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255))
{
	m_fn = fn;
	m_description = description;
}

void UIItem::OnClick()
{
	if (m_fn)
		m_fn((void*)this);
}

void UIItem::SetParent(UIMenu* parent)
{
	m_parent = parent;
}

UIMenu* UIItem::GetParent()
{
	return m_parent;
}

UIItem::~UIItem()
{
}

UIItemToggle::UIItemToggle(std::string title, std::string description, OptionCallback fn, bool* toggle) : UIItem(title, description, fn)
{
	m_toggle = toggle;	
}

void UIItemToggle::OnClick()
{
	if (m_toggle)
		*m_toggle = !*m_toggle;
	UIItem::OnClick();
}

void UIItemToggle::Draw()
{
	UIItem::Draw();
	if (m_toggle)
	{
		auto pos = Position;
		pos.m_x += (m_parent->GetContainer().GetSize().m_width) - 5;
		pos.m_y -= 5;
		UISprite("commonmenu", *m_toggle ? "shop_box_tick" : "shop_box_blank", Size_t(50, 50), pos, Color_t(255, 255, 255, 255)).Draw();
	}
}

UIItemToggle::~UIItemToggle()
{
}

UIItemSubMenu::UIItemSubMenu(std::string title, std::string description, Client* client, MenuCallback createMenu) : UIItem(title, description, [](void*) {})
{
	m_client = client;
	m_createMenu = createMenu;
}

void UIItemSubMenu::OnClick()
{
	if (m_client && m_createMenu)
	{
		auto menu = m_createMenu();
		menu->SetParent(m_parent);
		m_client->SetMenu(menu);
	}
}

UIItemSubMenu::~UIItemSubMenu()
{
}

UIItemSuperSelect::UIItemSuperSelect(std::string title, std::string description) : UIItem(title, description, [](void*) {})
{
	m_selected = false;
}
void UIItemSuperSelect::OnClick()
{
	m_selected = !m_selected;
	for each (auto i in m_parent->GetContainer().GetItems())
	{
		if (UIItemToggle* toggleItem = dynamic_cast<UIItemToggle*>(i))
		{
			*(toggleItem->GetToggle()) = m_selected;
		}
	}
}
UIItemSuperSelect::~UIItemSuperSelect()
{
}

UIItemList::UIItemList(std::string title, std::string description, OptionCallback cb, std::vector<std::string> items, bool passByIndex) : UIItem(title, description, cb)
{
	m_callByIndex = passByIndex;
	m_list = std::vector<std::string>(items);
	m_currentItem = m_list.begin();
}
void UIItemList::OnClick()
{
	if (m_callByIndex)
		m_fn((void*)std::distance(m_list.begin(), m_currentItem));
	else
		m_fn((void*)&(*m_currentItem));
}
void UIItemList::OnRightScroll()
{
	if (m_currentItem != m_list.end() - 1)
		m_currentItem++;
	else
		m_currentItem = m_list.begin();
}
void UIItemList::OnLeftScroll()
{
	if (m_currentItem == m_list.begin())
		m_currentItem = m_list.end() - 1;
	else
		m_currentItem--;
}

UIItemList::~UIItemList()
{
}
void UIItemDisplayObjectMenu::Draw()
{	
	UIItemAdditionalInformation<Object>::Draw();
	if ((UIItemDisplayObjectMenu*)(*m_parent->GetCurrentItem()) == this)
	{
		m_infoContainer.Draw();
		auto pnt = m_parent->GetContainer().GetPosition();
		pnt.m_x += (m_parent->GetContainer().GetSize().m_width * 2) + 30;
		pnt.m_y += 10;
		auto pos = ENTITY::GET_ENTITY_COORDS(m_item, FALSE);
		UIText(std::string("X: ") + std::to_string(static_cast<int>(pos.x)) + std::string(" Y: ") + std::to_string(static_cast<int>(pos.y)) + std::string(" Z : ") + std::to_string(static_cast<int>(pos.z)),
			pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		UIText(std::string("Health: ") + std::to_string(ENTITY::GET_ENTITY_HEALTH(m_item)) , pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		UIText(std::string("On Screen: ") + std::string(ENTITY::IS_ENTITY_ON_SCREEN(m_item) ? "True" : "False"), pnt, DEFAULT_FONT_SCALE, Color_t(255,255,255,255)).Draw();
	}
}

void UIItemDisplayPlayerMenu::Draw()
{
	UIItemAdditionalInformation<Player>::Draw();
	if ((UIItemDisplayPlayerMenu*)(*m_parent->GetCurrentItem()) == this)
	{
		m_infoContainer.Draw();
		auto pnt = m_parent->GetContainer().GetPosition();
		pnt.m_x += (m_parent->GetContainer().GetSize().m_width * 2) + 30;
		pnt.m_y += 10;
		auto pos = ENTITY::GET_ENTITY_COORDS(m_item, FALSE);
		UIText(std::string("X: ") + std::to_string(static_cast<int>(pos.x)) + std::string(" Y: ") + std::to_string(static_cast<int>(pos.y)) + std::string(" Z : ") + std::to_string(static_cast<int>(pos.z)),
			pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		UIText(std::string("Money: ") + std::to_string(PED::GET_PED_MONEY(m_item)), pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		UIText(std::string("Health: ") + std::to_string(ENTITY::GET_ENTITY_HEALTH(m_item)), pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		auto veh = PED::GET_VEHICLE_PED_IS_IN(m_item, FALSE);
		UIText(std::string("Vehicle: ") + std::string(veh ? GetVehicleModel(veh) : "None"), pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
	}

}

void UIItemDisplayPedMenu::Draw()
{

	UIItemAdditionalInformation<Ped>::Draw();
	if ((UIItemDisplayPedMenu*)(*m_parent->GetCurrentItem()) == this)
	{
		auto pnt = m_parent->GetContainer().GetPosition();
		pnt.m_x += (m_parent->GetContainer().GetSize().m_width * 2) + 30;
		pnt.m_y += 10;
		auto pos = ENTITY::GET_ENTITY_COORDS(m_item, FALSE);
		UIText(std::string("X: ") + std::to_string(static_cast<int>(pos.x)) + std::string(" Y: ") + std::to_string(static_cast<int>(pos.y)) + std::string(" Z : ") + std::to_string(static_cast<int>(pos.z)),
			pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		UIText(std::string("Money: ") + std::to_string(PED::GET_PED_MONEY(m_item)), pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		UIText(std::string("Health: ") + std::to_string(ENTITY::GET_ENTITY_HEALTH(m_item)), pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
		pnt.m_y += 20;
		auto veh = PED::GET_VEHICLE_PED_IS_IN(m_item, FALSE);
		UIText(std::string("Vehicle: ") + std::string(veh ? GetVehicleModel(veh) : "None"), pnt, DEFAULT_FONT_SCALE, Color_t(255, 255, 255, 255)).Draw();
	}
}