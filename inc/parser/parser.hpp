#pragma once
#include <parser/tokenizer.hpp>
#include <lexica/token.hpp>
#include <syntax/document.hpp>
#include <error_code.hpp>
#include <util/crc_hash.hpp>
#include <util/enum.hpp>
#include <format>
#include <stack>

namespace blitz_query_cpp
{
    enum NodeParseOptions
    {
        NodeParseDefault = 0,
        NodeIsLeaf = 1,
        ParseNodeIfMatch = 2
    };

    DECLARE_ENUM_OPERATIONS(NodeParseOptions);

    class parser
    {
        document &doc;
        tokenizer_t tokenizer;
        token current_token;
        std::stack<syntax_node *> nodes_stack;
        std::string error_msg;
        error_code_t error_code = error_code_t::OK;
        index_t last_token_end = 0;
        std::string_view current_description;

    public:
        parser(document &doc_)
            : doc(doc_),
              tokenizer(doc_.doc_value),
              current_token(token_type::None)
        {
            nodes_stack.push(&doc);
            (void)next_token();
        }

        std::string get_error_msg() const { return error_msg; }
        error_code_t get_error_code() const { return error_code; }

        [[nodiscard]] bool parse();

    public: // for internal use and tests
        template <class... Args>
        bool report_error(error_code_t code, std::string_view fmt, Args &&...args)
        {
            error_code = code;
            error_msg = std::vformat(fmt, std::make_format_args(args...));
            return false;
        }

        void pop_node()
        {
            update_node_size_and_content();
            if (nodes_stack.size() == 1)
            {
                report_error(error_code_t::SyntaxError, "Unbalanced nodes stack");
            }
            nodes_stack.pop();
        }

        void update_node_size_and_content();
        [[nodiscard]] bool create_new_node(syntax_node_type type, bool is_leaf);
        [[nodiscard]] bool unexpected_token();
        syntax_node &current_node() { return *nodes_stack.top(); }
        syntax_node &last_node() { return doc.all_nodes.back(); }
        index_t count_tokens();

        [[nodiscard]] bool expect_keyword_token(std::string_view keyword, bool optional = false);
        [[nodiscard]] bool expect_token(token_type expected_types);
        [[nodiscard]] bool next_token();
        [[nodiscard]] bool parse_argument_definitions();
        [[nodiscard]] bool parse_argument_definition();
        [[nodiscard]] bool parse_arguments(bool is_constant);
        [[nodiscard]] bool parse_definitions();
        [[nodiscard]] bool parse_description();
        [[nodiscard]] bool parse_directive_definition();
        [[nodiscard]] bool parse_directive(bool is_constant);
        [[nodiscard]] bool parse_directives(bool is_constant);
        [[nodiscard]] bool parse_enum_type_definition();
        [[nodiscard]] bool parse_enum_value();
        [[nodiscard]] bool parse_enum_values();
        [[nodiscard]] bool parse_field();
        [[nodiscard]] bool parse_fragment_definition();
        [[nodiscard]] bool parse_fragment();
        [[nodiscard]] bool parse_implements_interfaces();
        [[nodiscard]] bool parse_input_object_type_definition();
        [[nodiscard]] bool parse_interface_type_definition();
        [[nodiscard]] bool parse_keyword_token(syntax_node_type type, std::string_view keyword);
        [[nodiscard]] bool parse_list(bool is_constant);
        [[nodiscard]] bool parse_name(token_type name_type = token_type::Name);
        [[nodiscard]] bool parse_named_type(NodeParseOptions opts = NodeParseDefault);
        [[nodiscard]] bool parse_node(syntax_node_type type, token_type expected_types, NodeParseOptions opts = NodeParseDefault);
        [[nodiscard]] bool parse_object_type_definition();
        [[nodiscard]] bool parse_object(bool is_constant);
        [[nodiscard]] bool parse_operation_definition();
        [[nodiscard]] bool parse_operation_type_definition();
        [[nodiscard]] bool parse_scalar_type_definition();
        [[nodiscard]] bool parse_schema_definition();
        [[nodiscard]] bool parse_selection_set();
        [[nodiscard]] bool parse_selection();
        [[nodiscard]] bool parse_short_operation_definition();
        [[nodiscard]] bool parse_type_extension();
        [[nodiscard]] bool parse_type_reference();
        [[nodiscard]] bool parse_union_type_definition();
        [[nodiscard]] bool parse_value_literal(bool is_constant);
        [[nodiscard]] bool parse_variable_definition();
        [[nodiscard]] bool parse_variable_definitions();
    };
}