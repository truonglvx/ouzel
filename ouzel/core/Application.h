// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <mutex>
#include "utils/Noncopyable.h"

namespace ouzel
{
    class Application: public Noncopyable
    {
    public:
        Application();
        Application(int pArgc, char* pArgv[]);
        Application(const std::vector<std::string>& pArgs);
        virtual ~Application();

        virtual bool run();

        int getArgc() const { return argc; }
        char** getArgv() const { return argv; }
        const std::vector<std::string>& getArgs() { return args; }

        virtual void execute(const std::function<void(void)>& func);

    protected:
        void executeAll();

        int argc = 0;
        char** argv = nullptr;
        std::vector<std::string> args;

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;
    };

    extern Application* sharedApplication;
}
