#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "parser/parser.hpp"
#include <format>

using namespace blitz_query_cpp;

TEST(Parser, ParseArgumentDefinitions)
{
  document doc("(stringVal: String = \"foo\", intVal :Int)");
  doc.all_nodes.reserve(10);
  parser parser(doc);
  EXPECT_TRUE(parser.parse_argument_definitions());
  ASSERT_EQ(doc.children.size(), 2);
  ASSERT_EQ(doc.arguments.size(), doc.children.size());

  ASSERT_EQ(doc.children[0]->type, syntax_node_type::InputValueDefinition);
  ASSERT_EQ(doc.children[1]->type, syntax_node_type::InputValueDefinition);

  ASSERT_EQ(doc.children[0]->name, "stringVal");
  ASSERT_EQ(doc.children[1]->name, "intVal");

  ASSERT_NE(doc.children[0]->definition_type, nullptr);
  ASSERT_NE(doc.children[1]->definition_type, nullptr);

  ASSERT_NE(doc.children[0]->definition_type->name, "String");
  ASSERT_NE(doc.children[0]->definition_type->name, "Int");
}

TEST(Parser, ParseArguments)
{
  document doc("(stringVal: \"foo\", intVal :42, objVal: {foo:10 bar:null} boolVal: true enumVal: Hello, floatVal : 3.14e2)");
  doc.all_nodes.reserve(20);
  parser parser(doc);
  EXPECT_TRUE(parser.parse_arguments(false));
  ASSERT_EQ(doc.children.size(), 6);
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

TEST(Parser, ParseDirectives)
{
  document doc("@include(if:$foo) @custom");
  doc.all_nodes.reserve(20);
  parser parser(doc);
  EXPECT_TRUE(parser.parse_directives(false));
  ASSERT_EQ(doc.children.size(), 2);
  ASSERT_EQ(doc.directives.size(), doc.children.size());

  EXPECT_EQ(doc.directives[0]->type, syntax_node_type::Directive);
  EXPECT_EQ(doc.directives[1]->type, syntax_node_type::Directive);

  EXPECT_EQ(doc.directives[0]->name, "@include");
  EXPECT_EQ(doc.directives[1]->name, "@custom");

  ASSERT_EQ(doc.directives[0]->arguments.size(), 1);
  EXPECT_EQ(doc.directives[0]->arguments[0]->name, "if");
  ASSERT_EQ(doc.directives[0]->arguments[0]->children.size(), 1);
  EXPECT_EQ(doc.directives[0]->arguments[0]->children[0]->type, syntax_node_type::Variable);
  EXPECT_EQ(doc.directives[0]->arguments[0]->children[0]->name, "$foo");
}

TEST(Parser, LongQuery)
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

  document doc(query);

  parser parser(doc);
  EXPECT_TRUE(parser.parse());

  // for(auto& node : doc.all_nodes)
  // {
  //   std::cout << enum_name(node.type) << "\t(" << node.pos << ", " << node.size << "): '" << node.content << "'" << std::endl;
  // }
}
