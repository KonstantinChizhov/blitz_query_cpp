#include <parser/parser.hpp>

using namespace blitz_query_cpp;

bool parser_t::unexpected_token()
{
    return report_error(error_code_t::UnexpectedToken,
                        "UnexpectedToken {}",
                        current_token.value);
}

bool parser_t::create_new_node(syntax_node_type type, bool is_leaf)
{
    syntax_node &parent = *nodes_stack.top();
    syntax_node &node = doc.all_nodes.emplace_back();
    node.parent = &parent;
    node.pos = current_token.pos;
    node.type = type;

    if (current_description.size() > 0)
    {
        node.description = current_description;
        current_description = std::string_view{};
    }

    if (!parent.add_child(&node))
        return false;
    if (!is_leaf)
        nodes_stack.push(&node);
    return true;
}

bool parser_t::parse()
{
    index_t token_count = count_tokens();
    // reserve node storage to avoid future reallocations
    doc.all_nodes.reserve(token_count);

    while (current_token.type != token_type::End && current_token.type != token_type::InvalidToken)
    {
        if (!parse_definitions())
        {
            return false;
        }
    }
    if (current_token.type == token_type::InvalidToken)
    {
        return unexpected_token();
    }
    return true;
}

bool parser_t::expect_token(token_type expected_types)
{
    if (!current_token.of_type(expected_types))
    {
        return unexpected_token();
    }
    return next_token();
}

bool parser_t::parse_keyword_token(syntax_node_type type, std::string_view keyword)
{
    if (!current_token.of_type(token_type::Name) || current_token.value != keyword)
    {
        return report_error(error_code_t::UnexpectedToken,
                            "Expected keyword '{}' got: '{}'",
                            keyword,
                            current_token.value);
    }
    return parse_node(type, token_type::Name);
}

bool parser_t::expect_keyword_token(std::string_view keyword, bool optional)
{
    if (!current_token.of_type(token_type::Name) || current_token.value != keyword)
    {
        if (optional)
            return false;
        return report_error(error_code_t::UnexpectedToken,
                            "Expected keyword '{}' got: '{}'",
                            keyword,
                            current_token.value);
    }
    return next_token();
}

index_t parser_t::count_tokens()
{
    tokenizer_t local_tokenizer{doc.doc_value};
    token_t t;
    index_t count = 0;
    do
    {
        t = local_tokenizer.next_token();
        count++;
    } while (!t.of_type(token_type::End | token_type::InvalidToken));
    return count;
}

bool parser_t::next_token()
{
    last_token_end = current_token.pos + current_token.size;
    current_token = tokenizer.next_token();
    return true;
}

void parser_t::update_node_size_and_content()
{
    syntax_node &node = current_node();
    node.size = last_token_end - node.pos;
    std::string_view doc_view = doc.doc_value;
    node.content = doc_view.substr(node.pos, node.size);
}

bool parser_t::parse_description()
{
    if (current_token.of_type(token_type::StringBlock | token_type::StringLiteral))
    {
        current_description = current_token.value;
        return next_token();
    }
    return true;
}

bool parser_t::parse_definitions()
{
    if (!parse_description())
        return false;

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
            return parse_schema_definition(syntax_node_type::SchemaDefinition);
        case "scalar"_crc32:
            return parse_scalar_type_definition(syntax_node_type::ScalarTypeDefinition);
        case "type"_crc32:
            return parse_object_type_definition(syntax_node_type::ObjectTypeDefinition, "type");
        case "interface"_crc32:
            return parse_object_type_definition(syntax_node_type::ObjectTypeDefinition, "interface");
        case "union"_crc32:
            return parse_union_type_definition(syntax_node_type::UnionTypeDefinition);
        case "enum"_crc32:
            return parse_enum_type_definition(syntax_node_type::EnumTypeDefinition);
        case "input"_crc32:
            return parse_input_object_type_definition(syntax_node_type::InputObjectTypeDefinition);
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

bool parser_t::parse_node(syntax_node_type type, token_type expected_types, NodeParseOptions opts)
{
    if (!current_token.of_type(expected_types))
    {
        if (has_any_flag(opts, ParseNodeIfMatch))
            return false;
        return unexpected_token();
    }

    if (!create_new_node(type, has_any_flag(opts, NodeIsLeaf)))
        return false;
    syntax_node &node = last_node();
    node.pos = current_token.pos;
    node.size = current_token.size;
    node.content = current_token.value;

    return next_token();
}

bool parser_t::parse_operation_definition()
{
    operation_type_t op_type = parse_operation_type(current_token.value);
    if (op_type == operation_type_t::None)
    {
        report_error(error_code_t::InvalidOperationType, "Invalid operation type {}", last_node().content);
        return false;
    }

    if (!parse_node(syntax_node_type::OperationDefinition, token_type::Name))
        return false;
    syntax_node &operation_node = current_node();
    operation_node.operation_type = op_type;

    if (current_token.type == token_type::Name)
    {
        if (!parse_name())
            return false;
    }

    if (!parse_variable_definitions())
        return false;
    if (!parse_directives(false))
        return false;
    if (!parse_selection_set())
        return false;

   return pop_node();
}

bool parser_t::parse_short_operation_definition()
{
    if (!create_new_node(syntax_node_type::OperationDefinition, false))
        return false;
    auto &node = current_node();
    node.operation_type = operation_type_t::Query;
    node.size = doc.doc_value.size() - node.pos;
    if (!parse_selection_set())
        return false;
   return pop_node();
}

bool parser_t::parse_variable_definitions()
{
    size_t child_count = current_node().children.size();
    if (!current_token.of_type(token_type::LParen))
        return true;

    if (!next_token())
        return false;

    while (!current_token.of_type(token_type::RParen))
    {
        if (!parse_variable_definition())
        {
            return false;
        }
    }
    auto &node = current_node();
    node.variables = node.children.subspan(child_count);
    return expect_token(token_type::RParen);
}

bool parser_t::parse_variable_definition()
{
    if (!current_token.of_type(token_type::ParameterLiteral))
    {
        return unexpected_token();
    }
    if (!parse_node(syntax_node_type::VariableDefinition, token_type::ParameterLiteral))
        return false;
    auto &var_node = current_node();
    var_node.name = var_node.content;

    if (!expect_token(token_type::Colon))
        return false;
    if (!parse_type_reference())
        return false;

    if (current_token.of_type(token_type::Equal))
    {
        if (!next_token())
            return false;
        if (!parse_value_literal(true))
            return false;
    }
    size_t child_count = var_node.children.size();
    if (!parse_directives(true))
        return false;

    var_node.directives = var_node.children.subspan(child_count);

   return pop_node();
}

bool parser_t::parse_value_literal(bool is_constant)
{
    if (current_token.of_type(token_type::LBracket))
        return parse_list(is_constant);

    if (current_token.of_type(token_type::LBrace))
        return parse_object(is_constant);

    if (parse_node(syntax_node_type::StringValue, token_type::StringLiteral | token_type::StringBlock, NodeIsLeaf | ParseNodeIfMatch))
        return true;

    if (parse_node(syntax_node_type::IntValue, token_type::IntLiteral, NodeIsLeaf | ParseNodeIfMatch))
    {
        syntax_node &node = last_node();
        long long value = 0; 
        auto res = std::from_chars(node.content.begin(), node.content.end(), value);
        if(res.ec != std::errc{})
            return report_error(error_code_t::SyntaxError, "Failed to parse int value. Error: {}", (int)res.ec);
        node.intValue = value;
        return true;
    }

    if (parse_node(syntax_node_type::FloatValue, token_type::FloatLiteral, NodeIsLeaf | ParseNodeIfMatch))
    {
        syntax_node &node = last_node();
        node.floatValue = std::stod(node.content.data());
        return true;
    }

    if (parse_node(syntax_node_type::EnumValue, token_type::Name, NodeIsLeaf | ParseNodeIfMatch))
    {
        syntax_node &node = last_node();
        if (node.content == "true")
        {
            node.type = syntax_node_type::BoolValue;
            node.boolValue = true;
        }
        if (node.content == "false")
        {
            node.type = syntax_node_type::BoolValue;
            node.boolValue = false;
        }
        if (node.content == "null")
            node.type = syntax_node_type::NullValue;
        return true;
    }

    if (!is_constant && parse_node(syntax_node_type::Variable, token_type::ParameterLiteral, NodeIsLeaf | ParseNodeIfMatch))
    {
        syntax_node &node = last_node();
        node.name = node.content;
        return true;
    }

    return unexpected_token();
}

bool parser_t::parse_list(bool is_constant)
{
    if (!parse_node(syntax_node_type::ListValue, token_type::LBracket))
        return false;

    while (!current_token.of_type(token_type::RBracket))
    {
        if (!parse_value_literal(is_constant))
            return false;
    }

    if (!expect_token(token_type::RBrace))
        return false;

    return pop_node();
}

bool parser_t::parse_object(bool is_constant)
{
    if (!parse_node(syntax_node_type::ObjectValue, token_type::LBrace))
        return false;

    while (!current_token.of_type(token_type::RBrace))
    {
        if (!create_new_node(syntax_node_type::ObjectField, false))
            return false;
        if (!parse_name())
            return false;
        if (!expect_token(token_type::Colon))
            return false;
        if (!parse_value_literal(is_constant))
            return false;

        pop_node();
    }

    if (!expect_token(token_type::RBrace))
        return false;

    return pop_node();
}

bool parser_t::parse_type_reference()
{
    if (parse_node(syntax_node_type::ListType, token_type::LBracket, ParseNodeIfMatch))
    {
        if (!parse_type_reference())
            return false;
        if (!expect_token(token_type::RBracket))
            return false;
    }
    else
    {
        if (!parse_named_type())
            return false;
    }

    auto &node = current_node();
    index_t size = last_token_end - node.pos;
    node.name = std::string_view(doc.doc_value).substr(node.pos, size);

    if (current_token.of_type(token_type::NotNull))
    {
        current_node().nullability = nullability_t::Required;
        if (!next_token())
            return false;
    }
    else
    {
        current_node().nullability = nullability_t::Optional;
    }

    current_node().parent->definition_type = &current_node();
   return pop_node();
}

bool parser_t::parse_directives(bool is_constant)
{
    size_t child_count = current_node().children.size();
    while (current_token.of_type(token_type::Directive))
    {
        if (!parse_directive(is_constant))
            return false;
    }
    auto &node = current_node();
    node.directives = node.children.subspan(child_count);
    return true;
}

bool parser_t::parse_directive(bool is_constant)
{
    if (!parse_node(syntax_node_type::Directive, token_type::Directive))
        return false;
    syntax_node &directive_node = current_node();
    directive_node.name = directive_node.content;

    if (!parse_arguments(is_constant))
        return false;

    return pop_node();
}

bool parser_t::parse_arguments(bool is_constant)
{
    if (!current_token.of_type(token_type::LParen))
        return true;

    size_t child_count = current_node().children.size();

    if (!next_token())
        return false;

    while (!current_token.of_type(token_type::RParen))
    {
        if (!create_new_node(syntax_node_type::Argument, false))
            return false;
        if (!parse_name())
            return false;
        if (!expect_token(token_type::Colon))
            return false;
        if (!parse_value_literal(is_constant))
            return false;

        pop_node();
    }

    auto &node = current_node();
    node.variables = node.children.subspan(child_count);

    return expect_token(token_type::RParen);
}

bool parser_t::parse_selection_set()
{
    if (!parse_node(syntax_node_type::SelectionSet, token_type::LBrace))
        return false;
    syntax_node &selection_set_node = current_node();
    selection_set_node.parent->selection_set = &selection_set_node;

    while (!current_token.of_type(token_type::RBrace))
    {
        if (!parse_selection())
            return false;
    }
    if (!expect_token(token_type::RBrace))
        return false;

   return pop_node();
}

bool parser_t::parse_selection()
{
    if (parse_fragment())
        return true;
    if (parse_field())
        return true;
    return false;
}

bool parser_t::parse_field()
{
    if (parse_node(syntax_node_type::Comment, token_type::Comment, ParseNodeIfMatch | NodeIsLeaf))
        return true;

    if (!parse_node(syntax_node_type::Field, token_type::Name))
    {
        report_error(error_code_t::NameExpected, "Field name expected at {}", current_token.pos);
        return false;
    }
    syntax_node &selection_node = current_node();
    selection_node.alias = selection_node.name = selection_node.content;

    if (current_token.of_type(token_type::Colon))
    {
        if (!expect_token(token_type::Colon))
            return false;
        if (!parse_name())
        {
            report_error(error_code_t::NameExpected, "Field name expected at {}", current_token.pos);
            return false;
        }

        selection_node.name = current_token.value;
        selection_node.alias = selection_node.content;
        selection_node.size = selection_node.pos;
    }

    if (!parse_arguments(false))
        return false;

    if (!parse_directives(false))
        return false;

    if (current_token.of_type(token_type::LBrace))
    {
        if (!parse_selection_set())
            return false;
    }

    return pop_node();
}

bool parser_t::parse_fragment()
{
    if (!parse_node(syntax_node_type::FragmentSpread, token_type::FragmentSpread, ParseNodeIfMatch))
        return false;

    if (!current_token.of_type(token_type::Name))
        return report_error(error_code_t::SyntaxError, "fragment name or inline fragment expected");

    auto &node = current_node();
    // inline fragment
    if (current_token.value == "on")
    {
        if (!next_token())
            return false;
        if (!parse_node(syntax_node_type::NamedType, token_type::Name, NodeIsLeaf))
            return false;
        node.definition_type = &last_node();

        if (!parse_directives(false))
            return false;

        if (!parse_selection_set())
            return false;
    }
    else
    {
        node.name = current_token.value;
        if (!parse_directives(false))
            return false;
    }
    return pop_node();
}

bool parser_t::parse_type_extension()
{
    if (!next_token())
        return report_error(error_code_t::UnexpectedEndOfDocument, "Type extension expected");
    switch (hash_crc32(current_token.value))
    {
    case "scalar"_crc32:
        return parse_scalar_type_definition(syntax_node_type::ScalarTypeExtension);
    case "union"_crc32:
        return parse_union_type_definition(syntax_node_type::UnionTypeExtension);
    case "enum"_crc32:
        return parse_enum_type_definition(syntax_node_type::EnumTypeExtension);
    case "type"_crc32:
        return parse_object_type_definition(syntax_node_type::ObjectTypeExtension, "type");
    case "interface"_crc32:
        return parse_object_type_definition(syntax_node_type::InterfaceTypeDefinition, "interface");
    case "imput"_crc32:
        return parse_input_object_type_definition(syntax_node_type::InterfaceTypeDefinition);
    }
    return report_error(error_code_t::InvalidToken, "Expected type, scalar, union, enum, interface or union. Got: {}", current_token.value);
}

bool parser_t::parse_input_object_type_definition(syntax_node_type node_type)
{
    if (!parse_keyword_token(node_type, "input"))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(false))
        return false;

    if(node_type == syntax_node_type::InputObjectTypeExtension && !current_token.of_type(token_type::LBrace))
        return pop_node();

    if (!parse_argument_definitions(token_type::LBrace, token_type::RBrace))
        return false;

    return pop_node();
}

bool parser_t::parse_enum_type_definition(syntax_node_type node_type)
{
    if (!parse_keyword_token(node_type, "enum"))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(false))
        return false;

    if (node_type == syntax_node_type::EnumTypeExtension && !current_token.of_type(token_type::LBrace))
        return pop_node();

    if (!expect_token(token_type::LBrace))
        return false;

    if (!parse_enum_values())
        return false;

    if (!expect_token(token_type::RBrace))
        return false;

    return pop_node();
}

bool parser_t::parse_enum_values()
{
    while (!current_token.of_type(token_type::RBrace))
    {
        if (!parse_enum_value())
        {
            return false;
        }
    }
    return true;
}

bool parser_t::parse_enum_value()
{
    if (!create_new_node(syntax_node_type::EnumValueDefinition, false))
        return false;
    if (!parse_name())
        return false;
    if (!parse_directives(true))
        return false;
    return pop_node();
}

bool parser_t::parse_union_type_definition(syntax_node_type node_type)
{
    if (!parse_keyword_token(node_type, "union"))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(false))
        return false;

    if (node_type == syntax_node_type::UnionTypeExtension && !current_token.of_type(token_type::Equal))
       return pop_node();

    if (!expect_token(token_type::Equal))
        return false;

    if (current_token.of_type(token_type::Union))
    {
        if (!next_token())
            return false;
    }
    size_t child_count = current_node().children.size();
    do
    {
        if (!parse_named_type(NodeIsLeaf))
            return false;

        if (!current_token.of_type(token_type::Union))
            break;

        if (!next_token())
            break;

    } while (true);
    auto &node = current_node();
    node.implements = node.children.subspan(child_count);

    return pop_node();
}

bool parser_t::parse_object_type_definition(syntax_node_type type, std::string_view keyword)
{
    if (!parse_keyword_token(type, keyword))
        return false;

    if (!parse_name())
        return false;

    if (!parse_implements_interfaces())
        return false;

    if (!parse_directives(false))
        return false;

    if (!expect_token(token_type::LBrace))
        return false;

    while (!current_token.of_type(token_type::RBrace))
    {
        if (!parse_description())
            return false;
        if (!create_new_node(syntax_node_type::FieldDefinition, false))
            return false;
        if (!parse_name())
            return false;
        if (!parse_argument_definitions())
            return false;
        if (!expect_token(token_type::Colon))
            return false;
        if (!parse_type_reference())
            return false;
        if (!parse_directives(true))
            return false;
        pop_node();
    }
    pop_node();
    return expect_token(token_type::RBrace);
}

bool parser_t::parse_implements_interfaces()
{
    if (!expect_keyword_token("implements", true))
        return true;

    if (current_token.of_type(token_type::And))
    {
        if (!next_token())
            return false;
    }
    size_t child_count = current_node().children.size();

    while (current_token.of_type(token_type::Name))
    {
        if (!parse_named_type(NodeIsLeaf))
            return false;

        if (current_token.of_type(token_type::And))
        {
            if (!next_token())
                return false;
        }
    }
    auto &node = current_node();
    node.implements = node.children.subspan(child_count);

    return true;
}

bool parser_t::parse_argument_definitions(token_type start, token_type end)
{
    size_t child_count = current_node().children.size();
    if (!current_token.of_type(start))
        return true;

    if (!next_token())
        return false;

    while (!current_token.of_type(end))
    {
        if (!parse_argument_definition())
        {
            return false;
        }
    }
    auto &node = current_node();
    node.arguments = node.children.subspan(child_count);
    return expect_token(end);
}

bool parser_t::parse_argument_definition()
{
    if (!parse_description())
        return false;
    if (!create_new_node(syntax_node_type::InputValueDefinition, false))
        return false;
    if (!parse_name())
        return false;
    if (!expect_token(token_type::Colon))
        return false;
    if (!parse_type_reference())
        return false;
    if (current_token.of_type(token_type::Equal))
    {
        if (!next_token())
            return false;
        if (!parse_value_literal(true))
            return false;
    }
    if (!parse_directives(true))
        return false;
   return pop_node();
}

bool parser_t::parse_name(token_type name_type)
{
    if (!current_token.of_type(name_type))
    {
        return unexpected_token();
    }
    syntax_node &parent = current_node();

    parent.name = current_token.value;
    return next_token();
}

bool parser_t::parse_scalar_type_definition(syntax_node_type node_type)
{
    if (!parse_node(node_type, token_type::Name))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(true))
        return false;

   return pop_node();
}

bool parser_t::parse_schema_definition(syntax_node_type node_type)
{
    if (!parse_keyword_token(node_type, "schema"))
        return false;

    if (!parse_directives(true))
        return false;

    if(node_type == syntax_node_type::SchemaExtension && !current_token.of_type(token_type::LBrace))
        return pop_node();

    if (!expect_token(token_type::LBrace))
        return false;

    while (!current_token.of_type(token_type::RBrace))
    {
        if (!parse_operation_type_definition())
            return false;
    }

    if (!expect_token(token_type::RBrace))
        return false;

    return  pop_node();
}

bool parser_t::parse_operation_type_definition()
{
    if (!create_new_node(syntax_node_type::OperationTypeDefinition, false))
        return false;

    auto op_type = parse_operation_type(current_token.value);
    if (op_type == operation_type_t::None)
    {
        report_error(error_code_t::InvalidOperationType, "Invalid Operation Type: {}", current_token.value);
        return false;
    }
    current_node().operation_type = op_type;

    if (!expect_token(token_type::Name))
        return false;

    if (!expect_token(token_type::Colon))
        return false;
    if (!parse_named_type(NodeIsLeaf))
        return false;

    return pop_node();
}

bool parser_t::parse_named_type(NodeParseOptions opts)
{
    if (!parse_node(syntax_node_type::NamedType, token_type::Name, opts))
        return false;
    last_node().name = last_node().content;
    return true;
}

bool parser_t::parse_directive_definition()
{
    if (!parse_keyword_token(syntax_node_type::DirectiveDefinition, "directive"))
        return false;

    if (!parse_name(token_type::Directive))
        return false;

    if (!parse_argument_definitions())
        return false;

    if (!current_token.of_type(token_type::Name))
        return report_error(error_code_t::UnexpectedToken, "Expected to see 'on' or 'repeatable' keyword while parsing directive '{}' at {}", current_node().name, current_token.pos);

    if (expect_keyword_token("repeatable", true))
        current_node().directive_target = directive_target_t::IsRepeatable;

    if (!expect_keyword_token("on"))
        return false;

    directive_target_t target = directive_target_t::None;
    do
    {
        if (current_token.of_type(token_type::Union))
        {
            if (!next_token())
                return false;
        }
        if (!current_token.of_type(token_type::Name))
        {
            break;
        }
        target = parse_directive_target(current_token.value);
        if (target == directive_target_t::None)
        {
            break;
        }
        current_node().directive_target |= target;
        if (!next_token())
            return false;

    } while (target != directive_target_t::None);

    return pop_node();
}

bool parser_t::parse_fragment_definition()
{
    if (!parse_keyword_token(syntax_node_type::FragmentDefinition, "fragment"))
        return false;
    if (!parse_name())
        return false;
    if (!parse_variable_definitions())
        return false;
    if (!expect_keyword_token("on"))
        return false;
    if (!parse_named_type(NodeIsLeaf))
        return false;
    if (!parse_directives(false))
        return false;
    if (!parse_selection_set())
        return false;
   return pop_node();
}