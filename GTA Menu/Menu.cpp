#include "stdafx.h"
#include "Menu.h"
#include "Config.h"
#include "inc\natives.h"
#include <algorithm>
#include "Client.h"
#include "UI.h"

Menu::Menu(std::string title, Client* owner)
{
	Title = title;
	Owner = owner;
	TitleElem = UIText(title, Point(100, 50), DEFAULT_TITLE_SCALE, Color_t(0xFFFFFFFF), HouseScript, true);
	BackgroundElem = UIContainer(Point(100, 60), Size_t(200, 500), Color_t(0xFFFFFFFF));
	ScrollbarElem = UIRectangle(Point(100, 60), Size_t(200, 20), Color_t(255, 0,0, 130));
}

void Menu::Render()
{
	//TitleElem->SetText(Title, 0xFFFFFFFF, MENU_X_POSITION, MENU_Y_POSITION + MENU_TITLE_DISTANCE, DEFAULT_TITLE_SCALE, 7);
	BackgroundElem.Draw();
	TitleElem.Draw();
	ScrollbarElem.Draw();
	//BackgroundElem->SetMaterial(0.845f, 0.159f, 0.23f, (16 * 0.035f), hudelem_color_t(15, 15, 15, 220));//MENU_X_POSITION, (float)(MENU_ITEM_DIFFERENCE * min(Options.size(), MAX_MENU_PAGE_ITEMS) / 2) + MENU_SHADER_OFFSET, MENU_X_POSITION, (float)(MENU_ITEM_DIFFERENCE * min(Options.size(), MAX_MENU_PAGE_ITEMS)), hudelem_color_t(15, 15, 15, 220));
//	ScrollbarElem->SetMaterial(0.845f, (Owner->GetCurrentItemIndex() * 0.035f) + 0.159f, BackgroundElem->m_width, 0.035f, hudelem_color_t(200, 15, 15, 120));

	for each (auto var in Options)
	{
		var->Render();
	}

	//TitleElem->Render();
	//BackgroundElem->Render();
	//ScrollbarElem->Render();
	//GRAPHICS::DRAW_RECT(0.845f, (((16 * 0.035f) / 2) + 0.159f), 0.23f, (16 * 0.035f), 15, 15, 15, 220);
}

void Menu::ToggleMenu()
{
}

void Menu::AddOption(MenuOption* option)
{
	option->Position = static_cast<uint32_t>(Options.size());
	option->Parent = this;
	option->Elem.Position.m_x = BackgroundElem.Position.m_x;
	option->Elem.Position.m_y = BackgroundElem.Position.m_y + (option->Position * 20);
	Options.push_back(option);
	BackgroundElem.AddItem(&option->Elem);
}

Menu::~Menu()
{
}