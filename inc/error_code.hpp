#pragma once

namespace blitz_query_cpp
{
    enum class error_code_t
    {
        OK,
        EmptyDocument,
        UnexpectedEndOfDocument,
        SyntaxError,
        UnmatchedBraces,
        UnexpectedToken,
        InvalidToken
    };
}