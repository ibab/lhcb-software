#ifndef HistRRPC_H
#define HistRRPC_H
#include "dim/dis.hxx"
#include "RPCdefs.h"
#include "ObjSerializer.h"
#include "ObjRPC.h"
class HistRPC : public ObjRPC, DimRpc
{
public:
  HistRPC(ObjSerializer *, char *, char *, char*);
  virtual ~HistRPC();
  void rpcHandler();
};
#endif
