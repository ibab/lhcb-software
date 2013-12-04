#include "Gaucho/ObjRPC.h"
#include <stdio.h>
#include "Gaucho/MonSys.h"
#include "Gaucho/MonSubSys.h"
#include "Gaucho/Utilities.h"
//#include "iCCPCHist.h"
#include <vector>
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
ObjRPC::ObjRPC(ObjSerializer *srv, char *n, char *f_in, char *f_out, BRTLLock *mlid, BRTLLock *olid) : DimRpc(n, f_in, f_out)
{
  s = srv;
  buffersize = 0;
  buffer = 0;
  m_maplockid = mlid;
  m_objlockid = olid;
}
ObjRPC::ObjRPC(DimServerDns *dns,ObjSerializer *srv, char *n, char *f_in, char *f_out) : DimRpc(dns,n, f_in, f_out)
{
  s = srv;
  buffersize = 0;
  buffer = 0;
  m_maplockid = 0;
  m_objlockid = 0;
}
ObjRPC::ObjRPC(DimServerDns *dns,ObjSerializer *srv, char *n, char *f_in, char *f_out, BRTLLock *mlid, BRTLLock *olid) : DimRpc(dns,n, f_in, f_out)
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
  void *p;
  int siz;
  RPCComm *comm;
  RPCReply *reply;
  Comm  = RPCCIllegal;
  int status;
  p = getData();
  comm  = (RPCComm*)p;
  void * ptr;
  Comm = comm->c;
  siz = sizeof(RPCReply);
  if (m_maplockid !=0)
  {
    m_maplockid->lockMutex();
  }
  switch (Comm)
  {
  case RPCCRead:
    {
      void *pp;
      char *hlist=(char *)AddPtr(comm,sizeof(comm->c));
      int bs = siz;
      dyn_string *nams;
      nams = Strsplit(hlist,(char*)"\n");
      if (m_objlockid != 0)
      {
        m_objlockid->lockMutex();
      }
      ptr = s->SerializeObj(*nams,pp,bs);
      if (m_objlockid != 0)
      {
        m_objlockid->unlockMutex();
      }
      siz = bs;
      status = 0;
      break;
    }
  case RPCCClear:
    {
      void *pp;
      char *hlist=(char *)AddPtr(comm,sizeof(comm->c));
      int bs = siz;
      dyn_string *nams;
      nams = Strsplit(hlist,(char*)"\n");
      if (m_objlockid != 0) m_objlockid->lockMutex();
      ptr = s->SerializeObj(*nams,pp,bs,true);
      if (m_objlockid != 0) m_objlockid->unlockMutex();
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
        m_objlockid->lockMutex();
      }
      ptr = s->SerializeObj(pp,bs,true);
      if (m_objlockid != 0)
      {
        m_objlockid->unlockMutex();
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
        m_objlockid->lockMutex();
      }
      ptr = s->SerializeObj(pp,bs);
      if (m_objlockid != 0)
      {
        m_objlockid->unlockMutex();
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
        m_maplockid->unlockMutex();
      }
      return;
      break;
    }
  }
  if (m_maplockid !=0)
  {
    m_maplockid->unlockMutex();
  }

  reply = (RPCReply*)ptr;
  reply->status = status;
  reply->comm = Comm;
  setData(ptr, siz);
  return;
}
