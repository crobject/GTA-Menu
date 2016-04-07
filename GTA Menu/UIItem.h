#pragma once
#include <functional>

#include "Client.h"
#include "Config.h"

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
	std::string GetText() { return Text + (std::string)(*m_toggle ? "[On]" : "[Off]"); };
	bool* GetToggle() { return m_toggle; }
	~UIItemToggle();
private:
	bool* m_toggle;
};

class UIItemSubMenu : public UIItem
{
public:
	UIItemSubMenu(std::string title, std::string description, Client* client, MenuCallback createMenu);
	void OnClick();
	~UIItemSubMenu();
private:
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