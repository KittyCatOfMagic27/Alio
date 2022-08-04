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
#include <unistd.h>
#include <limits.h>
#include <utility>

using namespace std;

namespace lex{
  //ENUMS AND STUCTS
  enum ENUM_TYPE{
    //Error
    ERROR = 0,
    VALUE,
    //Types
    UINT,
    INT,
    LONG,
    SHORT,
    PTR,
    BOOL,
    CHAR,
    STRUCT,
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
    AND,
    BIT_AND,
    BIT_OR,
    SHL,
    SHR,
    DEREF,
    CALL,
    OFNAME, //::
    AMPERSAND,
    ARGLEA,
    SUBSET,
    //Keywords
    PROC,
    END,
    BEGIN,
    IN,
    OUT,
    MSTRUCT,
    PUSH,
    POP,
    SYSCALL,
    STATIC,
    EXTERN,
    GLOBAL,
    WHILE,
    IF,
    ELSE,
    //Symbols
    OPARA,
    CPARA,
    OSQRB,
    CSQRB,
    SEMICOLON,
    DOT,
    //Define
    DEFU, // UINT
    DEFI, // INT
    DEFL, // LONG
    DEFP, // PTR
    DEFB, // BOOL
    DEFC, // CHAR
    DEFS, // STRING
    DEFSHORT, //SHORT
    DEFSTRUCT, //STRUCT
    //Temp
    TEMP,
  };
  struct Token{
    ENUM_TYPE type;
    string value;
    unsigned int line;
    unsigned int fd;
  };
  struct Structure{
    string label;
    int size=0;
    vector<Token> fields;
  };
  
  //CONSTS
  string FILENAME;
  static const string WHITESPACE = " \n\r\t\f\v";
  static vector<string> TYPE_NAMES({
    //Error
    "ERROR",
    "VALUE",
    //Types
    "UINT",
    "INT",
    "LONG",
    "SHORT",
    "PTR",
    "BOOL",
    "CHAR",
    "STRUCT",
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
    "AND",
    "BIT_AND",
    "BIT_OR",
    "SHL",
    "SHR",
    "DEREF",
    "CALL",
    "OFNAME",
    "AMPERSAND",
    "ARGLEA",
    "SUBSET",
    //Keywords
    "PROC",
    "END",
    "BEGIN",
    "IN",
    "OUT",
    "MSTRUCT",
    "PUSH",
    "POP",
    "SYSCALL",
    "STATIC",
    "EXTERN",
    "GLOBAL",
    "WHILE",
    "IF",
    "ELSE",
    //Symbols
    "OPARA",
    "CPARA",
    "OSQRB",
    "CSQRB",
    "SEMICOLON",
    "DOT",
    //Define
    "DEFU", // UINT
    "DEFI", // INT
    "DEFL", // LONG
    "DEFP", // PTR
    "DEFB", // BOOL
    "DEFC", // CHAR
    "DEFS", // STRING
    "DEFSHORT", //SHORT
    "DEFSTRUCT",//STRUCT
    //Temp
    "TEMP",
  });
  vector<string> PTR_LIST;
  unordered_map<string, Structure> STRUCTS;
  errh::FileIdentity Files;
  
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
  int lsize_of(ENUM_TYPE type){
    if(type == UINT||type == INT) return 4;
    if(type == LONG) return 8;
    if(type == SHORT) return 2;
    if(type == PTR){
      if(options::target==options::X86_I386) return 4;
      return 8;
    }
    if(type == BOOL||type == CHAR) return 1;
    return -1;
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
  
  bool isOp(ENUM_TYPE type){
    return type>=ADD&&type<=AMPERSAND&&type!=CALL;
  }
  bool isType(ENUM_TYPE type){
    return type>=UINT&&type<=STRING;
  }
  bool isDef(ENUM_TYPE type){
    return type>=DEFU&&type<=DEFSTRUCT;
  }
  bool isType(string type){
    return type=="uint"||type=="long"||type=="int"||type=="bool"||type=="char"||type=="ptr"||type=="string"||type=="short"||type=="struct";
  }
  
  //LEXER
  class LEXER{
    public:
    
    vector <Token> Tokens;
    vector <string> Lines;
    unordered_map<string, string> VAR_STRUCT_TYPES;
    
    LEXER(const char* fd){
      FILENAME=fd;
      {
        errh::FileRange mainfile;
        mainfile.begin=0;
        mainfile.end=INT_MAX;
        mainfile.filename=FILENAME;
        Files.push_back(mainfile);
      }
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
      {
        vector<Token> tks = tokenize();
        parse(tks);
      }
      pregrammer_check();
      grammer_check();
      postprocess();
      return Tokens;
    }
    
    private:
    
    ENUM_TYPE defToType(ENUM_TYPE def){
      ENUM_TYPE type;
      switch(def){
        case DEFU: type = UINT;   break;
        case DEFI: type = INT;    break;
        case DEFL: type = LONG;   break;
        case DEFP: type = PTR;    break;
        case DEFB: type = BOOL;   break;
        case DEFC: type = CHAR;   break;
        case DEFS: type = STRING; break;
        case DEFSHORT: type = SHORT; break;
        case DEFSTRUCT: type = STRUCT; break;
      }
      return type;
    }
    string defToWord(ENUM_TYPE def){
      string type;
      switch(def){
        case DEFU: type = "uint";   break;
        case DEFI: type = "int";    break;
        case DEFL: type = "long";   break;
        case DEFP: type = "ptr";    break;
        case DEFB: type = "bool";   break;
        case DEFC: type = "char";   break;
        case DEFS: type = "string"; break;
        case DEFSHORT: type = "short"; break;
        case DEFSTRUCT: type = "struct"; break;
      }
      return type;
    }
    string typeToWord(ENUM_TYPE type){
      string word;
      switch(type){
        case UINT: word = "uint";     break;
        case INT: word = "int";       break;
        case LONG: word = "long";     break;
        case PTR: word = "ptr";       break;
        case BOOL: word = "bool";     break;
        case CHAR: word = "char";     break;
        case STRING: word = "string"; break;
        case SHORT: word = "short";   break;
        case STRUCT: word = "struct"; break;
      }
      return word;
    }
    int opInAmount(ENUM_TYPE op){
      int x;
      switch(op){
        case ADD:
        case SUB:
        case MULT:
        case DIV:
        case MOD:
        case GCMP:
        case LCMP:
        case LECMP:
        case GECMP:
        case ECMP:
        case NECMP:
        case AND:
        case BIT_AND:
        case BIT_OR:
        case SHL:
        case SHR:
        x = 2;
        break;
        case INC:
        case DEC:
        case DEREF:
        case AMPERSAND:
        x = 1;
        break;
      }
      return x;
    }
    
    vector<Token> tokenize(){
      unordered_map<string, ENUM_TYPE> KEYWORD_MAP({
        {"end",     END},
        {"in",      IN},
        {"out",     OUT},
        {"begin",   BEGIN},
        {"static",  STATIC},
        {"extern",  EXTERN},
        {"global",  GLOBAL},
        {"push",    PUSH},
        {"pop",     POP},
        {"syscall", SYSCALL},
        {"while",   WHILE},
        {"if",      IF},
        {"else",    ELSE},
        {"subset",  SUBSET},
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
        {">>",      SHR},
        {"<<",      SHL},
        {";",       SEMICOLON},
        {"<",       LCMP},
        {">",       GCMP},
        {"<=",      LECMP},
        {">=",      GECMP},
        {"=",       ECMP},
        {"!=",      NECMP},
        {"&&",      AND},
        {"-&",      BIT_AND},
        {"|",       BIT_OR},
        {"uint",    DEFU},
        {"long",    DEFL},
        {"int",     DEFI},
        {"bool",    DEFB},
        {"char",    DEFC},
        {"ptr",     DEFP},
        {"string",  DEFS},
        {"short",   DEFSHORT},
        {"struct",  MSTRUCT},
        {".",       DOT},
        {"proc",    PROC},
        {"::",      OFNAME}
      });
      printf("Tokenizing...\n");
      vector<string> SYM_TABLE({"+","-","/","*","%","&",";;","(",")",";","[","]","@","++","--","<",">","<=",">=","=","!=","&&","<<",">>","-&","|",".","::"});
      vector<string> LIBDEFS;
      unordered_map<string, string> CONSTS;
      vector<Token> tks;
      for(int LINE_ITER = 0; LINE_ITER < Lines.size(); LINE_ITER++){
        string line = Lines[LINE_ITER];
        if(line==""||line==" ") continue;
        int LINE_NUMBER = LINE_ITER;
        vector<string> split_line = split(line, SYM_TABLE);
        bool semicolonTR = true;
        for(int i = 0; i < split_line.size(); i++){
          string word=split_line[i];
          Token result;
          result.line=LINE_NUMBER;
          if(word==""||word==" "||word=="\n"||word=="\t"){
            
          } //temp fix, idfk wth
          else if(literal_or_var(word)!=ERROR){
            result.value = word;
            result.type  = literal_or_var(word);
            tks.push_back(result);
          }
          else if(KEYWORD_MAP.find(word)!=KEYWORD_MAP.end()){
            result.type  = KEYWORD_MAP[word];
            tks.push_back(result);
          }
          else if(CONSTS.find(word)!=CONSTS.end()){
            result.value = CONSTS[word];
            result.type  = literal_or_var(CONSTS[word]);
            tks.push_back(result);
          }
          else if(word=="self"){
            result.type  = PTR;
            result.value = "self";
            tks.push_back(result);
          }
          else if(word==";;"){
            semicolonTR = false;
            if(split_line[++i]=="include"){
              if(split_line[++i]=="static"){
                string filename;
                string fd;
                int j;
                if(split_line[++i]=="<"){
                  char result[ PATH_MAX ];
                  size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
                  j = count;
                  string path(result);
                  while(j > 0) if(path[--j]=='/') break;
                  path.erase(j+1);
                  fd = path+"include/"+split_line[++i]+".alio";
                  filename = split_line[i]+".alio";
                }
                else if(literal_or_var(split_line[i])==STRING){
                  fd = split_line[i];
                  fd.erase(0, 1);
                  fd.pop_back();
                  filename = fd;
                }
                else{
                  cerr << "\033[1;31m"<<FILENAME<<":"<<LINE_NUMBER<<":Unknown include file input '"<<split_line[i]<<"'.\033[0m\n";
                  assert(false);
                }
                i++;
                string line;
                ifstream source_code;
                source_code.open(fd.c_str());
                j = LINE_ITER+1;
                int begin=j-1;
                int end;
                if(source_code.is_open()){
                  while(getline(source_code,line)){
                    Lines.insert(Lines.begin()+j, line);
                    j++;
                  }
                  source_code.close();
                  end = j-3;
                }
                else {
                  cerr << "\033[1;31mUnable to open file '"<<filename<<"'.\033[0m\n";
                  assert(false);
                }
                Files.add_file(filename, begin, end);
                LINE_ITER+=3;
                break;
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
            else if(split_line[i]=="def"){
              LIBDEFS.push_back(split_line[++i]);
            }
            else if(split_line[i]=="ifndef"){
              if(find(LIBDEFS.begin(), LIBDEFS.end(), split_line[++i])!=LIBDEFS.end()){
                while(++LINE_ITER){
                  split_line = split(Lines[LINE_ITER], SYM_TABLE);
                  if(split_line[0]==";;"&&split_line[1]=="fi") break;
                }
              }
            }
            else if(split_line[i]=="setentry"){
              options::ENTRYPOINT = split_line[++i];
            }
          }
          else{
            result.value = word;
            result.type  = VALUE;
            tks.push_back(result);
          }
        }
        Token semicolon;
        semicolon.type = SEMICOLON;
        semicolon.line = LINE_NUMBER;
        if(split_line.size()>0&&semicolonTR&&tks[tks.size()-1].type!=SEMICOLON) tks.push_back(semicolon);
      }
      printf("Done Tokenizing!\n");
      return tks;
    }
    void parse(vector<Token> tks){
      printf("Parsing...\n");
      vector<Token> VAR_TKS;
      vector<string> PROCS({"__asm"});
      unordered_map<string, int> VARS;
      for(int i = 0; i < tks.size(); i++){
        Token tk = tks[i];
        Token result;
        if(tk.type==PROC){
          result.type  = PROC;
          if(tks[i+2].type==OFNAME){
            result.value = tks[i+3].value;
            if(STRUCTS.find(tks[i+1].value)!=STRUCTS.end()){
              STRUCTS[tks[i+1].value].fields.push_back(result);
              result.value = tks[i+1].value+"@"+tks[i+3].value;
            }
            else if(isDef(tks[i+1].type)){
              result.value = defToWord(tks[i+1].type)+"@"+tks[i+3].value;
            }
            i+=4;
            Tokens.push_back(result);
            Token t;
            t.type = SEMICOLON;
            Tokens.push_back(t);
            t.type = IN;
            Tokens.push_back(t);
            t.type = DEFP;
            t.value = "self";
            Tokens.push_back(t);
            t.type = SEMICOLON;
            t.value = "";
            Tokens.push_back(t);
          }
          else{
            result.value = tks[++i].value;
            Tokens.push_back(result);
          }
          PROCS.push_back(tks[i].value);
        }
        else if(tk.type==MSTRUCT){
          result.value = tks[++i].value;
          result.type  = MSTRUCT;
          Tokens.push_back(result);
          Structure s;
          s.label      = tks[i].value;
          STRUCTS[tks[i].value] = s;
        }
        else if(isDef(tk.type)){
          result.value = tks[++i].value;
          result.type  = tk.type;
          Tokens.push_back(result);
          result.type  = defToType(tk.type);
          VAR_TKS.push_back(result);
          VARS[result.value] = VAR_TKS.size()-1;
        }
        else if(tk.type==DOT){
          result.value = tks[++i].value;
          result.type  = DOT;
          Tokens.push_back(result);
        }
        else if(tk.type==OFNAME){
          result.value = Tokens[Tokens.size()-1].value;
          Tokens.pop_back();
          result.type  = OFNAME;
          Tokens.push_back(result);
        }
        else if(isType(tk.type)||tk.type==SEMICOLON||isOp(tk.type)||(tk.type>=END&&tk.type<=SEMICOLON)){
          Tokens.push_back(tk);
        }
        else if(tk.type==OPARA&&tks[i+2].type==CPARA&&isType(defToType(tks[i+1].type))){
          result.value = tks[i+3].value;
          result.type  = defToType(tks[i+1].type);
          Tokens.push_back(result);
          i+=3;
        }
        else if(VARS.find(tk.value)!=VARS.end()){
          Tokens.push_back(VAR_TKS[VARS[tk.value]]);
        }
        else if(find(PROCS.begin(), PROCS.end(), tk.value)!=PROCS.end()){
          result.value = tk.value;
          result.type  = CALL;
          Tokens.push_back(result);
        }
        else if(STRUCTS.find(tk.value)!=STRUCTS.end()){
          result.value = tk.value;
          result.type  = DEFSTRUCT;
          Tokens.push_back(result);
          result.value = tks[++i].value;
          result.type  = STRUCT;
          Tokens.push_back(result);
          VAR_TKS.push_back(result);
          VARS[result.value] = VAR_TKS.size()-1;
          VAR_STRUCT_TYPES[result.value] = tk.value;
        }
      }
      if(options::DEBUGMODE){
        for(lex::Token &tk : Tokens){
          cout << lex::TYPE_NAMES[tk.type] << " ";
          if(tk.value!=""){
            cout << tk.value << " ";
          }
          if(tk.type==lex::SEMICOLON){
            // cout << "\n" << formatErrorInfo(Files.get_ErrorInfo(tk.line)) << "   ";
            cout << "\n";
          }
        }
        cout << "\n";
      }
      printf("Done Parsing!\n");
    }
    void pregrammer_check(){
      for(int i = 0; i < Tokens.size(); i++){
        Token tk = Tokens[i];
        switch(tk.type){
          case SEMICOLON:
            if(Tokens[i+1].type==SEMICOLON){
              Tokens.erase(Tokens.begin()+i);
            }
          break;
          case MSTRUCT:{
            int begin = i;
            string label = Tokens[i].value;
            Structure S  = STRUCTS[label];
            if(Tokens[i+1].type==SEMICOLON)i++;
            if(Tokens[++i].type!=BEGIN){
              startErrorThrow(tk);
              cerr << "Expected 'begin' in declartion of the struct '"<<label<<"'.";
              endErrorThrow();
              assert(false);
            }
            while(Tokens[++i].type!=END){
              if(isDef(Tokens[i].type)){
                S.fields.push_back(Tokens[i]);
                if(Tokens[i].type!=DEFS){
                  uint csize = lsize_of(defToType(Tokens[i].type));
                  if(csize==-1){
                    startErrorThrow(Tokens[i]);
                    cerr <<"Uknown size of field name '"<<Tokens[i].value<<"' in declartion of the struct '"<<label<<"'.";
                    endErrorThrow();
                    assert(false);
                  }
                  S.size+=csize;
                }
                else{
                  if(Tokens[i+1].type!=OSQRB||Tokens[i+3].type!=CSQRB){
                    startErrorThrow(Tokens[i]);
                    cerr << "Uknown size of field name '"<<Tokens[i].value<<"' in declartion of the struct '"<<label<<"'.";
                    endErrorThrow();
                    assert(false);
                  }
                  uint csize = stoi(Tokens[i+2].value);
                  S.size+=csize;
                  S.fields[S.fields.size()-1].value+=" "+Tokens[i+2].value;
                  i+=3;
                }
              }
            }
            STRUCTS[label] = S;
            i+=2;
            Tokens.erase(Tokens.begin()+begin, Tokens.begin()+i);
            i = begin-1;
          }
          break;
          case DOT:{
            if(VAR_STRUCT_TYPES.find(Tokens[i-1].value)==VAR_STRUCT_TYPES.end()){
              if(!isType(Tokens[i-1].type)){
                startErrorThrow(tk);
                cerr << "Trying to access invalid method of '"<<tk.value<<"' on an instance of '"<<TYPE_NAMES[Tokens[i-1].type]<<"' called '"<<Tokens[i-1].value<<"'.";
                endErrorThrow();
                assert(false);
              }
              string s = Tokens[i-1].value;
              Tokens[i-1].value=typeToWord(Tokens[i-1].type)+"@"+tk.value;
              Tokens[i-1].type=CALL;
              Tokens.erase(Tokens.begin()+i);
              i--;
              Token ptr;
              ptr.type = ARGLEA;
              ptr.value = s;
              Tokens.insert(Tokens.begin()+i+2, ptr);
              break;
            }
            Structure S = STRUCTS[VAR_STRUCT_TYPES[Tokens[i-1].value]];
            Token field;
            field.type = ERROR;
            for(Token t : S.fields){
              if(t.value[t.value.size()-1]>='0'&&t.value[t.value.size()-1]<='9'){
                const char* begin = t.value.c_str();
                const char* str = begin;
                while(*str!=' '&&*str!=0)str++;
                if(*str==0){ if(t.value==tk.value) field = t; }
                else if(t.value.substr(0, (str-begin))==tk.value) field = t;
              }
              else if(t.value==tk.value) field = t;
            }
            if(field.type==ERROR){
              startErrorThrow(tk);
              cerr << "Trying to access invalid field of '"<<tk.value<<"' on an instance of '"<<S.label<<"' called '"<<Tokens[i-1].value<<"'.";
              endErrorThrow();
              assert(false);
            }
            if(field.type==PROC){
              string s = Tokens[i-1].value;
              Tokens[i-1].type=CALL;
              Tokens[i-1].value=S.label+"@"+field.value;
              Tokens.erase(Tokens.begin()+i);
              i--;
              Token ptr;
              ptr.type = ARGLEA;
              ptr.value = s;
              Tokens.insert(Tokens.begin()+i+2, ptr);
            }
            else{
              Tokens[i-1].type=defToType(field.type);
              if(Tokens[i-1].type==STRING){
                string field_name;
                const char* begin = field.value.c_str();
                const char* str = begin;
                while(*str!=' ')str++;
                Tokens[i-1].value+="."+field.value.substr(0, (str-begin));
              }
              else{
                Tokens[i-1].value+="."+field.value;
              }
              Tokens.erase(Tokens.begin()+i);
              i--;
            }
          }
          break;
          case DEFSTRUCT:{
            int begin = i;
            Structure S = STRUCTS[tk.value];
            string parent = Tokens[++i].value;
            int offset = 0;
            vector<Token> Subsets;
            for(Token &t : S.fields){
              Token tk1;
              string field_name;
              int csize;
              if(t.type==PROC) continue;
              ENUM_TYPE type=defToType(t.type);
              if(type==STRING){
                const char* begin = t.value.c_str();
                const char* str = begin;
                while(*(str)!=' ')str++;
                field_name = parent+"."+t.value.substr(0, (str-begin));
                str++;
                csize = stoi(t.value.substr((str-begin), t.value.size()));
              }
              else{
                field_name = parent+"."+t.value;
                csize = lsize_of(type);
              }
              tk1.type = SUBSET;
              Subsets.push_back(tk1);
              tk1.type = UINT;
              tk1.value = to_string(offset);
              offset+=csize;
              Subsets.push_back(tk1);
              tk1.value = to_string(csize);
              tk1.type = UINT;
              Subsets.push_back(tk1);
              tk1.value = field_name;
              tk1.type = STRUCT;
              Subsets.push_back(tk1);
              tk1.value = parent;
              tk1.type = STRUCT;
              Subsets.push_back(tk1);
              tk1.value = "";
              tk1.type = SEMICOLON;
              Subsets.push_back(tk1);
            }
            i+=2;
            Tokens.insert(Tokens.begin()+i, Subsets.begin(), Subsets.end());
            i+=Subsets.size();
          }
          break;
          default: break;
        }
      }
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
          case IF:
            block_tracker++;
            BLOCK_STACK.push_back("if");
          break;
          case ELSE:
            if(BLOCK_STACK[BLOCK_STACK.size()-1]!="if"&&BLOCK_STACK[BLOCK_STACK.size()-1]!="else if"){
              startErrorThrow(tk);
              cerr << "An else statement following a non-if block, instead follows a '"<<BLOCK_STACK[BLOCK_STACK.size()-1]<<"' block.";
              endErrorThrow();
              assert(false);
            }
            if(Tokens[i+1].type==IF){
              BLOCK_STACK[BLOCK_STACK.size()-1]="else if";
              i++;
            }
            else{
              BLOCK_STACK[BLOCK_STACK.size()-1]="else";
            }
          break;
          case END:
            block_tracker--;
            if(block_tracker<0){
              startErrorThrow(tk);
              cerr << "Unnessasary 'end'.";
              endErrorThrow();
              assert(false);
            }
            if(IN_BEGIN){
              startErrorThrow(tk);
              cerr << "Missing begin portion in procedure '"<<BLOCK_STACK[BLOCK_STACK.size()-1]<<"'.";
              endErrorThrow();
              assert(false);
            }
            Tokens[i].value=BLOCK_STACK[block_tracker];
            BLOCK_STACK.pop_back();
          break;
          case BEGIN:
            if(!IN_BEGIN){
              startErrorThrow(tk);
              cerr << "Unnessasary 'begin'.";
              endErrorThrow();
              assert(false);
            }
            IN_BEGIN = false;
          break;
          case IN:
          case OUT:
            if(!IN_BEGIN){
              startErrorThrow(tk);
              cerr << "You can only use the keywords in and out before the begin part of procedure.";
              endErrorThrow();
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
          case AND:
          case SHR:
          case SHL:
          case BIT_AND:
          case BIT_OR:
            prev = Tokens[i-1];
            if(prev.type<UINT||prev.type>CHAR){
              startErrorThrow(tk);
              cerr << "First argument of '"<< TYPE_NAMES[tk.type] <<"' is an invalid type of '"<< TYPE_NAMES[prev.type] <<"'.";
              endErrorThrow();
              assert(false);
            }
            Tokens[i-1] = tk;
            Tokens[i] = prev;
            i++;
            if(Tokens[i].type<UINT||Tokens[i].type>CHAR){
              startErrorThrow(tk);
              cerr << "Second argument of '"<< TYPE_NAMES[tk.type] <<"' is an invalid type of '"<< TYPE_NAMES[Tokens[i].type] <<"'.";
              endErrorThrow();
              assert(false);
            }
          break;
          case AMPERSAND:
            i++;
            if(Tokens[i].type<UINT||Tokens[i].type>STRING){
              startErrorThrow(tk);
              cerr << "Trying to create a pointer to an invalid type of '"<< TYPE_NAMES[Tokens[i].type] <<"'.";
              endErrorThrow();
              assert(false);
            }
            if(literal_or_var(Tokens[i].value)){
              startErrorThrow(tk);
              cerr << "Trying to create a pointer to a literal value; can only create pointers to variables.";
              endErrorThrow();
              assert(false);
            }
          break;
          case DEREF:
            if(Tokens[++i].type!=PTR){
              startErrorThrow(tk);
              cerr << "Trying to derefrence a non-pointer, instead of type '"<< TYPE_NAMES[Tokens[i].type] <<"'.";
              endErrorThrow();
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
    void processOp(int &index, kt::Tree<Token> &tree){
      tree.connect(tree[index], tree[index+1]);
      if(opInAmount(tree[index]->type)==2){
        tree.connect(tree[index], tree[index+2]);
        index+=2;
      }
      else{
        index++;
      }
      if(isOp(tree[index]->type)) processOp(index, tree);
    }
    vector<Token> reconstructOp(Token* var, Token* tk, int line_number, kt::Tree<Token> &tree){
      vector<Token*> children = tree[tk];
      vector<Token> line;
      Token semicolon;
      semicolon.type = SEMICOLON;
      semicolon.line = line_number;
      if(opInAmount(tk->type)==2){
        if(isOp(children[1]->type)){
          vector<Token> prev_line = reconstructOp(var, children[1], line_number, tree);
          line.insert(line.end(), prev_line.begin(), prev_line.end());
          if(isDef(var->type))var->type = defToType(var->type);
          line.push_back(*var);
          line.push_back(*tk);
          line.push_back(*children[0]);
          line.push_back(*var);
          line.push_back(semicolon);
        }
        else{
          line.push_back(*var);
          line.push_back(*tk);
          line.push_back(*children[0]);
          line.push_back(*children[1]);
          line.push_back(semicolon);
        }
      }
      else{
        if(isOp(children[0]->type)){
          vector<Token> prev_line = reconstructOp(var, children[0], line_number, tree);
          line.insert(line.end(), prev_line.begin(), prev_line.end());
          if(isDef(var->type))var->type = defToType(var->type);
          line.push_back(*var);
          line.push_back(*tk);
          line.push_back(*var);
          line.push_back(semicolon);
        }
        else{
          line.push_back(*var);
          line.push_back(*tk);
          line.push_back(*children[0]);
          line.push_back(semicolon);
        }
      }
      return line;
    }
    void printTree(kt::Tree<Token> &tree, Token* tk, int depth = 0){
      int _offset=depth*2;
      string offset = "";
      for(int i=0; i<_offset; i++) offset+=" ";
      cout << offset << TYPE_NAMES[tk->type] << " ";
      if(tk->value!=""){
        cout << tk->value << " ";
      }
      vector<Token*> children = tree[tk];
      if(children.size()<1) { cout << "\n"; return; }
      cout << " Children {\n";
      for(Token* c: children){
        printTree(tree, c, depth+1);
      }
      cout << offset << "}\n";
    }
    void postprocess(){
      vector<Token> _Tokens;
      typedef vector<Token*> Expr;
      vector<Expr> Exprs;
      for(int i = 0; i < Tokens.size(); i++){
        Expr expr;
        i--;
        while(Tokens[++i].type!=SEMICOLON) expr.push_back(&Tokens[i]);
        expr.push_back(&Tokens[i]);
        Exprs.push_back(expr);
      }
      //NOTE ADD PRORITIES AND MAKE IT TO WHERE THE EXPERESSIONS ARE EVAULATED LEFT TO RIGHT
      //AND NOT RIGHT TO LEFT
      
      // for(Expr &e : Exprs){
      //   cout << "Expression: ";
      //   for(Token* &tk : e){
      //     cout << TYPE_NAMES[tk->type] << " ";
      //     if(tk->value!=""){
      //       cout << tk->value << " ";
      //     }
      //   }
      //   cout << "\n";
      // }
      // cout << "\n";
      for(Expr &e : Exprs){
        kt::Tree<Token> tree;
        for(Token* &tk : e) tree.push_back(*tk);
        if((isType(e[0]->type)||isDef(e[0]->type))&&isOp(e[1]->type)){
          tree.connect(tree[0], tree[1]);
          int index = 1;
          processOp(index, tree);
          // cout << "Tree"<<tree[0]->line<<":\n";
          // printTree(tree, tree[0]);
          vector<Token> line = reconstructOp(tree[0], tree[1], tree[0]->line, tree);
          _Tokens.insert(_Tokens.end(), line.begin(), line.end());
        }
        else{
          for(Token* &tk : e) _Tokens.push_back(*tk);
        }
      }
      
      Tokens = _Tokens;
      // for(lex::Token &tk : Tokens){
      //   cout << lex::TYPE_NAMES[tk.type] << " ";
      //   if(tk.value!=""){
      //     cout << tk.value << " ";
      //   }
      //   if(tk.type==lex::SEMICOLON){
      //     cout << "\n";
      //   }
      // }
      // exit(1);
    }
  };
}

#endif