#include <gtest/gtest.h>
#include "cli/cli.h"

// to unittest private API components
#include "cli.c"

TEST(SmokeTest, Smoke) {
  ASSERT_EQ(1, 1);
}
