#include "Gaucho/MonInfo.h"
#include "Gaucho/MonTypes.h"
#include "stdio.h"
static int empty =-1;
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
MonInfo::MonInfo(char *targetservice, int period=0): DIMTYPE(targetservice,period, &empty,4)
{
  m_TargetService = targetservice;
  AdderFn = 0;
  adderobj = 0;
}
MonInfo::MonInfo(char *targetservice, void addfn(void*,void*, int, MonInfo *), void *addobj, int period=0): DIMTYPE(targetservice,period, &empty,4)
{
  m_TargetService = targetservice;
  AdderFn = addfn;
  adderobj = addobj;
}
MonInfo::MonInfo(char *targetservice, void addfn(void*,void*, int, MonInfo *), void *addobj): DIMTYPE(targetservice, &empty,4)
{
  m_TargetService = targetservice;
  AdderFn = addfn;
  adderobj = addobj;
}
void MonInfo::infoHandler()
{
  void *valin;
  int siz;
  siz = getSize();
  valin = getData();
  //printf("Received answer of size %d\n",siz);
  if (siz == 4 && *(int *)valin == -1)
  {
//    printf ("No link...\n");
    return;
  }
  if (AdderFn != 0)
  {
    (*AdderFn)(adderobj, valin, siz, this);
  }
  return;
}
