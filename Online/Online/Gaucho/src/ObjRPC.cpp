#include "Gaucho/ObjRPC.h"
#include <stdio.h>
#include "Gaucho/MonSys.h"
#include "Gaucho/MonSubSys.h"
//#include "iCCPCHist.h"
#include <vector>
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
ObjRPC::~ObjRPC()
{
  if (buffersize >0)
  {
    free (buffer);
  }
}
ObjRPC::ObjRPC(ObjSerializer *srv, char *n, char *f_in, char *f_out) : DimRpc(n, f_in, f_out)
{
  s = srv;
  buffersize = 0;
  buffer = 0;
  m_maplockid = 0;
  m_objlockid = 0;
}
ObjRPC::ObjRPC(ObjSerializer *srv, char *n, char *f_in, char *f_out, void *mlid, void *olid) : DimRpc(n, f_in, f_out)
{
  s = srv;
  buffersize = 0;
  buffer = 0;
  m_maplockid = mlid;
  m_objlockid = olid;
}
void ObjRPC::rpcHandler()
{
//  char toksep[] = " ";
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
  if (m_maplockid !=0)
  {
    lib_rtl_lock(m_maplockid);
  }
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
      if (m_objlockid != 0)
      {
        lib_rtl_lock(m_objlockid);
      }
      ptr = s->SerializeObj(nams,pp,bs);
      if (m_objlockid != 0)
      {
        lib_rtl_unlock(m_objlockid);
      }
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
      if (m_objlockid != 0) lib_rtl_lock(m_objlockid);
      ptr = s->SerializeObj(nams,pp,bs,true);
      if (m_objlockid != 0) lib_rtl_unlock(m_objlockid);
      siz = bs;
      status = 0;
      break;
    }
  case RPCCClearAll:
    {
      int bs = sizeof(RPCReply);
      void *pp;
      if (m_objlockid != 0)
      {
        lib_rtl_lock(m_objlockid);
      }
      ptr = s->SerializeObj(pp,bs,true);
      if (m_objlockid != 0)
      {
        lib_rtl_unlock(m_objlockid);
      }
      siz = bs;
      status = 0;
      break;
    }
  case RPCCReadAll:
    {
      int bs = sizeof(RPCReply);
      void *pp;
      if (m_objlockid != 0)
      {
        lib_rtl_lock(m_objlockid);
      }
      ptr = s->SerializeObj(pp,bs);
      if (m_objlockid != 0)
      {
        lib_rtl_unlock(m_objlockid);
      }
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
      if (m_maplockid !=0)
      {
        lib_rtl_unlock(m_maplockid);
      }
      return;
      break;
    }
  }
  if (m_maplockid !=0)
  {
    lib_rtl_unlock(m_maplockid);
  }

  reply = (RPCReply*)ptr;
  reply->status = status;
  reply->comm = Comm;
  setData(ptr, siz);
  return;
}
