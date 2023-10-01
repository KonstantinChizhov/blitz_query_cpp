#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "tokenizer.hpp"

using namespace blitz_query_cpp;

TEST(Tokenizer, Empty)
{
    std::string query = "";
    tokenizer<char> tokenizer(query);
    auto token = tokenizer.next_token();
    ASSERT_EQ(token.type, token_type::End);
}

TEST(Tokenizer, TokenizeQuery1)
{
    std::string query = "\"\"\" \
    This is a description \
\"\"\"\
    query{\r\n\
  file(skip: 0, take : 10 where:{name:{contains:\"\\\"\"}}) {\r\n\
    pageInfo {\r\n\
      hasNextPage\r\n\
      # hasPreviousPage\r\n\
    }\r\n\
    items{\r\n\
      filename : name,\r\n\
      bucket,\r\n\
      {\r\n\
        name\r\n\
      }\r\n\
    }\r\n\
    totalCount\r\n\
  }\r\n\
}";

    tokenizer<char> tokenizer(query);

    auto token = tokenizer.next_token();
    int token_count = 0;
    while (token.type != token_type::End && token.type != token_type::InvalidToken)
    {
        std::cout << token_type_name(token.type) << "\t" << token.pos << "\t" << token.value << std::endl;
        token = tokenizer.next_token();
        token_count++;
    }
    ASSERT_EQ(token_count, 42);
}
