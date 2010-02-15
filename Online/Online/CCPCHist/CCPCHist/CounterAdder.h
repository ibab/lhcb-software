#ifndef COUNTERADDER_H
#define COUNTERADDER_H
#include "hist_types.h"
#include "HistInfo.h"
#include "HistService.h"
#include <map>
#include <string>
#include "AddSerializer.h"
#include "CCPCAdder.h"

class HistService;
class HistServer;
class HistRPC;

class CounterAdder : public CCPCAdder, DimInfo
{
public:
  void add(void *buffer, int siz, HistInfo *h);
  CounterAdder(char *taskname, char *myName, char * serviceName, bool expand = false);
  void infoHandler (void) ;
};
#endif
