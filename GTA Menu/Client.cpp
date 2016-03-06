#include "stdafx.h"
#include "Client.h"
#include "keyboard.h"
#include "inc\natives.h"
Client::Client()
{
}

void Client::RunThreads()
{
}

void Client::ProcessInput()
{
	if (IsKeyJustUp(VK_NUMPAD0))
	{
		ToggleMenu();
	}
	if (IsKeyJustUp(VK_NUMPAD5))
	{
	}
	if (IsKeyJustUp(VK_DELETE))
	{
		extern HMODULE currentModule;
		scriptUnregister(currentModule);
		keyboardHandlerUnregister(OnKeyboardMessage);
		extern bool running;
		running = false;
	}
}

void Client::InitializeMenu()
{
	CurrentMenu = new Menu;
	CurrentItem = CurrentMenu->Options.begin();

	CurrentMenu->AddOption(new MenuOption("No Coppers", "Test description", [&](void* param)->void
	{
		PLAYER::SET_POLICE_IGNORE_PLAYER(PLAYER::PLAYER_ID(), TRUE);
	}));

	CurrentMenu->AddOption(new MenuOption("Fast Movement", "Test description", [&](void* param)->void
	{
		PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 2);
	}));

	CurrentMenu->AddOption(new MenuOption("Enable Ability", "Test description", [&](void* param)->void
	{
		PLAYER::ENABLE_SPECIAL_ABILITY(PLAYER::PLAYER_ID(), TRUE);
	}));

	CurrentMenu->AddOption(new MenuOption("Invincible", "Test description", [&](void* param)->void
	{
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), TRUE);
	}));
}

void Client::ToggleMenu()
{
	if (!CurrentMenu)
	{
		InitializeMenu();
	}
	else
	{
		delete CurrentMenu;
		CurrentMenu = nullptr;
	}
}

void Client::ScrollDown()
{
}

void Client::ScrollUp()
{
}

Client::~Client()
{
}