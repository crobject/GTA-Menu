#pragma once
#include <functional>

#include "UIMenu.h"
#include "Config.h"

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
	void SetParent(UIMenu* parent){ m_parent = parent; }
	UIMenu* GetParent(){ return m_parent; }
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
	~UIItemToggle();
private:
	bool* m_toggle;
};

class UIItemSubMenu : public UIItem
{
public:
	UIItemSubMenu(std::string title, std::string description, Client* client, std::function<UIMenu*()> createMenu);
	void OnClick();
	~UIItemSubMenu();
private:
	Client* m_client;
	std::function<UIMenu*(Client*)> m_createMenu;
};