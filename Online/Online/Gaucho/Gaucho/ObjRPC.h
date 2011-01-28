#ifndef OBJRRPC_H
#define OBJRRPC_H
#include "dim/dis.hxx"
#include "Gaucho/RPCdefs.h"
#include "Gaucho/ObjSerializer.h"
class ObjRPC : public DimRpc
{
protected:
  int buffersize;
  void *buffer;
  ObjSerializer *s;
  void *m_maplockid;
  void *m_objlockid;
public:
  ObjRPC();
  ObjRPC(ObjSerializer *, char *, char *, char*);
  ObjRPC(ObjSerializer *, char *, char *, char*, void*, void *);
  virtual ~ObjRPC();
  void rpcHandler();
};
#endif
