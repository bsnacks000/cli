#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../cli.h"

void fail_fast(cli_err err);

int main(int argc, char** argv) {
  // printf("token length: %d\n", CLI_OPT_TOKEN_MAX_LEN);

  cli_err err;
  cli_command* c = cli_command_new();

  const char* desc = "Says hi and does a pointless calculation...\n";
  const char* usage = "[-name] int float\n";

  err = cli_init(c, desc, usage, argc, argv);
  fail_fast(err);

  char name[CLI_OPT_TOKEN_MAX_LEN] = "";
  err = cli_add_str_option(c, "name", "Your name. Optional.", name, false,
                           CLI_OPT_TOKEN_MAX_LEN);
  fail_fast(err);

  float x = 0.0;
  err = cli_add_float_argument(c, &x);
  fail_fast(err);

  int y = 0;
  err = cli_add_int_argument(c, &y);
  fail_fast(err);

  if ((err = cli_parse(c)) != CLI_OK) {
    cli_print_err(err);
    cli_print_help_and_exit(c, 1);
  }

  if (strlen(name) == 0) {
    strcat(name, "(whoever you are)");
  }

  float result = x + (float)y;

  printf("Hello %s.\nResult: %.5f\n", name, result);
  return 0;
}

void fail_fast(cli_err err) {
  if (err != CLI_OK) {
    cli_print_err(err);
    exit(1);
  }
}