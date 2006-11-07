#include <memory.h>
#include <stdio.h>
#include "dimhist.h"
#include <vector>
//#include <iostream>

HistServer::HistServer()
{
};

HistServer::~HistServer()
{
};

HistService::HistService()
{
};

HistService::~HistService()
{
};

HistService::HistService (CCPCHisto *h, const char *name, char *format, void *buff, int siz)
:DimService(name, format, buff, siz)
{
  p  = h;
}

int HistService::serialize(void *&ptr, int &siz)
{
  return serialize(ptr, siz, 0);
}
int HistService::serialize(void *&ptr, int &siz, int offs)
{
  int status;
  switch (p->type())
  {
  case 	H_1DIM:
    {
      siz = sizeof(DimHistbuff1)-sizeof(dimtype)+2*(p->nx+2)*sizeof(dimtype);
      siz += offs;
      ptr = malloc(siz);
      if (ptr == 0)
      {
        printf("bad malloc %i\n",siz);
        return -3;
      }
      ptr = (void*)((int)ptr +offs);
      DimHistbuff1 *pp = (DimHistbuff1*)ptr;
      p->get(&pp->entries);
      pp->nentries = (float)p->getnents();
      dimtype *errp;
      errp  = &(pp->entries)+(p->nx+2);
      p->geterr(errp);
      pp->dim = 1;
      pp->nxbin = (float)p->nx;
      pp->xmin  = p->xmin;
      pp->xmax  = p->xmax;
      status  = 0;
      break;
    }
  case H_PROFILE:
    {
      siz = sizeof(DimHistbuff1)-sizeof(dimtype)+2*(p->nx+2)*sizeof(dimtype)+(p->nx+2)*sizeof(int);
      siz += offs;
      ptr = malloc(siz);
      if (ptr == 0)
      {
        printf("bad malloc %i\n",siz);
        return -3;
      }
      ptr = (void*)((int)ptr +offs);
      CCPCPHisto *ph = (CCPCPHisto*)p;
      DimHistbuff1 *pp = (DimHistbuff1*)ptr;
      float *nents;
      dimtype *sum, *sum2;

      pp->nentries = (float)ph->getnents();
      nents = (float*)&(pp->entries);
      sum = &(pp->entries)+(p->nx+2);
      sum2  = &(pp->entries)+2*(p->nx+2);
      ph->getentries(nents);
      ph->getsums(sum);
      ph->getsum2s(sum2);
      pp->dim = 1;
      pp->nxbin = (float)p->nx;
      pp->xmin  = p->xmin;
      pp->xmax  = p->xmax;
      status  = 0;
      break;
    }
  case H_2DIM:
    {
      siz = sizeof(DimHistbuff2)-sizeof(dimtype)+2*(p->nx+2)*(p->ny+2)*sizeof(dimtype);
      siz += offs;
      ptr = malloc(siz);
      if (ptr == 0)
      {
        printf("bad malloc %i\n",siz);
        return -3;
      }
      ptr = (void*)((int)ptr +offs);
      DimHistbuff2 *pp = (DimHistbuff2*)ptr;
      p->get(&pp->entries);
      dimtype *errp;
      errp  = &(pp->entries)+(p->nx+2)*(p->ny+2);
      p->geterr(errp);
      pp->nentries = (float)p->getnents();
      pp->dim = 2;
      pp->nxbin = (float)p->nx;
      pp->xmin  = p->xmin;
      pp->xmax  = p->xmax;
      pp->nybin = (float)p->ny;
      pp->ymin  = p->ymin;
      pp->ymax  = p->ymax;
      status  = 0;
     break;
    }
  default:
    {
      status  = -2;
      break;
    }
  }
  return status;
}
void HistService::serviceHandler()
{
  void* ptr;
  int  siz;
  int status;
  //dim_print_date_time();
  //std::cout << " Service Handler called" << std::endl; 
  status = serialize(ptr, siz,0);
  if (status == 0)
  {
    setData(ptr,siz);
    free(ptr);
  }
  return;
}
HistRPC::~HistRPC()
{
}
HistRPC::HistRPC(CCPCHSys *srv, char *n, char *f_in, char *f_out) : DimRpc(n, f_in, f_out)
{
  s = srv;
}
void HistRPC::rpcHandler()
{
  int clid;
  clid  = DimServer::getClientId();
  void *p;
  int siz;
  RPCComm *comm;
  int len;
  int status;
  p = getData();
  len = getSize();
  comm  = (RPCComm*)p;
  void * ptr;
  switch (comm->Comm)
  {
  case RPCCIllegal:
    {
      siz = 4;
      ptr = malloc(siz);
      status = -1;
      break;
    }
  case RPCCRead:
    {
      CCPCHisto *h;
      h = s->findhisto(&comm->what);
      if (h != 0)
      {
        h->serv->serialize(ptr, siz,4);
        status = 0;
      }
      else
      {
        siz = 4;
        ptr = malloc(siz);
        status = -2;
      }
      break;
    }
  case RPCCClear:
    {
      CCPCHisto *h;
      h = s->findhisto(&comm->what);
      if (h != 0)
      {
        h->serv->serialize(ptr, siz,4);
        h->clear();
        status = 0;
      }
      else
      {
        siz = 4;
        ptr = malloc(siz);
        status = -2;
      }
      break;
    }
  case RPCCClearAll:
    {
      std::vector <int>::size_type i;
      void *pp;
      int hs;
      for (i =0;i<s->hists.size();i++)
      {
        CCPCHisto *h = s->hists[i];
        h->serv->serialize(pp, hs,0);
        s->genSrv->selectiveUpdateService(pp, hs, &clid);
        h->clear();
      }
      siz = 4;
      ptr = malloc(siz);
      status  = 0;
      break;
    }
  case RPCCReadAll:
    {
      std::vector <int>::size_type i;
      void *pp;
      int hs;
      for (i =0;i<s->hists.size();i++)
      {
        CCPCHisto *h = s->hists[i];
        h->serv->serialize(pp, hs,0);
        s->genSrv->selectiveUpdateService(pp, hs, &clid);
      }
      siz = 4;
      ptr = malloc(siz);
      status  = 0;
      break;
    }
  default:
    {
      siz = 4;
      ptr = malloc(siz);
      status = -1;
      break;
    }
  }
  *(int*)ptr = status;
  setData(ptr, siz);
  free (ptr);
}
