/**
 * @file cli.c

 * @author bsnacks000
 * @brief
 * @version 0.1.0
 * @date 2024-05-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"

void cli_print_err(cli_err err) {
  switch (err) {
    case CLI_PARSE_FAILED_INT:
      fprintf(stderr, "err: token parse failed for integer.\n");
      break;
    case CLI_PARSE_FAILED_BOOL:
      fprintf(stderr, "err: token parse failed for boolean.\n");
      break;
    case CLI_PARSE_FAILED_FLOAT:
      fprintf(stderr, "err: token parse failed for float.\n");
      break;
    case CLI_PARSE_FAILED_STR:
      fprintf(stderr, "err: token parse failed for str: buf too small.\n");
      break;
    case CLI_FULL_REGISTRY:
      fprintf(stderr, "err: registry full.\n");
      break;
    case CLI_NOT_FOUND:
      fprintf(stderr, "err: token not found.\n");
      break;
    case CLI_NAME_REQUIRED:
      fprintf(stderr, "err: option name not found.\n");
      break;
    case CLI_UNSEEN_REQ_OPTS:
      fprintf(stderr, "err: unseen required options.\n");
      break;
    case CLI_OUT_OF_BOUNDS:
      fprintf(stderr, "err: out of bounds during parse.\n");
      break;
    case CLI_ALREADY_SEEN:
      fprintf(stderr, "err: option was already seen.\n");
      break;
    case CLI_ARG_COUNT:
      fprintf(stderr, "err: misconfigured positional arguments.\n");
      break;
    case CLI_TOKEN_TOO_LONG:
      fprintf(stderr, "err: token longer then allowed max.\n");
      break;
    case CLI_USAGE_STR_TOO_LONG:
      fprintf(stderr, "err: usage string longer then allowed max.\n");
      break;
    default:
      break;
  }
}

struct cli_opt;
struct cli_arg;

typedef cli_err (*cli_opt_parser)(struct cli_opt* opt, const char* token);
typedef cli_err (*cli_arg_parser)(struct cli_arg* arg, const char* token);

// internal box to carry the string buffer size so we don't overflow :(
// this is allocated in the public API and deallocated here after the parse.
typedef struct str_box {
  char* ptr;
  size_t sz;
} str_box;

// an array manager to deal with the liftime for the above.
typedef struct str_boxes {
  str_box** arr;
  size_t cap;
  size_t idx;
} str_boxes;

void str_boxes_init(str_boxes* b, size_t cap) {
  str_box** arr = (str_box**)calloc(cap, sizeof(str_box));
  CLI_CHECK_MEM_ALLOC(arr);
  b->arr = arr;
  b->cap = cap;
  b->idx = 0;
}

// create a new str_box and return a reference via sb_val
cli_err str_boxes_add(str_boxes* b, char* ptr, size_t sz, str_box** sb_val) {
  if (b->idx == b->cap) {
    return CLI_FULL_REGISTRY;
  }

  str_box* sb = (str_box*)malloc(sizeof(str_box));
  CLI_CHECK_MEM_ALLOC(sb);
  sb->ptr = ptr;
  sb->sz = sz;

  // add the the arena.
  b->arr[b->idx] = sb;
  b->idx++;
  *sb_val = sb;

  return CLI_OK;
}

void str_boxes_cleanup(str_boxes* b) {
  for (; (int)b->idx > 0; b->idx--) {
    free(b->arr[b->idx - 1]);
  }
  free(b->arr);
}

typedef struct cli_opt {
  const char* name;       // name of the arg without `-` or `--` prefix.
  const char* usage;      // A usage statement for help
  cli_opt_parser parser;  // the parse function.
  void* value;            // generic pointer to target value to set on parse.
  bool required;          // whether this option must be parsed
  bool seen;              // set when opt is seen in the parser
  bool is_flag;           // Tells parser to pass NULL argument to parser
} cli_opt;

typedef struct cli_opts {
  cli_opt** opts;  // the flag options to be parsed
  size_t cap;      // capacity for option array
  size_t idx;      // the current idx into the option array
} cli_opts;

// flag opts API

// TODO mem error handling
void cli_opts_init(cli_opts* opts, size_t cap) {
  cli_opt** opts_arr = (cli_opt**)calloc(cap, sizeof(cli_opt*));
  CLI_CHECK_MEM_ALLOC(opts_arr);

  opts->opts = opts_arr;
  opts->idx = 0;
  opts->cap = cap;
}

void cli_opts_cleanup(cli_opts* opts) {
  for (; (int)opts->idx > 0; opts->idx--) {
    free(opts->opts[opts->idx - 1]);
  }
  free(opts->opts);
}

cli_err cli_opts_add(cli_opts* opts,
                     const char* name,
                     const char* usage,
                     cli_opt_parser parser,
                     void* value,
                     bool required,
                     bool is_flag) {
  if (name == NULL) {
    return CLI_NAME_REQUIRED;
  }

  if (opts->idx == opts->cap) {
    return CLI_FULL_REGISTRY;
  }

  if (strlen(name) + 1 > CLI_OPT_TOKEN_MAX_LEN) {
    return CLI_TOKEN_TOO_LONG;
  }

  if (strlen(usage) + 1 > CLI_OPT_USAGE_MAX_LEN) {
    return CLI_USAGE_STR_TOO_LONG;
  }

  cli_opt* o = (cli_opt*)malloc(sizeof(cli_opt));
  CLI_CHECK_MEM_ALLOC(o);

  o->name = name;
  o->usage = usage;
  o->parser = parser;
  o->value = value;
  o->required = required;
  o->seen = false;
  o->is_flag = is_flag;

  opts->opts[opts->idx] = o;
  opts->idx++;  // current idx is always the len of the opts
  return CLI_OK;
}

bool cli_opts_n_required_seen(cli_opts* opts) {
  size_t count_req = 0;
  size_t count_seen = 0;
  for (size_t i = 0; i < opts->idx; i++) {
    if (opts->opts[i]->required) {
      count_req++;
      if (opts->opts[i]->seen) {
        count_seen++;
      }
    }
  }
  // printf("req: %zu - seen: %zu\n", count_req, count_seen);
  return count_req == count_seen;
}

cli_opt* cli_opts_find(cli_opts* opts, const char* name) {
  for (size_t i = 0; i < opts->idx; i++) {
    if (strcmp(opts->opts[i]->name, name) == 0) {
      return opts->opts[i];
    }
  }
  return NULL;
}

void cli_opt_print_message(cli_opt* o, char* buf) {
  size_t name_l = strlen(o->name);
  size_t usage_l = strlen(o->usage);
  int err =
      snprintf(buf, name_l + usage_l + 5, "\t-%s\t\t%s\n", o->name, o->usage);

  if (err < 0) {
    fprintf(stderr, "Assertion Error: cli_opt_print_message failed: %d\n", err);
  }
}

// flag arg API
// anything after `--` or any token after the flag parse finishes.

typedef struct cli_arg {
  cli_arg_parser parser;
  void* value;
} cli_arg;

typedef struct cli_args {
  cli_arg** args;
  size_t cap;
  size_t idx;
} cli_args;

void cli_args_init(cli_args* args, size_t cap) {
  cli_arg** args_arr = (cli_arg**)calloc(cap, sizeof(cli_arg*));
  CLI_CHECK_MEM_ALLOC(args_arr);
  args->args = args_arr;
  args->idx = 0;
  args->cap = cap;
}

void cli_args_cleanup(cli_args* args) {
  for (; (int)args->idx > 0; args->idx--) {
    free(args->args[args->idx - 1]);
  }
  free(args->args);
}

cli_err cli_args_add(cli_args* args, cli_arg_parser parser, void* value) {
  if (args->idx == args->cap) {
    return CLI_FULL_REGISTRY;
  }

  cli_arg* a = (cli_arg*)malloc(sizeof(cli_arg));
  CLI_CHECK_MEM_ALLOC(a);
  a->parser = parser;
  a->value = value;

  args->args[args->idx] = a;
  args->idx++;
  return CLI_OK;
}

// the main cli_parse function
// result type is used to report more info about the failed parse.

cli_err cli_parse_loop(cli_opts* opts, cli_args* args, int argc, char** argv) {
  int argv_i = 1;

  // if we've configured correctly we should always have help, h flags out of
  // the box...

  if (opts != NULL) {
    while (argv_i < argc) {
      char* token = argv[argv_i];

      // printf("current token: %s\n", token);
      //  check an exact match on delimiter first
      if ((strcmp(token, "--") == 0)) {
        argv_i++;
        break;
      }

      // move the token pointer based on whether we detect a flag prefix
      // (--, -)
      if ((strncmp(token, "--", 2)) == 0) {
        token += 2;
      } else if ((strncmp(token, "-", 1)) == 0) {
        token += 1;
      } else {
        break;
      }

      // short circuit the parse if we encounter help ... we immediately
      // break out of the parse and should exit with the usage message
      if ((strcmp(token, "h") == 0) || (strcmp(token, "help") == 0)) {
        return CLI_PRINT_HELP_AND_EXIT;
      }

      char tok_[CLI_OPT_TOKEN_MAX_LEN] = "";
      strncpy(tok_, token, strlen(token) + 1);

      char* tok_str = tok_;

      // sep on =
      char* first = strsep(&tok_str, "=");
      char* second = strsep(&tok_str, "=");
      // first should be guarateed
      //  second will either be none or garbage if = is abused

      // printf("[%s, %s]\n", first, second);

      cli_opt* opt;
      if ((opt = cli_opts_find(opts, first)) == NULL) {
        return CLI_NOT_FOUND;
      }
      // check if we've seen this flag
      if (opt->seen) {
        return CLI_ALREADY_SEEN;
      }

      // set the seen flag for the opt...
      opt->seen = true;

      // handle case where opt->is_flag = true;
      if (opt->is_flag) {
        cli_err err = opt->parser(opt, NULL);
        if (err != CLI_OK) {
          return err;
        }
        // incr += 1 and skip to next iter
        argv_i++;

        continue;
      }
      // we have a valid token like --data=42 split -> data, 42
      // it must be a value parser
      if (second != NULL) {
        cli_err err = opt->parser(opt, second);
        if (err != CLI_OK) {
          return err;
        }
        argv_i++;
      } else {
        // we have a single arg but we want to do a value lookup in the
        // next index...

        // check argv_i is not at the end so we don't reach over argv
        if (argv_i + 1 == argc) {
          return CLI_OUT_OF_BOUNDS;
        }

        cli_err err = opt->parser(opt, argv[argv_i + 1]);
        if (err != CLI_OK) {
          return err;
        }
        argv_i += 2;
      }
    }
    // check that we have seen all required opts
    // if the parse broke early
    if (!cli_opts_n_required_seen(opts)) {
      return CLI_UNSEEN_REQ_OPTS;
    }
  }

  if (args != NULL) {
    // check that argc - the current argv i == the number of registered
    // positional args
    if ((argc - argv_i) != (int)(args->idx)) {
      return CLI_ARG_COUNT;
    }

    for (size_t i = 0; i < args->idx; i++, argv_i++) {
      char* token = argv[argv_i];
      cli_arg* arg = args->args[i];

      cli_err err = arg->parser(arg, token);
      if (err != CLI_OK) {
        return err;
      }
    }
  }

  return CLI_OK;
}

/// these are some default parsers ... these should always be called from the

cli_err str_opt_parser(cli_opt* opt, const char* token) {
  str_box* box = (str_box*)(opt->value);

  size_t s = (sizeof(box->ptr) * box->sz) / sizeof(box->ptr);
  if (s < strlen(token) + 1) {
    return CLI_PARSE_FAILED_STR;
  }

  strncpy(box->ptr, token, box->sz);
  return CLI_OK;
}

cli_err str_arg_parser(cli_arg* arg, const char* token) {
  str_box* box = (str_box*)(arg->value);

  size_t s = (sizeof(box->ptr) * box->sz) / sizeof(box->ptr);
  if (s < strlen(token) + 1) {
    return CLI_PARSE_FAILED_STR;
  }

  strncpy(box->ptr, token, box->sz);
  return CLI_OK;
}

cli_err float_opt_parser(cli_opt* opt, const char* token) {
  float* val = (float*)(opt->value);
  char* endptr;
  *val = (float)strtof(token, &endptr);
  if (endptr == token) {
    return CLI_PARSE_FAILED_FLOAT;
  }
  return CLI_OK;
}

cli_err float_arg_parser(cli_arg* arg, const char* token) {
  float* val = (float*)(arg->value);
  char* endptr;
  *val = (float)strtof(token, &endptr);
  if (endptr == token) {
    return CLI_PARSE_FAILED_FLOAT;
  }
  return CLI_OK;
}

cli_err int_opt_parser(cli_opt* opt, const char* token) {
  int* val = (int*)(opt->value);
  char* endptr;
  *val = (int)strtol(token, &endptr, 10);
  if (endptr == token) {
    return CLI_PARSE_FAILED_INT;
  }
  return CLI_OK;
}

cli_err int_arg_parser(cli_arg* arg, const char* token) {
  int* val = (int*)(arg->value);
  char* endptr;
  *val = (int)strtol(token, &endptr, 10);
  if (endptr == token) {
    return CLI_PARSE_FAILED_INT;
  }
  return CLI_OK;
}

cli_err bool_opt_parser(cli_opt* opt, const char* arg) {
  bool* val = (bool*)opt->value;
  // most commonly handle a switch case like (--verbose) by passing null arg
  if (arg == NULL) {
    if (*val == true) {
      *val = false;
    } else {
      *val = true;
    }
    return CLI_OK;
  }
  return CLI_PARSE_FAILED_BOOL;
}

cli_err noop_parser(cli_opt* opt, const char* token) {
  CLI_UNUSED(opt);
  CLI_UNUSED(token);
  return CLI_OK;
}

// High level API

typedef struct cli_command {
  const char* desc;
  const char* usage;
  cli_opts* opts;
  cli_args* args;
  int argc;
  char** argv;
  str_boxes* sb;
} cli_command;

cli_command* cli_command_new(void) {
  cli_command* c = (cli_command*)malloc(sizeof(cli_command));
  CLI_CHECK_MEM_ALLOC(c);
  return c;
}

cli_err cli_init(cli_command* cli,
                 const char* desc,
                 const char* usage,
                 int argc,
                 char** argv) {
  cli->desc = desc;
  cli->usage = usage;
  cli->argc = argc;
  cli->argv = argv;

  // if we have opts allocate the requested amount
  // we should always allocate 2 for optional help message flag `-h, --help`
  cli_opts* opts = (cli_opts*)malloc(sizeof(cli_opts));
  CLI_CHECK_MEM_ALLOC(opts);
  cli_opts_init(opts, CLI_MAX_OPTS);
  cli->opts = opts;

  // help is really just used as token to break out of the parse.
  // since we always add them we can simply print info to stderr later if -h or
  // --help is raised.
  cli_opts_add(opts, "h", "", noop_parser, NULL, false, true);
  cli_opts_add(opts, "help", "", noop_parser, NULL, false, true);

  cli_args* args = (cli_args*)malloc(sizeof(cli_args));
  CLI_CHECK_MEM_ALLOC(args);
  cli_args_init(args, CLI_MAX_ARGS);
  cli->args = args;

  // if every opt + arg is a string we would at most have MAX args and opts.
  str_boxes* sb = (str_boxes*)malloc(sizeof(str_boxes));
  str_boxes_init(sb, CLI_MAX_ARGS + CLI_MAX_OPTS);
  cli->sb = sb;

  return CLI_OK;
}

void cli_cleanup(cli_command* cli) {
  if (cli->opts != NULL) {
    cli_opts_cleanup(cli->opts);
    free(cli->opts);
  }

  if (cli->args != NULL) {
    cli_args_cleanup(cli->args);
    free(cli->args);
  }

  if (cli->sb != NULL) {
    str_boxes_cleanup(cli->sb);
    free(cli->sb);
  }
}

void cli_command_destroy(cli_command* c) {
  cli_cleanup(c);
  free(c);
}

// high level API for adding options and arguments

cli_err cli_add_flag(cli_command* cli,
                     const char* name,
                     const char* usage,
                     bool* value) {
  return cli_opts_add(cli->opts, name, usage, bool_opt_parser, (void*)value,
                      false, true);
}

cli_err cli_add_int_argument(cli_command* cli, int* value) {
  return cli_args_add(cli->args, int_arg_parser, (void*)value);
}

cli_err cli_add_int_option(cli_command* cli,
                           const char* name,
                           const char* usage,
                           int* value,
                           bool required) {
  return cli_opts_add(cli->opts, name, usage, int_opt_parser, (void*)value,
                      required, false);
}

cli_err cli_add_float_argument(cli_command* cli, float* value) {
  return cli_args_add(cli->args, float_arg_parser, (void*)value);
}

cli_err cli_add_float_option(cli_command* cli,
                             const char* name,
                             const char* usage,
                             float* value,
                             bool required) {
  return cli_opts_add(cli->opts, name, usage, float_opt_parser, (void*)value,
                      required, false);
}

cli_err cli_add_str_argument(cli_command* cli, char* value, size_t buf_size) {
  cli_err err;
  str_box* box = NULL;

  if ((err = str_boxes_add(cli->sb, value, buf_size, &box)) != CLI_OK) {
    return err;
  }

  return cli_args_add(cli->args, str_arg_parser, (void*)box);
}

cli_err cli_add_str_option(cli_command* cli,
                           const char* name,
                           const char* usage,
                           char* value,
                           bool required,
                           size_t buf_size) {
  cli_err err;
  str_box* box = NULL;

  if ((err = str_boxes_add(cli->sb, value, buf_size, &box)) != CLI_OK) {
    return err;
  }

  return cli_opts_add(cli->opts, name, usage, str_opt_parser, (void*)box,
                      required, false);
}

void cli_print_help_and_exit(cli_command* cli, int status) {
  char buf[1024];

  char initial[] =
      "%s\n\nUsage:\n\t%s %s\nOptions:\n\t-h,--help\tPrint usage and exit.\n";

  snprintf(buf, 1024, initial, cli->desc, cli->argv[0], cli->usage);

  // check for options and append
  if (cli->opts != NULL) {
    for (size_t i = 0; i < cli->opts->idx; i++) {
      const char* name = cli->opts->opts[i]->name;
      // printf("NAME: %s\n", name);
      if ((strcmp(name, "help") == 0) || (strcmp(name, "h") == 0)) {
        continue;
      }
      char opt_buf[CLI_OPT_TOKEN_MAX_LEN + CLI_OPT_USAGE_MAX_LEN];
      cli_opt_print_message(cli->opts->opts[i], opt_buf);
      strcat(buf, opt_buf);
    }
  }

  fprintf(stderr, "%s\n", buf);
  exit(status);
}

cli_err cli_parse(cli_command* cli) {
  cli_err err = cli_parse_loop(cli->opts, cli->args, cli->argc, cli->argv);

  if (err == CLI_PRINT_HELP_AND_EXIT) {
    cli_print_help_and_exit(cli, 0);
  }

  return err;
}