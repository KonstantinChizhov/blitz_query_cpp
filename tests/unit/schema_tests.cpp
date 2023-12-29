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
   std::string scm = "scalar UUID @specifiedBy(url: \"https://tools.ietf.org/html/rfc4122\" foo: {bar: 1 baz: false}, param: null)";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   ASSERT_EQ(res, true);
   ASSERT_EQ(my_schema.types.size(), 1u);
   ASSERT_EQ(my_schema.types[0].kind, type_kind::Scalar);
   ASSERT_EQ(my_schema.types[0].directives.size(), 1u);
   ASSERT_EQ(my_schema.types[0].directives[0].name, "specifiedBy");
}

TEST(Schema, ParseEnumDef)
{
   std::string scm = "enum BookGenre {ART CHILDREN_BOOK SCIENCE}";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   ASSERT_EQ(res, true);
   ASSERT_EQ(my_schema.types.size(), 1u);
   ASSERT_EQ(my_schema.types[0].kind, type_kind::Enum);
   ASSERT_EQ(my_schema.types[0].fields.size(), 3u);
   ASSERT_EQ(my_schema.types[0].fields.contains("ART"), true);
   ASSERT_EQ(my_schema.types[0].fields.contains("CHILDREN_BOOK"), true);
   ASSERT_EQ(my_schema.types[0].fields.contains("SCIENCE"), true);
}