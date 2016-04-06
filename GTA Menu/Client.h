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
	uint32_t GetCurrentItemIndex();
	~Client();

	UIMenu* CurrentMenu;
	UIMenu* PreviousMenu;
	UIMenu* BaseMenu;
	std::vector<UIElement*>::iterator CurrentItem;
	bool GodMode;
};