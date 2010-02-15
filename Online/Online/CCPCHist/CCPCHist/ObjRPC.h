#ifndef OBJRRPC_H
#define OBJRRPC_H
#include "dim/dis.hxx"
#include "RPCdefs.h"
#include "ObjSerializer.h"
class ObjRPC 
{
protected:
  int buffersize;
  void *buffer;
  ObjSerializer *s;
public:
  ObjRPC(){};
  ObjRPC(ObjSerializer *, char *, char *, char*);
  virtual ~ObjRPC(){};
};
#endif
