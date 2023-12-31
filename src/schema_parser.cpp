
#include <type_system/schema_parser.hpp>
#include <parser/parser.hpp>

using namespace blitz_query_cpp;

bool schema_parser::report_type_already_defined(std::string_view type_name)
{
    return report_error("Type with name '{}' already defined", type_name);
}

bool schema_parser::report_filed_already_defined(std::string_view type_name, std::string_view field_name)
{
    return report_error("Fileld with name '{}' already defined in type '{}'", field_name, type_name);
}

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
            result = process_directive_type_def(schema, *definition);
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
            result = process_union_type_def(schema, *definition);
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
        return report_type_already_defined(definition.name);
    }

    scalar->description = definition.description;
    if (!process_directives(*scalar, definition))
        return false;
    return true;
}

bool schema_parser::process_enum_value(object_type &enum_type, const syntax_node &enum_field_node)
{
    if (enum_field_node.type != syntax_node_type::EnumValueDefinition)
    {
        if (enum_field_node.type != syntax_node_type::Directive)
            return report_error("unexpected node type in enum: {} in enum '{}' at {}", enum_name(enum_field_node.type), enum_field_node.name, enum_type.name, enum_field_node.pos);
        return true;
    }

    auto field_res = enum_type.fields.try_emplace(std::string(enum_field_node.name));
    if (!field_res.second)
        return report_filed_already_defined(enum_type.name, enum_field_node.name);
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
        return report_type_already_defined(definition.name);
    }
    enum_type->description = definition.description;
    enum_type->kind = type_kind::Enum;

    if (!process_directives(*enum_type, definition))
        return false;
    for (const syntax_node *child_node : definition.children)
    {
        if (!process_enum_value(*enum_type, *child_node))
            return false;
    }

    return true;
}

bool schema_parser::process_parameter_value(parameter_value &param, const syntax_node &value_node)
{
    switch (value_node.type)
    {
    case syntax_node_type::BoolValue:
        param.value_type = value_kind::Boolean;
        param.bool_value = value_node.boolValue;
        return true;
    case syntax_node_type::IntValue:
        param.value_type = value_kind::Integer;
        param.int_value = value_node.intValue;
        return true;
    case syntax_node_type::StringValue:
        param.value_type = value_kind::String;
        param.string_value = value_node.content;
        return true;
    case syntax_node_type::EnumValue:
        param.value_type = value_kind::Enum;
        param.string_value = value_node.content;
        return true;
    case syntax_node_type::NullValue:
        param.value_type = value_kind::Null;
        param.string_value = value_node.content;
        return true;
    case syntax_node_type::FloatValue:
        param.value_type = value_kind::Float;
        param.float_value = value_node.floatValue;
        return true;
    case syntax_node_type::ListValue:
        param.value_type = value_kind::List;
        return process_params(param.fields, value_node);
    case syntax_node_type::ObjectValue:
        param.value_type = value_kind::Object;
        return process_params(param.fields, value_node);
    default:
        return report_error("value expected at {}", value_node.pos);
    }
    return false;
}

bool schema_parser::process_params(std::vector<parameter_value> &arguments, const syntax_node &node)
{
    for (const syntax_node *child_node : node.children)
    {
        parameter_value &param = arguments.emplace_back();
        param.name = child_node->name;
        param.description = child_node->description;

        if (child_node->children.size() < 1)
            return false;
        syntax_node *value_node = child_node->children[0];

        if (!process_parameter_value(param, *value_node))
            return false;
    }
    return true;
}

bool schema_parser::process_directives(type_system_object_with_directives &type, const syntax_node &definition)
{
    for (const syntax_node *directive_node : definition.directives)
    {
        directive &dir = type.add_directive(directive_node->name.substr(1));

        // dir.directive_type will be resolved lated on schema processing

        if (!process_params(dir.parameters, *directive_node))
            return false;
    }
    return true;
}

bool schema_parser::process_directive_type_def(schema &schema, const syntax_node &definition)
{
    auto res = schema.directives.try_emplace(std::string(definition.name));
    if (!res.second)
    {
        return report_error("directive with name {} already defined", definition.name);
    }

    directive_type &dir = res.first->second;
    dir.kind = type_kind::Directive,
    dir.name = definition.name;
    dir.description = definition.description;
    dir.target = definition.directive_target;

    for (const syntax_node *child_node : definition.arguments)
    {
        if (!process_argument(dir.arguments, *child_node))
            return false;
    }

    return true;
}

bool schema_parser::process_argument(argument_collection &arguments, const syntax_node &arg_node)
{
    auto res = arguments.try_emplace(std::string(arg_node.name));
    if (!res.second)
    {
        return report_error("argument with name {} already specified", arg_node.name);
    }
    input_value &value = res.first->second;
    value.name = arg_node.name;
    value.description = arg_node.description;
    if (!arg_node.definition_type)
        return report_error("argument type is not defined for {} at {}", arg_node.name, arg_node.pos);

    const syntax_node *type_definition = arg_node.definition_type;
    while (type_definition && type_definition->type == syntax_node_type::ListType)
    {
        if (type_definition->nullability != nullability_t::Required)
            value.field_type_nullability |= (1 << value.list_nesting_depth);
        value.list_nesting_depth++;
        if (value.list_nesting_depth > int(sizeof(value.field_type_nullability) * CHAR_BIT - 1))
        {
            return report_error("maximum list nesting exceeded at {}", type_definition->pos);
        }
        type_definition = type_definition->definition_type;
    }

    if (type_definition->type != syntax_node_type::NamedType)
        return report_error("named type expected at {}", type_definition->pos);

    if (type_definition->nullability != nullability_t::Required)
        value.field_type_nullability |= (1 << value.list_nesting_depth);

    value.field_type.name = type_definition->name;

    // handle default value
    if (arg_node.children.size() < 2u)
        return true;

    const syntax_node *default_value_node = arg_node.children[1];
    if (!process_parameter_value(value.default_value, *default_value_node))
        return false;

    return true;
}

bool schema_parser::process_union_type_def(schema &schema, const syntax_node &definition)
{
    auto union_type = schema.create_type(type_kind::Union, definition.name);
    if (!union_type)
    {
        return report_type_already_defined(definition.name);
    }
    union_type->description = definition.description;
    union_type->kind = type_kind::Union;

    if (!process_directives(*union_type, definition))
        return false;

    for (const syntax_node *union_member_def : definition.children)
    {
        if(union_member_def->type != syntax_node_type::NamedType)
            continue;
        if (!process_union_type(*union_type, *union_member_def))
            return false;
    }

    return true;
}

bool schema_parser::process_union_type(object_type &union_type, const syntax_node &union_member_def)
{
    auto res = union_type.implements.try_emplace(std::string(union_member_def.name));
    if (!res.second)
    {
        return report_error(union_member_def, "type with name {} already implemented by union {} ", union_member_def.name, union_member_def.parent->name);
    }
    type_reference &type_ref = res.first->second;
    type_ref.name = union_member_def.name;

    return true;
}