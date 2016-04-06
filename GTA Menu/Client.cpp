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
		if (CurrentMenu)
		{
			auto currentItem = CurrentMenu->GetCurrentItem();
			if (*currentItem)
				((UIItem*)(*currentItem))->OnClick();
		}
	}
	if (IsKeyJustUp(VK_NUMPAD2))
	{
		if (CurrentMenu)
			CurrentMenu->ScrollDown();
	}
	if (IsKeyJustUp(VK_NUMPAD8))
	{
		if (CurrentMenu)
			CurrentMenu->ScrollUp();
	}
	if (IsKeyJustUp(VK_DELETE) && IsKeyJustUp(VK_END))
	{
		TERMINATE();
	}
}

void Client::InitializeMenu()
{
	Point title_point = Point(100, 75);
	Point sub_title_point = Point(110, 100);
	CurrentMenu = BaseMenu = new UIMenu(UIText("Main Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
		UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
	CurrentItem = CurrentMenu->GetContainer().GetItems().begin();

	CurrentMenu->Add(new UIItem("Quick Actions", "Very useful and common actions that effect your player", [&](void* param)->void
	{
	}));
	CurrentMenu->Add(new UIItemToggle("God Mode", "Toggle Invincibility", [&](void* param)->void
	{
		PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_PED_ID(), GodMode);
	}, &GodMode));

	CurrentMenu->Add(new UIItem("Spawn T20", "spawn a cool car", [&](void* param)->void
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

	/*CurrentMenu->AddOption(new MenuOption("Self Menu", "Actions that effect the player using the menu", [&](void* param)->void
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
	}));*/
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