#pragma once
#include "Menu.h"
#include "UIMenu.h"

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
	void SetMenu(UIMenu* menu);
	uint32_t GetCurrentItemIndex();
	~Client();

	UIMenu* CurrentMenu;
	UIMenu* PreviousMenu;
	UIMenu* BaseMenu;
	bool GodMode;
};