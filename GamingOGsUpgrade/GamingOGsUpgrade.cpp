#include <API\ARK\Ark.h>
#include "GamingOGsUpgradeCommands.h"
#include "Database.h"

#pragma comment(lib, "ArkApi.lib")

void Initialize() {
	Log::Get().Init("GamingOGsUpgrade");
	GamingOGsUpgrade::LoadConfig();
	GamingOGsUpgrade::AddCommands();
	Log::GetLog()->info("Starting GamingOGsUpgrade plugin");
}

void Shutdown() {
	GamingOGsUpgrade::RemoveCommands();
	Log::GetLog()->info("Shutdown GamingOGsUpgrade plugin");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Initialize();
		break;
	case DLL_PROCESS_DETACH:
		Shutdown();
		break;
	}
	return TRUE;
}
