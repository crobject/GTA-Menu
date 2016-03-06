// GTA Menu.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "inc\main.h"
#include <string>
#include <ctime>
#include "Client.h"

bool running = true;
Client client;
void ScriptMain()
{
	srand(GetTickCount());
	client.InitializeMenu();
	while (running)
	{
		client.ProcessInput();
		if (client.CurrentMenu)
			client.CurrentMenu->Render();
		client.RunThreads();
		WAIT(0);
	}
}