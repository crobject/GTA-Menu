#pragma once
#include <functional>

#include "UI.h"
class UIItem : public UIText
{
public:
	UIItem(std::string title, std::string description, std::function<void(void*)> fn);
	virtual void OnClick();
	virtual void OnHover() {}
	virtual void OnLeave() {}
	virtual void OnRightScroll() {}
	virtual void OnLeftScroll() {}
	virtual std::string GetText() { return Caption; };
	~UIItem();
private:
	std::string m_description;
	std::function<void(void*)> m_fn;
};

class UIItemToggle : public UIItem
{
public:
	UIItemToggle(std::string title, std::string description, std::function<void(void*)> fn, bool* toggle);
	void OnClick();
	std::string GetText() { return Caption + (std::string)(*m_toggle ? "[On]" : "[Off]"); };
	~UIItemToggle();
private:
	bool* m_toggle;
};