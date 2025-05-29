#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "haxsdk/haxsdk_logger.h"
#include "haxsdk/haxsdk_unity.h"
#include "haxsdk/haxsdk_gui.h"

#include "cheat/cheat.h"

static void Start()
{
    HaxSdk::InitLogger(true); // set false if you dont need console

    HaxSdk::InitUnity();

    unsafe::Thread* thread = unsafe::Thread::Attach();

    HaxSdk::ImplementImGui(GraphicsApi_Any, Cheat::Init, Cheat::Background, Cheat::Menu);
}

bool WINAPI DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved)
{
    HaxSdk::SetCheatHandle((HANDLE)module);

    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);

    return true;
}

