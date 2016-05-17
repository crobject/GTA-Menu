#pragma once
#include "Menu.h"
#include <functional>
#include <map>
#include <list>

class UIMenu;
struct ScriptThread
{
	ScriptThread(std::function<void(ScriptThread* thread)> cb)
	{
		fn = cb;
		waitEndTime = GetTickCount();
	}
	std::function<void(ScriptThread* thread)> fn;
	uint32_t waitEndTime;
	void Sleep(uint32_t val) { waitEndTime = GetTickCount() + val; }
	void Exec()
	{
		if (GetTickCount() >= waitEndTime)
			fn(this);
	}
};
class Client
{
public:
	Client();
	void RunThreads();
	void ProcessInput();
	void InitializeMenu();
	void ToggleMenu();
	void GoToPreviousMenu();
	void ScrollDown();
	void ScrollUp();

	void SetMenu(UIMenu* menu);
	uint32_t GetCurrentItemIndex();
	void AddObject(Object obj) { SpawnedObjects.push_back(obj); }
	UIMenu* SpawnMenu(std::string name, std::string description = "");
	void CheckModel();
	~Client();

	UIMenu* CurrentMenu;
	UIMenu* PreviousMenu;
	UIMenu* BaseMenu;
	bool GodMode;
	bool SuperGrip;
	bool SuperRun;
	bool SuperSwim;
	bool SuperJump;
	bool SuperPunch;
	bool ExplosiveBullets;
	bool FireBullets;
	bool RainbowCar;
	bool Aimbot;
	bool OpenDoor;
	bool CarBoost;
	bool Keyboard;
	bool UnlimitedAmmo;
	bool NeverWanted;
	bool InvisablePlayer;
	bool Flymode;
	bool DriveWater;
	bool DriveAir;
	bool MoneyDrop;
	bool VehicleWeapon;
	bool SpawnInVehicle;
	bool MobileRadio;
	bool DisableHUD;
	bool ShrinkPlayer;
	bool SilentPlayer;
	bool UnlimitedAbility;
	std::string Vision;
	uint32_t GroupId;
	bool Projectile;	
	uint32_t ProjectileType;
	Hash vehicleWeaponHash;
	Object WaterObject;
	Object AirObject;
	bool NoGravity;
	std::map<bool*, ScriptThread*> Threads;
	std::vector<Object> SpawnedObjects;
};