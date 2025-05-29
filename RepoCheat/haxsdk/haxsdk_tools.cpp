
// File Header
#include "haxsdk_tools.h"

// C Headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static char g_Buffer[512]; // Global buffer, useful for ImGui that copies text into own buffer

// UTF16 -> UTF8 based on WinAPI and global buffer
char* HaxSdk::UTF8(wchar_t* wstr, size_t len)
{
    memset(g_Buffer, 0, sizeof(g_Buffer));
    int nBytesNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)len, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(len), g_Buffer, nBytesNeeded, NULL, NULL);
    return g_Buffer;
}
