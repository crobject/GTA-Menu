#pragma once
#include <functional>
#include <string>
#include "HudElem.h"

typedef std::function<void(void* param)> OptionCallback;
class MenuOption
{
public:
	MenuOption(std::string title, std::string description, OptionCallback cb);
	virtual void OnHover() {}
	virtual void OnLeave() {}
	virtual void Call();
	virtual void Render();
	~MenuOption();
	OptionCallback Callback;
	std::string Text;
	std::string Description;
	HudText Elem;
	uint32_t Position;
};
