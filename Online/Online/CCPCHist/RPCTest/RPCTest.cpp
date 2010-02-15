#include "RPCRec.h"

main() 
{ 
  int rpcCBValue = 0; 
  RPCRec rpcCB("ALPC15_HIST_ADDER_shit_0/HistCommand", 1000); 
  //RPCRec rpcCB("ALPC15_HIST_shit_0/HistCommand", 1000); 

  void *req;
  RPCComm *rda;
  RPCCommRead *rrd;
  rda = new RPCComm;
  rda->c = RPCCReadAll;
  int rdlen = sizeof(RPCCommRead)+strlen("H1\nH3\n");
  rrd = (RPCCommRead*)malloc(rdlen);
  memset(rrd,0,rdlen);
  strcpy(&rrd->which,"H1\nH3\n");
  rrd->c = RPCCRead;
  while(1) 
  {
    int dat;
    dat = RPCCDirectory;
    rpcCB.setData(dat); 
    //rpcCB.setData(rda,sizeof(RPCComm));
    rpcCB.setData(rrd,rdlen);
    sleep(2); 
  } 
} 

