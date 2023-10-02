#pragma once
#include <parser/tokenizer.hpp>
#include <lexica/token.hpp>
#include <syntax/document.hpp>
#include <error_code.hpp>
#include <util/crc_hash.hpp>

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

        bool unexpected_token()
        {
            report_error(error_code_t::UnexpectedToken,
                         "UnexpectedToken: {} at {}",
                         current_token.value,
                         current_token.pos);
            return false;
        }

        syntax_node &get_new_node(syntax_node &parent)
        {
            syntax_node &node = doc.all_nodes.emplace_back();
            node.parent = &parent;
            parent.children.push_back(&node);
            return node;
        }

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

        bool parse()
        {
            if (!next_token())
                return false;

            while (current_token.type != token_type::End || current_token.type != token_type::InvalidToken)
            {
                if (!parse_definitions())
                {
                    return false;
                }
            }
            if (current_token.type != token_type::InvalidToken)
            {
                return unexpected_token();
            }
            return true;
        }

        bool next_token(bool expect_eof = false)
        {
            current_token = tokenizer.next_token();
            doc.tokens.push_back(current_token);
            if (!expect_eof && current_token.type == token_type::End)
            {
                report_error(error_code_t::UnexpectedEndOfDocument, "Unexpected end of document at {}", current_token.pos);
                return false;
            }
            return true;
        }

        bool parse_definitions()
        {
            current_node = &get_new_node(doc);

            if (current_token.of_type(token_type::StringBlock | token_type::StringLiteral))
            {
                if (!parse_leaf_node(syntax_node_type::StringValue, token_type::StringBlock | token_type::StringLiteral))
                    return false;
            }

            if (!next_token())
            {
                return false;
            }

            if (current_token.type == token_type::Name)
            {
                switch (hash_crc32(current_token.value))
                {
                case "query"_crc32:
                case "mutation"_crc32:
                case "subscription"_crc32:
                    return parse_operation_definition();
                case "fragment"_crc32:
                    return parse_fragment_definition();
                case "directive"_crc32:
                    return parse_directive_definition();
                case "schema"_crc32:
                    return parse_schema_definition();
                case "scalar"_crc32:
                    return parse_scalar_type_definition();
                case "type"_crc32:
                    return parse_object_type_definition();
                case "interface"_crc32:
                    return parse_interface_type_definition();
                case "union"_crc32:
                    return parse_union_type_definition();
                case "enum"_crc32:
                    return parse_enum_type_definition();
                case "input"_crc32:
                    return parse_input_object_type_definition();
                case "extend"_crc32:
                    return parse_type_extension();
                }
            }
            if (current_token.type == token_type::LBrace)
            {
                return parse_short_operation_definition();
            }

            return unexpected_token();
        }

        bool parse_leaf_node(syntax_node_type type, token_type expected_types)
        {
            if (current_token.of_type(expected_types))
            {
                return unexpected_token();
            }
            syntax_node &leaf_node = get_new_node(*current_node);
            leaf_node.type = type;
            leaf_node.pos = current_token.pos;
            leaf_node.size = current_token.size;
            leaf_node.content = current_token.value;
            if (type == syntax_node_type::Name)
            {
                current_node->name = leaf_node.content;
            }
            return next_token();
        }

        bool parse_operation_definition()
        {
            current_node->type = syntax_node_type::OperationDefinition;

            if (!parse_leaf_node(syntax_node_type::OperationTypeDefinition, token_type::Name))
                return false;

            
            if (current_token.type == token_type::Name)
            {
                if (!parse_leaf_node(syntax_node_type::Name, token_type::Name))
                    return false;
            }

            parse_variable_definitions();
            parse_directives(false);
            parse_selection_set();
            return true;
        }


        bool parse_short_operation_definition()
        {
            return true;
        }

        bool parse_variable_definitions()
        {
            return true;
        }

        bool parse_directives(bool isConstant)
        {
            return true;
        }

        bool parse_selection_set()
        {
            return true;
        }

        bool parse_type_extension()
        {
            return true;
        }

        bool parse_input_object_type_definition()
        {
            return true;
        }

        bool parse_enum_type_definition()
        {
            return true;
        }

        bool parse_union_type_definition()
        {
            return true;
        }

        bool parse_interface_type_definition()
        {
            return true;
        }

        bool parse_object_type_definition()
        {
            return true;
        }

        bool parse_scalar_type_definition()
        {
            return true;
        }

        bool parse_schema_definition()
        {
            return true;
        }

        bool parse_directive_definition()
        {
            return true;
        }

        bool parse_fragment_definition()
        {
            return true;
        }


    };
}