
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
            result = process_schema_def(schema, definition);
            break;
        case syntax_node_type::ScalarTypeDefinition:
            result = process_scalar_type_def(schema, definition);
            break;
        case syntax_node_type::DirectiveDefinition:

            break;
        case syntax_node_type::ObjectTypeDefinition:

            break;
        case syntax_node_type::EnumTypeDefinition:

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

bool schema_parser::process_schema_def(schema &schema, const syntax_node *definition)
{
    for(syntax_node *child : definition->children)
    {
        if(child->type != syntax_node_type::OperationTypeDefinition)
        {
            // invalid schema
            return false;
        }
        if(child->children.size() < 1)
        {
            return false;
        }
        if(child->children[0]->type != syntax_node_type::NamedType)
        {
            return false;
        }
        if(child->operation_type == operation_type_t::Query)
        {
            schema.query_type_name = child->children[0]->content;
        }
        if(child->operation_type == operation_type_t::Mutation)
        {
            schema.mutation_type_name = child->children[0]->content;
        }
    }
    return true;
}

bool schema_parser::process_scalar_type_def(schema & schema, const syntax_node *definition)
{
    if(definition->name.size() < 1)
    {
        return false;
    }

    scalar_type *scalar = schema.scalars.emplace(definition->name);
    if(!scalar)
    {
        // non uniq name
        return false;
    }
    scalar->description = definition->description;
    if(!process_directives(scalar, definition))
        return false;
    return true;
}

bool schema_parser::process_directives(type_system_object_with_directives *type, const syntax_node *definition)
{
    for(const syntax_node * directive_node : definition->directives)
    {
        directive *scalar = type->directives.emplace(definition->name);
    }
}