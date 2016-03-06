#pragma once
#include <vector>
#include <string>
#include "MenuOption.h"

class Menu
{
public:
	Menu();
	void Render();
	void ToggleMenu();
	void AddOption(MenuOption* option);
	~Menu();

	std::vector<MenuOption*> Options;
	std::string Title;

	HudText* TitleElem;
	HudMaterial* BackgroundElem;
	HudMaterial* ScrollbarElem;
};
