// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdarg>
#include <random>
#include <chrono>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <sys/syslog.h>
#endif

#if OUZEL_PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <strsafe.h>
#endif

#if OUZEL_PLATFORM_ANDROID
#include <android/log.h>
#endif

#include "Utils.h"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    AnrdoidNEONChecker anrdoidNEONChecker;
#endif
    char TEMP_BUFFER[65536];

    void log(const char* format, ...)
    {
        va_list list;
        va_start(list, format);

        vsprintf(TEMP_BUFFER, format, list);

        va_end(list);

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        printf("%s\n", TEMP_BUFFER);
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        syslog(LOG_WARNING, "%s", TEMP_BUFFER);
        printf("%s\n", TEMP_BUFFER);
#elif OUZEL_PLATFORM_WINDOWS
        wchar_t szBuffer[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, TEMP_BUFFER, -1, szBuffer, MAX_PATH);
        StringCchCat(szBuffer, sizeof(szBuffer), L"\n");
        OutputDebugString(szBuffer);
#elif OUZEL_PLATFORM_ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, "Ouzel", "%s", TEMP_BUFFER);
#endif
    }

    uint64_t getCurrentMicroSeconds()
    {
        auto t = std::chrono::steady_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch());

        return static_cast<uint64_t>(micros.count());
    }

    static std::mt19937 engine(std::random_device{}());

    uint32_t random(uint32_t min, uint32_t max)
    {
        return std::uniform_int_distribution<uint32_t>{min, max}(engine);
    }

    float randomf(float min, float max)
    {
        float diff = max - min;

        float rand = static_cast<float>(engine()) / engine.max();

        return rand * diff + min;
    }
}
