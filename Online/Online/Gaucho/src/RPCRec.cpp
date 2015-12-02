#include "Gaucho/RPCRec.h"
#include "Gaucho/ObjRPC.h"
#include "Gaucho/MonTypes.h"
#include "stdio.h"
#include "Gaucho/dimhist.h"
#include "Gaucho/RPCdefs.h"
#include "Gaucho/Utilities.h"
int RPCRec::analyseReply()
{
  void *valin;
  RPCReply *rep;
  int siz;
  void* valend;
  siz = getSize();
  valin = getData();
  valend = AddPtr(valin,siz);
  rep = (RPCReply*)valin;
  m_reply = RPCCIllegal;
  if (rep->status == -1)
  {
//    printf ("Timeout...\n");
    return 1;
  }
  if (rep->status == -2)
  {
//    printf ("RPC Error...\n");
    return 2;
  }
  m_reply = rep->comm;
  switch (rep->comm)
  {
    case RPCCIllegal:
    {
      return 3;
    }
    case RPCCRead:
    case RPCCReadAll:
    case RPCCClear:
    case RPCCClearAll:
    {
      valend = (void*)((char *)valin+siz);
//      printf("Received answer of size %d\n",siz);
      DimHistbuff1 *p1;
      hists->clear();
      p1 = (DimHistbuff1*)(AddPtr(valin,sizeof(RPCReply)));
      while (p1<valend)
      {
        int namoff;
        char *nam;
        if ((p1->type == H_1DIM) || (p1->type == H_PROFILE))
        {
          namoff = p1->nameoff;//sizeof(DimHistbuff1);
        }
        else
        {
          namoff = p1->nameoff;//sizeof(DimHistbuff2);
        }
        nam = (char*)AddPtr(p1,namoff);
        hists->insert(std::pair<char*,void*> (nam,p1));
  //          printf ("Type %d record Length %d dimension %d hdrlength %d\n title %s\n",
  //            p1->type,p1->reclen,p1->dim,p1->dataoff,(char*)((char*)p1+titoff));
        p1 = (DimHistbuff1*)((char*)p1+p1->reclen);
      }
      break;
    }
    case RPCCDirectory:
    {
//      printf("Received answer of size %d\n",siz);
      void *p1 = (AddPtr(valin,sizeof(RPCReply)));
      int recl;
      int cumul=sizeof(RPCReply);
      names->clear();
      while (p1<valend)
      {
        int titoff;
        titoff = 4;
        char *tptr = (char*)AddPtr(p1,titoff);
        names->push_back(tptr);
  //          printf ("Type %d name %s\n",*(int*)p1,tptr);
        recl = 4+strlen(tptr)+1;
        cumul += recl;
  //          printf("P1 %lli Valend %lli Record Length %i cumulative length %i\n", (long)p1, (long)valend, recl,cumul);
        p1 = AddPtr(p1,recl);
      }
      break;
    }
  }
//    cout << "Callback RPC Received : " << valin << endl;
  return 0;
}
int RPCRec::analyseReply(NAMEVEC *v)
{
  int stat = analyseReply();
  if (stat == 0)
  {
    *v = *names;
  }
  return stat;
}
int RPCRec::analyseReply(PTRMAP *p)
{
  int stat = analyseReply();
  if (stat == 0)
  {
    *p = *hists;
  }
  return stat;

}
int RPCRec::analyseReply(STRVEC *v)
{
  NAMEVEC::iterator i;
  int stat = analyseReply();
  if (stat == 0)
  {
    v->clear();
    for (i=names->begin();i!=names->end();i++)
    {
      v->push_back(new std::string(*i));
    }
  }
  return stat;
//      printf("Total Number of Histograms: %d\n",n_hist);

}

void RPCRec::rpcInfoHandler()
{
  if (m_synch) return;
  int stat = analyseReply();
  if (stat != 0) return;
  switch (m_reply)
  {
    case RPCCIllegal:
    {
      return;
    }
    case RPCCRead:
    case RPCCReadAll:
    case RPCCClear:
    case RPCCClearAll:
    {
      if (DatCB != 0)
      {
        DatCB(hists);
      }
      break;
    }
    case RPCCDirectory:
    {
      if (DirCB != 0)
      {
        DirCB(names);
      }
      break;
    }
  }
  return;
  }

RPCRec::RPCRec(char *name, int timeout, bool synch) : DimRpcInfo(name, timeout, -1)
{
  DirCB = 0;
  DatCB = 0;
  names = new NAMEVEC;
  names->clear();
  hists = new PTRMAP;
  hists->clear();
  m_synch = synch;
}
RPCRec::~RPCRec()
{
  deletePtr(names);
  deletePtr(hists);
}
