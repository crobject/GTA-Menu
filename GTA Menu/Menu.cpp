#include "stdafx.h"
#include "Menu.h"
#include "Config.h"
#include "inc\natives.h"
Menu::Menu()
{
	TitleElem = new HudText();
	BackgroundElem = new HudMaterial();
}

void Menu::Render()
{
	TitleElem->SetText("I am a title", 0xFFFFFFFF, MENU_X_POSITION, MENU_Y_POSITION + MENU_TITLE_DISTANCE, DEFAULT_TITLE_SCALE, 7);
	BackgroundElem->SetMaterial(MENU_X_POSITION, (float)(MENU_ITEM_DIFFERENCE * min(Options.size(), MAX_MENU_PAGE_ITEMS) / 2) + MENU_SHADER_OFFSET, MENU_X_POSITION, (float)(MENU_ITEM_DIFFERENCE * min(Options.size(), MAX_MENU_PAGE_ITEMS)), hudelem_color_t(15, 15, 15, 220));

	for each (auto var in Options)
	{
		var->Render();
	}

	TitleElem->Render();
	BackgroundElem->Render();
	//GRAPHICS::DRAW_RECT(0.845f, (((16 * 0.035f) / 2) + 0.159f), 0.23f, (16 * 0.035f), 15, 15, 15, 220);
}

void Menu::ToggleMenu()
{
}

void Menu::AddOption(MenuOption * option)
{
	option->Position = Options.size();
	Options.push_back(option);
}

Menu::~Menu()
{
}