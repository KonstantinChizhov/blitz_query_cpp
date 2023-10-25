#pragma once

#include <type_system/schema.hpp>
#include <syntax/syntax_node.hpp>
#include <syntax/document.hpp>

namespace blitz_query_cpp
{
    class schema_parser
    {
    public:
        bool parse(schema &schema, std::string_view schema_string);
        bool process_doc(schema &schema, const document &doc);
        bool process_schema_def(schema & schema, const syntax_node *definition);
        bool process_scalar_type_def(schema & schema, const syntax_node *definition);
        bool process_directives(type_system_object_with_directives *type, const syntax_node *definition);
    };
}