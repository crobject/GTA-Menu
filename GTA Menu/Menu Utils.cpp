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
		OBJECT::PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
		client->AddObject(obj);
	}
}