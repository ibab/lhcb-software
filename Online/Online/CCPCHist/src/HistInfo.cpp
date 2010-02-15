#include "HistInfo.h"
#include "hist_types.h"
#include "stdio.h"
static int empty =-1;
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
HistInfo::HistInfo(char *targetservice, int period): DIMTYPE(targetservice,period, &empty,4)
{
  m_TargetService = targetservice;
  AdderFn = 0;
  adderobj = 0;
}
HistInfo::HistInfo(char *targetservice, void addfn(void*,void*, int, HistInfo *), void *addobj, int period): DIMTYPE(targetservice,period, &empty,4)
{
  m_TargetService = targetservice;
  AdderFn = addfn;
  adderobj = addobj;
}
HistInfo::HistInfo(char *targetservice, void addfn(void*,void*, int, HistInfo *), void *addobj): DIMTYPE(targetservice, &empty,4)
{
  m_TargetService = targetservice;
  AdderFn = addfn;
  adderobj = addobj;
}
void HistInfo::infoHandler()
{
  void *valin; 
  int siz;
  void* valend;
  siz = getSize();
  valin = getData();
  valend = AddPtr(valin,siz);
  //printf("Received answer of size %d\n",siz);
  if (siz == 4 && *(int *)valin == -1)
  {
    printf ("No link...");
    return;
  }
  //DimHistbuff1 *p1;
  //p1 = (DimHistbuff1*)AddPtr(valin,sizeof(long long));//(AddPtr(valin,sizeof(RPCReply)));
  //while (p1<valend)
  //{
  //  int titoff;
  //  titoff = p1->titoff;//sizeof(DimHistbuff1);
  //  printf ("Type %d record Length %d dimension %d hdrlength %d\n title %s\n",
  //    p1->type,p1->reclen,p1->dim,p1->dataoff,(char*)((char*)p1+titoff));
  //  p1 = (DimHistbuff1*)((char*)p1+p1->reclen);
  //}
  if (AdderFn != 0)
  {
    (*AdderFn)(adderobj, valin, siz, this);
  }
  return;
}
