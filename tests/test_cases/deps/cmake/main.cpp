#include <gtest/gtest.h>

TEST(CastTest, float) { EXPECT_EQ(42.0f, float(42)); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
