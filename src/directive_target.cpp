#include <syntax/directive_target.hpp>
#include <util/crc_hash.hpp>

namespace blitz_query_cpp
{
    std::string enum_name(directive_target_t target)
    {
        switch (target)
        {
        case directive_target_t::None:
            return "None";
        case directive_target_t::Query:
            return "QUERY";
        case directive_target_t::Mutation:
            return "MUTATION";
        case directive_target_t::Subscription:
            return "SUBSCRIPTION";
        case directive_target_t::Field:
            return "FIELD";
        case directive_target_t::FragmentDefinition:
            return "FRAGMENT_DEFINITION";
        case directive_target_t::FragmentSpread:
            return "FRAGMENT_SPREAD";
        case directive_target_t::InlineFragment:
            return "INLINE_FRAGMENT";
        case directive_target_t::VariableDefinition:
            return "VARIABLE_DEFINITION";

        case directive_target_t::Schema:
            return "SCHEMA";
        case directive_target_t::Scalar:
            return "SCALAR";
        case directive_target_t::Object:
            return "OBJECT";
        case directive_target_t::FieldDefinition:
            return "FIELD_DEFINITION";
        case directive_target_t::ArgumentDefinition:
            return "ARGUMENT_DEFINITION";
        case directive_target_t::Interface:
            return "INTERFACE";
        case directive_target_t::Union:
            return "UNION";
        case directive_target_t::Enum:
            return "ENUM";
        case directive_target_t::EnumValue:
            return "ENUM_VALUE";
        case directive_target_t::InputObject:
            return "INPUT_OBJECT";
        case directive_target_t::InputFieldDefinition:
            return "INPUT_FIELD_DEFINITION";
        case directive_target_t::IsRepeatable:
        case directive_target_t::Executable:
        case directive_target_t::TypeSystem:
        case directive_target_t::Operation:
        case directive_target_t::Fragment:
        default:
            return "Unknown";
        }
        return "Unknown";
    }

    directive_target_t parse_directive_target(std::string_view str)
    {
        switch (hash_crc32(str))
        {
        case "QUERY"_crc32:
            return directive_target_t::Query;
        case "MUTATION"_crc32:
            return directive_target_t::Mutation;
        case "SUBSCRIPTION"_crc32:
            return directive_target_t::Subscription;
        case "FIELD"_crc32:
            return directive_target_t::Field;
        case "FRAGMENT_DEFINITION"_crc32:
            return directive_target_t::FragmentDefinition;
        case "FRAGMENT_SPREAD"_crc32:
            return directive_target_t::FragmentSpread;
        case "INLINE_FRAGMENT"_crc32:
            return directive_target_t::InlineFragment;
        case "VARIABLE_DEFINITION"_crc32:
            return directive_target_t::VariableDefinition;

        case "SCHEMA"_crc32:
            return directive_target_t::Schema;
        case "SCALAR"_crc32:
            return directive_target_t::Scalar;
        case "OBJECT"_crc32:
            return directive_target_t::Object;
        case "FIELD_DEFINITION"_crc32:
            return directive_target_t::FieldDefinition;
        case "ARGUMENT_DEFINITION"_crc32:
            return directive_target_t::ArgumentDefinition;
        case "INTERFACE"_crc32:
            return directive_target_t::Interface;
        case "UNION"_crc32:
            return directive_target_t::Union;
        case "ENUM"_crc32:
            return directive_target_t::Enum;
        case "ENUM_VALUE"_crc32:
            return directive_target_t::EnumValue;
        case "INPUT_OBJECT"_crc32:
            return directive_target_t::InputObject;
        case "INPUT_FIELD_DEFINITION"_crc32:
            return directive_target_t::InputFieldDefinition;
        }
        return directive_target_t::None;
    }
}