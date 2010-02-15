#ifndef RPCREC_H
#define RPCREC_H
#include "dim/dic.hxx"
#include "dimhist.h"
#include "RPCdefs.h"
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
class RPCRec : public DimRpcInfo 
{ 
public: 
  void rpcInfoHandler();
  RPCRec(char *name, int timeout); // : DimRpcInfo(name, timeout, -1)
}; 
#endif
