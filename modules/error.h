#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

using namespace std;

namespace errh{
  //Get file ect.
  struct FileRange{
    unsigned int begin;
    unsigned int end;
    string filename;
  };
  struct ErrorInfo{
    unsigned int line;
    string filename;
  };
  class FileIdentity{
    public:
    vector<FileRange> ranges;
    
    FileRange& get_FileRange(int line){
      FileRange* current;
      for(int i = ranges.size()-1; i >= 0; i--){
        FileRange* r = &ranges[i];
        if(line <= r->end){
          current = r;
          break;
        }
      }
      return *current;
    }
    ErrorInfo get_ErrorInfo(int line){
      FileRange* current = &get_FileRange(line);
      ErrorInfo info;
      info.filename = current->filename;
      info.line     = line-current->begin;
      return info;
    }
    void push_back(FileRange file){
      ranges.push_back(file);
    }
    void add_file(string filename, int begin, int end){
      FileRange* current = &get_FileRange(begin+1);
      current->begin = end+1;
      FileRange file;
      file.begin = begin;
      file.end = end;
      file.filename = filename;
      ranges.push_back(file);
    }
  };
  
  string formatErrorInfo(ErrorInfo err){
    string s;
    s+=err.filename+":";
    s+=to_string(err.line);
    s+=":";
    return s;
  }
  
  #define startErrorThrow(tk) \
    cerr << "\033[1;31m" << errh::formatErrorInfo(lex::Files.get_ErrorInfo(tk.line));
  
  #define endErrorThrow() \
    cerr << "\033[0m\n";
}

#endif