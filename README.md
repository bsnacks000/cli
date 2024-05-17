# cli

A simple, declarative single command parser in the style of argparse, goflags and others. 

## purpose 

I threw this together because I am often working on some small C only project where I want a basic cli but I don't want to spend an hour reading documentation for how to deal with `opts.h` or writing one by hand. 

To keep things as simple as possible the current API: 
* Allocs and frees internals with stdlib. Will auto fail on out of memory errors.
* Uses an opaque type to hide internals and make the public API smaller. 
* Only has a few basic types (boolean flags, ints, floats, strs).
* Does not allow hooks for post parse validation. We're just converting from strings and doing basic checks.
* Imposes hard limits on tokens at compile time.
* Does not allow for subcommands.

When properly configured: 
* Can parse a number of options in several input formats. 
* Checks for required options. 
* Requires the exact number of positional arguments registered.
* Returns errors if a parse was unsuccessful. 
* Checks for basic misconfigurational errors (mismatched n opts/args, repeats, unseen required args).
* Free help / usage options and message. 

## usage 

Just drop cli.c and cli.h in your project somewhere and you're good to go. Or use cmake and integrate as a library. 

The basic parsing rules: 
* An option is registered with a name (should contain no dashes). For example "x" is the name for `-x` or `--x` 
* An option_argument can be placed like `-x 42` or `--x=42`. Note that `-x=42` is not valid and would be seen as `x=42` which is not what you want. 
* Positional args start as soon as the parser encounters: 
    * The first value that does not start with a `-` or `--` (similar to goflags)
    * Or the lone `--` token (similar to clap)
* Positional args are parsed in the order they are registered in the app. 
* Calling `-h` or `--help` will automatically print the usage message and exit(0). This is added automatically to every cli.

Examples and tests show how to configure an app. It should be pretty similar to other cli APIs out there. 

## build 

For dev build tests and examples. 
```
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCLI_BUILD_TESTS -DCLI_BUILD_EXAMPLES
```

This installs gtest under a `libs` dir using cpm-cmake which you can run with `ctest`. 


## TODO 
maybe... though probably nah...

* better handling of error context for parse failure...
* list types?
* more stdlib types?
* subcommands?


