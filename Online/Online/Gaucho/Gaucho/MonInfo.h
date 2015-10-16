#ifndef MonInfo_H
#define MonInfo_H
#include "dimhist.h"
#include "dic.hxx"
#include <string>
#include <map>
class MonInfo : public DIMTYPE
{
  std::map<std::string,void*> m_histmap;
  void (*AdderFn)(void*,void*,int, MonInfo *);
  void *adderobj;
public:
  std::string m_TargetService;
  virtual ~MonInfo(){}
  MonInfo(char *target, int period);
  MonInfo(char *target, void addfn(void*,void*, int, MonInfo *), void * add, int period);
  MonInfo(char *target, void addfn(void*,void*, int, MonInfo *), void * add);
  void infoHandler (void) ;
};
#endif
