#pragma once

#include <string_view>
#include <format>

#define HAX_LOG(fmt, ...)       HaxSdk::Log(std::format(fmt, ##__VA_ARGS__))
#define HAX_LOG_ERROR(fmt, ...) HaxSdk::LogError(std::format(fmt, ##__VA_ARGS__))
#define HAX_LOG_DEBUG(fmt, ...) HaxSdk::LogDebug(std::format(fmt, ##__VA_ARGS__))
#define HAX_LOG_WARN(fmt, ...)  HaxSdk::LogWarning(std::format(fmt, ##__VA_ARGS__))

namespace HaxSdk
{
    /**
     * @brief Entry point of logging module. Initializes log file and creates console.
     * @param useConsole - Whether console should be created.
     */
    void InitLogger(bool useConsole);

    /**
     * @brief Writes info into log file and into the console.
     * @param message - Content of the log
     * @warning Logger should be initialized first
     * @see HAX_LOG
     */
    void Log(std::string_view message);

    /**
     * @brief Writes error into log file and into the console.
     * @param message - Content of the log
     * @warning Logger should be initialized first
     * @see HAX_LOG_ERROR
     */
    void LogError(std::string_view message);

    /**
     * @brief Writes warning into log file and into the console.
     * @param message - Content of the log
     * @warning Logger should be initialized first
     * @see HAX_LOG_WARN
     */
    void LogWarning(std::string_view message);

    /**
     * @brief Writes debug info into log file and into the console.
     * @param message - Content of the log
     * @warning Logger should be initialized first
     * @see HAX_LOG_DEBUG
     */
    void LogDebug(std::string_view message);
}