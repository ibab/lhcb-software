#ifndef DIMHIST_DEF
#define DIMHIST_DEF
#include "iCCPCHist.h"
#include "dis.hxx"
#define dimtype float
//#define L_TITLE 80
class DimHistbuff1
{
public:
	float dim;
	float nxbin;
	float xmin;
	float xmax;
	float nentries;
	dimtype entries;
};
class DimHistbuff2 
{
public:
	float dim;
	float nxbin;
	float xmin;
	float xmax;
	float nybin;
	float ymin;
	float ymax;
	float nentries;
	dimtype entries;
};
class CCPCHisto;
enum RPCCommType
{
  RPCCIllegal,
  RPCCRead,
  RPCCReadAll,
  RPCCClear,
  RPCCClearAll
};
class RPCComm
{
public:
  RPCCommType Comm;
  char what;
};
class HistServer : public DimServer
{
public:
  HistServer();
  virtual ~HistServer();
};
class HistRPC : public DimRpc
{
public:
  CCPCHSys *s;
  HistRPC(CCPCHSys *, char *, char *, char*);
  virtual ~HistRPC();
  void rpcHandler();
};
class HistService : public DimService
{
public:

	CCPCHisto *p;
	HistService ();
  virtual ~HistService();
	HistService (CCPCHisto *h, const char *name, char *format, void *buff, int siz);
  int HistService::serialize(void* &ptr, int &siz, int offs);
  int HistService::serialize(void* &ptr, int &siz);
	void serviceHandler();
};
#endif
