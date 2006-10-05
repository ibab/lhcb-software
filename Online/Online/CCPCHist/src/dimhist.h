#ifndef DIMHIST_DEF
#define DIMHIST_DEF
#include "CCPCHist/CCPCHist.h"
#include "dis.hxx"
class DimHistbuff1
{
public:
	float dim;
	float nxbin;
	float xmin;
	float xmax;
	float nentries;
	bintype entries;
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
	bintype entries;
};
class Histo;
class HistServer : public DimServer
{
public:
  HistServer();
  ~HistServer();
};
class HistService : public DimService
{
public:

	Histo *p;
	HistService ();
	HistService (Histo *h, const char *name, char *format, void *buff, int siz);
	void serviceHandler();
};
#endif
