#include "HistRPC.h"
#include <stdio.h>
#include "CCPCHsys.h"
#include "iCCPCHist.h"
#include <vector>
HistRPC::~HistRPC()
{
  if (buffersize >0)
  {
    free (buffer);
  }
}
HistRPC::HistRPC(ObjSerializer *srv, char *n, char *f_in, char *f_out) : DimRpc(n, f_in, f_out)
{
  s = srv;
  buffersize = 0;
  buffer = 0;
}
void HistRPC::rpcHandler()
{
  char toksep[] = " ";
  RPCCommType Comm;
  char *what;
  int clid;
  void *p;
  int siz;
  RPCComm *comm;
  RPCReply *reply;
  Comm  = RPCCIllegal;
  what  = 0;
  int len;
  int status;
  clid  = DimServer::getClientId();
  p = getData();
  len = getSize();
  comm  = (RPCComm*)p;
  void * ptr;
  Comm = comm->c;
  siz = sizeof(RPCReply);
  switch (Comm)
  {
  case RPCCRead:
    {
      std::vector<std::string> nams;
      void *pp;
      char *hlist=(char *)AddPtr(comm,sizeof(comm->c));
      char *nam;
      nams.reserve(100);
      nam =strtok(hlist,"\n");
      int bs = siz;
      while(nam!=0)
      {
        nams.push_back(nam);
        nam = strtok(0,"\n");
      }
      ptr = s->SerializeHist(nams,pp,bs);
      siz = bs;
      status = 0;
      break;
    }
  case RPCCClear:
    {
      std::vector<std::string> nams;
      void *pp;
      char *hlist=(char *)AddPtr(comm,sizeof(comm->c));
      char *nam;
      nams.reserve(100);
      nam =strtok(hlist,"\n");
      int bs = siz;
      while(nam!=0)
      {
        nams.push_back(nam);
        nam = strtok(0,"\n");
      }
      ptr = s->SerializeHist(nams,pp,bs,true);
      siz = bs;
      status = 0;
      break;
    }
  case RPCCClearAll:
    {
      int bs = sizeof(RPCReply);
      void *pp;
      ptr = s->SerializeHist(pp,bs,true);
      siz = bs;
      status = 0;
      break;
    }
  case RPCCReadAll:
    {
      int bs = sizeof(RPCReply);
      void *pp;
      ptr = s->SerializeHist(pp,bs);
      siz = bs;
      status = 0;
      break;
    }
  case RPCCDirectory:
    {
      void *pp;
      int bs=sizeof(RPCReply);
      ptr = s->SerializeDir(pp,bs);
      siz = bs;
      status = 0;
      break;
    }
  case RPCCIllegal:
  default:
    {
      siz = sizeof(RPCReply);
      ptr = malloc(siz);
      Comm = RPCCIllegal;
      status = -2;
      reply = (RPCReply*)ptr;
      reply->status = status;
      reply->comm = Comm;
      setData(ptr, siz);
      free(ptr);
      return;
      break;
    }
  }
  reply = (RPCReply*)ptr;
  reply->status = status;
  reply->comm = Comm;
  setData(ptr, siz);
}
