#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/Utilities.hpp"

#include "hooks.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "features/menu.hpp"

DWORD WINAPI OnDllAttach(LPVOID base)
{
	if (Utilities::WaitForModules(10000, { L"client_panorama.dll", L"engine.dll", L"shaderapidx9.dll" }) == WAIT_TIMEOUT) return FALSE;

		Interfaces::Initialize();

		NetvarSys::Get().Initialize();
		InputSys::Get().Initialize();
		Menu::Get().Initialize();
		Hooks::Initialize();

		InputSys::Get().RegisterHotkey(VK_INSERT, [base]() { Menu::Get().Toggle(); });
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved)
{
    switch(fdwReason)
	{
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        default:
            return TRUE;
    }
}