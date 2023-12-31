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
   std::string scm = "\"scalar description\" scalar UUID @specifiedBy(url: \"https://tools.ietf.org/html/rfc4122\" foo: {bar: 1 baz: false}, param: null)";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   ASSERT_EQ(res, true);
   ASSERT_EQ(my_schema.types.size(), 1u);
   auto &type = my_schema.types.begin()->second;
   EXPECT_EQ(type.description, "scalar description");
   EXPECT_EQ(type.kind, type_kind::Scalar);
   ASSERT_EQ(type.directives.size(), 1u);
   EXPECT_EQ(type.directives[0].name, "specifiedBy");
}

TEST(Schema, ParseEnumDef)
{
   std::string scm = "\"enum description\" enum BookGenre @foo {ART CHILDREN_BOOK SCIENCE}";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   ASSERT_EQ(res, true);
   ASSERT_EQ(my_schema.types.size(), 1u);
   auto &type = my_schema.types.begin()->second;
   ;
   EXPECT_EQ(type.description, "enum description");
   ASSERT_EQ(type.kind, type_kind::Enum);
   ASSERT_EQ(type.fields.size(), 3u);
   ASSERT_EQ(type.fields.contains("ART"), true);
   ASSERT_EQ(type.fields.contains("CHILDREN_BOOK"), true);
   ASSERT_EQ(type.fields.contains("SCIENCE"), true);
}

TEST(Schema, ParseDirectiveDef)
{
   std::string scm = "\"directive description\" directive @foo (\"param descr\" param: String!, foo :Foo = {bar: 1 buz: true}) repeatable on SCALAR | ENUM";
   schema my_schema;
   schema_parser parser;
   bool res = parser.parse(my_schema, scm);
   ASSERT_EQ(res, true);
   ASSERT_EQ(my_schema.directives.size(), 1u);
   auto &type = my_schema.directives.begin()->second;
   EXPECT_EQ(type.description, "directive description");
   EXPECT_EQ(type.name, "@foo");
   EXPECT_EQ(type.kind, type_kind::Directive);
   EXPECT_EQ(type.target, directive_target_t::Scalar | directive_target_t::Enum | directive_target_t::IsRepeatable);

   ASSERT_EQ(type.arguments.size(), 2u);
   ASSERT_EQ(type.arguments.contains("param"), true);
   ASSERT_EQ(type.arguments.contains("foo"), true);

   EXPECT_EQ(type.arguments["param"].field_type_name, "String");
   EXPECT_EQ(type.arguments["param"].field_type_nullability, 0u);
   EXPECT_EQ(type.arguments["param"].description, "param descr");
   EXPECT_EQ(type.arguments["param"].default_value.value_type, value_kind::None);

   EXPECT_EQ(type.arguments["foo"].field_type_name, "Foo");
   EXPECT_EQ(type.arguments["foo"].field_type_nullability, 1u);
   EXPECT_EQ(type.arguments["foo"].description, "");
   EXPECT_EQ(type.arguments["foo"].default_value.value_type, value_kind::Object);

}