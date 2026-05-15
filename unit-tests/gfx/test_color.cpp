#include "gfx/Color.h"
#include <gtest/gtest.h>

TEST(ColorTest, FromName) {
  gfx::Color white = gfx::Color::from_name("white");
  EXPECT_EQ(white.red, 255);
  EXPECT_EQ(white.green, 255);
  EXPECT_EQ(white.blue, 255);
  EXPECT_EQ(white.alpha, 255);

  gfx::Color black = gfx::Color::from_name("black");
  EXPECT_EQ(black.red, 0);
  EXPECT_EQ(black.alpha, 255);

  gfx::Color unknown = gfx::Color::from_name("non-existent");
  EXPECT_EQ(unknown.red, 0); // Defaults to Black
}

TEST(ColorTest, FromHex) {
  gfx::Color red = gfx::Color::from_hex("#FF0000");
  EXPECT_EQ(red.red, 255);
  EXPECT_EQ(red.green, 0);
  EXPECT_EQ(red.blue, 0);

  gfx::Color short_blue = gfx::Color::from_hex("#00F");
  EXPECT_EQ(short_blue.red, 0);
  EXPECT_EQ(short_blue.green, 0);
  EXPECT_EQ(short_blue.blue, 255);

  gfx::Color invalid = gfx::Color::from_hex("invalid");
  EXPECT_EQ(invalid.red, 0); // Defaults to Black
}

TEST(ColorTest, FromRGB) {
  gfx::Color custom = gfx::Color::from_rgb(10, 20, 30);
  EXPECT_EQ(custom.red, 10);
  EXPECT_EQ(custom.green, 20);
  EXPECT_EQ(custom.blue, 30);
  EXPECT_EQ(custom.alpha, 255);
}

TEST(ColorTest, Equality) {
  gfx::Color c1 = {100, 150, 200, 255};
  gfx::Color c2 = {100, 150, 200, 255};
  gfx::Color c3 = {101, 150, 200, 255};

  EXPECT_EQ(c1, c2);
  EXPECT_NE(c1, c3);
}
