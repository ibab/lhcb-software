#ifndef HISTINFO_H
#define HISTINFO_H
#include "dimhist.h"
#include "dim/dic.hxx"
#include <string>
#include <map>
class HistInfo : public DIMTYPE
{
  std::map<std::string,void*> m_histmap;
  void (*AdderFn)(void*,void*,int, HistInfo *);
  void *adderobj;
public:
  std::string m_TargetService;
  HistInfo(char *target, int period);
  HistInfo(char *target, void addfn(void*,void*, int, HistInfo *), void * add, int period);
  HistInfo(char *target, void addfn(void*,void*, int, HistInfo *), void * add);
  void infoHandler (void) ;
};
#endif
