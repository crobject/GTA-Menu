#include "stdafx.h"
#include "MenuOption.h"
#include "Config.h"

MenuOption::MenuOption(std::string title, std::string description, OptionCallback cb)
{
	Text = title;
	Description = description;
	Callback = cb;
}

void MenuOption::Call()
{
	if (Callback)
	{
		Callback(this);
	}
}

void MenuOption::Render()
{
	Elem.SetText(Text, 0xFFFFFFFF, MENU_X_POSITION, MENU_Y_POSITION + ((float)Position * MENU_ITEM_DIFFERENCE), DEFAULT_OPTION_SCALE, 4);
	Elem.Render();
}

MenuOption::~MenuOption()
{
}