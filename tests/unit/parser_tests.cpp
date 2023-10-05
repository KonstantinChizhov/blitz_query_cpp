#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "parser/parser.hpp"
#include <format>

using namespace blitz_query_cpp;

TEST(Parser, ParseArgumentDefinitions)
{
  document doc("(string: String = \"foo\", int :Int)");
  doc.all_nodes.reserve(10);
  parser parser(doc);
  EXPECT_TRUE(parser.parse_argument_definitions());
  ASSERT_EQ(doc.children.size(), 2);
  ASSERT_EQ(doc.arguments.size(), doc.children.size());

  ASSERT_EQ(doc.children[0]->type, syntax_node_type::InputValueDefinition);
  ASSERT_EQ(doc.children[1]->type, syntax_node_type::InputValueDefinition);

  ASSERT_EQ(doc.children[0]->name, "string");
  ASSERT_EQ(doc.children[1]->name, "int");

  ASSERT_NE(doc.children[0]->definition_type, nullptr);
  ASSERT_NE(doc.children[1]->definition_type, nullptr);
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
