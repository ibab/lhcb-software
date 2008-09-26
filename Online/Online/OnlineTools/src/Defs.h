#ifndef ONLINETOOLS_DEFS_H
#define ONLINETOOLS_DEFS_H

#include <vector>
#include <string>

namespace OnlineTools {
  enum { 
    CMD_BACKSPACE = -1,
    CMD_COM0    =  9990,
    CMD_COM1    =  9991,
    CMD_COM2    =  9992,

    CMD_COMLAST = 10000,
    CMD_CLOSE   = 11000, 
    CMD_APPLY   = 12008,
    CMD_SHOW    = 12010,
    CMD_DELETE  = 12014,
    CMD_CANCEL  = 12016,
    CMD_EXECUTE = 25000,
    CMD_LAST
  };

  union ioc_data {
    void* ptr;
    std::string* str;
    std::vector<std::string>* vec;
    ioc_data(void* p) : ptr(p) {}
  };
  void backSpaceCallBack (int menu,int cmd,int par,const void* param);
}
#endif // ONLINETOOLS_DEFS_H
