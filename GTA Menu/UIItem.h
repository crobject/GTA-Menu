#pragma once
#include <functional>

#include "UI.h"
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
	~UIItem();
protected:
	std::string m_description;
	OptionCallback m_fn;
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