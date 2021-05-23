#include "Application.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stack>
#include <string>

using namespace std::string_literals;

namespace bf
{
    using byte = char;

    void interpret(std::string const& program, std::size_t buffer_size = 30000)
    {
        auto buffer = std::vector<byte>(buffer_size);
        auto bp = std::begin(buffer);

        auto const begin = std::cbegin(program);
        auto const end = std::cend(program);
        auto ip = begin;

        while (ip != end)
        {
            switch (*ip)
            {
            default:
                throw std::runtime_error{ "Unrecognised command: " + std::to_string(*ip) };

            case '>':
                ++bp;
                ++ip;
                break;

            case '<':
                --bp;
                ++ip;
                break;

            case '+':
                ++*bp;
                ++ip;
                break;

            case '-':
                --*bp;
                ++ip;
                break;

            case '.':
                std::cout << *bp;
                ++ip;
                break;

            case ',':
                std::cin >> *bp;
                ++ip;
                break;

            case '[':
                if (*bp == '\0')
                {
                    auto bracket_level = 0;
                    do
                    {
                        if (ip == end)
                        {
                            throw std::runtime_error{ "Unmatched opening bracket" };
                        }

                        if (*ip == '[')
                        {
                            ++bracket_level;
                        }
                        else if (*ip == ']')
                        {
                            --bracket_level;
                            if (bracket_level < 0)
                            {
                                throw std::runtime_error{ "Unmatched closing bracket" };
                            }
                            if (bracket_level == 0)
                            {
                                ++ip;
                                break;
                            }
                        }
                        ++ip;
                    } while (true);
                }
                else
                {
                    ++ip;
                }

                break;

            case ']':
                if (*bp != '\0')
                {
                    auto bracket_level = 0;
                    do
                    {
                        if (*ip == ']')
                        {
                            ++bracket_level;
                        }
                        else if (*ip == '[')
                        {
                            --bracket_level;
                            if (bracket_level == 0)
                            {
                                ++ip;
                                break;
                            }
                        }

                        if (ip == begin)
                        {
                            throw std::runtime_error{ "Unmatched closing bracket" };
                        }
                        --ip;
                    } while (true);
                }
                else
                {
                    ++ip;
                }
                break;
            }
        }
    }

    int Application::run(int argc, char* argv[])
    {
        if (argc < 2)
        {
            std::cout << "Usage: bf <filename>" << std::endl;
            return 1;
        }

        auto const input_file = std::filesystem::path{ argv[1] };
        auto fs = std::ifstream{ input_file };
        auto program = std::string{};
        fs >> program;
        fs.close();

        interpret(program);
        return 0;
    }
}
