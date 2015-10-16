#ifndef RPCREC_H
#define RPCREC_H
#include "dic.hxx"
#include "Gaucho/dimhist.h"
#include "Gaucho/RPCdefs.h"
#include <string>
#include <vector>
#include <map>
//#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
typedef  std::vector<char*> NAMEVEC;
typedef  std::vector<std::string*> STRVEC;
typedef  std::map<char*,void*> PTRMAP;
class RPCRec : public DimRpcInfo
{
private:
  bool m_synch;
  RPCCommType m_reply;
public:
  NAMEVEC *names;
  PTRMAP *hists;
  void rpcInfoHandler();
  RPCRec(char *name, int timeout,bool synch=false); // : DimRpcInfo(name, timeout, -1)
  virtual ~RPCRec();
  void (*DirCB)(NAMEVEC*);
  void (*DatCB)(PTRMAP*);
  void declareDirCB(void CBfn(NAMEVEC*)){DirCB = CBfn;return;};
  void declareDatCB(void CBfn(PTRMAP*)){DatCB = CBfn;return;};
  int analyseReply();
  int analyseReply(NAMEVEC *);
  int analyseReply(PTRMAP *);
  int analyseReply(STRVEC *);
};
#endif
