#pragma once
#include <string>
#include <util/enum.hpp>

namespace blitz_query_cpp
{
    enum class directive_target_t
    {
        None = 0,
        IsRepeatable = 1 << 0,

        Query = 1 << 1,
        Mutation = 1 << 2,
        Subscription = 1 << 3,
        Field = 1 << 4,
        FragmentDefinition = 1 << 5,
        FragmentSpread = 1 << 6,
        InlineFragment = 1 << 7,
        VariableDefinition = 1 << 8,

        Schema = 1 << 16,
        Scalar = 1 << 17,
        Object = 1 << 18,
        FieldDefinition = 1 << 19,
        ArgumentDefinition = 1 << 20,
        Interface = 1 << 21,
        Union = 1 << 22,
        Enum = 1 << 23,
        EnumValue = 1 << 24,
        InputObject = 1 << 25,
        InputFieldDefinition = 1 << 26,

        Executable =
            Field |
            FragmentDefinition |
            FragmentSpread |
            InlineFragment |
            Mutation |
            Query |
            Subscription |
            VariableDefinition,

        TypeSystem =
            ArgumentDefinition |
            Enum |
            EnumValue |
            FieldDefinition |
            InputFieldDefinition |
            InputObject |
            Interface |
            Object |
            Scalar |
            Schema |
            Union,

        Operation =
            Query |
            Mutation |
            Subscription,

        Fragment =
            InlineFragment |
            FragmentSpread |
            FragmentDefinition,
    };

    std::string enum_name(directive_target_t);
    directive_target_t parse_directive_target(std::string_view);

    DECLARE_ENUM_OPERATIONS(directive_target_t)
}