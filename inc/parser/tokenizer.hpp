#pragma once
#include <string_view>
#include <lexica/token.hpp>

namespace blitz_query_cpp
{
    class tokenizer
    {
        std::string_view query;
        index_t current_pos = 0;
        int chars_left() { return query.size() - current_pos; }

    public:
        tokenizer(const tokenizer &) = delete;
        tokenizer &operator=(const tokenizer &) = delete;

        tokenizer(std::string_view query)
            : query(query)
        {
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