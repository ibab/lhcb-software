#ifndef HISTADDER_H
#define HISTADDER_H
#include "Gaucho/MonTypes.h"
#include "Gaucho/MonInfo.h"
#include "Gaucho/ObjService.h"
#include <map>
#include <string>
#include "Gaucho/AddSerializer.h"
#include "Gaucho/MonAdder.h"

class ObjService;
class MonServer;
class ObjRPC;
//class HistAdder;
class MyTProfile;

class HistAdder : public MonAdder
{
public:
  void add(void *buffer, int siz, MonInfo *h);
  void addBuffer(void *buffer, int siz, MonInfo *h);
  void Update();
  HistAdder(char *myName, char * serviceName/*, bool expand = false*/);
  virtual ~HistAdder();
//  void TaskHandler(char*,int);
//  void ServiceHandler(DimInfo *,char*,int);
};
#endif
