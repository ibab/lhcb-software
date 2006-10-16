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
void HistService::serviceHandler()
{
  void *ptr;
  int  siz;
  //dim_print_date_time();
  //std::cout << " Service Handler called" << std::endl; 
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
      p->get(&pp->entries);
      pp->nentries = (float)p->getnents();
      bintype *errp;
      errp  = &(pp->entries)+(p->nx+2);
      p->geterr(errp);
      pp->dim = 1;
      pp->nxbin = (float)p->nx;
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
      CCPCPHisto *ph = (CCPCPHisto*)p;
      DimHistbuff1 *pp = (DimHistbuff1*)ptr;
      float *nents;
      bintype *sum, *sum2;

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
      p->get(&pp->entries);
      bintype *errp;
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
  RPCComm *comm;
  int len;
  int status;
  p = getData();
  len = getSize();
  comm  = (RPCComm*)p;
  switch (comm->Comm)
  {
  case RPCCIllegal:
    {
      status = -1;
      break;
    }
  case RPCCPublish:
    {
      CCPCHisto *h;
      h = s->findhisto(&comm->what);
      if (h != 0)
      {
        h->serv->selectiveUpdateService(&clid);
        status = 0;
      }
      else
      {
        status = -2;
      }
      break;
    }
  case RPCCCLear:
    {
      CCPCHisto *h;
      h = s->findhisto(&comm->what);
      if (h != 0)
      {
        h->serv->selectiveUpdateService(&clid);
        h->clear();
        status = 0;
      }
      else
      {
        status = -2;
      }
      break;
    }
  case RPCCClearAll:
    {
      std::vector <int>::size_type i;
      for (i =0;i<s->hists.size();i++)
      {
        CCPCHisto *h = s->hists[i];
        h->serv->selectiveUpdateService(&clid);
        h->clear();
      }
      status  = 0;
      break;
    }
  default:
    {
      status = -1;
      break;
    }
  }
  setData(status);
}
