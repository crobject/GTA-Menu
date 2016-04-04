#pragma once
#include <vector>
#include <string>
#include "MenuOption.h"
#include "MenuOptionSubmenu.h"
#include "MenuOptionToggle.h"
#include "UI.h"

class Client;

class Menu
{
public:
	Menu(std::string title, Client* owner);
	void Render();
	void ToggleMenu();
	void AddOption(MenuOption* option);
	~Menu();

	std::vector<MenuOption*> Options;
	std::string Title;

	Client* Owner;

	UIText TitleElem;
	UIContainer BackgroundElem;
	UIRectangle ScrollbarElem;
};
