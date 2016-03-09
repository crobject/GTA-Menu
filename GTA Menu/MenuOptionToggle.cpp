#include "stdafx.h"
#include "MenuOptionToggle.h"

MenuOptionToggle::MenuOptionToggle(std::string name, bool* toggle, OptionCallback cb)
{
	Text = name;
	m_toggle = toggle;
	Callback = cb;
}

void MenuOptionToggle::Render()
{
	if (m_toggle)
		Elem.SetText(Text + (std::string)(*m_toggle ? "[On]" : "[Off]"), hudelem_color_t(0xFFFFFFFF), MENU_X_POSITION, MENU_Y_POSITION + ((float)Position * MENU_ITEM_DIFFERENCE), DEFAULT_OPTION_SCALE, 4);
	else
		Elem.SetText(Text, hudelem_color_t(0xFFFFFFFF), MENU_X_POSITION, MENU_Y_POSITION + ((float)Position * MENU_ITEM_DIFFERENCE), DEFAULT_OPTION_SCALE, 4);
	Elem.Render();
}

void MenuOptionToggle::Call()
{
	if (m_toggle)
		*m_toggle = !*m_toggle;

	if (Callback)
		Callback(this);
}

MenuOptionToggle::~MenuOptionToggle()
{
}