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

using namespace std;

namespace lex{
  //ENUMS AND STUCTS
  enum ENUM_TYPE{
    //Error
    ERROR = 0,
    //Types
    UINT,
    INT,
    LONG,
    SHORT,
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
    AND,
    BIT_AND,
    BIT_OR,
    SHL,
    SHR,
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
    //Define
    DEFU, // UINT
    DEFI, // INT
    DEFL, // LONG
    DEFP, // PTR
    DEFB, // BOOL
    DEFC, // CHAR
    DEFS, // STRING
    DEFSHORT, //SHORT
    //Temp
    TEMP,
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
    "SHORT",
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
    "AND",
    "BIT_AND",
    "BIT_OR",
    "SHL",
    "SHR",
    "DEREF",
    "CALL",
    "AMPERSAND",
    //Keywords
    "PROC",
    "END",
    "BEGIN",
    "IN",
    "OUT",
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
    //Define
    "DEFU", // UINT
    "DEFI", // INT
    "DEFL", // LONG
    "DEFP", // PTR
    "DEFB", // BOOL
    "DEFC", // CHAR
    "DEFS", // STRING
    "DEFSHORT", //SHORT
    //Temp
    "TEMP",
  });
  vector<string> PTR_LIST;
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
    {"uint",    UINT},
    {"long",    LONG},
    {"int",     INT},
    {"bool",    BOOL},
    {"char",    CHAR},
    {"ptr",     PTR},
    {"string",  STRING},
    {"short",   SHORT}
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
  
  bool isOp(ENUM_TYPE type){
    return type>=ADD&&type<=AMPERSAND&&type!=CALL;
  }
  bool isType(ENUM_TYPE type){
    return type>=UINT&&type<=STRING;
  }
  bool isDef(ENUM_TYPE type){
    return type>=DEFU&&type<=DEFS;
  }
  bool isType(string type){
    return type=="uint"||type=="long"||type=="int"||type=="bool"||type=="char"||type=="ptr"||type=="string"||type=="short";
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
      }
      return type;
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
    
    void parse(){
      printf("Parsing...\n");
      int LIB_OFFSET = 0;
      vector<string> SYM_TABLE({"+","-","/","*","%","&",";;","(",")",";","[","]","@","++","--","<",">","<=",">=","=","!=","&&","<<",">>","-&","|"});
      vector<Token> VAR_TKS;
      vector<string> PROCS({"__asm"});
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
          else if(word=="short"){
            result.value = split_line[++i];
            result.type  = DEFSHORT;
            Tokens.push_back(result);
            result.type  = SHORT;
            VAR_TKS.push_back(result);
            VARS[result.value] = VAR_TKS.size()-1;
          }
          else if(split_line.size()>=4&&word=="("&&split_line[i+2]==")"&&isType(split_line[i+1])){
            result.value = literal_or_var(split_line[i+3])!=ERROR ? split_line[i+3] : CONSTS[split_line[i+3]];
            result.type = KEYWORD_MAP[split_line[i+1]];
            Tokens.push_back(result);
            i+=3;
          }
          else if(word==""||word==" "||word=="\n"||word=="\t"){
            
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
                  char result[ PATH_MAX ];
                  size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
                  int j = count;
                  string path(result);
                  while(j > 0) if(path[--j]=='/') break;
                  path.erase(j+1);
                  string fd = path+"include/"+split_line[++i]+".alio";
                  i++;
                  string line;
                  ifstream source_code;
                  source_code.open(fd.c_str());
                  j = LINE_ITER+1;
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
      if(options::DEBUGMODE){
        for(lex::Token &tk : Tokens){
          cout << lex::TYPE_NAMES[tk.type] << " ";
          if(tk.value!=""){
            cout << tk.value << " ";
          }
          if(tk.type==lex::SEMICOLON){
            cout << "\n";
          }
        }
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
          case IF:
            block_tracker++;
            BLOCK_STACK.push_back("if");
          break;
          case ELSE:
            if(BLOCK_STACK[BLOCK_STACK.size()-1]!="if"&&BLOCK_STACK[BLOCK_STACK.size()-1]!="elif"){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":An else statement following a non-if block, instead follows a '"<<BLOCK_STACK[BLOCK_STACK.size()-1]<<"' block.\033[0m\n";
              assert(false);
            }
            if(Tokens[i+1].type==IF){
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":Else if statements not supported yet.\033[0m\n";
              assert(false);
            }
            else{
              BLOCK_STACK[BLOCK_STACK.size()-1]="else";
            }
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
              cerr << "\033[1;31m"<<FILENAME<<":"<<tk.line<<":You can only use the keywords in and out before the begin part of procedure.\033[0m\n";
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