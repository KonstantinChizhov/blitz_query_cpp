#include <string>
#include <stdint.h>
#include <util/enum.hpp>

namespace blitz_query_cpp
{
    enum class syntax_node_type : uint64_t
    {
        None = 0,
        Document = 1ull << 0,
        Comment = 1ull << 1,
        Name = 1ull << 2,
        Variable = 1ull << 3,
        Argument = 1ull << 4,
        SelectionSet = 1ull << 5,
        Field = 1ull << 6,
        ObjectField = 1ull << 7,
        FragmentSpread = 1ull << 8,
        Directive = 1ull << 9,
        StringValue = 1ull << 10,
        IntValue = 1ull << 11,
        FloatValue = 1ull << 12,
        BoolValue = 1ull << 13,
        NullValue = 1ull << 14,
        ListValue = 1ull << 15,
        EnumValue = 1ull << 16,
        ObjectValue = 1ull << 17,
        NamedType = 1ull << 18,
        ListType = 1ull << 19,
        NonNullType = 1ull << 20,
        FragmentDefinition = 1ull << 21,
        OperationDefinition = 1ull << 22,
        VariableDefinition = 1ull << 23,
        InputObjectTypeExtension = 1ull << 24,
        OperationTypeDefinition = 1ull << 25,
        InterfaceTypeExtension = 1ull << 26,
        SchemaDefinition = 1ull << 27,
        ScalarTypeDefinition = 1ull << 28,
        ObjectTypeExtension = 1ull << 29,
        ScalarTypeExtension = 1ull << 30,
        SchemaExtension = 1ull << 31,
        UnionTypeExtension = 1ull << 32,
        DirectiveDefinition = 1ull << 33,
        ObjectTypeDefinition = 1ull << 34,
        EnumTypeExtension = 1ull << 35,
        EnumTypeDefinition = 1ull << 36,
        FieldDefinition = 1ull << 37,
        InputObjectTypeDefinition = 1ull << 38,
        InputValueDefinition = 1ull << 39,
        InterfaceTypeDefinition = 1ull << 40,
        UnionTypeDefinition = 1ull << 41,
        EnumValueDefinition = 1ull << 42,
        ListNullability = 1ull << 43,
        OptionalModifier = 1ull << 44,
        RequiredModifier = 1ull << 45,

        ValueNode = StringValue | IntValue | FloatValue | BoolValue | NullValue | ListValue | EnumValue | ObjectValue,
        ShcemaNode = InputObjectTypeExtension | SchemaDefinition | InterfaceTypeExtension | ScalarTypeDefinition | ObjectTypeExtension | ScalarTypeExtension | SchemaExtension | UnionTypeExtension | DirectiveDefinition | ObjectTypeDefinition | EnumTypeExtension | EnumTypeDefinition | FieldDefinition | InputObjectTypeDefinition | InputValueDefinition | InterfaceTypeDefinition | UnionTypeDefinition | EnumValueDefinition,
    };

    DECLARE_ENUM_OPERATIONS(syntax_node_type);

    std::string enum_name(syntax_node_type);

}