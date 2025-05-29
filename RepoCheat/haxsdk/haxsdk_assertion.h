#pragma once

#include <crtdbg.h>

#include <string_view>
#include <format>

#define HAX_ASSERT(expr) _ASSERTE(expr)

#define HAX_ASSERT_E(expr, fmt, ...) HaxSdk::Assert(expr, std::format(fmt, ##__VA_ARGS__))

namespace HaxSdk
{
    /**
     * @brief Custom implementation of assertion 
     * @param expr - Expression to check
     * @param message - Message shown when assertion failed
     * @warning Game closes after assertion was failed.
     * @see HAX_ASSERT_E, HAX_ASSERT
     */
    void Assert(bool expr, std::string_view message);
}