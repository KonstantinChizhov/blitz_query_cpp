#pragma once
#include <string>
#include <vector>
#include <charconv>
#include <format>
#include <span>
#include <string_view>
#include <serialization/writer_options.hpp>

namespace blitz_query_cpp
{
    template <class WriterT>
    class json_writer : public WriterT
    {
        static constexpr size_t cvt_buffer_size = 256;

        void write_string(std::string_view v)
        {
            write_char('"');
            for (char c : v)
            {
                switch (c)
                {
                case '\n':
                    write_char('\\');
                    write_char('n');
                    continue;
                case '\r':
                    write_char('\\');
                    write_char('r');
                    continue;
                case '\b':
                    write_char('\\');
                    write_char('b');
                    continue;
                case '\f':
                    write_char('\\');
                    write_char('f');
                    continue;
                case '\t':
                    write_char('\\');
                    write_char('t');
                    continue;
                case '\\':
                case '/':
                case '"':
                    write_char('\\');
                    break;
                default:
                    break;
                }
                write_char(c);
            }
            write_char('"');
        }
        int _nesting_level = 0;
        using WriterT::_options;
        bool _first_value_in_object = true;

    public:
        using WriterT::write;
        using WriterT::write_char;
        using WriterT::write_indent;
        using WriterT::write_newline;

        json_writer(const writer_options &options)
            : WriterT(options)
        {
        }

        void write_name(std::string_view name)
        {
            if (!_first_value_in_object)
                write_char(',');
            _first_value_in_object = false;
            write_newline();
            write_indent(_nesting_level);
            write_string(name);
            if (_options.format)
                write_char(' ');
            write_char(':');
            if (_options.format)
                write_char(' ');
        }

        template <class T>
        bool write_value(std::string_view name, T v)
        {
            write_name(name);
            write_value(v);
            return true;
        }

        template <class T>
        void write_value(T v)
        {
            char cvt_buffer[cvt_buffer_size];
            auto res = std::to_chars(std::begin(cvt_buffer), std::end(cvt_buffer), v);
            write(std::string_view(std::begin(cvt_buffer), res.ptr));
        }

        void write_value(bool v)
        {
            write(v ? "true" : "false");
        }

        void write_value(std::string_view v)
        {
            write_string(v);
        }

        bool write_value(std::string_view name, std::string_view v)
        {
            write_name(name);
            write_string(v);
            return true;
        }

        bool write_str_value(std::string_view name, std::string_view v)
        {
            write_name(name);
            write_string(v);
            return true;
        }

        void begin_object()
        {
            _nesting_level++;
            _first_value_in_object = true;
            write_char('{');
        }

        void begin_object(std::string_view name)
        {
            write_name(name);
            begin_object();
        }

        void end_object()
        {
            _nesting_level--;
            write_newline();
            write_indent(_nesting_level);
            write_char('}');
        }

        void begin_array()
        {
            _nesting_level++;
            _first_value_in_object = true;
            write_char('[');
        }

        void begin_array(std::string_view name)
        {
            write_name(name);
            begin_array();
        }

        void end_array()
        {
            _nesting_level--;
            write_newline();
            write_indent(_nesting_level);
            write_char(']');
        }

        template <class T>
        void write_array(std::string_view name, std::span<const T> values)
        {
            write_name(name);
            write_char('[');
            bool first = true;
            for (const auto &value : values)
            {
                if (!first)
                {

                    write_char(',');
                    if (_options.format)
                        write_char(' ');
                }
                first = false;
                write_value(value);
            }
            write_char(']');
        }

        template <class T>
        void write_array(std::string_view name, std::initializer_list<T> values)
        {
            write_array(name, std::span<const T>(values.begin(), values.end()));
        }
    };
}