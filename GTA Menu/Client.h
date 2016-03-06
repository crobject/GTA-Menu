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
	~Client();
	Menu* CurrentMenu;
	std::vector<MenuOption*>::iterator CurrentItem;
};