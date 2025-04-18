#pragma once 
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#ifndef IsaLLVM_FLAGS_COMPILER
#define IsaLLVM_FLAGS_COMPILER
#define NAME_FILE 0
#define GENERETE_BYTECODE 1

struct Flagsmodel{
  bool states;
  int local;
  std::string value;

  Flagsmodel(bool states, int local) : local(local), states(states) {} 
};



static std::vector<bool> flags(int argc, char **argv) {
  std::vector<bool> flags {
    false, // run time execute
    false, // generete bytecode
    false
  };
  std::map<std::string, Flagsmodel> flags_total {
    {"-o", {true, NAME_FILE}},
    {"--bytecode", {true, GENERETE_BYTECODE}}
  };

  for (int i = 0; i < argc; ++i) {
    std::string arg(argv[i]);
    if (arg.substr(0,1) == "-" && arg.substr(1,1) != "-") {
      auto it = flags_total.find(arg);
      if(it != flags_total.end()) {
        flags[it->second.local] = it->second.states;
      } 
    } else if (arg.substr(0,1) == "-" && arg.substr(1,1) == "-") {
      auto it = flags_total.find(arg);
        if (it != flags_total.end() && i+1 < argc) {
          std::string value(argv[i+1]);
          if (value.substr(0,1) != "-") {
            it->second.value = std::string(argv[i+1]);
            flags[it->second.local] = it->second.states;  
          } else {
            
            //exit(EXIT_FAILURE);
          }
        } else {
       
        //exit(EXIT_FAILURE);
      } 
    }
  }
  return flags;
} 

#endif // !FLAGS_DATABASE
