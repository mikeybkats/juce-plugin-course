#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>
#include <vector>

namespace ws::detail {
TEST(StridedQueue, pushBack) {
  StridedQueue<int, 5> testee;
  testee.setStride(3u);

  testee.pushBack(std::vector{1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5});
  ASSERT_EQ(1, testee.at(0u));
  ASSERT_EQ(2, testee.at(1u));
  ASSERT_EQ(3, testee.at(2u));
  ASSERT_EQ(4, testee.at(3u));
  ASSERT_EQ(5, testee.at(4u));

  testee.pushBack(std::vector{0, 0, 6, 0, 0, 7, 0});

  ASSERT_EQ(3, testee.at(0u));
  ASSERT_EQ(4, testee.at(1u));
  ASSERT_EQ(5, testee.at(2u));
  ASSERT_EQ(6, testee.at(3u));
  ASSERT_EQ(7, testee.at(4u));

  testee.pushBack(std::vector{0, 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 0});
  ASSERT_EQ(1, testee.at(0u));
  ASSERT_EQ(2, testee.at(1u));
  ASSERT_EQ(3, testee.at(2u));
  ASSERT_EQ(4, testee.at(3u));
  ASSERT_EQ(5, testee.at(4u));
}
}  // namespace ws::detail
