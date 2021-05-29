// arg_parser.h

#ifndef _ARG_PARSER_H
#define _ARG_PARSER_H

typedef enum arg_type {
  ArgInt = 0,
  ArgFloat,
  ArgString,
  ArgBuffer,

  MAX_ARG_TYPE,
} arg_type;

static const char* ArgTypeDesc[MAX_ARG_TYPE] = {
  "integer",
  "float",
  "string",
  "buffer",
};

typedef struct parse_arg {
  char Flag;  // Single char to identify the argument flag
  const char* LongFlag; // Long string to identify the argument flag
  const char* Desc; // Description of this flag
  arg_type Type;  // Which type the data argument is to be
  i32 NumArgs;  // Can be either one or zero for any one flag
  void* Data; // Reference to the data which is going to be overwritten by the value of the argument(s)
} parse_arg;

void ArgsPrintHelp(FILE* File, parse_arg* Args, i32 NumArgs, i32 argc, char** argv);

i32 ParseArgs(parse_arg* Args, i32 NumArgs, i32 argc, char** argv);

#endif
