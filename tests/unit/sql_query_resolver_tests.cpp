#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <type_system/schema.hpp>
#include <type_system/schema_parser.hpp>
#include <processing/sql_query_resolver.hpp>
#include <processing/parse_document.hpp>
#include <data/sql/postgresql_renderer.hpp>
#include <string>

using namespace blitz_query_cpp;

TEST(SqlResolver, SimpleSelect)
{
    std::string scm = R""""(
        schema { query: Query }
        type Query
        {
           Users :[User]
        }
        type User @table(table: "user" schema: "test")
        {
           Id: Int @column(name: "id" IsPK: True)
           Name: String @column(name: "name")
           Age: Int  @column(name: "age")
           AccountId :Int @always_projected
        }

        directive @table(table: String schema: String) on OBJECT
        directive @column(name: String IsPK: Boolean = False) on FIELD_DEFINITION
        directive @always_projected(projected: Boolean = True) on FIELD_DEFINITION
        )"""";

    std::string query = R""""(
        {
            Users {     
                Name
                Age
                Id
            }
        }
 )"""";

    schema_t my_schema;
    schema_parser_t parser;
    bool res = parser.parse(my_schema, scm);
    EXPECT_EQ(res, true);
    EXPECT_EQ(my_schema.query_type_name, "Query");

    query_context context(&my_schema, query);
    parse_document doc_parser;
    sql_query_resolver resolver;
    EXPECT_TRUE(doc_parser.process(context));
    EXPECT_TRUE(resolver.process(context));

    if (const sql::sql_expr_t *pexpr = std::get_if<sql::sql_expr_t>(&context.result))
    {
        sql::postgresql_renderer renderer;
        renderer.render(*pexpr);
        std::string sql = renderer.get_string();
        EXPECT_EQ(sql, "SELECT _a1.name, _a1.age, _a1.id, _a1.\"AccountId\" FROM test.user as _a1");
    }
}