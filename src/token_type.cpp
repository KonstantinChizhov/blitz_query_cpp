#include <lexica/token_type.hpp>

namespace blitz_query_cpp
{

    std::string enum_name(token_type type)
    {
        switch (type)
        {
        case token_type::None:
            return "None";
        case token_type::Name:
            return "Name";
        case token_type::LBrace:
            return "LBrace";
        case token_type::RBrace:
            return "RBrace";
        case token_type::LParen:
            return "LParen";
        case token_type::RParen:
            return "RParen";
        case token_type::LBracket:
            return "LBracket";
        case token_type::RBracket:
            return "RBracket";
        case token_type::NotNull:
            return "NotNull";
        case token_type::Union:
            return "Union";
        case token_type::Colon:
            return "Colon";
        case token_type::Equal:
            return "Equal";
        case token_type::StringLiteral:
            return "StringLiteral";
        case token_type::StringBlock:
            return "StringBlock";
        case token_type::IntLiteral:
            return "IntLiteral";
        case token_type::FloatLiteral:
            return "FloatLiteral";
        case token_type::ParameterLiteral:
            return "ParameterLiteral";
        case token_type::MemberAccess:
            return "MemberAccess";
        case token_type::FragmentSpread:
            return "FragmentSpread";
        case token_type::Directive:
            return "Directive";
        case token_type::End:
            return "End";
        case token_type::Comment:
            return "Comment";
        case token_type::InvalidToken:
            return "InvalidToken";

        default:
            return "Unknown";
        }
    }
}