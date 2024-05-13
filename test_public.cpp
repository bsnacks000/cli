#include <gtest/gtest.h>
#include "cli.h"

// to unittest public API components

TEST(public, test_cli_command_new) {
  cli_command* c = cli_command_new();
  free(c);
}
