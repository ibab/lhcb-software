#ifndef OBJRRPC_H
#define OBJRRPC_H
#include "dis.hxx"
#include "Gaucho/RPCdefs.h"
#include "Gaucho/ObjSerializer.h"
#include "Gaucho/BRTL_Lock.h"
class ObjRPC : public DimRpc
{
protected:
  int buffersize;
  void *buffer;
  ObjSerializer *s;
  BRTLLock *m_maplockid;
  BRTLLock *m_objlockid;
public:
  ObjRPC();
  ObjRPC(ObjSerializer *, char *, char *, char*);
  ObjRPC(ObjSerializer *, char *, char *, char*, BRTLLock*, BRTLLock *);
  ObjRPC(DimServerDns *,ObjSerializer *, char *, char *, char*);
  ObjRPC(DimServerDns *,ObjSerializer *, char *, char *, char*, BRTLLock*, BRTLLock *);
  virtual ~ObjRPC();
  void rpcHandler();
};
#endif
