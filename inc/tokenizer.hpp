#pragma once
#include <string_view>
#include "token.hpp"

namespace blitz_query_cpp
{
    inline bool is_whitespace(char ch)
    {
        return std::isspace(ch);
    }

    inline bool is_whitespace(wchar_t ch)
    {
        return std::iswspace(ch);
    }

    inline bool is_name_char(char ch)
    {
        return std::isalnum(ch) || ch == '_';
    }

    inline bool is_name_char(wchar_t ch)
    {
        return std::iswalnum(ch) || ch == L'_';
    }

    inline bool is_name_first_char(char ch)
    {
        return std::isalpha(ch) || ch == '_';
    }

    inline bool is_name_first_char(wchar_t ch)
    {
        return std::iswalpha(ch) || ch == L'_';
    }

    inline bool is_newline_char(char ch)
    {
        return ch == '\n' || ch == '\r';
    }

    inline bool is_newline_char(wchar_t ch)
    {
        return ch == L'\n' || ch == L'\r';
    }

    template <typename CharT>
    class tokenizer
    {
        std::basic_string_view<CharT> query;
        index_t current_pos = 0;
        using token_t = token<CharT>;

    public:
        tokenizer(std::basic_string_view<CharT> query)
            : query(query)
        {
        }

        token_t next_token()
        {
            eat_whitespace();

            if (current_pos >= query.size())
            {
                return token_t(token_type::End);
            }

            CharT ch = query[current_pos];
            switch (ch)
            {
            case '\0':
                current_pos = query.size();
                return token_t(token_type::End);
            case '{':
                return single_char_token(token_type::LBrace);
            case '}':
                return single_char_token(token_type::RBrace);
            case '(':
                return single_char_token(token_type::LParen);
            case ')':
                return single_char_token(token_type::RParen);
            case ':':
                return single_char_token(token_type::Colon);
            case ',':
                return single_char_token(token_type::Comma);
            case '.':
                return handle_dot();
            case '=':
                return handle_eq();
            case '#':
                return read_comment();
            case '$':
                return read_name(1, token_type::ParameterLiteral);
            case '@':
                return read_name(1, token_type::Directive);
            case '"':
                return read_string();
            default:
                if (std::isdigit(ch) || ch == '-')
                {
                    return read_number();
                }
                else
                {
                    return read_name(0, token_type::Name);
                }
            }
        }

    private:
        void eat_whitespace()
        {
            while (current_pos < query.size())
            {
                CharT ch = query[current_pos];
                if (!is_whitespace(ch))
                {
                    break;
                }
                current_pos++;
            }
        }

        token_t single_char_token(token_type type)
        {
            index_t current = current_pos;
            current_pos++;
            return token_t(query.substr(current, 1), current, 1, type);
        }

        token_t handle_dot()
        {
            if (query.size() - current_pos > 3)
            {
                // ...fragmentName
                if (query[current_pos + 1] == '.' && query[current_pos + 2] == '.')
                {
                    return read_name(3, token_type::FragmentInclude);
                }
            }
            return single_char_token(token_type::MemberAccess);
        }

        token_t read_name(index_t skipChars, token_type type)
        {
            index_t start_pos = current_pos;
            current_pos += skipChars;
            if (current_pos < query.size())
            {
                CharT ch = query[current_pos];
                if (!is_name_first_char(ch))
                {
                    return token_t(query.substr(current_pos, 1), current_pos, 1, token_type::InvalidToken);
                }
                current_pos++;
            }

            while (current_pos < query.size())
            {
                CharT ch = query[current_pos];
                if (!is_name_char(ch))
                {
                    break;
                }
                current_pos++;
            }
            index_t len = current_pos - start_pos;
            return token_t(query.substr(start_pos, len), start_pos, len, type);
        }

        token_t read_comment()
        {
            index_t start_pos = current_pos;

            while (current_pos < query.size())
            {
                CharT ch = query[current_pos];
                if (is_newline_char(ch))
                {
                    break;
                }
                current_pos++;
            }
            index_t len = current_pos - start_pos;
            return token_t(query.substr(start_pos, len), start_pos, len, token_type::Comment);
        }

        token_t handle_eq()
        {
            return token_t(query.substr(current_pos, 1), current_pos++, 1, token_type::Equal);
        }

        token_t read_string()
        {
            current_pos++;
            index_t start_pos = current_pos;
            while (current_pos < query.size())
            {
                CharT ch = query[current_pos];
                if (ch == '\\') // escape sequence will be decoded later
                {
                    current_pos += 2; // skip backslash and escaped char
                    if (current_pos >= query.size())
                    {
                        return token_t(query.substr(current_pos - 1, 1), current_pos - 1, 1, token_type::InvalidToken);
                    }
                    ch = query[current_pos];
                }
                current_pos++;
                if (ch == '"')
                {
                    break;
                }
            }
            index_t len = current_pos - start_pos - 1;
            return token_t(query.substr(start_pos, len), start_pos, len, token_type::StringLiteral);
        }

        token_t read_number()
        {
            index_t start_pos = current_pos;
            token_type type = token_type::IntLiteral;
            if(query[current_pos] == '-')
            {
                current_pos++;
            }
            while (current_pos < query.size())
            {
                CharT ch = query[current_pos];
                if (ch == '.' && type == token_type::IntLiteral) // float
                {
                    current_pos++;
                    if (current_pos >= query.size())
                    {
                        return token_t(query.substr(current_pos - 1, 1), current_pos - 1, 1, token_type::InvalidToken);
                    }
                    ch = query[current_pos];
                    type = token_type::FloatLiteral;
                }
                if (!std::isdigit(ch))
                {
                    break;
                }
                current_pos++;
            }
            index_t len = current_pos - start_pos;
            return token_t(query.substr(start_pos, len), start_pos, len, type);
        }
    };
}