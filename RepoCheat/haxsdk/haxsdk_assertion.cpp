#include "haxsdk_assertion.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "haxsdk_logger.h"

void HaxSdk::Assert(bool expr, std::string_view message)
{
    do {
        if (!expr)
        {
            HAX_LOG_ERROR("{}", message);
            MessageBoxA(NULL, message.data(), "Hax assertion failed", MB_ICONERROR);
            TerminateProcess(GetCurrentProcess(), 0xDEAD);
        }
    } while (0);
}