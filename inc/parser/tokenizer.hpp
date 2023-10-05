#pragma once
#include <string_view>
#include <lexica/token.hpp>

namespace blitz_query_cpp
{
    class tokenizer_t
    {
        std::string_view query;
        index_t current_pos = 0;
        auto chars_left() { return query.size() - current_pos; }

    public:

        tokenizer_t(std::string_view query_)
            : query(query_)
        {
            // skip utf-8 BOM
            if (query.size() > 3 && query[0] == 0xEF )
            {
                if (query[1] == 0xBB && query[2] == 0xBF)
                    current_pos += 3;
            }
        }

        token next_token();

    private:
        void eat_whitespace();
        token single_char_token(token_type type);
        token handle_dot();
        token read_name(index_t skipChars, token_type type);
        token read_comment();
        token handle_eq();
        token read_string();
        token invalid_token_before(index_t offset);
        token read_number();
        std::string_view unescape_string_value(std::string_view value);
    };
}