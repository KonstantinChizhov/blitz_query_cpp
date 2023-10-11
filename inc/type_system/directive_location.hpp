#pragma once

namespace blitz_query_cpp
{

    enum class directive_location
    {
        None = 0,
        ArgumentDefinition = (1 << 0),
        Enum = (1 << 2),
        EnumValue = (1 << 3),
        Field = (1 << 4),
        FieldDefinition = (1 << 5),
        FragmentDefinition = (1 << 6),
        FragmentSpread = (1 << 7),
        InlineFragment = (1 << 8),
        InputFieldDefinition = (1 << 9),
        InputObject = (1 << 10),
        Interface = (1 << 11),
        Mutation = (1 << 12),
        Object = (1 << 13),
        Query = (1 << 14),
        Scalar = (1 << 15),
        Schema = (1 << 16),
        Subscription = (1 << 17),
        Union = (1 << 18),
        VariableDefinition = (1 << 19),

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
}