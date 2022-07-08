#ifndef COMP_H
#define COMP_H

#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
#include <utility>
#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace std;

namespace comp{
  stringstream ss;
  string baseptr = "rbp";
  vector<string> syscall_args({
    "rax",
    "rdi",
    "rsi",
    "rdx",
    "r10",
    "r8",
    "r9"
  });
  vector<string> syscall_sigs({
    "a",
    "di",
    "si",
    "d",
    "r10",
    "r8",
    "r9"
  });
  vector<string> WORD_MAP({
    "%%NA%%",
    "byte",
    "word",
    "%%NA%%",
    "dword",
    "%%NA%%",
    "%%NA%%",
    "%%NA%%",
    "qword",
  });
  
  enum VAR_TYPES{
    VTSTACK,
    VTDATA,
    VTBSS,
  };
  struct Variable{
    string label;
    unsigned int size;
    unsigned int byte_addr;
    VAR_TYPES type = VTSTACK;
  };
  struct Procedure{
    vector<pair<string, int>> args;
    unordered_map<string, Variable> VARS;
    string label;
    string out_sym;
    unsigned int out_size;
    unsigned int bytes;
    unsigned int call_amount=0;
    unsigned int offset=0;
  };
  
  string asm_get_value(string &value, Procedure &proc){
    if(lex::literal_or_var(value) == lex::ERROR){
      Variable var = proc.VARS[value];
      int ptr      = var.byte_addr;
      int size     = var.size;
      switch(size){
        case 1:
        ss << "byte ["<<baseptr<<"-" << ptr << "]";
        break;
        case 2:
        ss << "word ["<<baseptr<<"-" << ptr << "]";
        break;
        case 4:
        ss << "dword ["<<baseptr<<"-" << ptr << "]";
        break;
        case 8:
        ss << "qword ["<<baseptr<<"-" << ptr << "]";
        break;
        default:
        cerr << "\033[1;31mUknown size of '"<<size<<"'.\033[0m\n";
        assert(false);
        break;
      }
      string output = ss.str();
      ss.str("");
      return output;
    }
    else{
      if(value == "true"){
        return "1";
      }
      else if(value == "false"){
        return "0";
      }
      return value;
    }
  }
  
  string get_reg(int size, string reg_id){
    string result;
    switch(size){
      case 8:  result = "r"+reg_id+"x"; break;
      case 4:  result = "e"+reg_id+"x"; break;
      case 2:  result = reg_id+"x";     break;
      case 1:  result = reg_id+"l";     break;
    }
    return result;
  }
  
  string get_reg_adv(int size, string reg_id){
    string result;
    if(reg_id=="a"||reg_id=="b"||reg_id=="c"||reg_id=="d"){
      return get_reg(size, reg_id);
    }
    else if(reg_id=="di"||reg_id=="si"||reg_id=="bp"){
      switch(size){
        case 8:  result = "r"+reg_id; break;
        case 4:  result = "e"+reg_id; break;
        case 2:  result = reg_id;     break;
        case 1:  result = reg_id+"l"; break;
      }
    }
    else if(reg_id[0]=='r'){
      switch(size){
        case 8:  result = reg_id;     break;
        case 4:  result = reg_id+"d"; break;
        case 2:  result = reg_id+"w"; break;
        case 1:  result = reg_id+"b"; break;
      }
    }
    else{
      cerr << "\033[1;31mUnregocnized register signiture of '"<<reg_id<<"'.\033[0m\n";
      assert(false);
    }
    return result;
  }
  
  string asm_var(Variable &var){
    switch(var.type){
      case VTSTACK:
      {
        int ptr      = var.byte_addr;
        int size     = var.size;
        switch(size){
          case 1:
          ss << "byte ["<<baseptr<<"-" << ptr << "]";
          break;
          case 2:
          ss << "word ["<<baseptr<<"-" << ptr << "]";
          break;
          case 4:
          ss << "dword ["<<baseptr<<"-" << ptr << "]";
          break;
          case 8:
          ss << "qword ["<<baseptr<<"-" << ptr << "]";
          break;
          default:
          cerr << "\033[1;31mUknown size of '"<<size<<"'.\033[0m\n";
          assert(false);
          break;
        }
        string output = ss.str();
        ss.str("");
        return output;
      }
      break;
      case VTBSS:
      case VTDATA:
      {
        int ptr      = var.byte_addr;
        int size     = var.size;
        switch(size){
          case 1:
          ss << "byte ["<<var.label<<"]";
          break;
          case 2:
          ss << "word ["<<var.label<<"]";
          break;
          case 4:
          ss << "dword ["<<var.label<<"]";
          break;
          case 8:
          ss << "qword ["<<var.label<<"]";
          break;
          default:
          cerr << "\033[1;31mUknown size of '"<<size<<"'.\033[0m\n";
          assert(false);
          break;
        }
        string output = ss.str();
        ss.str("");
        return output;
      }
      break;
      default:
      cerr << "\033[1;31mUknown var_type of value '"<<var.type<<"'.\033[0m\n";
      assert(false);
      break;
    }
  }
  
  class COMPILER{
    public:
    
    vector<string> INTER;
    ofstream out_stream;
    unordered_map<string, Procedure> PROCS;
    
    COMPILER(string &wf, vector<string> &_INTER){
      out_stream.open(wf.c_str());
      if(!out_stream.is_open()){
        fprintf(stderr, "\033[1;31mUnable to open file.\033[0m\n");
        assert(false);
      }
      INTER = _INTER;
    }
    
    void run(){
      preparse();
      switch(options::target){
        case options::X86_I386:
        baseptr      = "ebp";
        syscall_args = {
          "eax",
          "ebx",
          "ecx",
          "edx",
          "esi",
          "edi",
          "ebp"
        };
        syscall_sigs = {
          "a",
          "b",
          "c",
          "d",
          "si",
          "di",
          "bp"
        };
        init_nasm();
        //to_nasmX86_I386();
        break;
        case options::X86_64:
        default:
        init_nasm();
        to_nasmX86_64();
        break;
      }
    }
    
    private:
    
    void preparse(){
      string CP;
      for(int i=0; i<INTER.size(); i++){
        string cur = INTER[i];
        if(cur == "proc"){
          Procedure new_proc;
          new_proc.label = INTER[++i];
          CP             = INTER[i];
          new_proc.bytes = 0;
          PROCS[CP]      = new_proc;
          i++;
        }
        else if(cur == "call"){
          PROCS[CP].call_amount++;
        }
        else if(cur == "def"&&INTER[i-1]!="static"){
          string symbol = INTER[i+2];
          // if(find(lex::PTR_LIST.begin(), lex::PTR_LIST.end(), symbol.erase(0,1))!=lex::PTR_LIST.end()){
            Variable new_var;
            string isize        = INTER[++i];
            int size            = stoi(isize.erase(0,1))/8;
            string sym          = INTER[++i];
            PROCS[CP].bytes    += size;
            PROCS[CP].offset   += size;
            new_var.byte_addr   = PROCS[CP].bytes;
            new_var.size        = size;
            new_var.label       = sym;
            PROCS[CP].VARS[sym] = new_var;
            i++;
          // }
        }
      }
    }
    
    void init_nasm(){
      out_stream <<
      "SECTION .text		; code section\n"
      "global main		  ; make label available to linker\n";
    }
    
    void op_init(int &size1, int &size2, string &reg1, string &reg2, int &i, Procedure &proc){
      if(size2<size1){
        reg1  = get_reg(size1, "a");
        reg2  = get_reg(size1, "d");
        out_stream <<
        "  mov "<< reg1 <<", " << asm_get_value(INTER[++i], proc) << "\n"
        "  movsx "<< reg2 <<", " << asm_get_value(INTER[++i], proc) << "\n";
      }
      else if(size2>size1){
        size1 = size2;
        reg1  = get_reg(size1, "a");
        reg2  = get_reg(size1, "d");
        out_stream <<
        "  movsx "<< reg1 <<", " << asm_get_value(INTER[++i], proc) << "\n"
        "  mov "<< reg2 <<", " << asm_get_value(INTER[++i], proc) << "\n";
      }
      else{
        reg1  = get_reg(size1, "a");
        reg2  = get_reg(size2, "d");
        out_stream <<
        "  mov "<< reg1 <<", " << asm_get_value(INTER[++i], proc) << "\n"
        "  mov "<< reg2 <<", " << asm_get_value(INTER[++i], proc) << "\n";
      }
    }
    
    void to_nasmX86_64(){
      printf("Intermediate to NASM...\n");
      if(options::DEBUGMODE){
        for(string &s : INTER){
          cout << s << " ";
          if(s == ";") cout << "\n";
        }
      }
      string VAR_ASSIGN;
      string CP;
      string DATA_SECTION = "SECTION .data:\n";
      string BSS_SECTION  = "SECTION .bss:\n";
      for(int i=0; i<INTER.size(); i++){
        string cur = INTER[i];
        if(cur == "proc"){
          out_stream << INTER[++i] << ":\n";
          CP = INTER[i];
          i++;
        }
        else if(cur == "end"){
          if(INTER[++i]=="main"){
            out_stream <<
            ".exit:\n"
            "  add rsp, " << PROCS[CP].offset << "\n"
            "  pop rbp\n"
            "  mov rax, 60\n"
            "  mov rdi, 0\n"
            "  syscall\n";
          }
          else{
            out_stream <<
            ".exit:\n";
            if(PROCS[CP].out_sym!=""){
              out_stream <<
              "  mov " << get_reg(PROCS[CP].out_size, "a") << ", " << asm_var(PROCS[CP].VARS[PROCS[CP].out_sym]) <<"\n";
            }
            out_stream <<
            "  add rsp, " << PROCS[CP].offset << "\n"
            "  pop rbp\n"
            "  ret\n";
          }
          CP = "///NA///";
          i++;
        }
        else if(cur == "def"){
          Variable new_var;
          int size   = stoi(INTER[++i].erase(0,1))/8;
          string sym = INTER[++i];
          if(PROCS[CP].VARS.find(sym)==PROCS[CP].VARS.end()){
            PROCS[CP].bytes    += size;
            new_var.byte_addr   = PROCS[CP].bytes;
            new_var.size        = size;
            new_var.label       = sym;
            PROCS[CP].VARS[sym] = new_var;
          }
          if(INTER[++i]=="=") VAR_ASSIGN = sym;
          else if(INTER[i]=="["){
            if(!lex::isNumber(INTER[++i])){
              cerr << "\033[1;31mLooking for uint literal in array definition of '"<<sym.erase(0,1)<<"', instead found '"<<INTER[i]<<"'\033[0m\n";
              assert(false);
            }
            PROCS[CP].VARS[sym].size = size*stoi(INTER[i]);
            if(INTER[++i]!="]"){
              cerr << "\033[1;31mDid not find closing square bracket in array definition of '"<<sym.erase(0,1)<<"'.\033[0m\n";
              assert(false);
            }
            i++;
          }
        }
        else if(cur == "in"){
          i++;
          Variable new_var;
          int size            = stoi(INTER[++i].erase(0,1))/8;
          string sym          = INTER[++i];
          if(PROCS[CP].VARS.find(sym)==PROCS[CP].VARS.end()){
            PROCS[CP].bytes    += size;
            new_var.byte_addr   = PROCS[CP].bytes;
            new_var.size        = size;
            new_var.label       = sym;
            PROCS[CP].VARS[sym] = new_var;
          }
          PROCS[CP].args.push_back({sym, size});
          i++;
        }
        else if(cur == "out"){
          i++;
          Variable new_var;
          int size            = stoi(INTER[++i].erase(0,1))/8;
          string sym          = INTER[++i];
          PROCS[CP].bytes    += size;
          PROCS[CP].out_size  = size;
          PROCS[CP].out_sym   = sym;
          new_var.byte_addr   = PROCS[CP].bytes;
          new_var.size        = size;
          new_var.label       = sym;
          PROCS[CP].VARS[sym] = new_var;
          i++;
        }
        else if(cur == "begin"){
          int local_storage = PROCS[CP].offset;
          local_storage    += PROCS[CP].call_amount>0 ? 16: 0;
          PROCS[CP].offset  = local_storage;
          out_stream <<
          "  push rbp\n"
          "  mov rbp, rsp\n"
          "  sub rsp, "<< local_storage <<"\n";
          for(int j = 0; j < PROCS[CP].args.size(); j++){
            pair<string, int> arg = PROCS[CP].args[j];
            out_stream <<
            "  mov " << asm_get_value(arg.first, PROCS[CP]) << ", " << get_reg_adv(arg.second, syscall_sigs[j])<< "\n";
          }
          i++;
        }
        else if(cur == "call"){
          string proc_name = INTER[++i];
          if(INTER[++i]!="("){
            cerr << "No open paren when calling procedure '"<<proc_name<<"'.\n";
            assert(false);
          }
          int j = 0;
          while(INTER[++i]!=")"){
            lex::ENUM_TYPE type = lex::literal_or_var(INTER[i]);
            int size            = lex::ERROR == type ?
                                  PROCS[CP].VARS[INTER[i]].size :
                                  stoi(intr::size_of(type).erase(0,1))/8;
            if(size<8&&type==lex::ERROR) out_stream << "  movsx ";
            else out_stream << "  mov ";
            out_stream << syscall_args[j] << ", " << asm_get_value(INTER[i], PROCS[CP]) << "\n";
            j++;
          }
          out_stream <<
          "  call " << proc_name << "\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            string outreg = get_reg(var_assign.size, "a");
            out_stream << "  mov " << asm_var(var_assign) << ", "<<outreg<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "static"){
          i++;
          Variable new_var;
          int size   = stoi(INTER[++i].erase(0,1))/8;
          string sym = INTER[++i];
          if(PROCS[CP].VARS.find(sym)==PROCS[CP].VARS.end()){
            new_var.byte_addr   = 0;
            new_var.size        = size;
            new_var.label       = sym;
            new_var.type        = VTBSS;
            PROCS[CP].VARS[sym] = new_var;
          }
          if(INTER[++i]=="="){
            PROCS[CP].VARS[sym].type = VTDATA;
            Variable var        = PROCS[CP].VARS[sym];
            string value        = INTER[++i];
            lex::ENUM_TYPE type = lex::literal_or_var(value);
            switch(type){
              case lex::STRING: DATA_SECTION  += "  "+var.label+": db "+value+", 0\n";                        break;
              case lex::INT:
              case lex::UINT: DATA_SECTION    += "  "+var.label+": dd "+value+"\n";                           break;
              case lex::CHAR: DATA_SECTION    += "  "+var.label+": db "+value+"\n";                           break;
              case lex::BOOL: DATA_SECTION    += "  "+var.label+": db "+asm_get_value(value, PROCS[CP])+"\n"; break;
              case lex::LONG: DATA_SECTION    += "  "+var.label+": dq "+value+"\n";                           break;
              case lex::ERROR:
              cerr << "\033[1;31mTrying to assign nonliteral to '"<<sym<<"' in static def.\033[0m\n";
              assert(false);
              break;
              default:
              cerr << "\033[1;31mUnhandled type in staic def of '"<<sym<<"'.\033[0m\n";
              assert(false);
              break;
            }
          }
          i++;
        }
        else if(cur == "label"){
          out_stream <<
          INTER[++i] << ":\n";
          i++;
        }
        else if(cur == "jmpc"){
          string label = INTER[++i];
          string value = INTER[++i];
          if(INTER[++i]=="!"){
            i++;
            out_stream <<
            "  cmp " << asm_get_value(value, PROCS[CP]) << ", 0\n"
            "  je "  << label << "\n";
          }
          else{
            out_stream <<
            "  cmp " << asm_get_value(value, PROCS[CP]) << ", 0\n"
            "  jne " << label << "\n";
          }
        }
        else if(cur == "jmp"){
          string label = INTER[++i];
          out_stream <<
          "  jmp " << label << "\n";
          i++;
        }
        else if(cur == "+"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  add "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "-"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  sub "<<reg1<<", "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "*"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  mul "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "/"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  div "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "%"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  div "<<reg2<<"\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            if(var_assign.size==size1)
              out_stream << "  mov " << asm_var(var_assign) << ", "<<reg1<<"\n";
            else if(var_assign.size<size1){
              out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "d")<<"\n";
            }
            else if(var_assign.size>size1){
              string outreg = get_reg(var_assign.size, "a");
              out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
              "  mov " << asm_var(var_assign) << ", "<< outreg <<"\n";
            }
            VAR_ASSIGN="";
          }
        }
        else if(cur == "++"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          out_stream << "  inc " << asm_var(var) << "\n";
          i++;
        }
        else if(cur == "--"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          out_stream << "  dec " << asm_var(var) << "\n";
          i++;
        }
        else if(cur == ">"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setg al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "<"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setl al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "=="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  sete al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "!="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setne al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == ">="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setge al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "<="){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  cmp "<<reg1<<", "<<reg2<<"\n"
          "  setle al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "&&"){
          int size1 = stoi(INTER[++i].erase(0,1))/8;
          int size2 = stoi(INTER[++i].erase(0,1))/8;
          string reg1;
          string reg2;
          op_init(size1, size2, reg1, reg2, i, PROCS[CP]);
          out_stream <<
          "  and "<<reg1<<", "<<reg2<<"\n"
          "  shr al, 1\n"
          "  setc al\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream << "  mov " << asm_var(var_assign) << ", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(cur == "&"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          switch(var.type){
            case VTSTACK:
            {
              out_stream <<
              "  lea rax, [rbp-" << var.byte_addr << "]\n";
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << asm_var(var_assign) << ", "<<outreg<<"\n";
                VAR_ASSIGN="";
              }
            }
            break;
            case VTDATA:
            case VTBSS:
            {
              out_stream <<
              "  mov rax, "<< var.label <<"\n";
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << asm_var(var_assign) << ", "<<outreg<<"\n";
                VAR_ASSIGN="";
              }
            }
            break;
          }
          i++;
        }
        else if(cur == "@"){
          Variable var = PROCS[CP].VARS[INTER[++i]];
          switch(var.type){
            case VTSTACK:
            {
              out_stream <<
              "  mov rax, " << asm_var(var) << "\n";
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << outreg << ", "<<WORD_MAP[var_assign.size]<<" [rax]\n"
                "  mov " << asm_var(var_assign) << ", " << outreg<< "\n";
                VAR_ASSIGN="";
              }
            }
            break;
            case VTDATA:
            case VTBSS:
            {
              if(VAR_ASSIGN!=""){
                Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
                string outreg = get_reg(var_assign.size, "a");
                out_stream << "  mov " << outreg << ", "<<WORD_MAP[var_assign.size]<<" ["<<var.label<<"]\n"
                "  mov " << asm_var(var_assign) << ", " << outreg<< "\n";
                VAR_ASSIGN="";
              }
            }
            break;
          }
          i++;
        }
        else if(cur == "pop"){
          int size = stoi(INTER[++i].erase(0,1))/8;
          out_stream <<
          "  mov " << get_reg(size, "a") << ", " << asm_get_value(INTER[++i], PROCS[CP]) << "\n"
          "  pop rax\n";
          i++;
        }
        else if(cur == "push"){
          int size = stoi(INTER[++i].erase(0,1))/8;
          out_stream <<
          "  mov " << get_reg(size, "a") << ", " << asm_get_value(INTER[++i], PROCS[CP]) << "\n"
          "  push rax\n";
          i++;
        }
        else if(cur == "syscall"){
          if(INTER[++i]!="("){
            cerr << "\033[1;31mNo open paren with syscall.\033[0m\n";
            assert(false);
          }
          int j = 0;
          while(INTER[++i]!=")"){
            lex::ENUM_TYPE type = lex::literal_or_var(INTER[i]);
            int size            = lex::ERROR == type ?
                                  PROCS[CP].VARS[INTER[i]].size :
                                  stoi(intr::size_of(type).erase(0,1))/8;
            if(size<8&&type==lex::ERROR) out_stream << "  movsx ";
            else out_stream << "  mov ";
            out_stream << syscall_args[j] << ", " << asm_get_value(INTER[i], PROCS[CP]) << "\n";
            j++;
          }
          out_stream << "  syscall\n";
          i++;
          if(VAR_ASSIGN!=""){
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            out_stream <<
            "  mov "<<asm_var(var_assign)<<", "<<get_reg(var_assign.size, "a")<<"\n";
            VAR_ASSIGN="";
          }
        }
        else if(PROCS[CP].VARS.find(cur)!=PROCS[CP].VARS.end()){
          if(VAR_ASSIGN!=""){
            Variable cvar       = PROCS[CP].VARS[cur];
            Variable var_assign = PROCS[CP].VARS[VAR_ASSIGN];
            int size = cvar.size;
            if(size==1||size==2||size==4||size==8){
              out_stream<<
              "  mov "<<get_reg(cvar.size, "a")<<", "<<asm_var(cvar)<<"\n"
              "  mov "<<asm_var(var_assign)<<", "<<get_reg(cvar.size, "a")<<"\n";
              VAR_ASSIGN="";
              i++;
            }
            else{
              int ptrc = cvar.byte_addr;
              int ptrv = var_assign.byte_addr;
              while(size>0){
                if(size>=8){
                  size-=8;
                  out_stream<<
                  "  mov rax, qword [rbp-"<<ptrc-size<<"]\n"
                  "  mov qword [rbp-"<<ptrv-size<<"]\n";
                }
                else if(size>=4){
                  size-=4;
                  out_stream<<
                  "  mov eax, dword [rbp-"<<ptrc-size<<"]\n"
                  "  mov dword [rbp-"<<ptrv-size<<"]\n";
                }
                else if(size>=2){
                  size-=2;
                  out_stream<<
                  "  mov ax, word [rbp-"<<ptrc-size<<"]\n"
                  "  mov word [rbp-"<<ptrv-size<<"]\n";
                }
                else if(size==1){
                  size-=1;
                  out_stream<<
                  "  mov al, byte [rbp-"<<ptrc-size<<"]\n"
                  "  mov byte [rbp-"<<ptrv-size<<"]\n";
                }
              }
              VAR_ASSIGN="";
              i++;
            }
          }
          else{
            if(INTER[++i]=="=") VAR_ASSIGN = cur;
            else{
              cerr << "\033[1;31mHanging variable of '"<<cur.erase(0,1)<<"' in compiler stage.\033[0m\n";
              assert(false);
            }
          }
        }
        else if(lex::literal_or_var(cur)!=lex::ERROR){
          if(VAR_ASSIGN!=""){
            Variable var = PROCS[CP].VARS[VAR_ASSIGN];
            if(lex::literal_or_var(cur)!=lex::STRING){
              out_stream <<
              "  mov "<<asm_var(var)<<", "<<asm_get_value(cur, PROCS[CP])<<"\n";
              VAR_ASSIGN="";
            }
            else{
              string value = cur.erase(0,1);
              value.erase(value.size()-1, 1);
              int size     = value.size();
              int ptrv     = var.byte_addr;
              int counter  = 0;
              string s;
              while(size-counter>0){
                if(size-counter>=8){
                  s="";
                  for(int i = counter; i < counter+8; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov rax, '"<<s<<"'\n"
                  "  mov qword [rbp-"<<ptrv-counter<<"], rax\n";
                  counter+=8;
                }
                else if(size-counter>=4){
                  s="";
                  for(int i = counter; i < counter+4; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov eax, '"<<s<<"'\n"
                  "  mov dword [rbp-"<<ptrv-counter<<"], eax\n";
                  counter+=4;
                }
                else if(size-counter>=2){
                  s="";
                  for(int i = counter; i < counter+2; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov ax, '"<<s<<"'\n"
                  "  mov word [rbp-"<<ptrv-counter<<"], ax\n";
                  counter+=2;
                }
                else if(size-counter==1){
                  s="";
                  for(int i = counter; i < counter+1; i++){
                    s += value[i];
                  }
                  out_stream<<
                  "  mov al, '"<<s<<"'\n"
                  "  mov byte [rbp-"<<ptrv-counter<<"], al\n";
                  counter+=1;
                }
              }
              VAR_ASSIGN="";
            }
            i++;
          }
        }
        else{
          cerr << i << "\n"
          << "BEFORE: " << INTER[i+1] << "\n"
          << "AFTER: "  << INTER[i-1] << "\n";
          cerr << "\033[1;31mUnhandled token of '"<< cur <<"' in compilation.\033[0m\n";
          assert(false);
        }
      }
      out_stream << DATA_SECTION << "\n" << BSS_SECTION << "\n";
      printf("Done converting intermediate to NASM!\n");
    }
    
    // void to_nasmX86_I386(){
    //   printf("Intermediate to NASM...\n");
    //   for(string &s : INTER){
    //     cout << s << "\n";
    //   }
    //   string VAR_ASSIGN;
    //   string CP;
    //   for(int i=0; i<INTER.size(); i++){
    //     string cur = INTER[i];
    //     if(cur == "proc"){
    //       out_stream << INTER[++i] << ":\n"
    //       "  push ebp\n"
    //       "  mov ebp, esp\n";
    //       CP = INTER[i];
    //       i++;
    //     }
    //     else if(cur == "end"){
    //       if(INTER[++i]=="main"){
    //         out_stream <<
    //         "  pop ebp\n"
    //         "  mov eax, 1\n"
    //         "  mov ebx, 0\n"
    //         "  int 80h\n";
    //       }
          
    //       else{
    //         out_stream <<
    //         "  pop ebp\n"
    //         "  ret\n";
    //       }
    //       CP = "///NA///";
    //       i++;
    //     }
    //     else if(cur == "def"){
    //       int size          = stoi(INTER[++i].erase(0,1))/8;
    //       string sym        = INTER[++i];
    //       BYTES[CP]        += size;
    //       BYTE_MAP[CP][sym] = BYTES[CP];
    //       VAR_SIZE[CP][sym] = size;
    //       if(INTER[++i]=="=") VAR_ASSIGN = sym;
    //     }
    //     else if(cur == "+"){
    //       int size1 = stoi(INTER[++i].erase(0,1))/8;
    //       int size2 = stoi(INTER[++i].erase(0,1))/8;
    //       string reg1;
    //       string reg2;
    //       op_init(size1, size2, reg1, reg2, i, CP);
    //       out_stream <<
    //       "  add "<<reg1<<", "<<reg2<<"\n";
    //       i++;
    //       if(VAR_ASSIGN!=""){
    //         if(VAR_SIZE[CP][VAR_ASSIGN]==size1)
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<reg1<<"\n";
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]<size1){
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a")<<"\n";
    //         }
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]>size1){
    //           string outreg = get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a");
    //           out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
    //           "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<< outreg <<"\n";
    //         }
    //         VAR_ASSIGN="";
    //       }
    //     }
    //     else if(cur == "-"){
    //       int size1 = stoi(INTER[++i].erase(0,1))/8;
    //       int size2 = stoi(INTER[++i].erase(0,1))/8;
    //       string reg1;
    //       string reg2;
    //       op_init(size1, size2, reg1, reg2, i, CP);
    //       out_stream <<
    //       "  sub "<<reg1<<", "<<reg2<<"\n";
    //       i++;
    //       if(VAR_ASSIGN!=""){
    //         if(VAR_SIZE[CP][VAR_ASSIGN]==size1)
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<reg1<<"\n";
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]<size1){
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a")<<"\n";
    //         }
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]>size1){
    //           string outreg = get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a");
    //           out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
    //           "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<< outreg <<"\n";
    //         }
    //         VAR_ASSIGN="";
    //       }
    //     }
    //     else if(cur == "*"){
    //       int size1 = stoi(INTER[++i].erase(0,1))/8;
    //       int size2 = stoi(INTER[++i].erase(0,1))/8;
    //       string reg1;
    //       string reg2;
    //       op_init(size1, size2, reg1, reg2, i, CP);
    //       out_stream <<
    //       "  mul "<<reg2<<"\n";
    //       i++;
    //       if(VAR_ASSIGN!=""){
    //         if(VAR_SIZE[CP][VAR_ASSIGN]==size1)
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<reg1<<"\n";
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]<size1){
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a")<<"\n";
    //         }
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]>size1){
    //           string outreg = get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a");
    //           out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
    //           "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<< outreg <<"\n";
    //         }
    //         VAR_ASSIGN="";
    //       }
    //     }
    //     else if(cur == "/"){
    //       int size1 = stoi(INTER[++i].erase(0,1))/8;
    //       int size2 = stoi(INTER[++i].erase(0,1))/8;
    //       string reg1;
    //       string reg2;
    //       op_init(size1, size2, reg1, reg2, i, CP);
    //       out_stream <<
    //       "  div "<<reg2<<"\n";
    //       i++;
    //       if(VAR_ASSIGN!=""){
    //         if(VAR_SIZE[CP][VAR_ASSIGN]==size1)
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<reg1<<"\n";
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]<size1){
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a")<<"\n";
    //         }
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]>size1){
    //           string outreg = get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a");
    //           out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
    //           "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<< outreg <<"\n";
    //         }
    //         VAR_ASSIGN="";
    //       }
    //     }
    //     else if(cur == "%"){
    //       int size1 = stoi(INTER[++i].erase(0,1))/8;
    //       int size2 = stoi(INTER[++i].erase(0,1))/8;
    //       string reg1;
    //       string reg2;
    //       op_init(size1, size2, reg1, reg2, i, CP);
    //       out_stream <<
    //       "  div "<<reg2<<"\n";
    //       i++;
    //       if(VAR_ASSIGN!=""){
    //         if(VAR_SIZE[CP][VAR_ASSIGN]==size1)
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<reg1<<"\n";
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]<size1){
    //           out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<get_reg(VAR_SIZE[CP][VAR_ASSIGN], "d")<<"\n";
    //         }
    //         else if(VAR_SIZE[CP][VAR_ASSIGN]>size1){
    //           string outreg = get_reg(VAR_SIZE[CP][VAR_ASSIGN], "d");
    //           out_stream << "  movsx " << outreg << ", " << reg1 << "\n"
    //           "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<< outreg <<"\n";
    //         }
    //         VAR_ASSIGN="";
    //       }
    //     }
    //     else if(cur == "&"){
    //       out_stream <<
    //       "  lea eax, " << asm_get_value(INTER[++i], VAR_SIZE[CP], BYTE_MAP[CP]) << "\n";
    //       if(VAR_ASSIGN!=""){
    //         string outreg = get_reg(VAR_SIZE[CP][VAR_ASSIGN], "a");
    //         out_stream << "  mov " << asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP]) << ", "<<outreg<<"\n";
    //       }
    //       i++;
    //     }
    //     else if(VAR_SIZE[CP].find(cur)!=VAR_SIZE[CP].end()){
    //       if(VAR_ASSIGN!=""){
    //         out_stream<<
    //         "mov "<<get_reg(VAR_SIZE[CP][cur], "a")<<", "<<asm_get_value(cur, VAR_SIZE[CP], BYTE_MAP[CP])<<"\n"
    //         "mov "<<asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP])<<", "<<get_reg(VAR_SIZE[CP][cur], "a")<<"\n";
    //         VAR_ASSIGN="";
    //         i++;
    //       }
    //       else{
    //         if(INTER[++i]=="=") VAR_ASSIGN = cur;
    //         else{
    //           cerr << "\033[1;31mHanging variable of '"<<cur.erase(0,1)<<"' in compiler stage.\033[0m\n";
    //           assert(false);
    //         }
    //       }
    //     }
    //     else if(lex::literal_or_var(cur)!=lex::ERROR){
    //       if(VAR_ASSIGN!=""){
    //         out_stream <<
    //         "  mov "<<asm_get_value(VAR_ASSIGN, VAR_SIZE[CP], BYTE_MAP[CP])<<", "<<cur<<"\n";
    //         VAR_ASSIGN="";
    //       }
    //     }
    //     else{
    //       cerr << "\033[1;31mUnhandled token of '"<< cur <<"' in compilation.\033[0m\n";
    //       assert(false);
    //     }
    //   }
    //   printf("Done converting intermediate to NASM!\n");
    // }
    
  };
}

#endif
