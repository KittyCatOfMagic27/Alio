#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <string>

using namespace std;

namespace options{

  enum COMP_TARGETS{
    X86_64,
    X86_I386
  };
  
  COMP_TARGETS target;
  string FILE_BASENAME;
  string EXEC_FILE;
  string INTER_FILE;
  bool KEEP_ASM = false;
  bool DEBUGMODE = false;
  bool INTERMODE = false;
  string ENTRYPOINT = "main";
}

#endif