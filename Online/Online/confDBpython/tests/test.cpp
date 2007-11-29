// test.cpp : Defines the entry point for the console application.
//
#ifdef _WIN32
#include <windows.h>
#else
#include "unistd.h"
#define Sleep(x) ::sleep(x/1000)
#endif
#include <string>
#include "oci.h"

#include "confDBpython/CONFDB.h"
#include "CDB.h"


int main(int /* argc */, char** /* argv */)
{
  DBContainer *cdb;
  int status;
  std::string db("devdb");
  std::string user("lhcb_configuration_dev");
  std::string passw("T_75_op_LbDB009");
	cdb = new DBContainer((char*)db.c_str(),(char*)user.c_str(),(char*)passw.c_str());
//  cdb->Connect();
  eConnectivityCache *ecache;
  iConnectivityCache *icache;
  ecache  = new eConnectivityCache(cdb);
  char devfro[]="MUONC_M4C20A1_PAD0_0_0";
	printf("Loading External Cache\n");
  status  = ecache->Fill(67);
	printf("External Cache filled...\n");
  icache  = new iConnectivityCache(cdb);
	printf("Loading Internal Cache\n");
  status  = icache->Fill(67);
	printf("Internal Cache filled...\n");
  HopTyp hop[100];

  //HopTyp indx [10];
	int nres;
  //int j;
  //int dev;
	printf("Cache filled\n");
  nres  = ecache->FindinCol(&ecache->devnamfrom,devfro, 2, hop);
  int i = 1;
  cdb->followPath(ecache,icache,hop[0].h,hop,&i,1);
  nres  = ecache->FindinCol(&ecache->devnamfrom,"MUONC_M4C20A1_PAD1_0_0", 2, hop);
	i=1;
	printf("\n");
  cdb->followPath(ecache,icache,hop[0].h,hop,&i,1);
  cdb->Disconnect();

  Sleep(0x7fffffff);
  return 0;
}
