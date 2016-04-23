#pragma once
#include "stdafx.h"
#include "inc\natives.h"
#include <string>

class Client;

void SpawnVehicle(std::string vehName, Ped ped);
void SpawnPed(std::string pedName, Ped ped);
std::string GetPedModel(Ped ped);
std::string GetEntityModel(Entity ent);
std::string GetVehicleModel(Vehicle veh);
void SpawnProp(std::string propName, Ped ped, Client* client);
void SetModel(std::string modelName);