#include <type_system/type_system_classes.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <type_system/schema.hpp>
#include <type_system/schema_parser.hpp>

using namespace blitz_query_cpp;

TEST(Schema, ParseSchemaDef)
{
   std::string scm = "schema { query: Query mutation: Mutation }";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   EXPECT_EQ(res, true);
   EXPECT_EQ(my_schema.query_type_name, "Query");
   EXPECT_EQ(my_schema.mutation_type_name, "Mutation");
}

TEST(Schema, ParseScalarDef)
{
   std::string scm = "scalar UUID @specifiedBy(url: \"https://tools.ietf.org/html/rfc4122\")";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   EXPECT_EQ(res, true);
}