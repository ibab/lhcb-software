#ifndef ROLOGGER_ROLOGGERDEFS_H
#define ROLOGGER_ROLOGGERDEFS_H

#include <vector>
#include <string>

namespace ROLogger {
  enum { 
    CMD_STORAGE = 9980,
    CMD_MONITOR = 9981,
    CMD_COM0    = 9990,
    CMD_COM1    = 9991,
    CMD_COM2    = 9992,
    CMD_COM3    = 9993,
    CMD_COM4    = 9994,
    CMD_COM5    = 9995,
    CMD_COM6    = 9996,
    CMD_COM7    = 9997,
    CMD_COM8    = 9998,
    CMD_CLOSE   = 9999, 
    CMD_CONNECT,
    CMD_UPDATE_FARMS,
    CMD_UPDATE_NODES,
    CMD_UPDATE_CLUSTERS,

    CMD_DELETE_PART_DISPLAY,
    CMD_DELETE_FARM_DISPLAY,

    CMD_CLEAR_HISTORY,
    CMD_SUMM_HISTORY,
    CMD_NODE_HISTORY,
    CMD_FARM_HISTORY,
    CMD_WILD_MESSAGE,
    CMD_WILD_NODE,

    CMD_CONNECT_STORAGE,
    CMD_DISCONNECT_STORAGE,

    CMD_CONNECT_MONITORING,
    CMD_DISCONNECT_MONITORING,

    CMD_CONNECT_CLUSTER,
    CMD_DISCONNECT_CLUSTER,

    CMD_LAST
  };

  union ioc_data {
    void* ptr;
    std::string* str;
    std::vector<std::string>* vec;
    ioc_data(void* p) : ptr(p) {}
  };

}
#endif // ROLOGGER_ROLOGGERDEFS_H
