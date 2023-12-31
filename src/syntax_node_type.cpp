
#include <syntax/syntax_node_type.hpp>

namespace blitz_query_cpp
{
    std::string enum_name(syntax_node_type type)
    {
        switch (type)
        {
        case syntax_node_type::None:
            return "None";
        case syntax_node_type::Document:
            return "Document";
        case syntax_node_type::Comment:
            return "Comment";
        case syntax_node_type::Name:
            return "Name";
        case syntax_node_type::Variable:
            return "Variable";
        case syntax_node_type::Argument:
            return "Argument";
        case syntax_node_type::SelectionSet:
            return "SelectionSet";
        case syntax_node_type::Field:
            return "Field";
        case syntax_node_type::ObjectField:
            return "ObjectField";
        case syntax_node_type::FragmentSpread:
            return "FragmentSpread";
        case syntax_node_type::Directive:
            return "Directive";
        case syntax_node_type::StringValue:
            return "StringValue";
        case syntax_node_type::IntValue:
            return "IntValue";
        case syntax_node_type::FloatValue:
            return "FloatValue";
        case syntax_node_type::BoolValue:
            return "BoolValue";
        case syntax_node_type::NullValue:
            return "NullValue";
        case syntax_node_type::ListValue:
            return "ListValue";
        case syntax_node_type::EnumValue:
            return "EnumValue";
        case syntax_node_type::ObjectValue:
            return "ObjectValue";
        case syntax_node_type::NamedType:
            return "NamedType";
        case syntax_node_type::ListType:
            return "ListType";
        case syntax_node_type::NonNullType:
            return "NonNullType";
        case syntax_node_type::FragmentDefinition:
            return "FragmentDefinition";
        case syntax_node_type::OperationDefinition:
            return "OperationDefinition";
        case syntax_node_type::VariableDefinition:
            return "VariableDefinition";
        case syntax_node_type::InputObjectTypeExtension:
            return "InputObjectTypeExtension";
        case syntax_node_type::OperationTypeDefinition:
            return "OperationTypeDefinition";
        case syntax_node_type::InterfaceTypeExtension:
            return "InterfaceTypeExtension";
        case syntax_node_type::SchemaDefinition:
            return "SchemaDefinition";
        case syntax_node_type::ScalarTypeDefinition:
            return "ScalarTypeDefinition";
        case syntax_node_type::ObjectTypeExtension:
            return "ObjectTypeExtension";
        case syntax_node_type::ScalarTypeExtension:
            return "ScalarTypeExtension";
        case syntax_node_type::SchemaExtension:
            return "SchemaExtension";
        case syntax_node_type::UnionTypeExtension:
            return "UnionTypeExtension";
        case syntax_node_type::DirectiveDefinition:
            return "DirectiveDefinition";
        case syntax_node_type::ObjectTypeDefinition:
            return "ObjectTypeDefinition";
        case syntax_node_type::EnumTypeExtension:
            return "EnumTypeExtension";
        case syntax_node_type::EnumTypeDefinition:
            return "EnumTypeDefinition";
        case syntax_node_type::FieldDefinition:
            return "FieldDefinition";
        case syntax_node_type::InputObjectTypeDefinition:
            return "InputObjectTypeDefinition";
        case syntax_node_type::InputValueDefinition:
            return "InputValueDefinition";
        case syntax_node_type::InterfaceTypeDefinition:
            return "InterfaceTypeDefinition";
        case syntax_node_type::UnionTypeDefinition:
            return "UnionTypeDefinition";
        case syntax_node_type::EnumValueDefinition:
            return "EnumValueDefinition";
        case syntax_node_type::ListNullability:
            return "ListNullability";
        case syntax_node_type::OptionalModifier:
            return "OptionalModifier";
        case syntax_node_type::RequiredModifier:
            return "RequiredModifier";

        case syntax_node_type::ValueNode:
        case syntax_node_type::ShcemaNode:
        default:
            return "Unknown";
        }
        return "Unknown";
    }
}