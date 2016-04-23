#include "stdafx.h"
#include "Menu Utils.h"
#include "Lists.h"
#include "Client.h"

void SpawnVehicle(std::string vehName, Ped ped)
{
	Hash hash = GAMEPLAY::GET_HASH_KEY((char*)vehName.c_str());
	Vector3 pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0, 5.0, 0);

	STREAMING::REQUEST_MODEL(hash);
	if (STREAMING::HAS_MODEL_LOADED(hash))
	{
		uint32_t VehicleHandle = VEHICLE::CREATE_VEHICLE(hash, pos.x, pos.y, pos.z, 0, FALSE, FALSE);
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(VehicleHandle);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	}
}

void SpawnPed(std::string pedName, Ped ped)
{
	Hash hash = GAMEPLAY::GET_HASH_KEY((char*)pedName.c_str());
	Vector3 pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 2.0, 0, 0);
	STREAMING::REQUEST_MODEL(hash);
	if (STREAMING::HAS_MODEL_LOADED(hash))
		PED::CREATE_PED(1, GAMEPLAY::GET_HASH_KEY((char*)pedName.c_str()), pos.x, pos.y, pos.z, 0, true, true);
}
std::string GetPedModel(Ped ped)
{
	for (uint32_t i = 0; i < ARRAYSIZE(pedModels); i++)
	{
		if (PED::IS_PED_MODEL(ped, GAMEPLAY::GET_HASH_KEY((char*)pedModels[i].c_str())))
			return pedModels[i];
	}
	return "";
}
void SpawnProp(std::string propName, Ped ped, Client* client)
{
	Vector3 pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 2.0, 0, 0);
	auto hash = GAMEPLAY::GET_HASH_KEY((char*)propName.c_str());
	STREAMING::REQUEST_MODEL(hash);
	if (STREAMING::HAS_MODEL_LOADED(hash))
	{
		Object obj = OBJECT::CREATE_OBJECT(hash, pos.x, pos.y, pos.z, TRUE, TRUE, TRUE);
		if (obj)
		{
			OBJECT::PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
			client->AddObject(obj);
		}
	}
}
void SetModel(std::string modelName)
{
	auto player = PLAYER::PLAYER_ID();
	auto playerPed = PLAYER::PLAYER_PED_ID();
	auto hash = GAMEPLAY::GET_HASH_KEY((char*)modelName.c_str());
	if (STREAMING::IS_MODEL_IN_CDIMAGE(hash) && STREAMING::IS_MODEL_VALID(hash))
	{
		STREAMING::REQUEST_MODEL(hash);
		while (!STREAMING::HAS_MODEL_LOADED(hash))	WAIT(0);
		//STREAMING::LOAD_ALL_OBJECTS_NOW();
		PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), hash);
		PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
		WAIT(0);
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 100; j++)
			{
				int drawable = rand() % 10;
				int texture = rand() % 10;
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), i, drawable, texture))
				{
					PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), i, drawable, texture, 0);
					break;
				}
			}
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	}
}
std::string GetEntityModel(Entity ent)
{
	for (uint32_t i = 0; i < ARRAYSIZE(propNames); i++)
	{
		if (GAMEPLAY::GET_HASH_KEY((char*)propNames[i].c_str()) == ENTITY::GET_ENTITY_MODEL(ent))
			return pedModels[i];
	}
	
}
std::string GetVehicleModel(Vehicle veh)
{
	for each(auto i in vehicleModels)
	{
		auto hash = GAMEPLAY::GET_HASH_KEY((char*)i.c_str());
		if (VEHICLE::IS_VEHICLE_MODEL(veh, hash))
			return VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(hash);
	}
}