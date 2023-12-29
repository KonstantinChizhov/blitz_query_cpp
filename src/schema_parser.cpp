
#include <type_system/schema_parser.hpp>
#include <parser/parser.hpp>

using namespace blitz_query_cpp;

bool schema_parser::parse(schema &schema, std::string_view schema_string)
{
    document doc{std::string(schema_string)};
    parser parser{doc};
    if (!parser.parse())
    {
        return false;
    }
    if (!process_doc(schema, doc))
    {
        return false;
    }
    return true;
}

bool schema_parser::process_doc(schema &schema, const document &doc)
{
    for (const syntax_node *definition : doc.children)
    {
        bool result = false;
        switch (definition->type)
        {
        case syntax_node_type::SchemaDefinition:
            result = process_schema_def(schema, *definition);
            break;
        case syntax_node_type::ScalarTypeDefinition:
            result = process_scalar_type_def(schema, *definition);
            break;
        case syntax_node_type::DirectiveDefinition:

            break;
        case syntax_node_type::ObjectTypeDefinition:

            break;
        case syntax_node_type::EnumTypeDefinition:
            result = process_enum_type_def(schema, *definition);
            break;
        case syntax_node_type::InputObjectTypeDefinition:

            break;
        case syntax_node_type::InputValueDefinition:

            break;
        case syntax_node_type::InterfaceTypeDefinition:

            break;
        case syntax_node_type::UnionTypeDefinition:

            break;

        case syntax_node_type::InputObjectTypeExtension:
        case syntax_node_type::EnumTypeExtension:
        case syntax_node_type::UnionTypeExtension:
        case syntax_node_type::ObjectTypeExtension:
        case syntax_node_type::ScalarTypeExtension:
        case syntax_node_type::SchemaExtension:
        case syntax_node_type::InterfaceTypeExtension:

        default:
            result = false;
        }

        if (!result)
        {
            return result;
        }
    }
    return true;
}

bool schema_parser::process_schema_def(schema &schema, const syntax_node &definition)
{
    for (syntax_node *child : definition.children)
    {
        if (child->type != syntax_node_type::OperationTypeDefinition)
        {
            // invalid schema
            return false;
        }
        if (child->children.size() < 1)
        {
            return false;
        }
        if (child->children[0]->type != syntax_node_type::NamedType)
        {
            return false;
        }
        if (child->operation_type == operation_type_t::Query)
        {
            schema.query_type_name = child->children[0]->content;
        }
        if (child->operation_type == operation_type_t::Mutation)
        {
            schema.mutation_type_name = child->children[0]->content;
        }
    }
    return true;
}

bool schema_parser::process_scalar_type_def(schema &schema, const syntax_node &definition)
{
    auto scalar = schema.create_type(type_kind::Scalar, definition.name);
    if (!scalar)
    {
        // non uniq name
        return false;
    }
    scalar->description = definition.description;
    if (!process_directives(*scalar, definition))
        return false;
    return true;
}

bool schema_parser::process_enum_value(object_type &enum_type, const syntax_node &enum_field_node)
{
    auto field_res = enum_type.fields.try_emplace(std::string(enum_field_node.name));
    if (!field_res.second)
        return report_error("redefiniton of value {} in enum {} at {}", enum_field_node.name, enum_type.name, enum_field_node.pos);
    auto &field = field_res.first->second;
    field.name = enum_field_node.name;
    field.description = enum_field_node.description;
    return process_directives(field, enum_field_node);
}

bool schema_parser::process_enum_type_def(schema &schema, const syntax_node &definition)
{
    auto enum_type = schema.create_type(type_kind::Enum, definition.name);
    if (!enum_type)
    {
        // non uniq name
        return false;
    }
    enum_type->description = definition.description;
    if (!process_directives(*enum_type, definition))
        return false;
    for (const syntax_node *child_node : definition.children)
    {
        if (!process_enum_value(*enum_type, *child_node))
            return false;
    }

    return true;
}

bool process_directive_params(std::vector<directive_parameter> &fields, const syntax_node *node)
{
    for (const syntax_node *child_node : node->children)
    {
        directive_parameter &param = fields.emplace_back();
        param.name = child_node->name;
        param.description = child_node->description;

        if (child_node->children.size() < 1)
            return false;
        syntax_node *value_node = child_node->children[0];

        switch (value_node->type)
        {
        case syntax_node_type::BoolValue:
            param.value_type = value_kind::Boolean;
            param.bool_value = value_node->boolValue;
            break;
        case syntax_node_type::IntValue:
            param.value_type = value_kind::Integer;
            param.int_value = value_node->intValue;
            break;
        case syntax_node_type::StringValue:
            param.value_type = value_kind::String;
            param.string_value = value_node->content;
            break;
        case syntax_node_type::EnumValue:
            param.value_type = value_kind::Enum;
            param.string_value = value_node->content;
            break;
        case syntax_node_type::NullValue:
            param.value_type = value_kind::Null;
            param.string_value = value_node->content;
            break;
        case syntax_node_type::FloatValue:
            param.value_type = value_kind::Float;
            param.float_value = value_node->floatValue;
            break;
        case syntax_node_type::ListValue:
            param.value_type = value_kind::List;
            if (!process_directive_params(param.fields, value_node))
                return false;
            break;
        case syntax_node_type::ObjectValue:
            param.value_type = value_kind::Object;
            if (!process_directive_params(param.fields, value_node))
                return false;
            break;
        default:
            return false;
        }
    }
    return true;
}

bool schema_parser::process_directives(type_system_object_with_directives &type, const syntax_node &definition)
{
    for (const syntax_node *directive_node : definition.directives)
    {
        directive &dir = type.add_directive(directive_node->name.substr(1));

        // dir.directive_type will be resolved lated on schema processing

        if (!process_directive_params(dir.parameters, directive_node))
            return false;
    }
    return true;
}