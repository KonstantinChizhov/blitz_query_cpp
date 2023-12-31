#pragma once

#include <type_system/schema.hpp>
#include <syntax/syntax_node.hpp>
#include <syntax/document.hpp>
#include <format>

namespace blitz_query_cpp
{
    class schema_parser
    {
        std::string error_msg;
        
        template <class... Args>
        bool report_error(std::string_view fmt, Args &&...args)
        {
            error_msg = std::vformat(fmt, std::make_format_args(args...));
            return false;
        }
        bool process_enum_value(object_type &enum_type, const syntax_node &enum_field_node);
        bool process_argument(argument_collection &arguments, const syntax_node &enum_field_node);

        bool report_type_already_defined(std::string_view type_name);
        bool report_filed_already_defined(std::string_view type_name, std::string_view field_name);
        bool process_parameter_value(parameter_value &param, const syntax_node &value_node);
        bool process_params(std::vector<parameter_value> &arguments, const syntax_node &node);

    public:
        std::string get_error_msg() const { return error_msg; }

        bool parse(schema &schema, std::string_view schema_string);
        bool process_doc(schema &schema, const document &doc);
        bool process_schema_def(schema & schema, const syntax_node &definition);
        bool process_scalar_type_def(schema & schema, const syntax_node &definition);
        bool process_enum_type_def(schema & schema, const syntax_node &definition);
        bool process_directives(type_system_object_with_directives &type, const syntax_node &definition);
        bool process_directive_type_def(schema & schema, const syntax_node &definition);
    };
}