#ifndef ROLOGGER_ROLOGGERDEFS_H
#define ROLOGGER_ROLOGGERDEFS_H

#include <vector>
#include <string>

namespace ROLogger {

  std::string fmcLogger(const std::string& host,const std::string& facility);

  enum { 
    CMD_BACKSPACE = -1,
    CMD_1000    =  1000,
    CMD_2000    =  2000,
    CMD_3000    =  3000,
    CMD_4000    =  4000,
    CMD_5000    =  5000,
    CMD_6000    =  6000,
    CMD_7000    =  7000,
    CMD_8000    =  8000,
    CMD_9000    =  9000,

    CMD_STORAGE =  9980,
    CMD_MONITOR =  9981,

    CMD_COM0    =  9990,
    CMD_COM1    =  9991,
    CMD_COM2    =  9992,
    CMD_COM3    =  9993,
    CMD_COM4    =  9994,
    CMD_COM5    =  9995,
    CMD_COM6    =  9996,
    CMD_COM7    =  9997,
    CMD_COM8    =  9998,
    CMD_COM9    =  9999,

    CMD_COMLAST = 10000,
    CMD_CLOSE   = 11000, 
    CMD_CONNECT = 12000,
    CMD_NEW     = 12001,
    CMD_ADD     = 12002,
    CMD_DIR     = 12003,
    CMD_FILE    = 12004,
    CMD_LOAD    = 12005,
    CMD_START   = 12006,
    CMD_STOP    = 12007,
    CMD_APPLY   = 12008,
    CMD_SAVE    = 12009,
    CMD_SHOW    = 12010,
    CMD_EDIT    = 12011,
    CMD_CLEAR   = 12012,
    CMD_RESET   = 12013,
    CMD_DELETE  = 12014,
    CMD_UPDATE  = 12015,
    CMD_CANCEL  = 12016,

    CMD_SEVERITY,
    CMD_ADD_NODES,
    CMD_UPDATE_FARMS,
    CMD_UPDATE_NODES,
    CMD_UPDATE_CLUSTERS,

    CMD_SUMM_HISTORY,
    CMD_NODE_HISTORY,
    CMD_FARM_HISTORY,
    CMD_WILD_MESSAGE,
    CMD_WILD_NODE,
    CMD_WILD_PROCESS,
    CMD_WILD_COMPONENT,

    CMD_CONNECT_STORAGE,
    CMD_DISCONNECT_STORAGE,

    CMD_CONNECT_MONITORING,
    CMD_DISCONNECT_MONITORING,

    CMD_CONNECT_CLUSTER,
    CMD_DISCONNECT_CLUSTER,

    CMD_CONNECT_RECONSTRUCTION,
    CMD_DISCONNECT_RECONSTRUCTION,

    CMD_SHOW_FILTERS,
    CMD_LOAD_FILTERS,
    CMD_SAVE_FILTERS,
    CMD_APPLY_FILTERS,

    CMD_EXECUTE = 25000,
    
    CMD_UPDATE_RUNNUMBER,
    CMD_SET_PARTITION,

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
#endif // ROLOGGER_ROLOGGERDEFS_H
