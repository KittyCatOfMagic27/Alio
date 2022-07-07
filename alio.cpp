#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <string>
#include "./modules/options.h"
#include "./modules/lexer.h"
#include "./modules/intermediate.h"
#include "./modules/compiler.h"
#include "./modules/flags.h"


//Functions for command line flags
namespace flags{
  void ALIO_FILE(string FILENAME){
    FILENAME.erase(FILENAME.size()-1);
    if(FILENAME.find(".alio") != string::npos){
      FILENAME.erase(FILENAME.size()-5);
      options::FILE_BASENAME = FILENAME;
    }else{
      options::FILE_BASENAME = FILENAME;
    }
  }
  
  void TARGETFLAG(string TARGET){
    TARGET.erase(TARGET.size()-1);
    if(TARGET=="elf64")
      options::target = options::X86_64;
    else if(TARGET=="elf32")
      options::target = options::X86_I386;
    else{
      cerr << "\033[1;31mUnsupported target given '"<<TARGET<<"'.\033[0m\n";
      assert(false);
    }
  }
  
  // void EXEC_FILE_SET(string EXEC_FILENAME){
  //   EXEC_FILE = EXEC_FILENAME;
  // }
  
  void ASMFLAG(string NA){
    options::KEEP_ASM = true;
  }
  
  void DEBUGFLAG(string NA){
    options::DEBUGMODE = true;
  }
}

int main(int argc, char** argv){
  
  //Command line flags
  string help_desc = "This is the Alio help desc!\nYou can bring up this page by using the --help flag.";
  kf::FlagProxy prox(help_desc);
  
  string file_flag = "-f";
  string file_flag_desc = "Provides .alio file to compiler (Required).";
  prox.AddFlag(file_flag, flags::ALIO_FILE, file_flag_desc, 1);
  
  // string exec_flag = "-o";
  // string exec_flag_desc = "Name the exec file.";
  // prox.AddFlag(exec_flag, flags::EXEC_FILE_SET, exec_flag_desc, 1);
  
  string target_flag = "-t";
  string target_flag_desc = "Provides the target to the compiler. (elf64 or elf32)";
  prox.AddFlag(target_flag, flags::TARGETFLAG, target_flag_desc, 1);
  
  string debug_flag = "-d";
  string debug_flag_desc = "Prints debug information while compiling.";
  prox.AddFlag(debug_flag, flags::DEBUGFLAG, debug_flag_desc, 0);
  
  string asm_flag = "-asm";
  string asm_flag_desc = "Keeps assembly code after compilation.";
  prox.AddFlag(asm_flag, flags::ASMFLAG, asm_flag_desc, 0);
  
  prox.AddHelp("For additional info check README.md /home/$USR/ALIO/.\n(Powered by KFlags)");
  
  prox.Parse(argc, argv);
  
  //Hello Alio
  printf("Hello to Another\n");
  
  //Files and SS
  string asm_file  = options::FILE_BASENAME+".asm";
  string alio_file = options::FILE_BASENAME+".alio";
  stringstream ss;
  
  //Compile Alio
  lex::LEXER lexer(alio_file.c_str());
  vector<lex::Token> Tokens = lexer.run();
  intr::INTERMEDIATE intr(Tokens);
  vector<string> INTER = intr.run();
  comp::COMPILER compiler(asm_file, INTER);
  compiler.run();
  
  string elf_target;
  string linker_target;
  switch(options::target){
    case options::X86_I386:
    elf_target    = "elf32";
    linker_target = "elf_i386";
    break;
    case options::X86_64:
    default:
    elf_target    = "elf64";
    linker_target = "elf_x86_64";
    break;
  }
  //Compile nasm
  ss << "nasm -f "<<elf_target<<" -o "<< options::FILE_BASENAME << ".o " << asm_file;
  string cmd_line = ss.str();
  const char* char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  cerr << char_line << "\n";
  
  //Link Program
  ss << "ld -m "<< linker_target <<" -e main -o " << options::FILE_BASENAME << " " << options::FILE_BASENAME << ".o ";
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  cerr << char_line << "\n";
  
  //Run
  cout << "PROGRAM OUTPUT:\n";
  ss << "./" << options::FILE_BASENAME;
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  
  //Deletes .o file
  ss << "shred -u " << options::FILE_BASENAME << ".o";
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  
  //Deletes .asm file
  if(!options::KEEP_ASM){
    ss << "shred -u " << asm_file;
    cmd_line = ss.str();
    char_line = cmd_line.c_str();
    ss.str("");
    system(char_line);
  }
  

  return 0;
}