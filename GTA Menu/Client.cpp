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
		if (*t.first)
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
			CurrentMenu->Call();
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
	if (IsKeyJustUp(VK_NUMPAD6))
	{
		if (CurrentMenu)
			CurrentMenu->ScrollRight();
	}
	if (IsKeyJustUp(VK_NUMPAD4))
	{
		if (CurrentMenu)
			CurrentMenu->ScrollLeft();
	}
	if (IsKeyJustUp(VK_NUMPAD1))
	{
		if(CurrentMenu)
			GoToPreviousMenu();
	}
	if (IsKeyJustUp(VK_CONTROL) && IsKeyJustUp('F') && CurrentMenu)
	{
		Keyboard = true;
		GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(1, "FMMC_MPM_NA", "", "", "", "", "", 30);
	}
	if (IsKeyJustUp(VK_DELETE) && IsKeyJustUp(VK_END))
	{
		TERMINATE();
	}
	if (Keyboard)
	{
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
		if (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD())
		{
			char* pResult = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
			if (pResult)
				if (CurrentMenu)
				{
					CurrentMenu->Search(pResult);
					Keyboard = false;
				}
		}
	}
}

void Client::GoToPreviousMenu()
{
	if (CurrentMenu->GetParent())
	{
		auto back = CurrentMenu;
		SetMenu(CurrentMenu->GetParent());
		delete back;
	}
}

UIMenu* Client::SpawnMenu(std::string name, std::string description)
{
	return new UIMenu(UIText(name, Point(100, 75), DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false), UIText(description, Point(110, 100), DEFAULT_TITLE_SCALE - 0.75), Point(100, 75), Size_t(200, 500), [] {}, [] {});	
}

void Client::CheckModel()
{
	//ripped from the native trainer
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!ENTITY::DOES_ENTITY_EXIST(playerPed)) return;

	Hash model = ENTITY::GET_ENTITY_MODEL(playerPed);
	if (ENTITY::IS_ENTITY_DEAD(playerPed) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
		if (model != GAMEPLAY::GET_HASH_KEY("player_zero") &&
			model != GAMEPLAY::GET_HASH_KEY("player_one") &&
			model != GAMEPLAY::GET_HASH_KEY("player_two"))
		{
			WAIT(1000);

			model = GAMEPLAY::GET_HASH_KEY("player_zero");
			STREAMING::REQUEST_MODEL(model);
			while (!STREAMING::HAS_MODEL_LOADED(model))
				WAIT(0);
			PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);

			// wait until player is ressurected
			while (ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
				WAIT(0);

		}	
}

void Client::InitializeMenu()
{
	CurrentMenu = BaseMenu = SpawnMenu("Main Menu");
#pragma region Quick Actions
	CurrentMenu->Add(new UIItemSubMenu("Quick Actions", "Very useful and common actions that effect your player", this, [this]()->UIMenu*
	{
		auto quick_menu = SpawnMenu("Quick Actions");

		quick_menu->Add(new UIItemToggle("God Mode", "Toggle Invincibility", [this](void* param)->void
		{
			PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), GodMode);
		}, &GodMode));

		quick_menu->Add(new UIItem("Spawn T20", "spawn a cool car", [this](void* param)->void
		{
			SpawnVehicle("t20", PLAYER::PLAYER_PED_ID());
		}));
		quick_menu->Add(new UIItemSuperSelect("Toggle All", ""));
		quick_menu->Add(new UIItemToggle("Super Grip", "Toggle Super Grip", [this](void* param)->void
		{
			if (!Threads.count(&SuperGrip))
				Threads[&SuperGrip] = new ScriptThread([](ScriptThread* thread)
			{
				auto VehicleHandle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (VehicleHandle)
					VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(VehicleHandle);
			});
		}, &SuperGrip));
		quick_menu->Add(new UIItem("Give All Weapons", "", [this](void* param)
		{
			for (int i = 0; i < ARRAYSIZE(WeapHashes); i++)
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), WeapHashes[i], 9999, 1);
		}));
		quick_menu->Add(new UIItemToggle("Infinite Ammo", "", [this](void* param)
		{
			WEAPON::SET_PED_INFINITE_AMMO_CLIP(PLAYER::PLAYER_PED_ID(), UnlimitedAmmo);
		}, &UnlimitedAmmo));
		quick_menu->Add(new UIItemToggle("Never Wanted", "", [this](void* param) {
			if (!Threads.count(&NeverWanted))
				Threads[&NeverWanted] = new ScriptThread([](ScriptThread* thread)
			{
				PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(), 0, TRUE);
				PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(PLAYER::PLAYER_ID(), TRUE);
			});
		}, &NeverWanted));
		quick_menu->Add(new UIItemToggle("Invisible Player", "", [this](void* param)
		{
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), !InvisablePlayer);
		}, &InvisablePlayer));
		return quick_menu;
	}));
#pragma endregion
#pragma region Self Menu
	CurrentMenu->Add(new UIItemSubMenu("Self Menu", "Actions that effect the player using the menu", this, [this]()->UIMenu*
	{
		auto self_menu = SpawnMenu("Self Menu");
		self_menu->Add(new UIItemToggle("Fly Mode", "", [this](void* param)
		{
			if (!Threads.count(&Flymode))
				Threads[&Flymode] = new ScriptThread([](ScriptThread* thread)
			{
				if (IsKeyDown(VK_SPACE))
				{
					WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 1);
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0, 65, 50, 0, 0, 0, true, true, true, true, false, false);
				}
			});
		}, &Flymode));
		self_menu->Add(new UIItemToggle("God Mode", "Toggle Invincibility", [this](void* param)->void
		{
			PLAYER::SET_PLAYER_INVINCIBLE(PLAYER::PLAYER_ID(), GodMode);
		}, &GodMode));
		self_menu->Add(new UIItemToggle("Invisible Player", "", [this](void* param)
		{
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), !InvisablePlayer);
		}, &InvisablePlayer));
		self_menu->Add(new UIItem("Clean Player", "", [this](void* param)
		{
			//TODO: this
		}));
		self_menu->Add(new UIItem("Give All Weapons", "", [this](void* param)
		{
			for (int i = 0; i < ARRAYSIZE(WeapHashes); i++)
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), WeapHashes[i], 9999, 1);
		}));

		self_menu->Add(new UIItemToggle("Super Run", "Toggle Super Run", [this](void* param)->void
		{
			if (SuperRun)
				PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 1.49);
			else
				PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), 1.00);
		}, &SuperRun));
		self_menu->Add(new UIItemToggle("Super Jump", "Toggle Super Grip", [this](void* param)->void
		{
			if (!Threads.count(&SuperJump))
				Threads[&SuperJump] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperJump));
		self_menu->Add(new UIItemToggle("Super Punch", "Toggle Super Grip", [this](void* param)->void
		{
			if (!Threads.count(&SuperPunch))
				Threads[&SuperPunch] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperPunch));
		return self_menu;
	}));
#pragma endregion
#pragma region Vehicle Menu
	CurrentMenu->Add(new UIItemSubMenu("Vehicle Menu", "Spawn, customize, and pimp your vehicles", this, [this]()->UIMenu*
	{
		auto veh_menu = SpawnMenu("Vehicle Menu");
		veh_menu->Add(new UIItemSubMenu("Vehicle List", "Spawn, customize, and pimp your vehicles", this, [this]()->UIMenu*
		{
			auto menu1 = SpawnMenu("Vehicle List");
			for each (auto i in vehicleModels)
			{
				menu1->Add(new UIItem(i, "", [this](void* param) {
					auto item = (UIItem*)param;
					SpawnVehicle(item->GetText(), PLAYER::PLAYER_PED_ID());
				}));
			}
			return menu1;
		}));
		veh_menu->Add(new UIItem("Fix Car", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
			{
				VEHICLE::SET_VEHICLE_FIXED(handle);
				VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(handle);
				VEHICLE::SET_VEHICLE_DIRT_LEVEL(handle, 0);
			}
		}));
		veh_menu->Add(new UIItem("Open Doors", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				for (int DoorIndex = 0; DoorIndex <= 6; DoorIndex++)
					VEHICLE::SET_VEHICLE_DOOR_SHUT(handle, DoorIndex, FALSE);
		}));
		veh_menu->Add(new UIItem("Close Doors", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				for (int DoorIndex = 0; DoorIndex <= 6; DoorIndex++)
					VEHICLE::SET_VEHICLE_DOOR_OPEN(handle, DoorIndex, FALSE, FALSE);
		}));
		veh_menu->Add(new UIItem("Bullet Proof Tires", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(handle, TRUE);
		}));
		veh_menu->Add(new UIItemToggle("Rainbow Car", "", [this](void* param)
		{
			if (!Threads.count(&RainbowCar))
				Threads[&RainbowCar] = new ScriptThread([](ScriptThread* thread)
			{
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				int Red = GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255),
					Green = GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255),
					Blue = GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(handle, Red, Green, Blue);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(handle, Red, Green, Blue);
				}
			});
		}, &RainbowCar));
		veh_menu->Add(new UIItem("Flip Car", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(handle);
		}));
		veh_menu->Add(new UIItemToggle("Enable Boost", "", [this](void* param)
		{
			if (!Threads.count(&CarBoost))
				Threads[&CarBoost] = new ScriptThread([](ScriptThread* thread)
			{
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle && IsKeyDown(VK_OEM_PLUS))
				{
					AUDIO::SET_VEHICLE_BOOST_ACTIVE(handle, TRUE);
					auto vel = ENTITY::GET_ENTITY_VELOCITY(handle);
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, vel.x + 50.0f);
					AUDIO::SET_VEHICLE_BOOST_ACTIVE(handle, FALSE);
				}
				else if (handle && IsKeyDown(VK_OEM_MINUS))
				{
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, 0.0f);
				}
			});
		}, &CarBoost));
		veh_menu->Add(new UIItem("Invincible Car", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				ENTITY::SET_ENTITY_INVINCIBLE(handle, TRUE);
		}));
		veh_menu->Add(new UIItemToggle("Drive On Water", "", [this](void* param)
		{
			if (!Threads.count(&DriveWater))
				Threads[&DriveWater] = new ScriptThread([this](ScriptThread* thread)
			{
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					auto pos = ENTITY::GET_ENTITY_COORDS(handle, FALSE);
					ENTITY::SET_ENTITY_COORDS(WaterObject, pos.x, pos.y, 360, TRUE, FALSE, FALSE, TRUE);
					ENTITY::SET_ENTITY_ROTATION(WaterObject, 180, 90, 180, 2, 1);
					ENTITY::SET_ENTITY_VISIBLE(WaterObject, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(WaterObject, TRUE);
				}
			});
			if (DriveWater)
			{
				WaterObject = OBJECT::CREATE_OBJECT(0xC42C019A, 0, 0, 0, TRUE, TRUE, TRUE);
			}
			else
			{
				OBJECT::DELETE_OBJECT(&WaterObject);
			}
		}, &DriveWater));
		veh_menu->Add(new UIItemToggle("Drive in Air", "", [this](void* param)
		{
			if (!Threads.count(&DriveAir))
				Threads[&DriveAir] = new ScriptThread([this](ScriptThread* thread)
			{
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					auto pos = ENTITY::GET_ENTITY_COORDS(handle, FALSE);
					ENTITY::SET_ENTITY_COORDS(WaterObject, pos.x, pos.y, -0.9, TRUE, FALSE, FALSE, TRUE);
					ENTITY::SET_ENTITY_ROTATION(WaterObject, 180, 90, 180, 2, 1);
					ENTITY::SET_ENTITY_VISIBLE(WaterObject, FALSE);
					ENTITY::FREEZE_ENTITY_POSITION(WaterObject, TRUE);
				}
			});
			if (DriveAir)
			{
				AirObject = OBJECT::CREATE_OBJECT(0xC42C019A, 0, 0, 0, TRUE, TRUE, TRUE);
			}
			else
			{
				OBJECT::DELETE_OBJECT(&AirObject);
			}
		}, &DriveAir));
		veh_menu->Add(new UIItemSubMenu("Mod Shop", "Pimp your vehicles", this, [this]()->UIMenu*
		{
			auto menu2 = SpawnMenu("Mod Shop");
			menu2->Add(new UIItemList("Engine", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					if (index == 0)
					{
						VEHICLE::REMOVE_VEHICLE_MOD(handle, 11);
					}
					else
					{
						VEHICLE::SET_VEHICLE_MOD(handle, 11, index - 1, FALSE);
					}
				}
			}, std::vector<std::string>({ "Stock", "Single Cylinder", "Twin Cylinder", "Inline-4", "Inline-6", "V8", "V12" })));
			menu2->Add(new UIItemList("Transmission", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					if (index == 0)
					{
						VEHICLE::REMOVE_VEHICLE_MOD(handle, 13);
					}
					else
					{
						VEHICLE::SET_VEHICLE_MOD(handle, 13, index - 1, FALSE);
					}
				}
			}, std::vector<std::string>({ "Stock", "Street", "Sports", "Race" })));
			menu2->Add(new UIItemList("Brakes", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					if (index == 0)
					{
						VEHICLE::REMOVE_VEHICLE_MOD(handle, 12);
					}
					else
					{
						VEHICLE::SET_VEHICLE_MOD(handle, 12, index - 1, FALSE);
					}
				}
			}, std::vector<std::string>({ "Stock", "Street", "Sports", "Race" })));
			menu2->Add(new UIItemList("Suspension", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					if (index == 0)
					{
						VEHICLE::REMOVE_VEHICLE_MOD(handle, 15);
					}
					else
					{
						VEHICLE::SET_VEHICLE_MOD(handle, 15, index - 1, FALSE);
					}
				}
			}, std::vector<std::string>({ "Stock", "Lowered", "Street", "Sports", "Competition" })));
			menu2->Add(new UIItemList("Highend Wheels", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					VEHICLE::SET_VEHICLE_WHEEL_TYPE(handle, 7);
					VEHICLE::SET_VEHICLE_MOD(handle, 23, 8 + index, FALSE);
				}
			}, std::vector<std::string>({ "GT Chrome", "Cheetah RR", "Shadow", "Hypher", "Blade", "Diamond", "Supa Gee", "Chromatic Z", "Merchie Ch.Lip", "Obey RS", "Solar", "Split Ten", "Dash VIP", "LozSpeed Ten", "Carbon Inferno", "Carbon Shadow", "Carbonic Z", "Carbon Solar", "Cheetah Cobra R", "Carbon S Racer" })));
			menu2->Add(new UIItemList("Lowrider Wheels", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					VEHICLE::SET_VEHICLE_WHEEL_TYPE(handle, 2);
					VEHICLE::SET_VEHICLE_MOD(handle, 23, index, FALSE);
				}
			}, std::vector<std::string>({ "Flare","Wired","Triple Golds","Big Worm","Seven Fives","Split Six","Fresh Mesh","Lead Sled","Turbine","Super Fin","Classic Rod","Dollar","Dukes","Low Five","Gooch" })));
			menu2->Add(new UIItemList("Muscle Wheels", "", [](void* param)
			{
				uint32_t index = (uint32_t)param;
				auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
				if (handle)
				{
					VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);
					VEHICLE::SET_VEHICLE_WHEEL_TYPE(handle, 1);
					VEHICLE::SET_VEHICLE_MOD(handle, 23, index, FALSE);
				}
			}, std::vector<std::string>({ "Classic Five", "Dukes", "Muscle Freak", "Kracka", "Azreal", "Mecha", "Black Top", "Drag SPL", "Revolver", "Classic Rod", "Fairlie", "Spooner", "Five Star", "Old School", "El Jefe", "Dodman", "Six Gun", "Mercenary" })));

			return menu2;
		}));

		return veh_menu;
	}));
#pragma endregion
#pragma region Model Menu
	CurrentMenu->Add(new UIItemSubMenu("Model  Menu", "Change your player model", this, [this]()->UIMenu*
	{
		auto model_menu = SpawnMenu("Model Menu");
		model_menu->Add(new UIItemSubMenu("Model List", "", this, [this]()->UIMenu* 
		{
			auto menu1 = SpawnMenu("Model List");
			for each (auto i in pedModels)
			{
				menu1->Add(new UIItem(i, "", [this](void* param) {
					auto item = (UIItem*)param;
					SetModel(item->GetText());
				}));
			}
			return menu1;
		}));
		return model_menu;
	}));
#pragma endregion
#pragma region Ped Menu
	CurrentMenu->Add(new UIItemSubMenu("Ped Menu", "Spawn, customize, and modify Pedestrians", this, [this]()->UIMenu*
	{
		auto ped_menu = SpawnMenu("Ped Menu");
		ped_menu->Add(new UIItemSubMenu("Ped List", "Spawn Peds", this, [this]()->UIMenu*
		{
			Point title_point = Point(100, 75);
			Point sub_title_point = Point(110, 100);
			auto menu1 = new UIMenu(UIText("Ped List", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
				UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
			for each (auto i in pedModels)
			{
				menu1->Add(new UIItem(i, "", [this](void* param) {
					auto item = (UIItem*)param;
					SpawnPed(item->GetText(), PLAYER::PLAYER_PED_ID());
				}));
			}
			return menu1;
		}));
		ped_menu->Add(new UIItemSubMenu("Nearby Ped List", "Spawn Peds", this, [this]()->UIMenu*
		{
			Point title_point = Point(100, 75);
			Point sub_title_point = Point(110, 100);
			auto menu2 = new UIMenu(UIText("Nearby List", title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
				UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {});
			const uint32_t numElements = 10;
			const uint32_t arrSize = numElements * 2 + 2;  //Start at index 2, and the odd elements are padding
			Ped peds[arrSize];
			//0 index is the size of the array
			peds[0] = numElements;
			uint32_t count = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), peds, -1);
			for (uint32_t i = 0; i < count; i++)
			{
				uint32_t offsettedID = i * 2 + 2;
				auto currentPed = peds[offsettedID];
				auto type = PED::GET_PED_TYPE(currentPed);
				if (ENTITY::DOES_ENTITY_EXIST(currentPed))
					menu2->Add(new UIItem((pedTypes.count(type) ? pedTypes[type] : "Unknown") + (std::string)" - " + GetPedModel(currentPed), "", [](void* param) {}));
			}
			return menu2;
		}));

		return ped_menu;
	}));
#pragma endregion
#pragma region Teleport Menu
	CurrentMenu->Add(new UIItemSubMenu("Teleport Menu", "Choose to teleport to common locations as well as save/load your current position", this, [this]()->UIMenu*
	{

		auto tele_menu = SpawnMenu("Teleport Menu");
		return tele_menu;
	}));
#pragma endregion
#pragma region Player Menu
	CurrentMenu->Add(new UIItemSubMenu("Player Menu", "Actions that effect other players in your current session", this, [this]()->UIMenu*
	{
		auto player_menu = SpawnMenu("Player Menu");
		for (uint32_t i = 0; i < 32; i++)
		{
			player_menu->Add(new UIItemSubMenu(PLAYER::GET_PLAYER_NAME(i), "Options that effect this player", this, [this]()->UIMenu*
			{
				auto currentPlayer = CurrentMenu->GetCurrentItem();
				Point title_point = Point(100, 75);
				Point sub_title_point = Point(110, 100);
				auto selected_player_menu = new UIMenuStorage<Ped>(UIText(((*currentPlayer))->GetText(), title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
					UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {}, PLAYER::GET_PLAYER_PED(std::distance(currentPlayer, CurrentMenu->GetContainer().GetItems().begin())));
				selected_player_menu->Add(new UIItem("Kill Player", "", [](void* param) {
					auto player = ((UIMenuStorage<Ped>*)param);
					PED::EXPLODE_PED_HEAD(player->GetItem(), GAMEPLAY::GET_HASH_KEY("WEAPON_SNIPERRIFLE"));
				}));
				selected_player_menu->Add(new UIItem("Fling Player", "", [](void* param) {
					auto player = ((UIMenuStorage<Ped>*)param);
					ENTITY::APPLY_FORCE_TO_ENTITY(player->GetItem(), 1, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), 0, 0, 0, true, true, true, true, false, false);
				}));
				return selected_player_menu;
			}));
		}
		return player_menu;
	}));
#pragma endregion
#pragma region Object Menu
	CurrentMenu->Add(new UIItemSubMenu("Object Menu", "Spawn objects such as UFOs, Ramps, and Blimps", this, [this]()->UIMenu*
	{
		auto obj_menu = SpawnMenu("Object Menu");
		obj_menu->Add(new UIItemSubMenu("Object Spawner", "Spawn Props", this, [this]()->UIMenu*
		{
			auto menu1 = SpawnMenu("Object Spawner");
			for each (auto i in propNames)
			{
				menu1->Add(new UIItem(i, "", [this](void* param) {
					auto item = (UIItem*)param;
					SpawnProp(item->GetText(), PLAYER::PLAYER_PED_ID(), this);
				}));
			}
			return menu1;
		}));
		obj_menu->Add(new UIItemSubMenu("Spawned Objects", "Spawn Props", this, [this]()->UIMenu*
		{
			auto menu2 = SpawnMenu("Object Spawner");
			for each (auto i in SpawnedObjects)
			{
				menu2->Add(new UIItemSubMenu((std::string)"Object " + std::to_string(i), "", this, [this, i]()->UIMenu* {
					Point title_point = Point(100, 75);
					Point sub_title_point = Point(110, 100);
					auto selected_obj = i;
					auto current_obj = new UIMenuStorage<Object>(UIText("Object " + std::to_string(selected_obj), title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
						UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {}, selected_obj);
					current_obj->Add(new UIItem("Delete Object", "", [this](void* param)
					{
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)param;
						auto obj = menu->GetItem();
						auto it = std::find(SpawnedObjects.begin(), SpawnedObjects.end(), obj);
						if (it != SpawnedObjects.end())
							SpawnedObjects.erase(it);
						OBJECT::DELETE_OBJECT(&obj);
					}));
					return current_obj;
				}));
			}
			return menu2;
		}));
		return obj_menu;
	}));
#pragma endregion
#pragma region Recovery Menu
	CurrentMenu->Add(new UIItemSubMenu("Recovery Menu", "Modify your stats", this, [this]()->UIMenu*
	{
		auto recovery_menu = SpawnMenu("Recovery Menu");
		return recovery_menu;
	}));
#pragma endregion
}

void Client::ToggleMenu()
{
	if (!CurrentMenu)
	{
		InitializeMenu();
	}
	else
	{
		auto parent = CurrentMenu->GetParent();
		while (parent)
		{
			auto old_parent = parent;
			parent = parent->GetParent();
			delete old_parent;
		}
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