#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"

#include "hooks.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "features/menu.hpp"

bool Unload = false;

DWORD WINAPI OnDllAttach(LPVOID base)
{
    if(Utils::WaitForModules(10000, { L"client_panorama.dll", L"engine.dll", L"shaderapidx9.dll" }) == WAIT_TIMEOUT) {
        return FALSE;
    }

#ifdef _DEBUG
    Utils::AttachConsole();
#endif

    try 
	{
        Utils::ConsolePrint("Initializing...\n");

        Interfaces::Initialize();
        Interfaces::Dump();

        NetvarSys::Get().Initialize();
		InputSys::Get().Initialize();
		Menu::Get().Initialize();
        Hooks::Initialize();


		InputSys::Get().RegisterHotkey(VK_INSERT, [base]() { Menu::Get().Toggle(); });
		InputSys::Get().RegisterHotkey(VK_END, [base]() { Unload = true; });

        Utils::ConsolePrint("Finished.\n");

        while(!Unload)
            Sleep(1000);

        g_CVar->FindVar("crosshair")->SetValue(true);

        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

    } catch(const std::exception& ex) 
	{
        Utils::ConsolePrint("An error occured during initialization:\n");
        Utils::ConsolePrint("%s\n", ex.what());
        Utils::ConsolePrint("Press any key to exit.\n");
        Utils::ConsoleReadKey();
        Utils::DetachConsole();

        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
    }
}

BOOL WINAPI OnDllDetach()
{
#ifdef _DEBUG
    Utils::DetachConsole();
#endif

    Hooks::Shutdown();
	Menu().Shutdown();
    return TRUE;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        case DLL_PROCESS_DETACH:
            if(lpvReserved == nullptr)
                return OnDllDetach();
            return TRUE;
        default:
            return TRUE;
    }
}