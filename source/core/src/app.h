#pragma once
#include "cmd_line/parser.hpp"
#include <memory>
#include <string>

namespace core
{
    class App
    {
    public:
        int run(std::string&& title, int w, int h, int argc, char* argv[]);

    protected:
        void setup();
        void start();
        void stop();
        void run_one_frame(float dt);

        bool running_ = true;
    };
} // namespace core
