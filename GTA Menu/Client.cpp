#include "stdafx.h"
#include "Client.h"
#include "keyboard.h"
#include "inc\natives.h"
#include "UIMenu.h"
#include "UIItem.h"
#include "Lists.h"
#include "Menu Utils.h"

Client::Client()
{
}

void Client::RunThreads()
{
	for each (auto t in Threads)
	{
		if(*t.first)
			t.second->Exec();
	}
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
	if (IsKeyJustUp(VK_NUMPAD9))
	{
		if (CurrentMenu)
			CurrentMenu->PageRight();
	}
	if (IsKeyJustUp(VK_NUMPAD7))
	{
		if (CurrentMenu)
			CurrentMenu->PageLeft();
	}
	if (IsKeyJustUp(VK_DELETE) && IsKeyJustUp(VK_END))
	{
		TERMINATE();
	}
}

void Client::InitializeMenu()
{
	CurrentMenu = BaseMenu = new UIMenu(UIText("Main Menu", Point(100, 75), DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
		UIText("", Point(110, 100), DEFAULT_TITLE_SCALE - 0.75), Point(100, 75), Size_t(200, 500), [] {}, [] {});
#pragma region Quick Actions
	CurrentMenu->Add(new UIItemSubMenu("Quick Actions", "Very useful and common actions that effect your player", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto quick_menu = new UIMenu(UIText("Quick Actions", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});

		quick_menu->Add(new UIItemToggle("God Mode", "Toggle Invincibility", [&](void* param)->void
		{
			PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), GodMode);
		}, &GodMode));

		quick_menu->Add(new UIItem("Spawn T20", "spawn a cool car", [&](void* param)->void
		{
			SpawnVehicle("t20", PLAYER::PLAYER_PED_ID());
		}));
		quick_menu->Add(new UIItemSuperSelect("Toggle All", ""));
		quick_menu->Add(new UIItemToggle("Super Grip", "Toggle Super Grip", [&](void* param)->void
		{
			if (!Threads.count(&SuperGrip))
				Threads[&SuperGrip] = new ScriptThread([](ScriptThread* thread)
				{
					auto VehicleHandle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
					if (VehicleHandle)
						VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(VehicleHandle);
				});
		}, &SuperGrip));
		quick_menu->Add(new UIItemToggle("Super Run", "Toggle Super Run", [&](void* param)->void
		{
			if (SuperRun)
				PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 1.49);
			else
				PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 1.00);
		}, &SuperRun));
		quick_menu->Add(new UIItemToggle("Super Jump", "Toggle Super Grip", [&](void* param)->void
		{
			if (!Threads.count(&SuperJump))
				Threads[&SuperJump] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperJump));
		quick_menu->Add(new UIItemToggle("Super Punch", "Toggle Super Grip", [&](void* param)->void
		{
			if (!Threads.count(&SuperPunch))
				Threads[&SuperPunch] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperPunch));
		quick_menu->Add(new UIItem("Give All Weapons", "", [&](void* param)
		{			
			for (int i = 0; i < ARRAYSIZE(WeapHashes); i++)
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), WeapHashes[i], 9999, 1);
		}));
		
		return quick_menu;
	}));	
#pragma endregion
	CurrentMenu->Add(new UIItemSubMenu("Self Menu", "Actions that effect the player using the menu", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto self_menu = new UIMenu(UIText("Self Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
		return self_menu;
	}));

	CurrentMenu->Add(new UIItemSubMenu("Vehicle Menu", "Spawn, customize, and pimp your vehicles", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto veh_menu = new UIMenu(UIText("Vehicle Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
		veh_menu->Add(new UIItemSubMenu("Vehicle List", "Spawn, customize, and pimp your vehicles", this, [&]()->UIMenu*
		{
			Point title_point = Point(100, 75);
			Point sub_title_point = Point(110, 100);
			auto menu1 = new UIMenu(UIText("Vehicle List", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
				UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
			for each (auto i in vehicleModels)
			{
				menu1->Add(new UIItem(i, "", [&](void* param) {
					auto item = (UIItem*)param;
					SpawnVehicle(item->GetText(), PLAYER::PLAYER_PED_ID());
				}));
			}
			return menu1;
		}));
		return veh_menu;
	}));

	CurrentMenu->Add(new UIItemSubMenu("Teleport Menu", "Choose to teleport to common locations as well as save/load your current position", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto tele_menu = new UIMenu(UIText("Teleport Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
		return tele_menu;
	}));

	CurrentMenu->Add(new UIItemSubMenu("Player Menu", "Actions that effect other players in your current session", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto player_menu = new UIMenu(UIText("Player Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
		return player_menu;

	}));

	CurrentMenu->Add(new UIItemSubMenu("Object Menu", "Spawn objects such as UFOs, Ramps, and Blimps", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto obj_menu = new UIMenu(UIText("Object Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
		return obj_menu;

	}));

	CurrentMenu->Add(new UIItemSubMenu("Recovery Menu", "Modify your stats", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto recovery_menu = new UIMenu(UIText("Recovery Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
		return recovery_menu;

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

void Client::SetMenu(UIMenu* menu)
{
	CurrentMenu = menu;
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