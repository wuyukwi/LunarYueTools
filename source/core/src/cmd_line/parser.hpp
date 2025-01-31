#pragma once
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace core
{
    class Parser
    {
    public:
        static Parser& instance(int argc = 0, char* argv[] = nullptr)
        {
            static Parser instance(argc, argv);
            return instance;
        }

        bool hasOption(const std::string& option) const { return options.find(option) != options.end(); }

        std::string getOptionValue(const std::string& option, const std::string& defaultValue = "") const
        {
            auto it = options.find(option);
            if (it != options.end())
            {
                return it->second;
            }
            return defaultValue;
        }

        std::string getRequiredOptionValue(const std::string& option) const
        {
            auto it = options.find(option);
            if (it != options.end())
            {
                return it->second;
            }
            throw std::invalid_argument("Required option not found: " + option);
        }

        std::string getOptionsString() const
        {
            std::string result;
            for (const auto& option : options)
            {
                result += option.first + ": " + option.second + "\n";
            }
            return result;
        }

    private:
        Parser(int argc, char* argv[]) { parse(argc, argv); }

        Parser(const Parser&)            = delete;
        Parser& operator=(const Parser&) = delete;

        void parse(int argc, char* argv[])
        {
            for (int i = 1; i < argc; ++i)
            {
                std::string arg = argv[i];
                if (arg.starts_with("--"))
                {
                    std::string key   = arg.substr(2);
                    std::string value = "";
                    if (i + 1 < argc && std::string(argv[i + 1]).starts_with("-") == false)
                    {
                        value = argv[++i];
                    }
                    options[key] = value;
                }
                else if (arg.starts_with("-"))
                {
                    std::string key   = arg.substr(1);
                    std::string value = "";
                    if (i + 1 < argc && std::string(argv[i + 1]).starts_with("-") == false)
                    {
                        value = argv[++i];
                    }
                    options[key] = value;
                }
                else
                {
                    positionalArgs.push_back(arg);
                }
            }
        }

        std::unordered_map<std::string, std::string> options;
        std::vector<std::string>                     positionalArgs;
    };
} // namespace core
