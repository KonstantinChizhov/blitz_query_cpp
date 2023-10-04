#pragma once
#include <string>
#include <util/enum.hpp>

namespace blitz_query_cpp
{
    enum class token_type : uint32_t
    {
        None = 0,
        Name = (1 << 0),              // unquoted name string
        LBrace = (1 << 1),            // {
        RBrace = (1 << 2),            // }
        LParen = (1 << 3),            // (
        RParen = (1 << 4),            // )
        LBracket = (1 << 5),          // [ list begin
        RBracket = (1 << 6),          // ] list end
        NotNull = (1 << 7),           // !
        Union = (1 << 8),             // |
        Colon = (1 << 9),             // :
        Equal = (1 << 10),            // =
        StringLiteral = (1 << 11),    // "Hello"
        StringBlock = (1 << 12),      // """ Hello. "world" """
        IntLiteral = (1 << 13),       // 42
        FloatLiteral = (1 << 14),     // 3.14
        ParameterLiteral = (1 << 15), // $id
        MemberAccess = (1 << 16),     // foo.bar
        FragmentInclude = (1 << 17),  // ...fragment
        Directive = (1 << 18),        // @include | @skip
        End = (1 << 19),              // end of dodument
        Comment = (1 << 20),          // #comment<newline>
        InvalidToken = (1 << 21),     // Error indicator
    };

    DECLARE_ENUM_OPERATIONS(token_type)

    std::string enum_name(token_type);
}