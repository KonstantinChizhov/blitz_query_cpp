#pragma once
#include <string_view>
#include <lexica/token.hpp>

namespace blitz_query_cpp
{
    class tokenizer_t
    {
        std::string_view query;
        index_t current_pos = 0;
        index_t line_number = 0;
        index_t line_start = 0;
        inline auto chars_left() { return query.size() - current_pos; }

    public:
        tokenizer_t(std::string_view query_)
            : query(query_)
        {
            // skip utf-8 BOM
            if (query.size() > 3 && query[0] == '\xEF')
            {
                if (query[1] == '\xBB' && query[2] == '\xBF')
                    current_pos += 3;
            }
        }

        token next_token();
        index_t get_line_number() const { return line_number; }
        index_t get_pos_in_line() const { return current_pos - line_start; }

    private:
        inline void eat_whitespace();
        inline token single_char_token(token_type type);
        inline token handle_dot();
        inline token read_name(index_t skipChars, token_type type);
        inline token read_comment();
        inline token handle_eq();
        inline token read_string();
        inline token invalid_token_before(index_t offset);
        inline token read_number();
        inline std::string_view unescape_string_value(std::string_view value);
    };
}