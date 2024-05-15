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

TEST(public, test_cli_parse_set_int_option_correctly) {
  const char* argv[] = {"./myapp", "-x", "42"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 1;
  size_t n_args = 0;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  int expected = 42;
  ASSERT_EQ(x, expected);

  free(c);
}

TEST(public, test_cli_parse_set_float_option_correctly) {
  const char* argv[] = {"./myapp", "-x", "42.12345"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 1;
  size_t n_args = 0;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  float x = 0.0;
  err = cli_add_float_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  float expected = 42.12345;
  ASSERT_EQ(x, expected);

  free(c);
}

TEST(public, test_cli_parse_set_str_args_correctly) {
  const char* argv[] = {"./myapp", "hello", "world"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 0;  // note:: getting segfaults if misconfigured... needs a
                      // check somewhere.
  size_t n_args = 2;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  char hello[10] = "";
  char world[10] = "";
  err = cli_add_str_argument(c, hello);  // pos 1
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_str_argument(c, world);  // pos 2
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  char expected_hello[] = "hello";
  ASSERT_TRUE(strcmp(hello, expected_hello) == 0);

  char expected_world[] = "world";
  // printf("world -> %s\n", world);
  ASSERT_TRUE(strcmp(world, expected_world) == 0);

  free(c);
}

TEST(public, test_cli_parse_set_int_args_correctly) {
  const char* argv[] = {"./myapp", "42", "43", "44"};
  int argc = 4;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 0;  // note:: getting segfaults if misconfigured... needs a
                      // check somewhere.
  size_t n_args = 3;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  int x, y, z = 0;

  err = cli_add_int_argument(c, &x);  // pos 1
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_int_argument(c, &y);  // pos 2
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_int_argument(c, &z);  // pos 3
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  ASSERT_EQ(x, 42);
  ASSERT_EQ(y, 43);
  ASSERT_EQ(z, 44);

  free(c);
}

TEST(public, test_cli_parse_set_float_args_correctly) {
  const char* argv[] = {"./myapp", "42.123", "43.456", "44.789"};
  int argc = 4;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";
  size_t n_opts = 0;  // note:: getting segfaults if misconfigured... needs a
                      // check somewhere.
  size_t n_args = 3;

  err = cli_init(c, desc, usage, argc, (char**)argv, n_opts, n_args);
  ASSERT_EQ(err, CLI_OK);

  float x, y, z = 0.0;

  err = cli_add_float_argument(c, &x);  // pos 1
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_float_argument(c, &y);  // pos 2
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_float_argument(c, &z);  // pos 3
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  ASSERT_FLOAT_EQ(x, 42.123);
  ASSERT_FLOAT_EQ(y, 43.456);
  ASSERT_FLOAT_EQ(z, 44.789);

  free(c);
}