#include <string>

namespace blitz_query_cpp
{
    enum class syntax_node_type
    {
        None = 0,
        Document,
        Comment,
        Name,
        Variable,
        Argument,
        SelectionSet,
        Field,
        ObjectField,
        FragmentSpread,
        Directive,
        StringValue,
        IntValue,
        FloatValue,
        BoolValue,
        NullValue,
        ListValue,
        EnumValue,
        ObjectValue,
        NamedType,
        ListType,
        NonNullType,
        FragmentDefinition,
        OperationDefinition,
        VariableDefinition,
        InputObjectTypeExtension,
        OperationTypeDefinition,
        InterfaceTypeExtension,
        SchemaDefinition,
        ScalarTypeDefinition,
        ObjectTypeExtension,
        ScalarTypeExtension,
        SchemaExtension,
        UnionTypeExtension,
        DirectiveDefinition,
        ObjectTypeDefinition,
        EnumTypeExtension,
        EnumTypeDefinition,
        FieldDefinition,
        InputObjectTypeDefinition,
        InputValueDefinition,
        InterfaceTypeDefinition,
        UnionTypeDefinition,
        EnumValueDefinition,
        ListNullability,
        OptionalModifier,
        RequiredModifier,
    };

    std::string enum_name(syntax_node_type);
 
}