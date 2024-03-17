#pragma once
#include <string>
#include <vector>
#include <charconv>
#include <serialization/writer_options.hpp>
#include <format>

namespace blitz_query_cpp
{
 

    class buffer_writer
    {
    protected:
        std::vector<char> _buffer;
        writer_options _options;
        char _indent_char;
        int _indent_chars_for_level;

        std::string error_msg;
        bool _has_error = false;

        template <class... Args>
        bool report_error(std::string_view fmt, Args &&...args)
        {
            _has_error = true;
            error_msg = std::vformat(fmt, std::make_format_args(args...));
            return false;
        }

    public:
        buffer_writer(const writer_options &options)
            : _options{options}
        {
            _buffer.reserve(_options.initial_capacity);
            _indent_char = _options.use_tabs ? '\t' : ' ';
            _indent_chars_for_level = _options.use_tabs ? 1 : _options.tab_spaces;
        }

        void write_char(char c) { _buffer.push_back(c); }
        void write(std::string_view str) { _buffer.insert(_buffer.end(), str.begin(), str.end()); }

        void write_newline()
        {
            if (!_options.format)
                return;
            _buffer.push_back('\r');
            _buffer.push_back('\n');
        }

        void write_indent(int level)
        {
            if (!_options.format)
                return;
            size_t count = _indent_chars_for_level * level;
            _buffer.insert(_buffer.end(), count, _indent_char);
        }

        std::string_view get_string() const { return std::string_view(_buffer.data(), _buffer.size()); }
        std::string_view get_error_msg() const { return error_msg; }
        bool has_error() const { return _has_error; }
    };
}
