#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk_gui.h"
#include "haxsdk_unity.h"
#include "cheat/cheat.h"

static void Start() {
    HaxSdk::InitLogger(true);

    HaxSdk::InitializeCore();
    CheatMenu::Initialize();
    HaxSdk::ImplementImGui(GraphicsApi_Any);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
    HaxSdk::GetGlobals().cheatModule = module;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
    }
    if (reason == DLL_PROCESS_DETACH) {
        HaxSdk::Log("DETACHED\n");
    }
    return true;
}