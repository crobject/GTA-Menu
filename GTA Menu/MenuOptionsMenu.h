#pragma once
#include "Client.h"
#include "stdafx.h"

namespace options_menu
{
	hudelem_color_t GetColorForNum(int index);
	void SetScrollbar(MENU_OPTION_PARAMS);
	void SetBackgroundColor(MENU_OPTION_PARAMS);
	void MoveRight(MENU_OPTION_PARAMS);
	void MoveLeft(MENU_OPTION_PARAMS);
	void ChangeFont(MENU_OPTION_PARAMS);
}