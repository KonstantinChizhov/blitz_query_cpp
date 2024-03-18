
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
        return report_error("syntax error: {}", parser.get_error_msg());
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
            result = process_output_type_def(schema, *definition);
            break;
        case syntax_node_type::EnumTypeDefinition:
            result = process_enum_type_def(schema, *definition);
            break;
        case syntax_node_type::InputObjectTypeDefinition:
            result = process_input_type_def(schema, *definition);
            break;
        case syntax_node_type::InterfaceTypeDefinition:
            result = process_interface_type_def(schema, *definition);
            break;
        case syntax_node_type::UnionTypeDefinition:
            result = process_union_type_def(schema, *definition);
            break;
        case syntax_node_type::SchemaExtension:
            result = process_schema_ext(schema, *definition);
            break;
        case syntax_node_type::InputObjectTypeExtension:
            result = process_input_ext(schema, *definition);
            break;
        case syntax_node_type::EnumTypeExtension:
            result = process_enum_ext(schema, *definition);
            break;
        case syntax_node_type::UnionTypeExtension:
            result = process_union_ext(schema, *definition);
            break;
        case syntax_node_type::ObjectTypeExtension:
            result = process_output_ext(schema, *definition, type_kind::Object);
            break;
        case syntax_node_type::ScalarTypeExtension:
            result = process_scalar_ext(schema, *definition);
            break;
        case syntax_node_type::InterfaceTypeExtension:
            result = process_output_ext(schema, *definition, type_kind::Interface);
             break;
        default:
            return report_error("Not a schema definition found: {}", enum_name(definition->type));
        }
        if(!result)
            return false;
    }
    return true;
}

bool schema_parser::process_schema_def(schema &schema, const syntax_node &definition)
{
    for (syntax_node *child : definition.children)
    {
        if (child->type == syntax_node_type::OperationTypeDefinition)
        {
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
            continue;
        }

        if (child->type == syntax_node_type::Directive)
        {
            continue;
        }
        return report_error("Unexpected schema element of type: {}", enum_name(child->type));
    }
    if (!process_directives(schema.directives, definition))
        return false;
    return true;
}

bool schema_parser::process_schema_ext(schema &schema, const syntax_node &definition)
{
    for (syntax_node *child : definition.children)
    {
        if (child->type == syntax_node_type::OperationTypeDefinition)
        {
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
                if (schema.query_type_name.size() > 0)
                    return report_error(*child, "Query type is already defined in schema");
                schema.query_type_name = child->children[0]->content;
            }
            if (child->operation_type == operation_type_t::Mutation)
            {
                if (schema.mutation_type_name.size() > 0)
                    return report_error(*child, "Mutation type is already defined in schema");
                schema.mutation_type_name = child->children[0]->content;
            }
            continue;
        }

        if (child->type == syntax_node_type::Directive)
        {
            continue;
        }
        return report_error("Unexpected schema element of type: {}", enum_name(child->type));
    }
    if (!process_directives(schema.directives, definition))
        return false;
    return true;
}

bool schema_parser::process_scalar_type_def(schema &schema, const syntax_node &definition)
{
    object_type type{type_kind::Scalar, definition.name, definition.description};
    if (!process_directives(type, definition))
        return false;

    if (!schema.types.insert(std::move(type)).second)
    {
        return report_type_already_defined(definition.name);
    }

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

    field field{enum_field_node.name, enum_field_node.description};

    if (!process_directives(field, enum_field_node))
        return false;

    if (!enum_type.fields.insert(std::move(field)).second)
        return report_filed_already_defined(enum_type.name, enum_field_node.name);

    return true;
}

bool schema_parser::process_enum_type_def(schema &schema, const syntax_node &definition)
{
    object_type enum_type{type_kind::Enum, definition.name, definition.description};

    if (!process_directives(enum_type, definition))
        return false;
    for (const syntax_node *child_node : definition.children)
    {
        if (!process_enum_value(enum_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(enum_type)).second)
        return report_type_already_defined(definition.name);

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

bool schema_parser::process_params(named_collection<parameter_value> &arguments, const syntax_node &node)
{
    for (const syntax_node *child_node : node.children)
    {
        parameter_value param{child_node->name, child_node->description};

        if (child_node->children.size() < 1)
            return false;
        syntax_node *value_node = child_node->children[0];

        if (!process_parameter_value(param, *value_node))
            return false;

        if (!arguments.insert(param).second)
            return report_error(*child_node, "parameter with name '{}' already defined", child_node->name);
    }
    return true;
}

bool schema_parser::process_directives(std::vector<directive> &directives, const syntax_node &definition)
{
    for (const syntax_node *directive_node : definition.directives)
    {
        directive dir{directive_node->name};

        if (!process_params(dir.parameters, *directive_node))
            return false;
        
        directives.push_back(std::move(dir));
    }
    return true;
}

bool schema_parser::process_directive_type_def(schema &schema, const syntax_node &definition)
{
    directive_type dir{definition.name, definition.description, definition.directive_target};

    if (!process_arguments(dir.arguments, definition))
        return false;

    if (!schema.directive_types.insert(dir).second)
        return report_error("directive type with name {} already defined", definition.name);

    return true;
}

bool schema_parser::process_arguments(named_collection<input_value> &arguments, const syntax_node &node)
{
    for (const syntax_node *child_node : node.arguments)
    {
        if (!child_node->definition_type)
            return report_error(*child_node, "argument type is not defined for {} at {}", child_node->name);

        input_value value{child_node->name, child_node->description};
        value.index = arguments.size();

        if (!process_input_value(value, *child_node))
            return true;

        if (!arguments.insert(std::move(value)).second)
            return report_error(*child_node, "argument with name {} already specified", child_node->name);
    }
    return true;
}

bool schema_parser::process_filed_type(input_value &value, const syntax_node &node)
{
    const syntax_node *type_definition = node.definition_type;
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
    return true;
}

bool schema_parser::process_input_value(input_value &value, const syntax_node &node)
{
    if (!process_filed_type(value, node))
        return false;
    // handle default value
    if (node.children.size() > 1u)
    {
        const syntax_node *default_value_node = node.children[1];
        if (!process_parameter_value(value.default_value, *default_value_node))
            return false;
    }

    if (!process_directives(value, node))
        return false;

    return true;
}

bool schema_parser::process_union_type_def(schema &schema, const syntax_node &definition)
{
    object_type union_type{std::string(definition.name), std::string(definition.description)};
    union_type.kind = type_kind::Union;

    if (!process_directives(union_type, definition))
        return false;

    for (const syntax_node *union_member_def : definition.implements)
    {
        if (!process_implemented_type(union_type, *union_member_def))
            return false;
    }

    if (!schema.types.insert(std::move(union_type)).second)
        return report_type_already_defined(definition.name);

    return true;
}

bool schema_parser::process_implemented_type(object_type &type, const syntax_node &member_def)
{
    auto res = type.implements.emplace(member_def.name);
    if (!res.second)
    {
        return report_error(member_def, "type with name {} already implemented by type {} ", member_def.name, type.name);
    }

    return true;
}

bool schema_parser::process_input_type_def(schema &schema, const syntax_node &definition)
{
    object_type input_type{type_kind::InputObject, std::string(definition.name), std::string(definition.description)};

    if (!process_directives(input_type, definition))
        return false;

    for (const syntax_node *child_node : definition.children)
    {
        if (!process_input_field(input_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(input_type)).second)
        return report_type_already_defined(definition.name);

    return true;
}

bool schema_parser::process_input_ext(schema &schema, const syntax_node &definition)
{
    auto res = schema.types.find(definition.name);
    if (res == schema.types.end())
        return report_error(definition, "Type {} is not defined", definition.name);
    if (res->kind != type_kind::InputObject)
        return report_error(definition, "Type {} is not InputObject", definition.name);

    object_type input_type = std::move(schema.types.extract(res).value());

    if (!process_directives(input_type, definition))
        return false;

    for (const syntax_node *child_node : definition.children)
    {
        if (!process_input_field(input_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(input_type)).second)
        return report_error("Unable to extend type: {}", definition.name);

    return true;
}

bool schema_parser::process_scalar_ext(schema &schema, const syntax_node &definition)
{
    auto res = schema.types.find(definition.name);
    if (res == schema.types.end())
        return report_error(definition, "Type {} is not defined", definition.name);
    if (res->kind != type_kind::Scalar)
        return report_error(definition, "Type {} is not Scalar", definition.name);

    object_type type = std::move(schema.types.extract(res).value());

    if (!process_directives(type, definition))
        return false;

    if (!schema.types.insert(std::move(type)).second)
        return report_error("Unable to extend type: {}", definition.name);

    return true;
}

bool schema_parser::process_enum_ext(schema &schema, const syntax_node &definition)
{
    auto res = schema.types.find(definition.name);
    if (res == schema.types.end())
        return report_error(definition, "Type {} is not defined", definition.name);
    if (res->kind != type_kind::Enum)
        return report_error(definition, "Type {} is not Enum", definition.name);

    object_type enum_type = std::move(schema.types.extract(res).value());

    if (!process_directives(enum_type, definition))
        return false;
    for (const syntax_node *child_node : definition.children)
    {
        if (!process_enum_value(enum_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(enum_type)).second)
        return report_error("Unable to extend type: {}", definition.name);

    return true;
}

bool schema_parser::process_union_ext(schema &schema, const syntax_node &definition)
{
    auto res = schema.types.find(definition.name);
    if (res == schema.types.end())
        return report_error(definition, "Type {} is not defined", definition.name);
    if (res->kind != type_kind::Union)
        return report_error(definition, "Type {} is not Union", definition.name);

    object_type union_type = std::move(schema.types.extract(res).value());

    if (!process_directives(union_type, definition))
        return false;

    for (const syntax_node *union_member_def : definition.implements)
    {
        if (!process_implemented_type(union_type, *union_member_def))
            return false;
    }

    if (!schema.types.insert(std::move(union_type)).second)
        return report_error("Unable to extend type: {}", definition.name);

    return true;
}

bool schema_parser::process_output_ext(schema &schema, const syntax_node &definition, type_kind kind)
{
    auto res = schema.types.find(definition.name);
    if (res == schema.types.end())
        return report_error(definition, "Type {} is not defined", definition.name);
    if (res->kind != kind)
        return report_error(definition, "Type {} is not matched", definition.name);

    object_type output_type = std::move(schema.types.extract(res).value());

    if (!process_directives(output_type, definition))
        return false;

    for (const syntax_node *inter : definition.implements)
    {
        if (!process_implemented_type(output_type, *inter))
            return false;
    }

    for (const syntax_node *child_node : definition.children)
    {
        if (!process_output_field(output_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(output_type)).second)
        return report_error("Unable to extend type: {}", definition.name);

    return true;
}


bool schema_parser::process_input_field(object_type &type, const syntax_node &field_node)
{
    if (field_node.type != syntax_node_type::InputValueDefinition)
    {
        if (field_node.type != syntax_node_type::Directive)
            return report_error(field_node, "unexpected node type: {} in input type '{}' at {}", enum_name(field_node.type), type.name);
        return true;
    }

    field field{field_node.name, field_node.description};

    if (!process_input_value(field, field_node))
        return false;
    field.index = type.fields.size();
    field.declaring_type.name = type.name;

    if (!type.fields.insert(std::move(field)).second)
        return report_filed_already_defined(type.name, field_node.name);

    return true;
}

bool schema_parser::process_output_type_def(schema &schema, const syntax_node &definition)
{
    object_type output_type{type_kind::Object, std::string(definition.name), std::string(definition.description)};

    if (!process_directives(output_type, definition))
        return false;

    for (const syntax_node *inter : definition.implements)
    {
        if (!process_implemented_type(output_type, *inter))
            return false;
    }

    for (const syntax_node *child_node : definition.children)
    {
        if (!process_output_field(output_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(output_type)).second)
        return report_type_already_defined(definition.name);

    return true;
}

bool schema_parser::process_interface_type_def(schema &schema, const syntax_node &definition)
{
    object_type output_type{type_kind::Interface, std::string(definition.name), std::string(definition.description)};

    if (!process_directives(output_type, definition))
        return false;

    for (const syntax_node *inter : definition.implements)
    {
        if (!process_implemented_type(output_type, *inter))
            return false;
    }

    for (const syntax_node *child_node : definition.children)
    {
        if (!process_output_field(output_type, *child_node))
            return false;
    }

    if (!schema.types.insert(std::move(output_type)).second)
        return report_type_already_defined(definition.name);

    return true;
}

bool schema_parser::process_output_field(object_type &type, const syntax_node &field_node)
{
    if (field_node.type != syntax_node_type::FieldDefinition)
    {
        if (!has_any_flag(field_node.type, syntax_node_type::Directive | syntax_node_type::NamedType))
            return report_error(field_node, "unexpected node type: {} in type type '{}'", enum_name(field_node.type), type.name);
        return true;
    }

    field field{field_node.name, field_node.description};

    if (!process_filed_type(field, field_node))
        return false;

    if (!process_directives(field, field_node))
        return false;

    if (!process_arguments(field.arguments, field_node))
        return false;

    field.index = type.fields.size();
    field.declaring_type.name = type.name;

    if (!type.fields.insert(std::move(field)).second)
        return report_filed_already_defined(type.name, field_node.name);

    return true;
}