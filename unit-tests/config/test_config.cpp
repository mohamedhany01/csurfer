#include "config/Config.h"
#include <gtest/gtest.h>

/**
 * Story: We test our configuration to ensure that the relationships between
 * UI elements remain consistent. This prevents regressions where one value
 * is changed but its dependents are not.
 */
TEST(ConfigTest, UIHeightConsistency) {
  // Arrange & Act (using constants)
  int calculated_height =
      config::TAB_HEIGHT + config::ADDR_HEIGHT + config::UI_PADDING;

  // Assert
  EXPECT_EQ(config::UI_HEIGHT, calculated_height)
      << "UI_HEIGHT must be the sum of TAB_HEIGHT, ADDR_HEIGHT, and UI_PADDING";
}

TEST(ConfigTest, SanityChecks) {
  EXPECT_GT(config::WINDOW_WIDTH, 0);
  EXPECT_GT(config::WINDOW_HEIGHT, 0);
  EXPECT_GT(config::DEFAULT_FONT_SIZE, 0);
}
