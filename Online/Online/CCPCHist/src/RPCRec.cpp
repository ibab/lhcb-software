#include "RPCRec.h"
#include "HistRPC.h"
#include "hist_types.h"
#include "stdio.h"
void RPCRec::rpcInfoHandler() { 
    void *valin; 
    RPCReply *rep;
    int siz;
    void* valend;
    siz = getSize();
    valin = getData();
    valend = AddPtr(valin,siz);
    rep = (RPCReply*)valin;
    if (rep->status == -1)
    {
      printf ("Timeout...\n");
      return;
    }
    if (rep->status == -2)
    {
      printf ("RPC Error...\n");
      return;
    }
    switch (rep->comm)
    {
    case RPCCIllegal:
      {
        break;
      }
    case RPCCRead:
    case RPCCReadAll:
    case RPCCClear:
    case RPCCClearAll:
      {
        valend = (void*)((char *)valin+siz);
        printf("Received answer of size %d\n",siz);
        DimHistbuff1 *p1;
        p1 = (DimHistbuff1*)(AddPtr(valin,sizeof(RPCReply)));
        while (p1<valend)
        {
          int titoff;
          if ((p1->type == (int)H_1DIM) || (p1->type == (int)H_PROFILE))
          {
            titoff = p1->titoff;//sizeof(DimHistbuff1);
          }
          else
          {
            titoff = p1->titoff;//sizeof(DimHistbuff2);
          }
          printf ("Type %d record Length %d dimension %d hdrlength %d\n title %s\n",
            p1->type,p1->reclen,p1->dim,p1->dataoff,(char*)((char*)p1+titoff));
          p1 = (DimHistbuff1*)((char*)p1+p1->reclen);
        }
        break;
      }
    case RPCCDirectory:
      {
        void *p1 = (AddPtr(valin,sizeof(RPCReply)));
        while (p1<valend)
        {
          int titoff;
          titoff = 4;
          char *tptr = (char*)AddPtr(p1,titoff);
          printf ("Type %d name %s\n",*(int*)p1,tptr);
          p1 = AddPtr(p1,4+strlen(tptr)+1);
        }
        
        break;
      }
    }
//    cout << "Callback RPC Received : " << valin << endl; 
  } 
  RPCRec::RPCRec(char *name, int timeout) : DimRpcInfo(name, timeout, -1) 
  {
  }; 

