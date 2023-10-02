#pragma once
#include <parser/tokenizer.hpp>
#include <lexica/token.hpp>
#include <syntax/document.hpp>
#include <error_code.hpp>
#include <util/crc_hash.hpp>
#include <format>

namespace blitz_query_cpp
{
    class parser
    {
        tokenizer tokenizer;
        document &doc;
        token current_token;
        syntax_node *current_node;

        std::string error_msg;
        error_code_t error_code = error_code_t::OK;

        template <class... Args>
        void report_error(error_code_t code, std::string_view fmt, Args &&...args)
        {
            error_code = code;
            error_msg = std::vformat(fmt, std::make_format_args(args...));
        }

        bool unexpected_token();
        syntax_node &get_new_node(syntax_node &parent);
        syntax_node &last_node() { return doc.all_nodes.back(); }

    public:
        parser(document &doc_)
            : doc(doc_),
              tokenizer(doc.doc_value),
              current_token(token_type::None)
        {
        }

        std::string get_error_msg() const { return error_msg; }
        error_code_t get_error_code() const { return error_code; }

        bool parse();
        bool next_token(bool expect_eof = false);
        bool parse_definitions();
        bool parse_leaf_node(syntax_node_type type, token_type expected_types);
        bool parse_operation_definition();
        bool parse_short_operation_definition();
        bool parse_variable_definitions();
        bool parse_directives(bool isConstant);
        bool parse_selection_set();
        bool parse_type_extension();
        bool parse_input_object_type_definition();
        bool parse_enum_type_definition();
        bool parse_union_type_definition();
        bool parse_interface_type_definition();
        bool parse_object_type_definition();
        bool parse_scalar_type_definition();
        bool parse_schema_definition();
        bool parse_directive_definition();
        bool parse_fragment_definition();
    };
}