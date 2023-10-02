#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "parser/parser.hpp"
#include <format>

using namespace blitz_query_cpp;


TEST(Parser, ParseQuery1)
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
      bucket,\r\n\
      {\r\n\
        name\r\n\
      }\r\n\
    }\r\n\
    totalCount\r\n\
  }\r\n\
}";

    document doc(query);

    parser parser(doc);
    parser.parse();
}
