#include <gtest/gtest.h>
#include <stdbool.h>

#include "cli.h"

// tests public API components

TEST(public, test_cli_command_new) {
  cli_command* c = cli_command_new();
  free(c);
}

TEST(public, test_cli_init_) {
  // myapp -n 42 -- 43
  const char* argv[] = {"./myapp", "-n", "42", "--", "43"};
  int argc = 5;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv, 1, 1);
  ASSERT_EQ(err, CLI_OK);

  free(c);
}

TEST(public, test_cli_parse_sets_boolean_flag_correctly) {
  const char* argv[] = {"./myapp", "-x", "-y"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 2;
  size_t n_args = 0;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  bool x_bool = false;
  err = cli_add_flag(c, "x", "usage", &x_bool);
  ASSERT_EQ(err, CLI_OK);

  bool y_bool = false;
  err = cli_add_flag(c, "y", "usage", &y_bool);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  ASSERT_EQ(x_bool, true);
  ASSERT_EQ(y_bool, true);

  free(c);
}

TEST(public, test_cli_parse_set_str_option_correctly) {
  const char* argv[] = {"./myapp", "-x", "my_string"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 1;
  size_t n_args = 0;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  char buf[20] = "";
  err = cli_add_str_option(c, "x", "usage", buf, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  char expected[] = "my_string";
  ASSERT_TRUE(strcmp(buf, expected) == 0);

  free(c);
}