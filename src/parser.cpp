#include <parser/parser.hpp>

using namespace blitz_query_cpp;

bool parser::unexpected_token()
{
    report_error(error_code_t::UnexpectedToken,
                 "UnexpectedToken: {} at {}",
                 current_token.value,
                 current_token.pos);
    return false;
}

bool parser::create_new_node(syntax_node_type type, bool is_leaf)
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

bool parser::parse()
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

bool parser::expect_token(token_type expected_types)
{
    if (!current_token.of_type(expected_types))
    {
        return unexpected_token();
    }
    return next_token();
}

bool parser::parse_keyword_token(syntax_node_type type, std::string_view keyword)
{
    if (!current_token.of_type(token_type::Name) || current_token.value != keyword)
    {
        report_error(error_code_t::UnexpectedToken,
                     "Expected keyword {} got: {} at {}",
                     keyword,
                     current_token.value,
                     current_token.pos);
        return false;
    }
    return parse_node(type, token_type::Name);
}

bool parser::expect_keyword_token(std::string_view keyword, bool optional)
{
    if (!current_token.of_type(token_type::Name) || current_token.value != keyword)
    {
        if (!optional)
            report_error(error_code_t::UnexpectedToken,
                         "Expected keyword '{}' got: '{}' at {}",
                         keyword,
                         current_token.value,
                         current_token.pos);
        return false;
    }
    return next_token();
}

index_t parser::count_tokens()
{
    tokenizer_t local_tokenizer{doc.doc_value};
    token t;
    index_t count = 0;
    do
    {
        t = local_tokenizer.next_token();
        count++;
    } while (!t.of_type(token_type::End | token_type::InvalidToken));
    return count;
}

bool parser::next_token()
{
    last_token_end = current_token.pos + current_token.size;
    current_token = tokenizer.next_token();
    return true;
}

void parser::update_node_size_and_content()
{
    syntax_node &node = current_node();
    node.size = last_token_end - node.pos;
    std::string_view doc_view = doc.doc_value;
    node.content = doc_view.substr(node.pos, node.size);
}

bool parser::parse_description()
{
    if (current_token.of_type(token_type::StringBlock | token_type::StringLiteral))
    {
        current_description = current_token.value;
        return next_token();
    }
    return true;
}

bool parser::parse_definitions()
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

bool parser::parse_node(syntax_node_type type, token_type expected_types, NodeParseOptions opts)
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

bool parser::parse_operation_definition()
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

    pop_node();
    return true;
}

bool parser::parse_short_operation_definition()
{
    if (!create_new_node(syntax_node_type::OperationDefinition, false))
        return false;
    auto &node = current_node();
    node.operation_type = operation_type_t::Query;
    node.size = doc.doc_value.size() - node.pos;
    if (!parse_selection_set())
        return false;
    pop_node();
    return true;
}

bool parser::parse_variable_definitions()
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

bool parser::parse_variable_definition()
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

    pop_node();
    return true;
}

bool parser::parse_value_literal(bool is_constant)
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
        node.intValue = std::stoll(node.content.data());
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

bool parser::parse_list(bool is_constant)
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

    pop_node();
    return true;
}

bool parser::parse_object(bool is_constant)
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

    pop_node();
    return true;
}

bool parser::parse_type_reference()
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

    auto & node = current_node();
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
    pop_node();
    return true;
}

bool parser::parse_directives(bool is_constant)
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

bool parser::parse_directive(bool is_constant)
{
    if (!parse_node(syntax_node_type::Directive, token_type::Directive))
        return false;
    syntax_node &directive_node = current_node();
    directive_node.name = directive_node.content;

    if (!parse_arguments(is_constant))
        return false;

    pop_node();
    return true;
}

bool parser::parse_arguments(bool is_constant)
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

bool parser::parse_selection_set()
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

    pop_node();
    return true;
}

bool parser::parse_selection()
{
    if (parse_fragment())
        return true;
    if (parse_field())
        return true;
    return false;
}

bool parser::parse_field()
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

    pop_node();
    return true;
}

bool parser::parse_fragment()
{

    return false;
}

bool parser::parse_type_extension()
{
    return true;
}

bool parser::parse_input_object_type_definition()
{
    if (!parse_keyword_token(syntax_node_type::InputObjectTypeDefinition, "input"))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(false))
        return false;

    if (!expect_token(token_type::LBrace))
        return false;

    if (!parse_argument_definitions())
        return false;

    if (!expect_token(token_type::RBrace))
        return false;

    pop_node();
    return true;
}

bool parser::parse_enum_type_definition()
{
    if (!parse_keyword_token(syntax_node_type::EnumTypeDefinition, "enum"))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(false))
        return false;

    if (!expect_token(token_type::LBrace))
        return false;

    if (!parse_enum_values())
        return false;

    if (!expect_token(token_type::RBrace))
        return false;

    pop_node();
    return true;
}

bool parser::parse_enum_values()
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

bool parser::parse_enum_value()
{
    if (!create_new_node(syntax_node_type::EnumValueDefinition, false))
        return false;
    if (!parse_name())
        return false;
    if (!parse_directives(true))
        return false;
    pop_node();
    return true;
}

bool parser::parse_union_type_definition()
{
    return true;
}

bool parser::parse_interface_type_definition()
{
    return true;
}

bool parser::parse_object_type_definition()
{
    if (!parse_keyword_token(syntax_node_type::InputObjectTypeDefinition, "type"))
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

bool parser::parse_implements_interfaces()
{
    return false;
}

bool parser::parse_argument_definitions()
{
    size_t child_count = current_node().children.size();
    if (!current_token.of_type(token_type::LParen))
        return true;

    if (!next_token())
        return false;

    while (!current_token.of_type(token_type::RParen))
    {
        if (!parse_argument_definition())
        {
            return false;
        }
    }
    auto &node = current_node();
    node.arguments = node.children.subspan(child_count);
    return expect_token(token_type::RParen);
}

bool parser::parse_argument_definition()
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
    pop_node();
    return true;
}

bool parser::parse_name(token_type name_type)
{
    if (!current_token.of_type(name_type))
    {
        return unexpected_token();
    }
    syntax_node &parent = current_node();

    parent.name = current_token.value;
    return next_token();
}

bool parser::parse_scalar_type_definition()
{
    if (!parse_node(syntax_node_type::ScalarTypeDefinition, token_type::Name))
        return false;

    if (!parse_name())
        return false;

    if (!parse_directives(true))
        return false;

    pop_node();
    return true;
}

bool parser::parse_schema_definition()
{
    if (!parse_keyword_token(syntax_node_type::SchemaDefinition, "schema"))
        return false;

    if (!parse_directives(true))
        return false;

    if (!expect_token(token_type::LBrace))
        return false;

    while (!current_token.of_type(token_type::RBrace))
    {
        if (!parse_operation_type_definition())
            return false;
    }

    if (!expect_token(token_type::RBrace))
        return false;

    pop_node();
    return true;
}

bool parser::parse_operation_type_definition()
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

    pop_node();
    return true;
}

bool parser::parse_named_type(NodeParseOptions opts)
{
    return parse_node(syntax_node_type::NamedType, token_type::Name, opts);
}

bool parser::parse_directive_definition()
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
        if (target != directive_target_t::None)
        {
            current_node().directive_target |= target;
        }
        if (!next_token())
            return false;

    } while (target != directive_target_t::None);

    pop_node();
    return true;
}

bool parser::parse_fragment_definition()
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
    pop_node();
    return true;
}