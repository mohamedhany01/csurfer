#include "utils/Geometry.h"
#include <gtest/gtest.h>

using namespace utils;

TEST(GeometryTest, PointCreation) {
  Point point = {10, 20};
  EXPECT_EQ(point.x, 10);
  EXPECT_EQ(point.y, 20);
}

TEST(GeometryTest, RectCreation) {
  Rect rect = {{10, 20}, 100, 200};
  EXPECT_EQ(rect.origin.x, 10);
  EXPECT_EQ(rect.origin.y, 20);
  EXPECT_EQ(rect.width, 100);
  EXPECT_EQ(rect.height, 200);
}

TEST(GeometryTest, RectContainsPoint) {
  Rect rect = {{10, 10}, 50, 50};

  // Inside
  EXPECT_TRUE(rect.contains({20, 20}));
  EXPECT_TRUE(rect.contains({10, 10})); // Top-left boundary
  EXPECT_TRUE(rect.contains({60, 60})); // Bottom-right boundary

  // Outside
  EXPECT_FALSE(rect.contains({5, 5}));
  EXPECT_FALSE(rect.contains({61, 61}));
  EXPECT_FALSE(rect.contains({30, 70}));
  EXPECT_FALSE(rect.contains({70, 30}));
}

TEST(GeometryTest, Equality) {
  Point p1 = {10, 20};
  Point p2 = {10, 20};
  Point p3 = {11, 20};

  EXPECT_EQ(p1, p2);
  EXPECT_NE(p1, p3);

  Rect r1 = {{10, 20}, 100, 200};
  Rect r2 = {{10, 20}, 100, 200};
  Rect r3 = {{11, 20}, 100, 200};

  EXPECT_EQ(r1, r2);
  EXPECT_NE(r1, r3);
}
