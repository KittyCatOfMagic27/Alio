#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <fstream>
#include <string>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <stdbool.h>
#include <algorithm>
#include <unordered_map>

using namespace std;

namespace lex{
  //ENUMS AND STUCTS
  enum ENUM_TYPE{
    //Error
    ERROR,
    //Types
    UINT,
    INT,
    LONG,
    PTR,
    BOOL,
    CHAR,
    STRING,
    //Ops
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,
    INC,
    DEC,
    GCMP,
    LCMP,
    LECMP,
    GECMP,
    ECMP,
    NECMP,
    DEREF,
    CALL,
    AMPERSAND,
    //Keywords
    PROC,
    END,
    BEGIN,
    IN,
    OUT,
    PUSH,
    POP,
    SYSCALL,
    STATIC,
    WHILE,
    //Symbols
    OPARA,
    CPARA,
    OSQRB,
    CSQRB,
    SEMICOLON,
    //Define
    DEFU, // UINT
    DEFI, // INT
    DEFL, // LONG
    DEFP, // PTR
    DEFB, // BOOL
    DEFC, // CHAR
    DEFS, // STRING
  };
  struct Token{
    ENUM_TYPE type;
    string value;
    int line;
  };
  
  //CONSTS
  string FILENAME;
  static const string WHITESPACE = " \n\r\t\f\v";
  static vector<string> TYPE_NAMES({
    //Error
    "ERROR",
    //Types
    "UINT",
    "INT",
    "LONG",
    "PTR",
    "BOOL",
    "CHAR",
    "STRING",
    //Ops
    "ADD",
    "SUB",
    "MULT",
    "DIV",
    "MOD",
    "INC",
    "DEC",
    "GCMP",
    "LCMP",
    "LECMP",
    "GECMP",
    "ECMP",
    "NECMP",
    "DEREF",
    "AMPERSAND",
    //Keywords
    "PROC",
    "END",
    "BEGIN",
    "IN",
    "OUT",
    "CALL",
    "PUSH",
    "POP",
    "SYSCALL",
    "STATIC",
    "WHILE",
    //Symbols
    "OPARA",
    "CPARA",
    "OSQRB",
    "CSQRB",
    "SEMICOLON",
    //Define
    "DEFU", // UINT
    "DEFI", // INT
    "DEFL", // LONG
    "DEFP", // PTR
    "DEFB", // BOOL
    "DEFC", // CHAR
    "DEFS"  // STRING
  });
  vector<string> PTR_LIST;
  unordered_map<string, ENUM_TYPE> KEYWORD_MAP({
    {"end",     END},
    {"in",      IN},
    {"out",     OUT},
    {"begin",   BEGIN},
    {"static",  STATIC},
    {"push",    PUSH},
    {"pop",     POP},
    {"syscall", SYSCALL},
    {"while",   WHILE},
    {"(",       OPARA},
    {")",       CPARA},
    {"[",       OSQRB},
    {"]",       CSQRB},
    {"+",       ADD},
    {"-",       SUB},
    {"*",       MULT},
    {"/",       DIV},
    {"%",       MOD},
    {"&",       AMPERSAND},
    {"@",       DEREF},
    {"++",      INC},
    {"--",      DEC},
    {";",       SEMICOLON},
    {"<",       LCMP},
    {">",       GCMP},
    {"<=",      LECMP},
    {">=",      GECMP},
    {"=",       ECMP},
    {"!=",      NECMP}
  });
  
  //UTIL
  void print_token(Token tk){
    cout << "{\n  Value: " << tk.value << ";\n  Line:  " << tk.line << ";\n  Type:  " << TYPE_NAMES[tk.type] << "\n}\n";
  }
  bool isNumber(const string& str){
    for (char const &c : str) {
      if (isdigit(c) == 0) return false;
    }
    return true;
  }
  ENUM_TYPE literal_or_var(string &val){
    if(val=="true" || val=="false")return BOOL;
    if(isNumber(val))return UINT;
    if(val[0]=='"'&&val[val.size()-1]=='"')
      if(val.size()<=3) return CHAR;
      else return STRING;
    return ERROR;
  }
  string ltrim(const string &s){
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
  }
  string convertToString(char a){
    string s = "";
    return s + a;
  }
  vector<string> split(string &x, vector<string> &SYMBOLS, char delim = ' '){
    x = ltrim(x);
    x += delim;
    vector<string> splitted;
    string temp = "";
    bool in_string = false;
    for (int i = 0; i < x.length(); i++){
      if (x[i] == delim){
        splitted.push_back(temp);
        temp = "";
        i++;
      }
      char y;
      while(true){
        y = x[i];
        if(x[i]=='"'){
          if(temp != "") splitted.push_back(temp);
          temp = "";
          temp += x[i];
          i++;
          while(true){
            if(i>=x.length()){
              cerr << "CAN'T FIND END OF STRING.\n";
              assert(false);
            }
            temp+=x[i];
            if(x[i]=='"'){
              i++;
              break;
            }
            i++;
          }
          splitted.push_back(temp);
          temp = "";
        }
        else if(find(SYMBOLS.begin(), SYMBOLS.end(), convertToString(y)+x[i+1]) != SYMBOLS.end()){
          if(temp!="")splitted.push_back(temp);
          temp = "";
          temp += y;
          temp += x[i+1];
          splitted.push_back(temp);
          temp = "";
          i+=2;
        }
        else if(find(SYMBOLS.begin(), SYMBOLS.end(), convertToString(y)) != SYMBOLS.end()){
          if(temp!="")splitted.push_back(temp);
          temp = "";
          temp += x[i];
          splitted.push_back(temp);
          temp = "";
          i++;
        }
        else{
          break;
        }
      }
      y = x[i];
      if(y=='#'){
        if(temp != "") splitted.push_back(temp);
        return splitted;
      }
      if(y!=' '||in_string) temp += x[i];
    }
    return splitted;
  }
  
  //LEXER
  class LEXER{
    public:
    
    vector <Token> Tokens;
    vector <string> Lines;
    
    LEXER(const char* fd){
      FILENAME=fd;
      string line;
      ifstream source_code;
      source_code.open(fd);
      if(source_code.is_open()){
        while(getline(source_code,line)){
          Lines.push_back(line);
        }
        source_code.close();
      }
      else {
        fprintf(stderr, "\033[1;31mUnable to open file.\033[0m\n");
        assert(false);
      }
    }
    vector <Token> run(){
      parse();
      grammer_check();
      return Tokens;
    }
    
    private:
    
    void parse(){
      printf("Parsing...\n");
      int LIB_OFFSET = 0;
      vector<string> SYM_TABLE({"+","-","/","*","%","&",";;","(",")",";","[","]","@","++","--","<",">","<=",">=","=","!="});
      vector<Token> VAR_TKS;
      vector<string> PROCS;
      unordered_map<string, int> VARS;
      unordered_map<string, string> CONSTS;
      for(int LINE_ITER = 0; LINE_ITER < Lines.size(); LINE_ITER++){
        string line = Lines[LINE_ITER];
        if(line=="") continue;
        int LINE_NUMBER = LINE_ITER-LIB_OFFSET+1;
        vector<string> split_line = split(line, SYM_TABLE);
        bool semicolonTR = true;
        for(int i = 0; i < split_line.size(); i++){
          string word=split_line[i];
          Token result;
          result.line=LINE_NUMBER;
          if(word=="proc"){
            result.value = split_line[++i];
            result.type  = PROC;
            Tokens.push_back(result);
            PROCS.push_back(split_line[i]);
          }
          else if(word=="uint"){
            result.value = split_line[++i];
            result.type  = DEFU;
            Tokens.push_back(result);
            result.type  = UINT;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(word=="long"){
            if(options::target==options::X86_I386){
              cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":The type long is not supported in x86_32 mode.\033[0m\n";
              assert(false);
            }
            result.value = split_line[++i];
            result.type  = DEFL;
            Tokens.push_back(result);
            result.type  = LONG;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(word=="int"){
            result.value = split_line[++i];
            result.type  = DEFI;
            Tokens.push_back(result);
            result.type  = INT;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(word=="bool"){
            result.value = split_line[++i];
            result.type  = DEFB;
            Tokens.push_back(result);
            result.type  = BOOL;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(word=="char"){
            result.value = split_line[++i];
            result.type  = DEFC;
            Tokens.push_back(result);
            result.type  = CHAR;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(word=="ptr"){
            result.value = split_line[++i];
            result.type  = DEFP;
            Tokens.push_back(result);
            result.type  = PTR;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
            PTR_LIST.push_back(result.value);
          }
          else if(word=="string"){
            result.value = split_line[++i];
            result.type  = DEFS;
            Tokens.push_back(result);
            result.type  = STRING;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(word==""||word==" "){
            
          } //temp fix, idfk wth
          else if(KEYWORD_MAP.find(word)!=KEYWORD_MAP.end()){
            result.type  = KEYWORD_MAP[word];
            Tokens.push_back(result);
          }
          else if(VARS.find(word)!=VARS.end()){
            Tokens.push_back(VAR_TKS[VARS[word]]);
          }
          else if(literal_or_var(word)!=ERROR){
            result.value = word;
            result.type  = literal_or_var(word);
            Tokens.push_back(result);
          }
          else if(CONSTS.find(word)!=CONSTS.end()){
            result.value = CONSTS[word];
            result.type  = literal_or_var(CONSTS[word]);
            Tokens.push_back(result);
          }
          else if(find(PROCS.begin(), PROCS.end(), word)!=PROCS.end()){
            result.value = word;
            result.type  = CALL;
            Tokens.push_back(result);
          }
          else if(word==";;"){
            semicolonTR = false;
            if(split_line[++i]=="include"){
              if(split_line[++i]=="static"){
                if(split_line[++i]=="<"){
                  string fd = "./include/"+split_line[++i]+".adpl";
                  i++;
                  string line;
                  ifstream source_code;
                  source_code.open(fd.c_str());
                  int j = LINE_NUMBER+1;
                  if(source_code.is_open()){
                    while(getline(source_code,line)){
                      Lines.insert(Lines.begin()+j, line);
                      j++;
                    }
                    source_code.close();
                    LIB_OFFSET += j-2;
                  }
                  else {
                    fprintf(stderr, "\033[1;31mUnable to open file '%s'.\033[0m\n", fd);
                    assert(false);
                  }
                  break;
                }
                else if(literal_or_var(split_line[i])==STRING){
                  string fd = split_line[i];
                  i++;
                  string line;
                  ifstream source_code;
                  source_code.open(fd.c_str());
                  int j = LINE_NUMBER+1;
                  if(source_code.is_open()){
                    while(getline(source_code,line)){
                      Lines.insert(Lines.begin()+j, line);
                      j++;
                    }
                    source_code.close();
                    LIB_OFFSET += j-2;
                  }
                  else {
                    fprintf(stderr, "\033[1;31mUnable to open file '%s'.\033[0m\n", fd);
                    assert(false);
                  }
                  break;
                }
                else{
                  cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":Unknown include file input '"<<split_line[i]<<"'.\033[0m\n";
                  assert(false);
                }
              }
              else{
                cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":Unknown include type of '"<<split_line[i]<<"'.\033[0m\n";
                  assert(false);
              }
            }
            else if(split_line[i]=="define"){
              CONSTS[split_line[i+1]]=split_line[i+2];
              i+=2;
            }
          }
          else{
            cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":Unknown symbol of '"<<word<<"'.\033[0m\n";
            assert(false);
          }
        }
        Token semicolon;
        semicolon.type = SEMICOLON;
        if(split_line.size()>0&&semicolonTR&&Tokens[Tokens.size()-1].type!=SEMICOLON) Tokens.push_back(semicolon);
      }
      printf("Done Parsing!\n");
    }
    void grammer_check(){
      printf("Grammer checking...\n");
      int block_tracker = 0;
      vector<string> BLOCK_STACK;
      Token prev;
      bool IN_BEGIN = false;
      // for(Token &tk : Tokens) print_token(tk);
      for(int i = 0; i < Tokens.size(); i++){
        Token tk = Tokens[i];
        switch(tk.type){
          case PROC:
            block_tracker++;
            BLOCK_STACK.push_back(tk.value);
            IN_BEGIN = true;
          break;
          case WHILE:
            block_tracker++;
            BLOCK_STACK.push_back("while");
          break;
          case END:
            block_tracker--;
            if(block_tracker<0){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Unnessasary 'end'.\033[0m\n";
              assert(false);
            }
            if(IN_BEGIN){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Missing begin portion in procedure '"<<BLOCK_STACK[BLOCK_STACK.size()-1]<<"'.\033[0m\n";
              assert(false);
            }
            Tokens[i].value=BLOCK_STACK[block_tracker];
            BLOCK_STACK.pop_back();
          break;
          case BEGIN:
            if(!IN_BEGIN){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Unnessasary 'begin'.\033[0m\n";
              assert(false);
            }
            IN_BEGIN = false;
          break;
          case IN:
          case OUT:
            if(!IN_BEGIN){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":You can only use the keywords in or out before the begin part of procedure.\033[0m\n";
              assert(false);
            }
          break;
          case ADD:
          case SUB:
          case MULT:
          case DIV:
          case MOD:
          case GCMP:
          case LCMP:
          case ECMP:
          case GECMP:
          case LECMP:
          case NECMP:
            prev = Tokens[i-1];
            if(prev.type<UINT||prev.type>CHAR){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":First agrument of '"<< TYPE_NAMES[tk.type] <<"' is not a Valid Type.\033[0m\n";
              assert(false);
            }
            Tokens[i-1] = tk;
            Tokens[i] = prev;
            i++;
            if(Tokens[i].type<UINT||Tokens[i].type>CHAR){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Second agrument of '"<< TYPE_NAMES[tk.type] <<"' is not a Valid Type.\033[0m\n";
              assert(false);
            }
          break;
          case AMPERSAND:
            i++;
            if(Tokens[i].type<UINT||Tokens[i].type>STRING){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Trying to create a pointer to an invalid type of '"<< TYPE_NAMES[tk.type] <<"'.\033[0m\n";
              assert(false);
            }
            if(literal_or_var(Tokens[i].value)){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Trying to create a pointer to a literal value; can only create pointers to variables.\033[0m\n";
              assert(false);
            }
          break;
          case DEREF:
            if(Tokens[++i].type!=PTR){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Trying to derefrence a non-pointer, instead of type '"<< TYPE_NAMES[tk.type] <<"'.\033[0m\n";
              assert(false);
            }
          break;
        }
      }
      if(block_tracker>0){
        cerr << "\033[1;31mUnhandled block.\033[0m\n";
        assert(false);
      }
      printf("Done grammer checking!\n");
    }
  };
}

#endif
