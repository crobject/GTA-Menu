#include "stdafx.h"
#include "Client.h"
#include "keyboard.h"
#include "inc\natives.h"
#include "UIMenu.h"
#include "UIItem.h"

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
#pragma region Quick Actions
	CurrentMenu->Add(new UIItemSubMenu("Quick Actions", "Very useful and common actions that effect your player", this, [&]()->UIMenu*
	{
		Point title_point = Point(100, 75);
		Point sub_title_point = Point(110, 100);
		auto menu = new UIMenu(UIText("Quick Actions", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});

		menu->Add(new UIItemToggle("God Mode", "Toggle Invincibility", [&](void* param)->void
		{
			PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), GodMode);
		}, &GodMode));

		menu->Add(new UIItem("Spawn T20", "spawn a cool car", [&](void* param)->void
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
		menu->Add(new UIItemSuperSelect("Toggle All", ""));
		menu->Add(new UIItemToggle("Super Grip", "Toggle Super Grip", [&](void* param)->void
		{
			if (!Threads.count(&SuperGrip))
				Threads[&SuperGrip] = new ScriptThread([](ScriptThread* thread)
				{
					auto VehicleHandle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
					if (VehicleHandle)
						VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(VehicleHandle);
				});
		}, &SuperGrip));
		menu->Add(new UIItemToggle("Super Run", "Toggle Super Run", [&](void* param)->void
		{
			if (SuperRun)
				PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 1.49);
			else
				PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 1.00);
		}, &SuperRun));
		menu->Add(new UIItemToggle("Super Jump", "Toggle Super Grip", [&](void* param)->void
		{
			if (!Threads.count(&SuperJump))
				Threads[&SuperJump] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperJump));
		menu->Add(new UIItemToggle("Super Punch", "Toggle Super Grip", [&](void* param)->void
		{
			if (!Threads.count(&SuperPunch))
				Threads[&SuperPunch] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperPunch));
		menu->Add(new UIItem("Give All Weapons", "", [&](void* param)
		{
			uint32_t WeapHashes[] = { 0x99B507EA, 0x678B81B1, 0x4E875F73, 0x958A4A8F, 0x440E4788, 0x84BD7BFD, 0x1B06D571, 0x5EF9FEC4, 0x22D8FE39, 
				0x99AEEB3B, 0x13532244, 0x2BE6766B, 0xEFE7E2DF, 0xBFEFFF6D, 0x83BF0278, 0xAF113F99, 0x9D07F764, 0x7FD62962, 0x1D073A89, 0x7846A318,
				0xE284C527, 0x9D61E50F, 0x3656C8C1, 0x05FC3C11, 0x0C472FE2, 0x33058E22, 0xA284510B, 0x4DD2DC56, 0xB1CA77B1, 0x687652CE, 0x42BF8A85, 
				0x93E220BD, 0x2C3731D9, 0xFDBC8A50, 0x24B17070, 0x060EC506, 0x34A67B97, 0xFDBADCED, 0x23C9F95C, 0x497FACC3, 0xF9E6AA4B, 0x61012683, 
				0xC0A3098D, 0xD205520E, 0xBFD21232, 0x7F229F94, 0x92A27487, 0x083839C4, 0x7F7497E5, 0xA89CB99E, 0x3AABBBAA, 0xC734385A, 0x787F0BB, 
				0x47757124, 0xD04C944D };
			for (int i = 0; i < ARRAYSIZE(WeapHashes); i++)
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), WeapHashes[i], 9999, 1);
		}));
		
		return menu;
	}));	
#pragma endregion
	CurrentMenu->Add(new UIItemSubMenu("Self Menu", "Actions that effect the player using the menu", this, [&]()->UIMenu*
	{
		auto menu = new UIMenu(UIText("Self Menu", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
			UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});

	}));

	CurrentMenu->Add(new UIItemSubMenu("Vehicle Menu", "Spawn, customize, and pimp your vehicles", this, [&]()->UIMenu*
	{
	}));

	CurrentMenu->Add(new UIItemSubMenu("Teleport Menu", "Choose to teleport to common locations as well as save/load your current position", this, [&]()->UIMenu*
	{
	}));

	CurrentMenu->Add(new UIItemSubMenu("Player Menu", "Actions that effect other players in your current session", this, [&]()->UIMenu*
	{
	}));

	CurrentMenu->Add(new UIItemSubMenu("Object Menu", "Spawn objects such as UFOs, Ramps, and Blimps", this, [&]()->UIMenu*
	{
	}));

	CurrentMenu->Add(new UIItemSubMenu("Recovery Menu", "Modify your stats", this, [&]()->UIMenu*
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