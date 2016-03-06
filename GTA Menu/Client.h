#pragma once
#include "Menu.h"

class Client
{
public:
	Client();
	void RunThreads();
	void ProcessInput();
	void InitializeMenu();
	void ToggleMenu();

	void ScrollDown();
	void ScrollUp();
	uint32_t GetCurrentItemIndex();
	~Client();

	Menu* CurrentMenu;
	Menu* PreviousMenu;
	Menu* BaseMenu;
	std::vector<MenuOption*>::iterator CurrentItem;
	bool GodMode;
};