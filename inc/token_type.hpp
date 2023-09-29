#pragma once
#include <string>

namespace blitz_query_cpp
{
    enum class token_type
    {
        None,
        Name,             // unquoted name string
        LBrace,           // {
        RBrace,           // }
        LParen,           // (
        RParen,           // )
        Colon,            // :
        Equal,            // =
        Comma,            // considered as whitespace
        StringLiteral,    // "Hello"
        IntLiteral,       // 42
        FloatLiteral,     // 3.14
        ParameterLiteral, // $id
        MemberAccess,     // foo.bar
        FragmentInclude,  // ...fragment
        Directive,        // @include | @skip
        End,              // end of dodument
        Comment,          // #comment<newline>
        InvalidToken,     // Error indicator
    };

    std::string token_type_name(token_type);
}