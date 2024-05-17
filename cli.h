#ifndef __CLI_H__
#define __CLI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifndef CLI_OPT_TOKEN_MAX_LEN
#define CLI_OPT_TOKEN_MAX_LEN 64
#endif

#ifndef CLI_OPT_USAGE_MAX_LEN
#define CLI_OPT_USAGE_MAX_LEN 128
#endif

#ifndef CLI_MAX_OPTS
#define CLI_MAX_OPTS 64
#endif

#ifndef CLI_MAX_ARGS
#define CLI_MAX_ARGS 64
#endif

#define CLI_UNUSED(x) (void)(x)

#define CLI_CHECK_MEM_ALLOC(value)       \
  do {                                   \
    if ((value) == NULL) {               \
      fprintf(stderr, "Out of memory."); \
      exit(EXIT_FAILURE);                \
    }                                    \
  } while (0)

typedef enum cli_err {
  CLI_OK = 0,
  CLI_PARSE_FAILED,
  CLI_FULL_REGISTRY,
  CLI_NOT_FOUND,
  CLI_NAME_REQUIRED,
  CLI_UNSEEN_REQ_OPTS,
  CLI_OUT_OF_BOUNDS,
  CLI_ALREADY_SEEN,
  CLI_ARG_COUNT,
  CLI_MISCONFIGURED,
  CLI_PRINT_HELP_AND_EXIT,
  CLI_TOKEN_TOO_LONG,
  CLI_USAGE_STR_TOO_LONG
} cli_err;

void cli_print_err(cli_err err);

typedef struct cli_command cli_command;

cli_command* cli_command_new(void);

void cli_command_destroy(cli_command* cli);

cli_err cli_init(cli_command* cli,
                 const char* desc,
                 const char* usage,
                 int argc,
                 char** argv);

void cli_cleanup(cli_command* cli);

// high level API for adding options and arguments

cli_err cli_add_flag(cli_command* cli,
                     const char* name,
                     const char* usage,
                     bool* value);

cli_err cli_add_int_argument(cli_command* cli, int* value);

cli_err cli_add_int_option(cli_command* cli,
                           const char* name,
                           const char* usage,
                           int* value,
                           bool required);

cli_err cli_add_float_argument(cli_command* cli, float* value);

cli_err cli_add_float_option(cli_command* cli,
                             const char* name,
                             const char* usage,
                             float* value,
                             bool required);

cli_err cli_add_str_argument(cli_command* cli, char* value);

cli_err cli_add_str_option(cli_command* cli,
                           const char* name,
                           const char* usage,
                           char* value,
                           bool required);

void cli_print_help_and_exit(cli_command* cli, int status);

cli_err cli_parse(cli_command* cli);

#ifdef __cplusplus
}
#endif

#endif  //!__CLI_H__