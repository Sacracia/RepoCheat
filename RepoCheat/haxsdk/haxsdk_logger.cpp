#include "haxsdk_logger.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <filesystem>
#include <fstream>

struct HaxLogger
{
    void Initialize(bool useConsole);
    void Log(std::string_view message, std::string_view tag, WORD textAttrs);

private:
    bool m_Initialized;
    bool m_UseConsole;
    std::filesystem::path m_LogPath;
    HANDLE m_Console;
};

static HaxLogger g_Logger;

namespace HaxSdk
{
    void InitLogger(bool useConsole)
    {
        g_Logger.Initialize(useConsole);
    }

    void Log(std::string_view message)
    {
        g_Logger.Log(message, "INFO", 15);
    }

    void LogError(std::string_view message)
    {
        g_Logger.Log(message, "ERROR", 12);
    }

    void LogWarning(std::string_view message)
    {
        g_Logger.Log(message, "WARNING", 14);
    }

    void LogDebug(std::string_view message)
    {
        g_Logger.Log(message, "DEBUG", 8);
    }
}

void HaxLogger::Initialize(bool useConsole)
{
    if (m_Initialized)
        return;

    m_UseConsole = useConsole;
    if (m_UseConsole)
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        m_Console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    char buff[MAX_PATH];
    GetModuleFileName(NULL, buff, MAX_PATH);
    const auto path = std::filesystem::path(buff);

    const auto logPath = path.parent_path() / "haxsdk-logs.txt";
    const auto prevLogPath = path.parent_path() / "haxsdk-prev-logs.txt";

    std::error_code errCode;
    std::filesystem::remove(prevLogPath, errCode);
    std::filesystem::rename(logPath, prevLogPath, errCode);
    std::filesystem::remove(logPath, errCode);

    m_LogPath = logPath;
    m_Initialized = true;
}

void HaxLogger::Log(std::string_view message, std::string_view tag, WORD textAttrs)
{
    if (!m_Initialized)
        return;

    // Change console text attributes
    WORD savedColor = 0;
    if (m_UseConsole)
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(m_Console, &info))
        {
            savedColor = info.wAttributes;
            SetConsoleTextAttribute(m_Console, textAttrs);
        }
    }

    // Generate message
    std::stringstream ss{};
    ss << '[' << std::left << std::setw(7) << tag << ":   HAXSDK] " << message << '\n';

    // Save to file
    std::ofstream file(m_LogPath, std::ios_base::app);
    file << ss.str();
    file.close();

    // Output and reset console text attributes
    if (m_UseConsole)
    {
        std::cout << ss.str();
        if (savedColor > 0)
            SetConsoleTextAttribute(m_Console, savedColor);
    }
}