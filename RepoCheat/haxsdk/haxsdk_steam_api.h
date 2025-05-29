#pragma once

#include <cstdint>

namespace HaxSdk::Steam
{
    bool IsPlatformAvailable();

    class UserStats
    {
    public:

        static UserStats* GetUserInterfacePointer();

        uint32_t GetNumAchievements();
        const char* GetAchievementName(uint32_t iAchievement);
        bool GetAchievement(const char* pchName, bool* pbAchieved);
        bool SetAchievement(const char* pchName);
    };

    class Utils
    {
    public:

        static Utils* GetUserInterfacePointer();
        bool GetImageSize(int iImage, uint32_t* width, uint32_t* height);
        bool GetImageRGBA(int iImage, char* pubDest, int nDestBufferSize);
    };


}