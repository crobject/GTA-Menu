#include "stdafx.h"
#include "Client.h"
#include "keyboard.h"
#include "inc\natives.h"
#include "UIMenu.h"
#include "UIItem.h"
#include "Lists.h"
#include "Menu Utils.h"
#include <ctime>
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
	if (IsKeyDown(VK_CONTROL) && IsKeyDown('F') && CurrentMenu && !Keyboard)
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
	return new UIMenu(UIText(name, Point(100, 75), DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
		UIText(description, Point(100, 75 + 40), DEFAULT_TITLE_SCALE - 0.5), Point(100, 75), Size_t(200, 500), [] {}, [] {});	
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
			SpawnVehicle("t20", PLAYER::PLAYER_PED_ID(), SpawnInVehicle);
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

			ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID()));
			PED::ADD_ARMOUR_TO_PED(PLAYER::PLAYER_PED_ID(), PLAYER::GET_PLAYER_MAX_ARMOUR(PLAYER::PLAYER_ID()) - PED::GET_PED_ARMOUR(PLAYER::PLAYER_PED_ID()));
			if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0))
			{
				Vehicle playerVeh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()) && !ENTITY::IS_ENTITY_DEAD(playerVeh))
					VEHICLE::SET_VEHICLE_FIXED(PLAYER::PLAYER_PED_ID());
			}
		}));
		self_menu->Add(new UIItem("Give All Weapons", "", [this](void* param)
		{
			for (int i = 0; i < ARRAYSIZE(WeapHashes); i++)
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), WeapHashes[i], 9999, 1);
		}));

		self_menu->Add(new UIItemToggle("Super Run", "Toggle Super Run", [this](void* param)->void
		{
			PLAYER::_SET_MOVE_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), SuperRun ? 1.49 : 1.0);
		}, &SuperRun));
		self_menu->Add(new UIItemToggle("Super Run", "Toggle Super Run", [this](void* param)->void
		{
			PLAYER::_SET_SWIM_SPEED_MULTIPLIER(PLAYER::PLAYER_ID(), SuperSwim ? 1.49 : 1.0);
		}, &SuperSwim));
		self_menu->Add(new UIItemToggle("Super Jump", "", [this](void* param)->void
		{
			if (!Threads.count(&SuperJump))
				Threads[&SuperJump] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperJump));
		self_menu->Add(new UIItemToggle("Super Punch", "", [this](void* param)->void
		{
			if (!Threads.count(&SuperPunch))
				Threads[&SuperPunch] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_EXPLOSIVE_MELEE_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &SuperPunch));
		self_menu->Add(new UIItemToggle("Explosive Bullets", "", [this](void* param)
		{
			if (!Threads.count(&ExplosiveBullets))
				Threads[&ExplosiveBullets] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_EXPLOSIVE_AMMO_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &ExplosiveBullets));
		self_menu->Add(new UIItemToggle("Fire Bullets", "", [this](void* param)
		{
			if (!Threads.count(&FireBullets))
				Threads[&FireBullets] = new ScriptThread([](ScriptThread* thread)
			{
				GAMEPLAY::SET_FIRE_AMMO_THIS_FRAME(PLAYER::PLAYER_ID());
			});
		}, &FireBullets));
		self_menu->Add(new UIItemToggle("Recharge Ability", "", [this](void* param)
		{
			if (!Threads.count(&UnlimitedAbility))
				Threads[&UnlimitedAbility] = new ScriptThread([](ScriptThread* thread)
			{
				PLAYER::_RECHARGE_SPECIAL_ABILITY(PLAYER::PLAYER_ID(), TRUE);
			});
		}, &UnlimitedAbility));

		self_menu->Add(new UIItemList("Projectile", "", [this](void* param)
		{
			auto item = (uint32_t)param;
			Projectile = (bool)item;
			ProjectileType = item;
			if (!Threads.count(&Projectile))
				Threads[&Projectile] = new ScriptThread([this](ScriptThread* thread)
			{
				if (PED::IS_PED_SHOOTING(PLAYER::PLAYER_PED_ID()))
				{
					Vector3 pos;

					switch (ProjectileType)
					{

					case 1:
						WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &pos);
						FIRE::ADD_OWNED_EXPLOSION(PLAYER::PLAYER_PED_ID(), pos.x, pos.y, pos.z, 1, FLT_MAX, TRUE, TRUE, TRUE);
						break;
					case 2:
						//http://gtaforums.com/topic/792378-replace-bullets-with-objectsvehicles/
						float offset;
						Hash model = GAMEPLAY::GET_HASH_KEY((char *)vehicleModels[rand() % ARRAYSIZE(vehicleModels)].c_str());
						while (!VEHICLE::IS_THIS_MODEL_A_CAR(model))
							model = GAMEPLAY::GET_HASH_KEY((char *)vehicleModels[rand() % ARRAYSIZE(vehicleModels)].c_str());
						STREAMING::REQUEST_MODEL(model);
						while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(0);
						
						if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_A_VEHICLE(model))
						{
							Vector3 dim1, dim2;
							GAMEPLAY::GET_MODEL_DIMENSIONS(model, &dim1, &dim2);

							offset = dim2.y * 1.6;
						}

						Vector3 dir = ENTITY::GET_ENTITY_FORWARD_VECTOR(PLAYER::PLAYER_PED_ID());
						pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
						float rot = (ENTITY::GET_ENTITY_ROTATION(PLAYER::PLAYER_PED_ID(), 0)).z;

						Vehicle veh = VEHICLE::CREATE_VEHICLE(model, pos.x + (dir.x * offset), pos.y + (dir.y * offset), pos.z, rot, 1, 1);

						VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);

						VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 70.0);

						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
						break;
					}
				}
			});
		}, { "None", "Explosion", "Car" }));
		self_menu->Add(new UIItemToggle("Aimbot", "", [this](void* param) 
		{
			if (!Threads.count(&Aimbot))
				Threads[&Aimbot] = new ScriptThread([](ScriptThread* thread)
			{
				const uint32_t numElements = 10;
				const uint32_t arrSize = numElements * 2 + 2;  //Start at index 2, and the odd elements are padding
				Ped peds[arrSize];
				peds[0] = numElements;
				uint32_t count = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), peds, -1);
				for (uint32_t i = 0; i < count; i++)
				{
					uint32_t offsettedID = i * 2 + 2;
					auto currentPed = peds[offsettedID];
					if (currentPed == PLAYER::PLAYER_PED_ID() || ENTITY::IS_ENTITY_DEAD(currentPed) || ENTITY::IS_ENTITY_VISIBLE(currentPed) || !PED::IS_PED_ON_FOOT(currentPed))
						continue;
					
				}
				
			});
		}, &Aimbot));
		self_menu->Add(new UINumberItem<uint32_t>("Wanted Level", "", [this](void* param) 
		{
			auto item = (UINumberItem<uint32_t>*)(param);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NO_DROP(PLAYER::PLAYER_ID(), item->GetValue(), FALSE);
			PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(), item->GetValue(), FALSE);
		}));
		self_menu->Add(new UIItem("Teleport into closest vehicle", "", [](void* param)
		{
			const uint32_t numElements = 10;
			const uint32_t arrSize = numElements * 2 + 2;  //Start at index 2, and the odd elements are padding
			Vehicle vehs[arrSize];
			//0 index is the size of the array
			vehs[0] = numElements;
			uint32_t count = PED::GET_PED_NEARBY_VEHICLES(PLAYER::PLAYER_PED_ID(), vehs);
			uint32_t offsettedID = 2;
			uint32_t seatIndex = -1;
			while (seatIndex == -1)
			{
				for (uint32_t i = 0; i < 7; i++)
				{
					if (VEHICLE::IS_VEHICLE_SEAT_FREE(vehs[offsettedID], i))
						seatIndex = i;
				}
				offsettedID += 2;
			}
			
			PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), vehs[offsettedID], -1);
		}));
		self_menu->Add(new UIItem("Set Default Model", "", [](void* param)
		{
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
		}));
		self_menu->Add(new UIItemToggle("Silent Player", "", [this](void* param)
		{
			PLAYER::SET_PLAYER_NOISE_MULTIPLIER(PLAYER::PLAYER_ID(), SilentPlayer ? 0.0 : 1.0);
		},&SilentPlayer));
		self_menu->Add(new UIItemToggle("Walking Radio", "", [this](void* param) 
		{
			AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(MobileRadio);
		}, &MobileRadio));
		self_menu->Add(new UIItemToggle("Disable HUD", "", [this](void* param)
		{
			if (!Threads.count(&DisableHUD))
				Threads[&DisableHUD] = new ScriptThread([](ScriptThread* thread)
			{
				UI::HIDE_HUD_AND_RADAR_THIS_FRAME();

			});
		}, &DisableHUD));
		self_menu->Add(new UIItem("Sync Time", "", [](void* param)
		{
			time_t now = time(0);
			tm t;
			localtime_s(&t, &now);
			TIME::SET_CLOCK_TIME(t.tm_hour, t.tm_min, t.tm_sec);
		}));
		self_menu->Add(new UIItemToggle("Shrink Player", "", [this](void* param)
		{
			PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 223, ShrinkPlayer);
		}, &ShrinkPlayer));
		self_menu->Add(new UIItemList("Set Vision", "", [this](void* param) 
		{
			std::string* effect = (std::string*)param;			
			if (effect)
			{
				if (GRAPHICS::_GET_SCREEN_EFFECT_IS_ACTIVE((char*)effect->c_str()))
				{
					GRAPHICS::_STOP_SCREEN_EFFECT((char*)effect->c_str());
				}
				GRAPHICS::_START_SCREEN_EFFECT((char*)effect->c_str(), 0, false);
				Vision = *effect;
			}

		}, { "PeyoteIn", "PeyoteOut", "DrugsTrevorClownsFight", "RaceTurbo", "Rampage", "Dont_tazeme_bro" }, false));
		self_menu->Add(new UIItemSubMenu("Component Menu", "Change what your player is wearing", this, [this]()->UIMenu*
		{
			auto componentMenu = SpawnMenu("Component Menu");
			componentMenu->Add(new UINumberItem<int32_t>("Face (type)", "", [](void* param) 
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 0, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Beard", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Haircut", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 2, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Shirt", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Pants", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Hands / Gloves", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 5, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Shoes", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Eyes", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 7, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Accessories", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 8, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Mission Items/Tasks", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 9, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Decals", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 10, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			componentMenu->Add(new UINumberItem<int32_t>("Other Shirts", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			return componentMenu;
		}));
		self_menu->Add(new UIItemSubMenu("Prop Menu", "Change what your player is holding", this, [this]()->UIMenu*
		{
			auto propMenu = SpawnMenu("Component Menu");
			propMenu->Add(new UINumberItem<int32_t>("Head Props", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), 0, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			propMenu->Add(new UINumberItem<int32_t>("Eye Props", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), 1, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			propMenu->Add(new UINumberItem<int32_t>("Ear Props", "", [](void* param)
			{
				auto item = (UINumberItem<uint32_t>*)(param);
				PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), 2, item->GetValue(), 0, PED::GET_PED_PALETTE_VARIATION(PLAYER::PLAYER_PED_ID(), 0));
			}));
			return propMenu;
		}));
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
					SpawnVehicle(item->GetText(), PLAYER::PLAYER_PED_ID(), SpawnInVehicle);
				}));
			}
			return menu1;
		}));
		veh_menu->Add(new UIItemToggle("Teleport to Spawned Vehicle", "", [this](void* param) {}, &SpawnInVehicle));
		veh_menu->Add(new UIItemSubMenu("Modify Primary Color", "", this, [this]()->UIMenu*
		{
			uint32_t veh_color[3] = { 0 };
			auto veh_handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (veh_handle)
				VEHICLE::GET_VEHICLE_COLOR(veh_handle, (int*)&veh_color[0], (int*)&veh_color[1], (int*)&veh_color[2]);
			return new UIMenuRGB(UIText("Modify Primary Color", Point(100, 75), DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
				UIText("", Point(100, 75 + 40), DEFAULT_TITLE_SCALE - 0.5), Point(100, 75), Size_t(200, 500), [](uint32_t* param)
				{
					auto veh_handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
					if (veh_handle)
						VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh_handle, param[0], param[1], param[2]);
				},[] {}, [] {}, veh_color);

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
					VEHICLE::SET_VEHICLE_DOOR_OPEN(handle, DoorIndex, FALSE, FALSE);
		}));
		veh_menu->Add(new UIItem("Close Doors", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				for (int DoorIndex = 0; DoorIndex <= 6; DoorIndex++)
					VEHICLE::SET_VEHICLE_DOOR_SHUT(handle, DoorIndex, FALSE);
		}));
		veh_menu->Add(new UIItem("Bullet Proof Tires", "", [this](void* param)
		{
			auto handle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (handle)
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(handle, TRUE);
		}));
		veh_menu->Add(new UIItemToggle("Rainbow Car", "", [this](void* param)
		{
			if (!Threads.count(&RainbowCar)) {
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
					thread->Sleep(500);
				});		
			}
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
					auto vel = ENTITY::GET_ENTITY_SPEED(handle);
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, vel + vel * 0.05f);
					AUDIO::SET_VEHICLE_BOOST_ACTIVE(handle, FALSE);
				}
				else if (handle && IsKeyDown(VK_OEM_MINUS))
				{
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, 0.0f);
				}
				thread->Sleep(500);
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
		veh_menu->Add(new UIItemList("Vehicle Weapons", "", [this](void* param)
		{
			uint32_t index = (uint32_t)param;
			VehicleWeapon = (bool)index;
			switch (index)
			{
			case 1:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_VEHICLE_ROCKET");
				break;
			case 2:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_PISTOL");
				break;
			case 3:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_STUNGUN");
				break;
			case 4:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_GRENADELAUNCHER");
				break;
			case 5:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_MOLOTOV");
				break;
			case 6:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_RPG");
				break;				
			case 7:
				vehicleWeaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_FLARE");
				break;
			}
			if (!Threads.count(&VehicleWeapon))
				Threads[&VehicleWeapon] = new ScriptThread([this](ScriptThread* thread)
			{
				if (IsKeyDown(VK_INSERT) && PLAYER::IS_PLAYER_CONTROL_ON(PLAYER::PLAYER_ID()) && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0))
				{
					//ripped from the native trainer...
					Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());

					Vector3 v0, v1;
					GAMEPLAY::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(veh), &v0, &v1);

					Hash weaponAssetRocket = vehicleWeaponHash;
					if (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponAssetRocket))
					{
						WEAPON::REQUEST_WEAPON_ASSET(weaponAssetRocket, 31, 0);
						while (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponAssetRocket))
							WAIT(0);
					}

					Vector3 coords0from = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, -(v1.x + 0.25f), v1.y + 1.25f, 0.1);
					Vector3 coords1from = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, (v1.x + 0.25f), v1.y + 1.25f, 0.1);
					Vector3 coords0to = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, -v1.x, v1.y + 100.0f, 0.1f);
					Vector3 coords1to = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, v1.x, v1.y + 100.0f, 0.1f);

					GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(coords0from.x, coords0from.y, coords0from.z,
						coords0to.x, coords0to.y, coords0to.z,
						250, 1, weaponAssetRocket, PLAYER::PLAYER_PED_ID(), 1, 0, -1.0);
					GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(coords1from.x, coords1from.y, coords1from.z,
						coords1to.x, coords1to.y, coords1to.z,
						250, 1, weaponAssetRocket, PLAYER::PLAYER_PED_ID(), 1, 0, -1.0);

				}
			});

		}, {"None", "Rockets", "Pistol", "Stun Gun", "Grenade Launcher", "Molotov", "RPG", "Flare"}));

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
					menu2->Add(new UIItemDisplayPedMenu((pedTypes.count(type) ? pedTypes[type] : "Unknown") + (std::string)" - " + GetPedModel(currentPed), "", this, [this, currentPed]()->UIMenu*
					{
						Point title_point = Point(100, 75);
						Point sub_title_point = Point(110, 100);
						auto selected_ped_menu = new UIMenuStorage<Ped>(UIText(((*CurrentMenu->GetCurrentItem()))->GetText(), title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
							UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {}, currentPed);
						selected_ped_menu->Add(new UIItem("Explode Head", "", [](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;
							PED::EXPLODE_PED_HEAD(ped->GetItem(), 0x5FC3C11);
						}));
						selected_ped_menu->Add(new UIItem("Set as Cop", "", [](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;
							PED::SET_PED_AS_COP(ped->GetItem(), TRUE);
						}));
						selected_ped_menu->Add(new UIItem("Delete", "", [](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;
							auto p = ped->GetItem();
							PED::DELETE_PED(&p);
						}));
						selected_ped_menu->Add(new UIItem("Clone Ped", "", [](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;
							PED::CLONE_PED(ped->GetItem(), ENTITY::GET_ENTITY_HEADING(ped->GetItem()), TRUE, TRUE);
						}));
						selected_ped_menu->Add(new UIItem("Give All Weapons", "", [](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;
							for (int i = 0; i < ARRAYSIZE(WeapHashes); i++)
								WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped->GetItem(), WeapHashes[i], 9999, 1);
						}));
						selected_ped_menu->Add(new UIItem("Give Body Armour", "", [](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;
							PED::ADD_ARMOUR_TO_PED(ped->GetItem(), 9999);
						}));
						selected_ped_menu->Add(new UIItem("Add to Gang", "", [this](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Ped>* ped = (UIMenuStorage<Ped>*)parent;							
							PED::SET_PED_AS_GROUP_MEMBER(ped->GetItem(), PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID()));
						}));
						return selected_ped_menu;
					}, currentPed));
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
		tele_menu->Add(new UIItem("Teleport to Blip", "", [](void* param)
		{
			//ripped from native trainer
			Vector3 coords;
			bool blipFound = false;
			// search for marker blip
			int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
			for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
			{
				if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
				{
					coords = UI::GET_BLIP_INFO_ID_COORD(i);
					blipFound = true;
					break;
				}
			}
			if (blipFound)
			{
				// load needed map region and check height levels for ground existence
				bool groundFound = false;
				static float groundCheckHeight[] = {
					100.0, 150.0, 50.0, 0.0, 200.0, 250.0, 300.0, 350.0, 400.0,
					450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
				};
				for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
				{
					auto coord2 = coords;
					coord2.z = groundCheckHeight[i];
					Teleport(PLAYER::PLAYER_PED_ID(), coord2);
					WAIT(100);
					if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, groundCheckHeight[i], &coords.z))
					{
						groundFound = true;
						coords.z += 3.0;
						break;
					}
				}
				// if ground not found then set Z in air and give player a parachute
				if (!groundFound)
				{
					coords.z = 1000.0;
					WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 0);
				}
			}

		}));
		tele_menu->Add(new UIItemList("Airfield", "", [](void* param)
		{
			auto item = (int32_t)param;
			float locations[][3] = { { -1102.290,-2894.520,13.947 },
									{ 2121.700,	4796.300,41.100 },
									{ 1747.000,	3273.700,41.100 },
								   };
			if (item <= ARRAYSIZE(locations))
			{
				Vector3 pos;
				pos.x = locations[item][0];
				pos.y = locations[item][1];
				pos.z = locations[item][2];

				Teleport(PLAYER::PLAYER_PED_ID(), pos);
			}
		}, { "Airport - Helipad","McKenzie Field Runway","Sandy Shores Airfield"}));
		tele_menu->Add(new UIItemList("Easter Egg", "", [](void* param)
		{
			auto item = (int32_t)param;
			float locations[][3] = { 
				{ -595.000,2087.000,132.000 },
				{ -420.000,2065.750,120.000 },
				{ -598.000,2096.000,131.000 },
				{ 3862.411,-4959.250,93.000 },
				{ -3161.078,3001.998,-37.974 },
				{ 3199.748,-379.018,-22.500 },
				{ -942.350,6608.752,-20.912 },
				{ 4273.950,2975.714,-170.746 },
				{ 4287.000,2967.000,-176.000 },
				{ -1254.831,6795.000,-175.000 },
				{ 762.426,7380.371,-111.377 },
				{ 4201.633,3643.821,-39.016 },

			};
			if (item <= ARRAYSIZE(locations))
			{
				Vector3 pos;
				pos.x = locations[item][0];
				pos.y = locations[item][1];
				pos.z = locations[item][2];

				Teleport(PLAYER::PLAYER_PED_ID(), pos);
			}
		}, { "Black UFO","Dead Sea Monster","Mineshaft, Great Chaparral - Inside (Next Gen)","Mineshaft, Great Chaparral - Inside Inaccessible Barrier","Mineshaft, Great Chaparral - Outside",
			"North Yankton - Alien","Sunken Body","Sunken Cargo Ship","Sunken Plane","Underwater Hatch","Underwater Hatch from Lost Series","Underwater Hatch with Light","Underwater UFO","Underwater WW2 Tank" }));
		tele_menu->Add(new UIItemList("Restricted Areas", "", [](void* param)
		{
			auto item = (int32_t)param;
			float locations[][3] = {
				{ -1170.115,4926.134,224.355 },
				{ -1166.000,4927.000,224.000 },
				{ -1143.150,4951.188,230.153 },
				{ -1002.000,4842.000,280.000 },
				{ -2019.060,2956.453,32.810 },
				{ -2148.350,3031.762,32.810 },
				{ -2047.400,3132.100,32.800 },
				{ -2012.847,2956.527,32.810 },
				{ -2148.351,3031.762,33.810 },
				{ -2356.094,3248.645,101.451 },
				{ -2345.793,3267.464,33.811 },
				{ -2501.939,3309.898,92.967 },
				{ 1690.000,2653.000,58.000 },
				{ 1690.000,2536.000,58.000 },

			};
			if (item <= ARRAYSIZE(locations))
			{
				Vector3 pos;
				pos.x = locations[item][0];
				pos.y = locations[item][1];
				pos.z = locations[item][2];

				Teleport(PLAYER::PLAYER_PED_ID(), pos);
			}
		}, { "Altruist Camp","Altruist Camp - Altar","Altruist Camp - Roof","Altruist Camp Mountain - Radio Tower","Military - Jet Spawn point 1","Military - Jet Spawn point 2","Military Base","Military Base","Military Base (Jet Spawn)",
			"Military Base Control Tower (Inside Tower)","Military Base Control Tower (Level 1)","Military Base Radio Tower","Prison Guard Tower (North)","Prison Guard Tower (South)", }));
		tele_menu->Add(new UIItemList("Shops", "", [](void* param)
		{
			auto item = (int32_t)param;
			float locations[][3] = { 
				{ 1696.105,3755.167,34.705 },
				{ 247.365,-45.878,69.941 },
				{ 53.940,-1390.770,29.373 },
				{ -367.734,-130.859,39.117 },
				{ -335.000,-136.000,40.000 },
				{ -159.300,-304.329,39.733 },
				{ 319.788,172.231,104.744 },
			};
			if (item <= ARRAYSIZE(locations))
			{
				Vector3 pos;
				pos.x = locations[item][0];
				pos.y = locations[item][1];
				pos.z = locations[item][2];

				Teleport(PLAYER::PLAYER_PED_ID(), pos);
			}
		}, { "Ammunation (Sandy Shore)","Ammunation, Hawick","Car Wash - Hands on","Los Santos Customs","Los Santos Customs - Inside","Ponsonbys Clothing, Burton","Tattoo Parlor" }));
		tele_menu->Add(new UIItemList("Interesting Spots", "", [](void* param)
		{
			auto item = (int32_t)param;
			float locations[][3] = {
				{ -1034.600,-2733.600,13.800 },
				{ -1561.525,-3232.345,27.336 },
				{ -935.936,4836.747,310.520 },
				{ -2177.440,5182.596,16.475 },
				{ -167.982,-1001.930,296.206 },
				{ -120.351,-977.861,304.248 },
				{ -119.860,-976.439,306.339 },
				{ -147.593,-964.695,254.133 },
				{ -129.384,-951.133,218.882 },
				{ -1670.700,-1125.000,13.000 },
				{ -769.800,331.208,234.269 },
				{ -438.796,1075.821,353.000 },
				{ 130.682,-634.945,262.851 },
				{ 3369.224,5184.150,1.460 },
				{ 3336.174,5172.941,18.316 },
				{ 3430.990,5174.220,43.500 },
				{ 895.561,-987.624,44.271 },
				{ -74.942,-818.635,327.174 },
				{ -74.942,-818.635,2590.223 },
				{ -74.942,-818.635,5841.424 },
				{ -74.942,-818.635,10800.000 },
				{ -250.604,-2030.000,30.000 },
				{ -104.820,-856.374,41.087 },
				{ -131.063,-865.810,29.468 },
				{ 546.037,-183.368,54.498 },
				{ 495.000,5586.000,794.000 },
				{ 430.204,5614.734,766.168 },
				{ 425.400,5614.300,766.500 },
				{ 489.317,5580.887,792.852 },
				{ 413.471,5572.821,779.682 },
				{ -149.345,7130.246,700.117 },
				{ 2876.984,5911.455,370.000 },
				{ -1186.107,3849.753,489.064 },
				{ -1212.987,3848.685,491.000 },
				{ -179.984,6150.478,42.637 },
				{ 178.330,7041.822,1.867 },
				{ 17.832,7630.014,13.507 },
				{ -125.654,7271.894,16.737 },
				{ 106.697,7282.055,1.882 },
				{ -1464.000,182.000,55.000 },
				{ 334.210,-1644.770,98.496 },
				{ -540.482,4402.359,35.379 },
				{ -175.219,4244.194,44.073 },
				{ -589.532,4395.390,18.148 },
				{ -463.662,4483.654,36.037 },
				{ -597.953,4475.291,25.689 },
				{ -530.675,4534.996,89.046 },
				{ -551.000,-194.000,77.000 },
				{ -129.964,8130.873,6705.651 },
				{ 16.969,-646.180,16.088 },
				{ 1033.729,-270.564,50.855 },
				{ -4.579,-742.428,16.503 },
				{ -2183.034,-1051.004,-142.771 },
				{ -1942.354,-1128.529,-34.993 },
				{ 684.593,573.285,131.000 },
				{ -558.981,2945.701,14.592 },
				{ -1202.090,2802.440,14.826 },

			};
			if (item <= ARRAYSIZE(locations))
			{
				Vector3 pos;
				pos.x = locations[item][0];
				pos.y = locations[item][1];
				pos.z = locations[item][2];

				Teleport(PLAYER::PLAYER_PED_ID(), pos);
			}
		}, { "Airport Entrance","Airport Terminal","Altruist Camp Mountain","Avi Hideout (Kill Boost Island)","Construction Site - Crane (1)","Construction Site - Crane (2) Balcony",
			"Construction Site - Crane 3 (Scenic)","Construction Site on building","Construction Site, Building Wireframe","Del Perro Pier - Ferris Wheel","Eclipse Towers Roof","Galileo Observatory Roof",
			"IAA Building Roof","Lighthouse - Dock","Lighthouse - Dock Winding stairs","Lighthouse near Mt. Gondor (Highest point)","Maibatsu Motors Inc. - Roof","Maze Bank - 0 Helipad","Maze Bank - 1 In the Sky",
			"Maze Bank - 2 Above Clouds","Maze Bank - 3 In the Atmosphere","Maze Bank Arena Entrance","Maze Bank Plaza - Water Fountain","Maze Bank Plaza - Water Fountain with cube","Mechanic, Hawick","Mt. Chiliad",
			"Mt. Chiliad - Jump","Mt. Chiliad - Ramp","Mt. Chiliad - View (City)","Mt. Chiliad - View (Scenic)","Mt. Chiliad Near - Above Cloud","Mt. Gordo","Mt. Josiah","Mt. Josiah","Paleto Bay - Factory Lookout",
			"Paleto Bay Beach","Paleto Bay Beach - Island (Farthest)","Paleto Bay Beach - Island with flowers","Paleto Bay Beach - Underwater","Playboy Mansion, Richman","Police, Rancho - Parking Roof","Raton Canyon - A Waterfall",
			"Raton Canyon - Calafia Bridge","Raton Canyon - Creek swimming (Next to waterfall)","Raton Canyon - Private Hangout (Creek)","Raton Canyon - Private Hangout 2","Raton Canyon Train Tracks Bridge","Rockford Hills City Hall, Snipering Spot",
			"Sky","Underground - Tunnel Construction","Underground - Tunnel Entrance (Drain)","Underground - Tunnel Loop","Underwater Hotspot (For Underwater Car)","Vespucci Beach Coral Reef","Vinewood Bowl",
			"Zancudo River", "Zancudo River - Dirtbike trail" }));
		return tele_menu;
	}));

#pragma endregion
#pragma region Player Menu
	CurrentMenu->Add(new UIItemSubMenu("Player Menu", "Actions that effect other players in your current session", this, [this]()->UIMenu*
	{
		auto player_menu = SpawnMenu("Player Menu");
		for (Player i = 0; i < 32; i++)
		{
			player_menu->Add(new UIItemDisplayPlayerMenu(PLAYER::GET_PLAYER_NAME(i), "Options that effect this player", this, [this, i]()->UIMenu*
			{
				Point title_point = Point(100, 75);
				Point sub_title_point = Point(110, 100);
				auto selected_player_menu = new UIMenuStorage<Player>(UIText(((*CurrentMenu->GetCurrentItem()))->GetText(), title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
					UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {}, i);
				selected_player_menu->Add(new UIItem("Kill Player", "", [](void* param) {
					auto parent = ((UIItem*)(param))->GetParent();
					UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
					PED::EXPLODE_PED_HEAD(PLAYER::GET_PLAYER_PED(player->GetItem()), GAMEPLAY::GET_HASH_KEY("WEAPON_SNIPERRIFLE"));
				}));
				selected_player_menu->Add(new UIItem("Fling Player", "", [](void* param) {
					auto parent = ((UIItem*)(param))->GetParent();
					UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::GET_PLAYER_PED(player->GetItem()), 1, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), 0, 0, 0, true, true, true, true, false, false);
				}));
				selected_player_menu->Add(new UIItem("Teleport to Player", "", [](void* param) {
					auto parent = ((UIItem*)(param))->GetParent();
					UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
					Teleport(PLAYER::PLAYER_PED_ID(), ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(player->GetItem()), TRUE));
				}));
				selected_player_menu->Add(new UIItem("Give Stun Gun", "", [](void* param) {
					auto parent = ((UIItem*)(param))->GetParent();
					UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
					WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::GET_PLAYER_PED(player->GetItem()), GAMEPLAY::GET_HASH_KEY("WEAPON_STUNGUN"), 300, FALSE);
				}));
				selected_player_menu->Add(new UIItem("Clone Player", "", [](void* param) {
					auto parent = ((UIItem*)(param))->GetParent();
					UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
					PED::CLONE_PED(PLAYER::GET_PLAYER_PED(player->GetItem()), 1.0, TRUE, FALSE);
				}));
				selected_player_menu->Add(new UIItem("Clear Animation", "", [](void* param) {
					auto parent = ((UIItem*)(param))->GetParent();
					UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
					AI::CLEAR_PED_TASKS(PLAYER::GET_PLAYER_PED(player->GetItem()));
				}));
				selected_player_menu->Add(new UIItemDisplayPlayerMenu("Animation Options", "Set animations for the player", this, [this, i]()->UIMenu*
				{
					Point title_point = Point(100, 75);
					Point sub_title_point = Point(110, 100);
					auto anim_player_menu = new UIMenuStorage<Player>(UIText(((*CurrentMenu->GetCurrentItem()))->GetText(), title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
						UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {}, i);
					for each (auto i in animNames)
					{
						anim_player_menu->Add(new UIItem(std::get<0>(i), std::get<1>(i), [i](void* param)
						{
							auto parent = ((UIItem*)(param))->GetParent();
							UIMenuStorage<Player>* player = (UIMenuStorage<Player>*)parent;
							SetAnim(PLAYER::GET_PLAYER_PED(player->GetItem()), std::get<0>(i), std::get<1>(i));
						}));
					}
					return anim_player_menu;
				}, i));
				selected_player_menu->Add(new UIItemDisplayPlayerMenu("Attach Prop", "Attach a prop to this player", this, [this, i]() ->UIMenu*
				{
					auto menu1 = SpawnMenu("Attach Object");
					for each (auto j in propNames)
					{
						menu1->Add(new UIItem(j, "", [this, i](void* param) {
							auto item = (UIItem*)param;
							auto prop = SpawnProp(item->GetText(), PLAYER::GET_PLAYER_PED(i), this);
							ENTITY::ATTACH_ENTITY_TO_ENTITY(PLAYER::GET_PLAYER_PED(i), prop, PED::GET_PED_BONE_INDEX(PLAYER::GET_PLAYER_PED(i), 60309), 0.07, 0.10, 0.0, 0.0, -90.0, -90.0, 0, 0, 0, 0, 2, 1);
						}));
					}
					return menu1;
				}, i));
				return selected_player_menu;
			}, i));
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
				menu2->Add(new UIItemDisplayObjectMenu(std::string("Object ") + GetEntityModel(i), "", this, [this, i]()->UIMenu* {
					Point title_point = Point(100, 75);
					Point sub_title_point = Point(110, 100);
					auto selected_obj = i;
					auto current_obj = new UIMenuStorage<Object>(UIText("Object " + std::to_string(selected_obj), title_point, DEFAULT_TITLE_SCALE, Color_t(255, 255, 255, 255), HouseScript, false),
						UIText("", sub_title_point, DEFAULT_TITLE_SCALE - 0.75), title_point, Size_t(200, 500), [] {}, [] {}, selected_obj);
					current_obj->Add(new UIItem("Delete Object", "", [this](void* param)
					{
						auto parent = ((UIItem*)(param))->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto it = std::find(SpawnedObjects.begin(), SpawnedObjects.end(), obj);
						if (it != SpawnedObjects.end())
						{
							SpawnedObjects.erase(it);
							OBJECT::DELETE_OBJECT(&obj);
						}						
					}));
					current_obj->Add(new UINumberItem<float>("Move X", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto pos = ENTITY::GET_ENTITY_COORDS(obj, TRUE);
						pos.x += item->GetValue();
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(obj, pos.x, pos.y, pos.z, TRUE, TRUE, TRUE);
					}, 1.0, 0.5));
					current_obj->Add(new UINumberItem<float>("Move Y", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto pos = ENTITY::GET_ENTITY_COORDS(obj, TRUE);
						pos.y += item->GetValue();
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(obj, pos.x, pos.y, pos.z, TRUE, TRUE, TRUE);
					}, 1.0, 0.5));
					current_obj->Add(new UINumberItem<float>("Move Z", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto pos = ENTITY::GET_ENTITY_COORDS(obj, TRUE);
						pos.z += item->GetValue();
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(obj, pos.x, pos.y, pos.z, TRUE, TRUE, TRUE);
					}, 1.0, 0.5));
					current_obj->Add(new UINumberItem<float>("Change Roll", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto pos = ENTITY::GET_ENTITY_COORDS(obj, TRUE);
						pos.x += item->GetValue();
						ENTITY::SET_ENTITY_ROTATION(obj, pos.x, pos.y, pos.z, TRUE, TRUE);
					}, 15.0, 15.0));
					current_obj->Add(new UINumberItem<float>("Change Pitch", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto pos = ENTITY::GET_ENTITY_ROTATION(obj, TRUE);
						pos.y += item->GetValue();
						ENTITY::SET_ENTITY_ROTATION(obj, pos.x, pos.y, pos.z, TRUE, TRUE);
					}, 15.0, 15.0));
					current_obj->Add(new UINumberItem<float>("Change Yaw", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						auto pos = ENTITY::GET_ENTITY_COORDS(obj, TRUE);
						pos.z += item->GetValue();
						ENTITY::SET_ENTITY_ROTATION(obj, pos.x, pos.y, pos.z, TRUE, TRUE);
					}, 15.0, 15.0));
					current_obj->Add(new UIItem("Fling Object", "", [this](void* param)
					{
						auto item = (UINumberItem<float>*)(param);
						auto parent = item->GetParent();
						UIMenuStorage<Object>* menu = (UIMenuStorage<Object>*)parent;
						auto obj = menu->GetItem();
						ENTITY::APPLY_FORCE_TO_ENTITY(obj, 1, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0, 100), 0, 0, 0, true, true, true, true, false, false);
					}));

					return current_obj;
				}, i));
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
#pragma region World Menu 
	CurrentMenu->Add(new UIItemSubMenu("World Menu", "Modify the world", this, [this]()->UIMenu*
	{
		auto worldMenu = SpawnMenu("World");
		float waterHeight = 10.0;
		auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), FALSE);
		WATER::GET_WATER_HEIGHT_NO_WAVES(pos.x, pos.y, pos.z, &waterHeight);
		worldMenu->Add(new UINumberItem<float>("Set Water Height", "", [this](void* param)
		{
			float item = *(float*)(&param);
			auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), FALSE);
			WATER::MODIFY_WATER(pos.x, pos.y, pos.z, item);
		}, waterHeight));
		worldMenu->Add(new UIItemToggle("Gravity", "", [this](void* param)
		{
			if (!Threads.count(&NoGravity))
				Threads[&NoGravity] = new ScriptThread([this](ScriptThread* thread)
			{
				GAMEPLAY::SET_GRAVITY_LEVEL(NoGravity);
			}
		}, &NoGravity));
		worldMenu->Add(new UIItemSubMenu("Weather Menu", "", this, [this]()->UIMenu*
		{
			auto weatherMenu = SpawnMenu("Weather");
			weatherMenu->Add(new UIItem("Blizzard", "", [](void* param) 
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("XMAS");
			}));
			weatherMenu->Add(new UIItem("Clear", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("CLEAR");
			}));
			weatherMenu->Add(new UIItem("Clearing", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("CLEARNING");
			}));
			weatherMenu->Add(new UIItem("Overcast", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("OVERCAST");
			}));
			weatherMenu->Add(new UIItem("Cloudy", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("CLOUDS");
			}));
			weatherMenu->Add(new UIItem("Rainy", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("RAIN");
			}));
			weatherMenu->Add(new UIItem("Extra Sunny", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("EXTRASUNNY");
			}));
			weatherMenu->Add(new UIItem("Thunder", "", [](void* param)
			{
				GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("THUNDER");
				GAMEPLAY::_SET_WEATHER_TYPE_OVER_TIME("THUNDER", 30.0f);
			}));
			return weatherMenu;
		}));
		return worldMenu;
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