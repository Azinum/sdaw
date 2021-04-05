// arg_parser.c

// TODO(lucas): Improve alignments of flags which stand alone (without long flag)
void ArgsPrintHelp(FILE* File, parse_arg* Args, i32 NumArgs, i32 argc, char** argv) {
  assert(argc != 0);
  i32 LongestArgLength = 0;
  // Find the longest argument (longflag)
  for (i32 ArgIndex = 0; ArgIndex < NumArgs; ++ArgIndex) {
    parse_arg* Arg = &Args[ArgIndex];
    if (!Arg->LongFlag) {
      continue;
    }
    i32 ArgLength = strlen(Arg->LongFlag);
    if (LongestArgLength < ArgLength) {
      LongestArgLength = ArgLength;
    }
  }

  fprintf(File, "USAGE:\n  %s [options]\n\n", argv[0]);
  fprintf(File, "FLAGS:\n");
  for (i32 ArgIndex = 0; ArgIndex < NumArgs; ++ArgIndex) {
    parse_arg* Arg = &Args[ArgIndex];
    fprintf(File, "  ");
    if (Arg->Flag) {
      fprintf(File, "-%c", Arg->Flag);
    }
    if (Arg->Flag && Arg->LongFlag) {
      fprintf(File, ", --%-*s", LongestArgLength, Arg->LongFlag);
    }
    else if (!Arg->Flag && Arg->LongFlag) {
      fprintf(File, "--%-*s", LongestArgLength, Arg->LongFlag);
    }
    if (Arg->NumArgs > 0) {
      fprintf(File, " ");
      fprintf(File, "<%s>", ArgTypeDesc[Arg->Type]);
    }
    fprintf(File, " %s\n", Arg->Desc);
  }
  fprintf(File, "  -h, --%-*s show help menu\n\n", LongestArgLength, "help");
}

i32 ParseArgs(parse_arg* Args, i32 NumArgs, i32 argc, char** argv) {
  for (i32 Index = 1; Index < argc; ++Index) {
    char* Arg = argv[Index];
    u8 LongFlag = 0;
    u8 FoundFlag = 0;

    if (*Arg == '-') {
      Arg++;
      if (*Arg == '-') {
        LongFlag = 1;
        Arg++;
      }
      if (*Arg == 'h' && !LongFlag) {
        ArgsPrintHelp(stdout, Args, NumArgs, argc, argv);
        return NoError;
      }
      if (LongFlag) {
        if (!strcmp(Arg, "help")) {
          ArgsPrintHelp(stdout, Args, NumArgs, argc, argv);
          return NoError;
        }
      }
      parse_arg* ParseArg = NULL;
      // Linear search over the array of user defined arguments
      for (i32 ArgIndex = 0; ArgIndex < NumArgs; ++ArgIndex) {
        ParseArg = &Args[ArgIndex];
        if (LongFlag) {
          if (!strcmp(ParseArg->LongFlag, Arg)) {
            FoundFlag = 1;
            break;
          }
        }
        else {
          if (ParseArg->Flag == *Arg) {
            // We found the flag
            FoundFlag = 1;
            break;
          }
        }
      }

      if (FoundFlag) {
        if (ParseArg->NumArgs > 0) {
          if (Index + 1 < argc) {
            char* Buffer = argv[++Index];
            Assert(Buffer != NULL);
            Assert(ParseArg);
            switch (ParseArg->Type) {
              case ArgInt: {
                sscanf(Buffer, "%i", (i32*)ParseArg->Data);
                break;
              }
              case ArgFloat: {
                sscanf(Buffer, "%f", (float*)ParseArg->Data);
                break;
              }
              case ArgString: {
                char** String = ParseArg->Data;
                *String = Buffer;
                break;
              }
              case ArgBuffer: {
                sscanf(Buffer, "%s", (char*)ParseArg->Data);
                break;
              }
              default:
                Assert(0);
                break;
            }
          }
          else {
            fprintf(stderr, "Missing parameter after flag '%c'\n", *Arg);
            return Error;
          }
        }
        else {
          switch (ParseArg->Type) {
            case ArgInt: {
              *(i32*)ParseArg->Data = 1;
              break;
            }
            case ArgFloat: {
              *(float*)ParseArg->Data = 1.0f;
              break;
            }
            default:
              break;
          }
        }
      }
      else {
        fprintf(stderr, "Flag '%s' not defined\n", Arg);
        return Error;
      }
    }
    else {
      parse_arg* ParseArg = NULL;
      for (i32 ArgIndex = 0; ArgIndex < NumArgs; ++ArgIndex) {
        ParseArg = &Args[ArgIndex];
        if (ParseArg->Flag == 0 && ParseArg->LongFlag == NULL) {
          switch (ParseArg->Type) {
            case ArgString: {
              char** String = ParseArg->Data;
              *String = argv[Index];
              break;
            }
            case ArgBuffer: {
              strcpy((char*)ParseArg->Data, argv[Index]);
            }
            default:
              break;
          }
          break;
        }
      }
    }
  }
  return NoError;
}

