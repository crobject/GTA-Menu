#include "stdafx.h"
#include "Menu Utils.h"
void SpawnVehicle(std::string vehName, Ped ped)
{
	Hash hash = GAMEPLAY::GET_HASH_KEY((char*)vehName.c_str());
	Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
	STREAMING::REQUEST_MODEL(hash);
	if (STREAMING::HAS_MODEL_LOADED(hash))
	{
		uint32_t VehicleHandle = VEHICLE::CREATE_VEHICLE(hash, pos.x, pos.y, pos.z, 0, FALSE, FALSE);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	}
}