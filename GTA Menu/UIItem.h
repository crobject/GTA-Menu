#pragma once
#include <functional>

#include "Client.h"
#include "Config.h"
#include <list>
#include "inc\natives.h"

class UIMenu;
typedef std::function<UIMenu*(void)> MenuCallback;
typedef std::function<void(void* param)> OptionCallback;
class UIItem : public UIText
{
public:
	UIItem(std::string title, std::string description, OptionCallback fn);
	virtual void OnClick();
	virtual void OnHover() {}
	virtual void OnLeave() {}
	virtual void OnRightScroll() {}
	virtual void OnLeftScroll() {}
	void SetParent(UIMenu* parent);
	std::string& GetDescription() { return m_description; }
	UIMenu* GetParent();
	~UIItem();
protected:
	std::string m_description;
	OptionCallback m_fn;
	UIMenu* m_parent;
};

class UIItemToggle : public UIItem
{
public:
	UIItemToggle(std::string title, std::string description, OptionCallback fn, bool* toggle);
	void OnClick();
	//std::string GetText() { return Text + (std::string)(*m_toggle ? "[On]" : "[Off]"); };
	bool* GetToggle() { return m_toggle; }
	void UIItemToggle::Draw();
	~UIItemToggle();
private:
	bool* m_toggle;
	UISprite m_sprite;
};

class UIItemSubMenu : public UIItem
{
public:
	UIItemSubMenu(std::string title, std::string description, Client* client, MenuCallback createMenu);
	void OnClick();
	~UIItemSubMenu();
protected:
	Client* m_client;
	MenuCallback m_createMenu;
};

class UIItemSuperSelect : public UIItem
{
public:
	UIItemSuperSelect(std::string title, std::string description);
	void OnClick();
	std::string GetText() { return Text + (std::string)(m_selected ? "[On]" : "[Off]"); };
	~UIItemSuperSelect();
private:
	bool m_selected;
};

class UIItemList : public UIItem
{
public:
	UIItemList(std::string title, std::string description, OptionCallback cb, std::vector<std::string> items, bool passByIndex = true);
	void OnClick();
	std::string GetText() { return Text + (std::string)"<" + *m_currentItem + (std::string)">"; }
	void OnRightScroll();
	void OnLeftScroll();
	~UIItemList();
private:
	std::vector<std::string> m_list;
	std::vector<std::string>::iterator m_currentItem;
	bool m_callByIndex;
};

#include "UIMenu.h"
template <class T>
class UIItemAdditionalInformation : public UIItemSubMenu
{
public:
	UIItemAdditionalInformation(std::string title, std::string description, Client* client, MenuCallback createMenu, T item) : UIItemSubMenu(title,description, client, createMenu), m_item(item)
	{}
	virtual void Draw()
	{
		auto pnt = m_parent->GetContainer().GetPosition();
		pnt.m_x += (m_parent->GetContainer().GetSize().m_height / 2) + 30;
		m_infoContainer = UIRectangle(pnt, Size_t(200, 200), Color_t(0, 0, 0, 120));
		UIItemSubMenu::Draw();
	}
protected:
	T m_item;
	UIRectangle m_infoContainer;
};

class UIItemDisplayObjectMenu : public UIItemAdditionalInformation<Object>
{
public:
	UIItemDisplayObjectMenu(std::string title, std::string description, Client* client, MenuCallback createMenu, Object item) :
		UIItemAdditionalInformation<Object>(title, description, client, createMenu, item)
	{}
	void Draw();
};

class UIItemDisplayPlayerMenu : public UIItemAdditionalInformation<Ped>
{
public:
	UIItemDisplayPlayerMenu(std::string title, std::string description, Client* client, MenuCallback createMenu, Ped item) :
		UIItemAdditionalInformation<Ped>(title, description, client, createMenu, item)
	{}
	void Draw();
};