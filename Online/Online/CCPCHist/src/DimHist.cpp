#include <memory.h>
#include <stdio.h>
#include "dimhist.h"
HistService::HistService()
{
}
HistService::HistService (Histo *h, const char *name, char *format, void *buff, int siz)
:DimService(name, format, buff, siz)
{
  p  = h;
}
void HistService::serviceHandler()
{
  void *ptr;
  int  siz;
  switch (p->type())
  {
  case 	H_1DIM:
    {
      siz = sizeof(DimHistbuff1)-sizeof(bintype)+2*(p->nx+2)*sizeof(bintype);
      ptr = malloc(siz);
      if (ptr == 0)
      {
        printf("bad malloc %i\n",siz);
        return;
      }
      DimHistbuff1 *pp = (DimHistbuff1*)ptr;
      p->get(&pp->nentries,&pp->entries);
      bintype *errp;
      errp  = &(pp->entries)+(p->nx+2);
      p->geterr(&pp->nentries,errp);
      pp->dim = 1;
      pp->nxbin = p->nx;
      pp->xmin  = p->xmin;
      pp->xmax  = p->xmax;
      setData(pp,siz);
//      updateService(pp,siz);
      break;
    }
  case H_PROFILE:
    {
      siz = sizeof(DimHistbuff1)-sizeof(bintype)+2*(p->nx+2)*sizeof(bintype)+(p->nx+2)*sizeof(int);
      ptr = malloc(siz);
      if (ptr == 0)
      {
        printf("bad malloc %i\n",siz);
        return;
      }
      PHisto *ph = (PHisto*)p;
      DimHistbuff1 *pp = (DimHistbuff1*)ptr;
      float *nents;
      bintype *sum, *sum2;

      nents = (float*)&(pp->entries);
      sum = &(pp->entries)+(p->nx+2);
      sum2  = &(pp->entries)+2*(p->nx+2);
      ph->getentries(&pp->nentries,nents);
      ph->getsums(&pp->nentries, sum);
      ph->getsum2s(&pp->nentries, sum2);
      pp->dim = 1;
      pp->nxbin = p->nx;
      pp->xmin  = p->xmin;
      pp->xmax  = p->xmax;
      setData(pp,siz);
      break;
    }
  case H_2DIM:
    {
      siz = sizeof(DimHistbuff2)-sizeof(bintype)+2*(p->nx+2)*(p->ny+2)*sizeof(bintype);
      ptr = malloc(siz);
      if (ptr == 0)
      {
        printf("bad malloc %i\n",siz);
        return;
      }
      DimHistbuff2 *pp = (DimHistbuff2*)ptr;
      p->get(&pp->nentries,&pp->entries);
      bintype *errp;
      errp  = &(pp->entries)+(p->nx+2)*(p->ny+2);
      p->geterr(&pp->nentries,errp);
      pp->dim = 2;
      pp->nxbin = p->nx;
      pp->xmin  = p->xmin;
      pp->xmax  = p->xmax;
      pp->nybin = p->ny;
      pp->ymin  = p->ymin;
      pp->ymax  = p->ymax;
      setData(pp,siz);
//      updateService(pp,siz);
     break;
    }
  default:
    {
      return;
    }
  }
  return;
}

HistServer::HistServer()
{
}
HistServer::~HistServer()
{
}
