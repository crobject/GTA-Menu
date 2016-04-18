#pragma once
#include "stdafx.h"
#include "inc\natives.h"
#include <string>

class Client;

void SpawnVehicle(std::string vehName, Ped ped);
void SpawnPed(std::string pedName, Ped ped);
std::string GetPedModel(Ped ped);
void SpawnProp(std::string propName, Ped ped, Client* client);