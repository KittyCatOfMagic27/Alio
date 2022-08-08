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
  
  enum OS{
    LINUX_DEBIAN,
    WIN32,
  };
  
  COMP_TARGETS target;
  OS OSTYPE = LINUX_DEBIAN;
  string FILE_BASENAME;
  string EXEC_FILE;
  string INTER_FILE;
  bool KEEP_ASM = false;
  bool DEBUGMODE = false;
  bool INTERMODE = false;
  bool LIBC = false;
  string ENTRYPOINT = "main";
}

#endif