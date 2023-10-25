#include <util/crc_hash.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace blitz_query_cpp;


TEST(Hash, Crc32)
{
    auto res = hash_crc32(Crc32::CheckMessage());
    EXPECT_EQ(res, Crc32::Check);

    res = hash_crc32(Crc32WithTable::CheckMessage());
    EXPECT_EQ(res, Crc32WithTable::Check);
}