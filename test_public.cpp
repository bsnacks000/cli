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

  err = cli_init(c, desc, usage, argc, (char**)argv);
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  char buf[20] = "";
  err = cli_add_str_option(c, "x", "usage", buf, true, 20);
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  char hello[10] = "";
  char world[10] = "";
  err = cli_add_str_argument(c, hello, 10);  // pos 1
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_str_argument(c, world, 10);  // pos 2
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
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

  err = cli_init(c, desc, usage, argc, (char**)argv);
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

TEST(public, test_cli_parse_int_option_fails_on_bad_conversion) {
  const char* argv[] = {"./myapp", "-x", "hai"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_PARSE_FAILED);

  free(c);
}

TEST(public, test_cli_parse_set_int_arg_fails_on_bad_conversion) {
  const char* argv[] = {"./myapp", "42", "43", "oops"};
  int argc = 4;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x, y, z = 0;

  err = cli_add_int_argument(c, &x);  // pos 1
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_int_argument(c, &y);  // pos 2
  ASSERT_EQ(err, CLI_OK);

  err = cli_add_int_argument(c, &z);  // pos 3
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_PARSE_FAILED);

  free(c);
}

TEST(public, test_cli_parse_handles_mixed_opt_types_args) {
  const char* argv[] = {"./myapp", "--x=42",          "-y",
                        "43.5",    "--z=hello",       "-my-flag",
                        "--",      "./some-file.txt", "999"};
  int argc = 9;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  float y = 0.0;
  err = cli_add_float_option(c, "y", "usage", &y, false);
  ASSERT_EQ(err, CLI_OK);

  int a = 0;
  err = cli_add_int_option(c, "a", "usage", &a, false);
  ASSERT_EQ(err, CLI_OK);

  char z[32] = "";
  err = cli_add_str_option(c, "z", "usage", z, false, 32);
  ASSERT_EQ(err, CLI_OK);

  bool my_flag = false;
  err = cli_add_flag(c, "my-flag", "usage", &my_flag);
  ASSERT_EQ(err, CLI_OK);

  char fname[32] = "";
  err = cli_add_str_argument(c, fname, 32);
  ASSERT_EQ(err, CLI_OK);

  int n_arg = 0;
  err = cli_add_int_argument(c, &n_arg);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OK);

  ASSERT_EQ(x, 42);
  ASSERT_FLOAT_EQ(y, 43.5);
  ASSERT_TRUE(strcmp("hello", z) == 0);
  ASSERT_TRUE(my_flag);

  ASSERT_TRUE(strcmp("./some-file.txt", fname) == 0);
  ASSERT_EQ(n_arg, 999);

  free(c);
}

TEST(public, test_cli_parse_error_on_missing_pos_args) {
  const char* argv[] = {"./myapp", "--x=42"};
  int argc = 2;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  char fname[32] = "";
  err = cli_add_str_argument(c, fname, 32);
  ASSERT_EQ(err, CLI_OK);

  int n_arg = 0;
  err = cli_add_int_argument(c, &n_arg);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_ARG_COUNT);

  free(c);
}

TEST(public, test_cli_parse_error_on_missing_req_opts) {
  const char* argv[] = {"./myapp"};
  int argc = 1;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_UNSEEN_REQ_OPTS);

  free(c);
}

TEST(public, test_cli_parse_handles_missing_opt_val) {
  const char* argv[] = {"./myapp", "-x"};
  int argc = 2;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_OUT_OF_BOUNDS);

  free(c);
}

TEST(public, test_cli_parse_handles_unseen_required_args) {
  const char* argv[] = {"./myapp", "-x", "42"};
  int argc = 3;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  int y = 0;
  err = cli_add_int_option(c, "y", "usage", &y, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_UNSEEN_REQ_OPTS);

  free(c);
}

TEST(public, test_cli_parse_handles_duplicate_opts) {
  const char* argv[] = {"./myapp", "-x", "42", "-x", "43"};
  int argc = 5;

  cli_command* c = cli_command_new();

  cli_err err;
  const char* desc = "A useful app";
  const char* usage = "[OPTIONS]... [N]";

  err = cli_init(c, desc, usage, argc, (char**)argv);
  ASSERT_EQ(err, CLI_OK);

  int x = 0;
  err = cli_add_int_option(c, "x", "usage", &x, true);
  ASSERT_EQ(err, CLI_OK);

  err = cli_parse(c);
  ASSERT_EQ(err, CLI_ALREADY_SEEN);

  free(c);
}