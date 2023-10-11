#include <parser/tokenizer.hpp>

using namespace blitz_query_cpp;

inline bool is_whitespace(char ch)
{
    return std::isspace(ch) || ch == ',';
}

inline bool is_name_char(char ch)
{
    return std::isalnum(ch) || ch == '_';
}

inline bool is_name_first_char(char ch)
{
    return std::isalpha(ch) || ch == '_';
}

inline bool is_newline_char(char ch)
{
    return ch == '\n' || ch == '\r';
}

token tokenizer_t::next_token()
{
    eat_whitespace();

    if (current_pos >= query.size())
    {
        return token(std::string_view(), query.size(), 0, token_type::End);
    }

    char ch = query[current_pos];
    switch (ch)
    {
    case '\0':
        current_pos = query.size();
        return token(token_type::End);
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
    case '[':
        return single_char_token(token_type::LBracket);
    case ']':
        return single_char_token(token_type::RBracket);
    case '!':
        return single_char_token(token_type::NotNull);
    case '|':
        return single_char_token(token_type::Union);
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
        if (std::isdigit(ch) || ch == '-' || ch == '+')
        {
            return read_number();
        }
        else
        {
            return read_name(0, token_type::Name);
        }
    }
}

void tokenizer_t::eat_whitespace()
{
    if (current_pos >= query.size())
        return;
    do
    {
        char ch = query[current_pos];
        if (!is_whitespace(ch))
        {
            break;
        }
        current_pos++;
    }while (current_pos < query.size());
}

token tokenizer_t::single_char_token(token_type type)
{
    index_t current = current_pos;
    current_pos++;
    return token(query.substr(current, 1), current, 1, type);
}

token tokenizer_t::handle_dot()
{
    if (query.size() - current_pos > 3)
    {
        // ...fragmentName
        if (query[current_pos + 1] == '.' && query[current_pos + 2] == '.')
        {
            index_t current = current_pos;
            current_pos += 3;
            return token(query.substr(current, 3), current, 3, token_type::FragmentSpread);
        }
    }
    return single_char_token(token_type::MemberAccess);
}

token tokenizer_t::read_name(index_t skipChars, token_type type)
{
    index_t start_pos = current_pos;
    current_pos += skipChars;
    if (current_pos < query.size())
    {
        char ch = query[current_pos];
        if (!is_name_first_char(ch))
        {
            return token(query.substr(current_pos, 1), current_pos, 1, token_type::InvalidToken);
        }
        current_pos++;
    }

    while (current_pos < query.size())
    {
        char ch = query[current_pos];
        if (!is_name_char(ch))
        {
            break;
        }
        current_pos++;
    }
    index_t len = current_pos - start_pos;
    return token(query.substr(start_pos, len), start_pos, len, type);
}

token tokenizer_t::read_comment()
{
    eat_whitespace();
    index_t start_pos = current_pos;
    while (current_pos < query.size())
    {
        char ch = query[current_pos];
        if (is_newline_char(ch))
        {
            break;
        }
        current_pos++;
    }
    index_t len = current_pos - start_pos;
    return token(query.substr(start_pos, len), start_pos, len, token_type::Comment);
}

token tokenizer_t::handle_eq()
{
    index_t pos = current_pos++;
    return token(query.substr(pos, 1), pos, 1, token_type::Equal);
}

token tokenizer_t::read_string()
{
    current_pos++;
    index_t start_pos = current_pos;
    bool block_string = false;
    if (chars_left() >= 5) // could not be a block string """ """
    {
        // check block string
        if (query[current_pos] == '"' && query[current_pos + 1] == '"')
        {
            block_string = true;
            current_pos += 2;
            start_pos += 2;
        }
    }

    while (current_pos < query.size())
    {
        char ch = query[current_pos];
        if (ch == '\\') // escape sequence will be decoded later
        {
            // TODO: unicode codepoints \u
            current_pos += 2; // skip backslash and escaped char
            if (current_pos >= query.size())
            {
                return invalid_token_before(1);
            }
            ch = query[current_pos];
        }
        current_pos++;
        if (ch == '"')
        {
            if (!block_string)
            {
                break;
            }
            if (chars_left() < 2)
            {
                return invalid_token_before(0);
            }
            if (query[current_pos] == '"' && query[current_pos + 1] == '"') // block string end
            {
                break;
            }
        }
    }
    index_t len = current_pos - start_pos - 1;
    if (block_string)
    {
        current_pos += 2;
    }
    auto value = unescape_string_value(query.substr(start_pos, len));
    return token(value, start_pos, len, block_string ? token_type::StringBlock : token_type::StringLiteral);
}

token tokenizer_t::invalid_token_before(index_t offset)
{
    return token(query.substr(current_pos - offset, 1), current_pos - offset, 1, token_type::InvalidToken);
}

token tokenizer_t::read_number()
{
    index_t start_pos = current_pos;
    token_type type = token_type::IntLiteral;
    bool has_dot = false;
    bool has_exponent = false;

    if (query[current_pos] == '-')
    {
        current_pos++;
    }
    while (current_pos < query.size())
    {
        char ch = query[current_pos];
        if (ch == '.' && !has_dot) // float
        {
            if (has_exponent)
            {
                return invalid_token_before(0);
            }
            current_pos++;
            if (current_pos >= query.size())
            {
                return invalid_token_before(1);
            }
            current_pos++;
            ch = query[current_pos];
            has_dot = true;
        }
        if ((ch == 'e' || ch == 'E') && !has_exponent) // float exp
        {
            current_pos++;
            if (current_pos >= query.size())
            {
                return invalid_token_before(1);
            }
            if (ch == '-' || ch == '+') // exp sign
            {
                current_pos++;
                if (current_pos >= query.size())
                {
                    return invalid_token_before(1);
                }
            }
            ch = query[current_pos];
            has_exponent = true;
        }

        if (!std::isdigit(ch))
        {
            break;
        }
        current_pos++;
    }
    if (has_dot || has_exponent)
        type = token_type::FloatLiteral;
    index_t len = current_pos - start_pos;
    return token(query.substr(start_pos, len), start_pos, len, type);
}

std::string_view tokenizer_t::unescape_string_value(std::string_view value)
{
    // TODO: implement
    return value;
}