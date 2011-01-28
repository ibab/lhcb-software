#ifndef COUNTERADDER_H
#define COUNTERADDER_H
#include "Gaucho/MonTypes.h"
#include "Gaucho/MonInfo.h"
#include "Gaucho/ObjService.h"
#include <map>
#include <string>
#include "Gaucho/AddSerializer.h"
#include "Gaucho/MonAdder.h"

class MonService;
//class HistServer;
class ObjRPC;

class CounterAdder : public MonAdder, DimInfo
{
public:
  void add(void *buffer, int siz, MonInfo *h);
  CounterAdder(char *taskname, char *myName, char * serviceName, bool expand = false);
  void infoHandler (void) ;
};
#endif
