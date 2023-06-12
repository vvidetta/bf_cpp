// Copyright (c) 2023 Vito Videtta. All rights reserved.
#include "Application.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std::string_literals;

namespace bf
{
    class abstract_machine
    {
    public:

        using byte = char;
        using bf_buffer = std::vector<byte>;
        using bf_program = std::string;
        using buffer_pointer = bf_buffer::iterator;
        using instruction_pointer = bf_program::const_iterator;

        abstract_machine(bf_buffer& buffer, bf_program const& program)
            : m_buffer{ buffer },
            m_bp{ std::begin(buffer) },
            m_program{ program },
            m_begin{ std::cbegin(program) },
            m_end{ std::cend(program) },
            m_ip{ std::cbegin(program) }
        {
        }

        [[nodiscard]] byte current_instruction() const noexcept
        {
            return *m_ip;
        }

        [[nodiscard]] bool finished() const noexcept
        {
            return m_ip == m_end;
        }

        void increment_buffer_pointer()
        {
            ++m_bp;
            ++m_ip;
        }

        void decrement_buffer_pointer()
        {
            --m_bp;
            ++m_ip;
        }

        void increment_buffer_data()
        {
            ++*m_bp;
            ++m_ip;
        }

        void decrement_buffer_data()
        {
            --*m_bp;
            ++m_ip;
        }

        void read_byte()
        {
            std::cin >> *m_bp;
            ++m_ip;
        }

        void write_byte()
        {
            std::cout << *m_bp;
            ++m_ip;
        }

        void start_loop()
        {
            if (*m_bp == '\0')
            {
                auto bracket_level = 0;
                do
                {
                    if (m_ip == m_end)
                    {
                        throw std::runtime_error{ "Unmatched opening bracket" };
                    }

                    if (*m_ip == '[')
                    {
                        ++bracket_level;
                    }
                    else if (*m_ip == ']')
                    {
                        --bracket_level;
                        if (bracket_level < 0)
                        {
                            throw std::runtime_error{ "Unmatched closing bracket" };
                        }
                        if (bracket_level == 0)
                        {
                            ++m_ip;
                            break;
                        }
                    }
                    ++m_ip;
                } while (true);
            }
            else
            {
                ++m_ip;
            }
        }

        void end_loop()
        {
            if (*m_bp != '\0')
            {
                auto bracket_level = 0;
                do
                {
                    if (*m_ip == ']')
                    {
                        ++bracket_level;
                    }
                    else if (*m_ip == '[')
                    {
                        --bracket_level;
                        if (bracket_level == 0)
                        {
                            ++m_ip;
                            break;
                        }
                    }

                    if (m_ip == m_begin)
                    {
                        throw std::runtime_error{ "Unmatched closing bracket" };
                    }
                    --m_ip;
                } while (true);
            }
            else
            {
                ++m_ip;
            }
        }

    private:
        bf_buffer& m_buffer;
        buffer_pointer m_bp;

        bf_program const& m_program;
        instruction_pointer const m_begin;
        instruction_pointer const m_end;
        instruction_pointer m_ip;
    };

    class interpreter
    {
    public:
        void interpret(abstract_machine& am)
        {
            while (!am.finished())
            {
                auto const instruction = am.current_instruction();
                switch (instruction)
                {
                default:
                    throw std::runtime_error{ "Unrecognised command: " + std::to_string(instruction) };

                case '>':
                    am.increment_buffer_pointer();
                    break;

                case '<':
                    am.decrement_buffer_pointer();
                    break;

                case '+':
                    am.increment_buffer_data();
                    break;

                case '-':
                    am.decrement_buffer_data();
                    break;

                case '.':
                    am.write_byte();
                    break;

                case ',':
                    am.read_byte();
                    break;

                case '[':
                    am.start_loop();
                    break;

                case ']':
                    am.end_loop();
                    break;
                }
            }
        }
    };

    void interpret(abstract_machine::bf_buffer& buffer, abstract_machine::bf_program& program)
    {
        auto inter = interpreter{};
        auto am = abstract_machine{ buffer, program };
        inter.interpret(am);
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
        auto program = abstract_machine::bf_program{};
        fs >> program;
        fs.close();

        auto buffer = abstract_machine::bf_buffer(30000);
        interpret(buffer, program);
        return 0;
    }
}
