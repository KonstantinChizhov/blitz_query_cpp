#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "parser/parser.hpp"
#include <format>

using namespace blitz_query_cpp;

TEST(Parser_t, ParseArgumentDefinitions)
{
  document_t doc("(stringVal: String = \"foo\", intVal :Int)");
  doc.all_nodes.reserve(10);
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse_argument_definitions());
  ASSERT_EQ(doc.children.size(), 2u);
  ASSERT_EQ(doc.arguments.size(), doc.children.size());

  ASSERT_EQ(doc.children[0]->type, syntax_node_type::InputValueDefinition);
  ASSERT_EQ(doc.children[1]->type, syntax_node_type::InputValueDefinition);

  ASSERT_EQ(doc.children[0]->name, "stringVal");
  ASSERT_EQ(doc.children[1]->name, "intVal");

  ASSERT_NE(doc.children[0]->definition_type, nullptr);
  ASSERT_NE(doc.children[1]->definition_type, nullptr);

  ASSERT_EQ(doc.children[0]->definition_type->name, "String");
  ASSERT_EQ(doc.children[1]->definition_type->name, "Int");
}

TEST(Parser_t, ParseArguments)
{
  document_t doc("(stringVal: \"foo\", intVal :42, objVal: {foo:10 bar:null} boolVal: true enumVal: Hello, floatVal : 3.14e2)");
  doc.all_nodes.reserve(20);
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse_arguments(false));
  ASSERT_EQ(doc.children.size(), 6u);
  ASSERT_EQ(doc.arguments.size(), doc.children.size());

  EXPECT_EQ(doc.children[0]->type, syntax_node_type::Argument);
  EXPECT_EQ(doc.children[1]->type, syntax_node_type::Argument);
  EXPECT_EQ(doc.children[2]->type, syntax_node_type::Argument);
  EXPECT_EQ(doc.children[3]->type, syntax_node_type::Argument);
  EXPECT_EQ(doc.children[4]->type, syntax_node_type::Argument);
  EXPECT_EQ(doc.children[5]->type, syntax_node_type::Argument);

  EXPECT_EQ(doc.children[0]->name, "stringVal");
  EXPECT_EQ(doc.children[1]->name, "intVal");
  EXPECT_EQ(doc.children[2]->name, "objVal");
  EXPECT_EQ(doc.children[3]->name, "boolVal");
  EXPECT_EQ(doc.children[4]->name, "enumVal");
  EXPECT_EQ(doc.children[5]->name, "floatVal");

  EXPECT_EQ(doc.arguments[0]->children[0]->content, "foo");
  EXPECT_EQ(doc.arguments[1]->children[0]->intValue, 42);
  EXPECT_EQ(doc.arguments[2]->children[0]->children[0]->children[0]->intValue, 10);
  EXPECT_EQ(doc.arguments[3]->children[0]->boolValue, true);
  EXPECT_EQ(doc.arguments[4]->children[0]->content, "Hello");
  EXPECT_EQ(doc.arguments[5]->children[0]->floatValue, 314.0);
}

TEST(Parser_t, ParseDirectives)
{
  document_t doc("@include(if:$foo) @custom");
  doc.all_nodes.reserve(20);
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse_directives(false));
  ASSERT_EQ(doc.children.size(), 2u);
  ASSERT_EQ(doc.directives.size(), doc.children.size());

  EXPECT_EQ(doc.directives[0]->type, syntax_node_type::Directive);
  EXPECT_EQ(doc.directives[1]->type, syntax_node_type::Directive);

  EXPECT_EQ(doc.directives[0]->name, "@include");
  EXPECT_EQ(doc.directives[1]->name, "@custom");

  ASSERT_EQ(doc.directives[0]->arguments.size(), 1u);
  EXPECT_EQ(doc.directives[0]->arguments[0]->name, "if");
  ASSERT_EQ(doc.directives[0]->arguments[0]->children.size(), 1u);
  EXPECT_EQ(doc.directives[0]->arguments[0]->children[0]->type, syntax_node_type::Variable);
  EXPECT_EQ(doc.directives[0]->arguments[0]->children[0]->name, "$foo");
}

TEST(Parser_t, ParseField)
{
  document_t doc("my_pic :\tpicture(size:128)@include(if:$foo){type data}");
  doc.all_nodes.reserve(20);
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse_field());
  ASSERT_EQ(doc.children.size(), 1u);
  ASSERT_NE(doc.children[0]->selection_set, nullptr);
  ASSERT_EQ(doc.children[0]->directives.size(), 1u);
  ASSERT_EQ(doc.children[0]->arguments.size(), 1u);
  ASSERT_EQ(doc.children[0]->selection_set->children.size(), 2u);
}

TEST(Parser_t, ShortQuery)
{
  document_t doc("{file(skip:0,take:10){items{name Id}totalCount}}");
  doc.all_nodes.reserve(20);
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse());
  ASSERT_EQ(doc.children.size(), 1u);
  EXPECT_EQ(doc.children[0]->type, syntax_node_type::OperationDefinition);
  ASSERT_NE(doc.children[0]->selection_set, nullptr);
  EXPECT_EQ(doc.children[0]->directives.size(), 0u);
  EXPECT_EQ(doc.children[0]->arguments.size(), 0u);
  ASSERT_EQ(doc.children[0]->selection_set->children.size(), 1u);
  EXPECT_EQ(doc.children[0]->selection_set->children[0]->arguments.size(), 2u);
}

TEST(Parser_t, FragmentQuery)
{
  document_t doc("query withFragments{file{items{...fileFields}}} fragment fileFields on File{id name}");
  doc.all_nodes.reserve(20);
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse());
  ASSERT_EQ(doc.children.size(), 2u);
  EXPECT_EQ(doc.children[0]->type, syntax_node_type::OperationDefinition);
  EXPECT_EQ(doc.children[0]->name, "withFragments");
  ASSERT_NE(doc.children[0]->selection_set, nullptr);
  EXPECT_EQ(doc.children[0]->directives.size(), 0u);
  EXPECT_EQ(doc.children[0]->arguments.size(), 0u);
  ASSERT_EQ(doc.children[0]->selection_set->children.size(), 1u);
  EXPECT_EQ(doc.children[0]->selection_set->children[0]->arguments.size(), 0u);

  EXPECT_EQ(doc.children[1]->type, syntax_node_type::FragmentDefinition);
  EXPECT_EQ(doc.children[1]->name, "fileFields");
  
}

TEST(Parser_t, Query)
{
  std::string query = "query files($pattern: String = \"\\\"\"){\r\n\
  file(skip: 0, take : 10 where:{name:{contains: $pattern}}) {\r\n\
    pageInfo {\r\n\
      hasNextPage\r\n\
      # hasPreviousPage\r\n\
    }\r\n\
    items{\r\n\
      filename : name,\r\n\
      Id,\r\n\
      bucket_id,\r\n\
      bucket @skip_if(bucket_id:null),\r\n\
      {\r\n\
        name\r\n\
      }\r\n\
    }\r\n\
    totalCount\r\n\
  }\r\n\
}";

  document_t doc{std::string(query)};
  parser_t parser(doc);
  EXPECT_TRUE(parser.parse());

  // for(auto& node : doc.all_nodes)
  // {
  //   std::cout << enum_name(node.type) << "\t(" << node.pos << ", " << node.size << "): '" << node.content << "'" << std::endl;
  // }
}
