#include "haxsdk_steam_api.h"

#include "haxsdk_logger.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define STEAM_API(ret, f, ...)\
using t_ ## f = ret (*)(__VA_ARGS__);\
static t_ ## f f

#define GET_STEAM_API(f) f = (t_ ## f)GetProcAddress(g_SteamHandle, #f)

namespace HaxSdk::Steam
{
    STEAM_API(UserStats*, SteamAPI_SteamUserStats);
    STEAM_API(uint32_t, SteamAPI_ISteamUserStats_GetNumAchievements, UserStats* __this);
    STEAM_API(bool, SteamAPI_ISteamUserStats_GetAchievement, UserStats* __this, const char* pchName, bool* pbAchieved);
    STEAM_API(bool, SteamAPI_ISteamUserStats_SetAchievement, UserStats* self, const char* pchName);
    STEAM_API(const char*, SteamAPI_ISteamUserStats_GetAchievementName, UserStats* __this, uint32_t iAchievement);
    STEAM_API(Utils*, SteamAPI_SteamUtils);
    STEAM_API(bool, SteamAPI_ISteamUtils_GetImageSize, Utils* __this, int iImage, uint32_t* width, uint32_t* height);
    STEAM_API(bool, SteamAPI_ISteamUtils_GetImageRGBA, Utils* __this, int iImage, char* dest, int nDestBufferSize);

    static HMODULE g_SteamHandle = 0;

    bool IsPlatformAvailable()
    {
        static bool s_Initialized = false;
        if (!s_Initialized)
        {
            g_SteamHandle = GetModuleHandleA("steam_api64");
            s_Initialized = true;
        }
        return g_SteamHandle != 0;
    }

    UserStats* UserStats::GetUserInterfacePointer()
    {
        if (!IsPlatformAvailable())
        {
            HAX_LOG_ERROR("Steam API is not available for your game");
            return nullptr;
        }

        char buff[30] = {0};
        for (int i = 0; i < 100; ++i)
        {
            memset(buff, 0, sizeof(buff));
            sprintf_s(buff, "SteamAPI_SteamUserStats_v%03d", i);
            SteamAPI_SteamUserStats = (t_SteamAPI_SteamUserStats)GetProcAddress(g_SteamHandle, buff);
            if (SteamAPI_SteamUserStats)
                break;
        }
        
        if (!SteamAPI_SteamUserStats)
        {
            HAX_LOG_ERROR("SteamAPI_SteamUserStats_vXXX not found");
            return nullptr;
        }

        return SteamAPI_SteamUserStats();
    }

    uint32_t UserStats::GetNumAchievements()
    {
        if (!SteamAPI_ISteamUserStats_GetNumAchievements)
            GET_STEAM_API(SteamAPI_ISteamUserStats_GetNumAchievements);

        if (!SteamAPI_ISteamUserStats_GetNumAchievements)
        {
            HAX_LOG_ERROR("SteamAPI_ISteamUserStats_GetNumAchievements not found");
            return 0;
        }

        return SteamAPI_ISteamUserStats_GetNumAchievements(this);
    }

    bool UserStats::GetAchievement(const char* pchName, bool* pbAchieved)
    {
        if (!SteamAPI_ISteamUserStats_GetAchievement)
            GET_STEAM_API(SteamAPI_ISteamUserStats_GetAchievement);

        if (!SteamAPI_ISteamUserStats_GetAchievement)
        {
            HAX_LOG_ERROR("SteamAPI_ISteamUserStats_GetAchievement not found");
            return false;
        }

        return SteamAPI_ISteamUserStats_GetAchievement(this, pchName, pbAchieved);
    }

    bool UserStats::SetAchievement(const char* pchName)
    {
        if (!SteamAPI_ISteamUserStats_SetAchievement)
            GET_STEAM_API(SteamAPI_ISteamUserStats_SetAchievement);

        if (!SteamAPI_ISteamUserStats_SetAchievement)
        {
            HAX_LOG_ERROR("SteamAPI_ISteamUserStats_SetAchievement not found");
            return false;
        }

        return SteamAPI_ISteamUserStats_SetAchievement(this, pchName);
    }

    const char* UserStats::GetAchievementName(uint32_t iAchievement)
    {
        if (!SteamAPI_ISteamUserStats_GetAchievementName)
            GET_STEAM_API(SteamAPI_ISteamUserStats_GetAchievementName);

        if (!SteamAPI_ISteamUserStats_GetAchievementName)
        {
            HAX_LOG_ERROR("SteamAPI_ISteamUserStats_GetAchievementName not found");
            return nullptr;
        }

        return SteamAPI_ISteamUserStats_GetAchievementName(this, iAchievement);
    }

    Utils* Utils::GetUserInterfacePointer()
    {
        if (!IsPlatformAvailable())
        {
            HAX_LOG_ERROR("Steam API is not available for your game");
            return nullptr;
        }

        char buff[24] = { 0 };
        for (int i = 0; i < 100; ++i)
        {
            memset(buff, 0, sizeof(buff));
            sprintf_s(buff, "SteamAPI_SteamUtils_v%03d", i);
            SteamAPI_SteamUtils = (t_SteamAPI_SteamUtils)GetProcAddress(g_SteamHandle, buff);
            if (SteamAPI_SteamUtils)
                break;
        }

        if (!SteamAPI_SteamUtils)
        {
            HAX_LOG_ERROR("SteamAPI_SteamUtils_vXXX not found");
            return nullptr;
        }

        return SteamAPI_SteamUtils();
    }

    bool Utils::GetImageSize(int iImage, uint32_t* width, uint32_t* height)
    {
        if (!SteamAPI_ISteamUtils_GetImageSize)
            GET_STEAM_API(SteamAPI_ISteamUtils_GetImageSize);

        if (!SteamAPI_ISteamUtils_GetImageSize)
        {
            HAX_LOG_ERROR("SteamAPI_ISteamUtils_GetImageSize not found");
            return false;
        }

        return SteamAPI_ISteamUtils_GetImageSize(this, iImage, width, height);
    }

    bool Utils::GetImageRGBA(int iImage, char* dest, int nDestBufferSize)
    {
        if (!SteamAPI_ISteamUtils_GetImageRGBA)
            GET_STEAM_API(SteamAPI_ISteamUtils_GetImageRGBA);

        if (!SteamAPI_ISteamUtils_GetImageRGBA)
        {
            HAX_LOG_ERROR("SteamAPI_ISteamUtils_GetImageRGBA not found");
            return false;
        }

        return SteamAPI_ISteamUtils_GetImageRGBA(this, iImage, dest, nDestBufferSize);
    }
}