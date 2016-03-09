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
		if (CurrentMenu && *CurrentItem)
			(*CurrentItem)->Call();
	}
	if (IsKeyJustUp(VK_NUMPAD2))
	{
		ScrollDown();
	}
	if (IsKeyJustUp(VK_NUMPAD8))
	{
		ScrollUp();
	}
	if (IsKeyJustUp(VK_DELETE) && IsKeyJustUp(VK_END))
	{
		TERMINATE();
	}
}

void Client::InitializeMenu()
{
	CurrentMenu = BaseMenu = new Menu("Main Menu", this);
	CurrentItem = CurrentMenu->Options.begin();

	CurrentMenu->AddOption(new MenuOption("Quick Actions", "Very useful and common actions that effect your player", [&](void* param)->void
	{
		CurrentMenu->AddOption(new MenuOptionToggle("God Mode", &GodMode, [&](void* param)->void
		{
			PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_PED_ID(), GodMode);
		}));

		CurrentMenu->AddOption(new MenuOption("Spawn T20", "spawn a cool car", [&](void* param)->void
		{
			Hash hash = GAMEPLAY::GET_HASH_KEY("t20");
			Vector3 pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);
			STREAMING::REQUEST_MODEL(hash);
			if (STREAMING::HAS_MODEL_LOADED(hash))
			{
				uint32_t VehicleHandle = VEHICLE::CREATE_VEHICLE(hash, pos.x, pos.y, pos.z, 0, FALSE, FALSE);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
			}
		}));
	}));

	CurrentMenu->AddOption(new MenuOption("Self Menu", "Actions that effect the player using the menu", [&](void* param)->void
	{
	}));

	CurrentMenu->AddOption(new MenuOption("Vehicle Menu", "Spawn, customize, and pimp your vehicles", [&](void* param)->void
	{
	}));

	CurrentMenu->AddOption(new MenuOption("Teleport Menu", "Choose to teleport to common locations as well as save/load your current position", [&](void* param)->void
	{
	}));

	CurrentMenu->AddOption(new MenuOption("Player Menu", "Actions that effect other players in your current session", [&](void* param)->void
	{
	}));

	CurrentMenu->AddOption(new MenuOption("Object Menu", "Spawn objects such as UFOs, Ramps, and Blimps", [&](void* param)->void
	{
	}));

	CurrentMenu->AddOption(new MenuOption("Recovery Menu", "Modify your stats", [&](void* param)->void
	{
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

uint32_t Client::GetCurrentItemIndex()
{
	if (CurrentMenu)
		return 0;//return (uint32_t)(CurrentMenu->Options.begin() - CurrentItem) / sizeof(MenuOption*);
}

Client::~Client()
{
}