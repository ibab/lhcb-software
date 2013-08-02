#include "FarmStatus.h"
#include <math.h>
#include <time.h>
#include "ROMon/Utilities.h"
#include "RTL/strdef.h"
#include <algorithm>
static FILE *outf;
static bool inhibit_act;



FarmStatus::FarmStatus()
{
//  DimUpdatedInfo *sfinfo;
  DimInfo *sfinfo;
  m_InfoHandler = new StatusInfoHandler(this);
//  m_MBMInfoHandler = new MBMInfoHandler(this);
  static int NoLink=-1;
  char sf[128];
  char row;
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;
  m_low = 10;
  m_high = 20;
  m_enabledFarm.clear();
  for (row='A';row<='E';row++)
  {
    for (int rack=1;rack<=10;rack++)
    {
//      if (rack==5) continue;
      sprintf(sf,"/RO/HLT%c%02d/TaskSupervisor/Summary",row,rack);
//      sfinfo = new DimUpdatedInfo(sf,(void*)(&NoLink),sizeof(int),m_InfoHandler);
      sfinfo = new DimInfo(sf,(void*)(&NoLink),sizeof(int),m_InfoHandler);
      m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
      fprintf(outf,"%s\n",sf);
      sprintf(sf,"HLT%c%02d",row,rack);
      m_AllpFarms.insert(std::string(sf));
      for (int j=1;j<=32;j++)
      {
        sprintf(sf,"HLT%c%02d%02d",row,rack,j);
        m_AllpNodes.insert(std::string(sf));
      }
//      for (int j=12;j<=32;j++)
//      {
//        sprintf(sf,"hlt%c%02d%02d",row,rack,j);
//        m_AllpNodes.insert(std::string(sf));
//      }
    }
  }
  row = 'F';
  for (int rack=1;rack<=7;rack++)
  {
//    if (rack==5) continue;
    sprintf(sf,"/RO/HLT%c%02d/TaskSupervisor/Summary",row,rack);
//    sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_InfoHandler);
    sfinfo = new DimInfo(sf,(void*)&NoLink,sizeof(int),m_InfoHandler);
    m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
    fprintf(outf,"%s\n",sf);
    sprintf(sf,"HLT%c%02d",row,rack);
    m_AllpFarms.insert(std::string(sf));
    for (int j=1;j<=32;j++)
    {
      sprintf(sf,"HLT%c%02d%02d",row,rack,j);
      m_AllpNodes.insert(std::string(sf));
    }
  }
//  m_enabledFarm.insert(std::string("hltb01"));
//  m_enabledFarm.insert(std::string("hltb02"));
}
namespace
{
//  inline void toLowerCase(std::string &s)
//  {
//    for (unsigned int i=0;i<s.size();i++)
//    {
//      s[i] = tolower(s[i]);
//    }
//  }
  inline void toUpperCase(std::string &s)
  {
    for (unsigned int i=0;i<s.size();i++)
    {
      s[i] = toupper(s[i]);
    }
  }
}
void FarmStatus::Analyze()
{
  myActionMap Actions;
  dim_lock();
  m_servdat.erase();
  for (NodeSet::iterator nit=m_everrecvd.begin();nit != m_everrecvd.end();nit++)
  {
    char Line[1024];
    MyNode *nod;
    string nname= *nit;
    nod = (*m_AllNodes.find(nname)).second;
    if (m_recvNodes.find(nname)!= m_recvNodes.end())
    {
      sprintf(Line,"%s %d/%d/%d|",nname.c_str(),nod->m_state,nod->m_badtasks,nod->m_badconns);
    }
    else
    {
      sprintf(Line,"%s %d/%d/%d|",nname.c_str(),-1,nod->m_badtasks,nod->m_badconns);
    }
    m_servdat += Line;
  }
  m_servdat += '\0';
  m_StatusService->setData((void*)m_servdat.c_str(),m_servdat.size());
  m_StatusService->updateService();
  m_recvNodes.clear();
  m_Nodes.clear();
  dim_unlock();
}
void FarmStatus::BufferDump()
{
//  fprintf(outf,"Dump of Events Buffer Distribution at ");
//  {
//    time_t rawtime;
//    time(&rawtime);
//    fprintf(outf,"%s\n",asctime(localtime(&rawtime)));
//  }
//  fprintf(outf,"          ");
//  for (int i=1;i<=32;i++)
//  {
//    fprintf(outf,"  %02i  ",i);
//
//  }
//  std::string cfarm="";
//  std::string eline(256,' ');
//  std::string line=eline;
//  for (NodeSet::iterator nit=m_BufferrecvNodes.begin();nit != m_BufferrecvNodes.end();nit++)
//  {
//    std::string nname;
//    nname = *nit;
//    myNodeMap::iterator nodeit = m_AllNodes.find(nname);
//    MyNode *nod = (*nodeit).second;
//    if (cfarm != nod->m_subfarm)
//    {
//      if (cfarm.size()>2)
//      {
//        fprintf(outf,"\n%s",line.substr(0,line.find_last_not_of(" ")+1).c_str());
//      }
//      line = eline;
//      line.replace(0,nod->m_subfarm.size(),nod->m_subfarm);
////      fprintf(outf,"\n%s",nod->m_subfarm);
//      cfarm = nod->m_subfarm;
//    }
//    int indx;
//    sscanf(nod->m_name.substr(6,2).c_str(),"%d",&indx);
//    char nfil[10];
//    sprintf(nfil,"%4.3f",nod->Events.p_rate);
//    line.replace(9+(indx-1)*6,5,nfil);
//  }
//  fprintf(outf,"\n%s\n",line.substr(0,line.find_last_not_of(" ")+1).c_str());
//  fprintf(outf,"          ");
//  for (int i=1;i<=32;i++)
//  {
//    fprintf(outf,"  %02i  ",i);
//
//  }
//  fprintf(outf,"\n");
//  fflush(outf);
}
StatusInfoHandler::StatusInfoHandler(FarmStatus *e)
{
  m_Equalizer = e;
//      m_subfarm = sf;
}

//MBMInfoHandler::MBMInfoHandler(HLTFileEqualizer *e)
//{
//  m_Equalizer = e;
//}
//using namespace ROMon;
//typedef Nodeset::Nodes               Nodes;
//typedef Node::Buffers                Buffers;
//
//void MBMInfoHandler::infoHandler()
//{
//  int siz;
////  int status;
//  siz = this->itsService->getSize();
////      gettimeofday()
//  if (siz == sizeof(int)) return;
//  m_sfstatus = (_MBMSF *)itsService->getData();
//  const _MBMSF *ns = m_sfstatus;
//  Nodes::const_iterator n;
//    for (n=ns->nodes.begin();n!=ns->nodes.end(); n=ns->nodes.next(n))
//    {
//      std::string nname = (*n).name;
//      m_Equalizer->m_BufferrecvNodes.insert(nname);
//      myNodeMap::iterator anit;
//      anit = m_Equalizer->m_AllNodes.find(nname);
//      myNode* nod;
//      if (anit == m_Equalizer->m_AllNodes.end())
//      {
//        nod = new myNode(nname);
//        m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
//      }
//      else
//      {
//        nod = (*anit).second;
//      }
//      if (nod->ReadTime_prev == 0)
//      {
//        nod->ReadTime_prev = nod->ReadTime;
//        nod->Events_prev = nod->Events;
//        nod->Overflow_prev = nod->Overflow;
//        nod->Send_prev = nod->Send;
//        nod->ProcPerf.produced = nod->Events.produced+nod->Overflow.produced;
//        nod->ProcPerf.seen = nod->Events.seen+nod->Overflow.seen;
//      }
//      nod->ReadTime = (*n).time;
//      nod->ReadTime *= 1000;
//      nod->ReadTime += (*n).millitm;
//      const Buffers& buffs = *(*n).buffers();
//      for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))
//      {
//        const Buffers::value_type::Control& c = (*ib).ctrl;
//        std::string bnam = (*ib).name;
//        if (bnam == std::string("Events"))
//        {
////          nod->Events_prev = nod->Events;
//          nod->Events.name = bnam;
//          nod->Events.produced = c.tot_produced;
//          nod->Events.seen  = c.tot_seen;
//        }
//        else if (bnam == std::string("Overflow"))
//        {
////          nod->Overflow_prev = nod->Overflow;
//          nod->Overflow.name = bnam;
//          nod->Overflow.produced = c.tot_produced;
//          nod->Overflow.seen  = c.tot_seen;
//        }
//        else if (bnam == std::string("Send"))
//        {
////          nod->Send_prev = nod->Send;
//          nod->Send.name = bnam;
//          nod->Send.produced = c.tot_produced;
//          nod->Send.seen  = c.tot_seen;
//        }
//      }
//    }
//}

void StatusInfoHandler::infoHandler()
{
  typedef _SFSumm::Nodes _N;
  int siz;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz == sizeof(int)) return;
  m_sfstatus = (_SFSumm*)this->itsService->getData();
  const _SFSumm* stats = m_sfstatus;
  const _N& nodes = (stats->nodes);
//      _N::const_iterator i;
  for (_N::const_iterator i = nodes.begin();i!= nodes.end();i=nodes.next(i))
  {
    std::string nname = RTL::str_lower((*i).name);
    myNodeMap::iterator nit;
    myNodeMap::iterator anit;
    m_Equalizer->m_recvNodes.insert(nname);
    m_Equalizer->m_everrecvd.insert(nname);
    anit = m_Equalizer->m_AllNodes.find(nname);
    MyNode* nod;
    if (anit == m_Equalizer->m_AllNodes.end())
    {
      nod = new MyNode(nname);
      m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
    }
    else
    {
      nod = (*anit).second;
    }
    nod->m_state = (*i).state;
    nod->m_badconns = (*i).numBadConnections;
    nod->m_badtasks = (*i).numBadTasks;
    nod->m_subfarm = stats->name;
    NodeSet::iterator en = m_Equalizer->m_exclNodes.find(nod->m_name);
    nod->m_excl = (en != m_Equalizer->m_exclNodes.end());
  }
//  status =1;
  return;
}




void FarmStatus::Dump()
{
//  fprintf(outf,"Dump of File Distribution at ");
//  {
//    time_t rawtime;
//    time(&rawtime);
//    fprintf(outf,"%s\n",asctime(localtime(&rawtime)));
//  }
//  fprintf(outf,"          ");
//  for (int i=1;i<=32;i++)
//  {
//    fprintf(outf," %02i  ",i);
//
//  }
////  fprintf(outf,"\n");
//  std::string cfarm="";
//  std::string eline(256,' ');
//  std::string line=eline;
////  int maxidx = -1;
//  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
//  {
//    myNode *nod = (*nit).second;
//    if (cfarm != nod->m_subfarm)
//    {
//      if (cfarm.size()>2)
//      {
//        fprintf(outf,"\n%s",line.substr(0,line.find_last_not_of(" ")+1).c_str());
//      }
//      line = eline;
//      line.replace(0,nod->m_subfarm.size(),nod->m_subfarm);
////      fprintf(outf,"\n%s",nod->m_subfarm);
//      cfarm = nod->m_subfarm;
//    }
//    int indx;
//    sscanf(nod->m_name.substr(6,2).c_str(),"%d",&indx);
//    char nfil[10];
//    if (nod->m_ROC_state == 'Y')
//    {
//      sprintf(nfil,"%4d",nod->m_nofiles);
//      line.replace(9+(indx-1)*5,4,nfil);
//    }
//    else
//    {
//      sprintf(nfil,"%4d*",nod->m_nofiles);
//      line.replace(9+(indx-1)*5,5,nfil);
//    }
//  }
//  fprintf(outf,"\n%s\n",line.c_str());
//  fflush(outf);
};






class ExitCommand : public DimCommand
{
public:
  myNodeMap *m_nodemap;
  FarmStatus *m_equl;
  ExitCommand(const char *name, char *format, myNodeMap *nodm,FarmStatus *elz):  DimCommand(name, format)
  {
    m_nodemap = nodm;
    m_equl = elz;
  }
  void ableAll(int StateValue)
  {
  myActionMap Actions;
  myActionMap::iterator fit;
  myNodeMap::iterator nit;
  for (nit = m_nodemap->begin();nit != m_nodemap->end();nit++)
  {
    Actions[(*nit).second->m_subfarm].push_back(std::make_pair((*nit).first,StateValue));
  }
  for (fit = Actions.begin();fit!=Actions.end();fit++)
  {
    std::list<std::pair<std::string,int> >::iterator i;
    std::string sf_mesg = "";
    std::string endisSvc;
    endisSvc = (*fit).first+"_HLTDefBridge/EnDisCommand";
    for (i =(*fit).second.begin();i != (*fit).second.end();i++)
    {
      std::string svcname;
      std::string node = (*i).first;
      toUpperCase(node);
      svcname = node+"_MEPRx_01/setOverflow";
//      DimClient::sendCommand(svcname.c_str(), (*i).second);
      char cmd[1024];
//      sprintf(cmd,"dim_send_command.exe %s %d -dns %s -s -i&",svcname.c_str(),(*i).second,(*fit).first.c_str());
      sprintf(cmd,"%s %d|",svcname.c_str(),StateValue);
      sf_mesg.append(cmd);

//      ::system(cmd);
//      fprintf(outf,"\tMEPRX on Node %s (%s) value %d\n",(*i).first.c_str(),svcname.c_str(),(*i).second);
    }
    DimClient::sendCommandNB(endisSvc.c_str(),(void*)(sf_mesg.c_str()),sf_mesg.size());
//    printf("message to Subfarm %s:\n%s\n",(*fit).first.c_str(),sf_mesg.c_str());
  }
}

  void enableAll()
  {
    ableAll(1);
  }
  void disableAll()
  {
    ableAll(0);
  }
  virtual void commandHandler()
  {
//    int command = getInt();
//    switch (command)
//    {
//      case 0:
//      {
//        ableAll(0);
//        break;
//      }
//      case 1:
//      {
//        ableAll(1);
//        break;
//      }
//      case 2:
//      {
//        ableAll(1);
//        ::sleep(5);
//        ::exit(0);
//      }
//      case 3:
//      {
//        this->m_equl->Dump();
//        break;
//      }
//
//    }
  }
};

LHCb1RunStatus::LHCb1RunStatus(char *name, int nolink,FarmStatus *e) : DimInfo(name,nolink)
{
  m_state = 0;
  m_nolink = nolink;
  m_equalizer = e;
};

void LHCb1RunStatus::infoHandler()
{
#define READY 1
  int data;
  data = getInt();
  if ((m_state == 0) && (data == READY))
  {
    m_equalizer->Dump();
  }
  m_state = data;
};

ExclInfo::ExclInfo(char *name, NodeSet *nodeset) : DimInfo(name,(char*) "\0")
{
  this->m_exclNodes = nodeset;
}

void ExclInfo::infoHandler()
{
  char *input;
  input = getString();
  dyn_string *nlist = Strsplit(input,"\0");
  m_exclNodes->clear();
  for (size_t i=0;i<nlist->size();i++)
  {
    if (nlist->at(i).size()>0)
    {
      nlist->at(i) = RTL::str_lower(nlist->at(i));
      m_exclNodes->insert(nlist->at(i));
    }
  }
}

int main(int /* argc   */, char** /* argv */)
{
  char *ofile = getenv("HLTEQ_LOGF");
  if (ofile == 0)
  {
    outf = fopen("/group/online/FarmStatus.log","a+");
  }
  else
  {
    outf = fopen(ofile,"a+");

  }
  fprintf(outf,"HLTFileEqualizer starting at...");
  {
    time_t rawtime;
    time(&rawtime);
    fprintf(outf,"%s",asctime(localtime(&rawtime)));
  }

  DimClient::setDnsNode("ecs03");
  char *disact=getenv("HLTEQ_INHIBIT_ACT");
  inhibit_act = (disact != 0);
  char *dns = getenv("HLTEQ_DNSNODE");
  if (dns == 0)
  {
    DimServer::setDnsNode("ecs03");
  }
  else
  {
    DimServer::setDnsNode(dns);
  }
  FarmStatus elz;
//  HLTPerfFormatter *pform = new HLTPerfFormatter();
//  pform->Extract(elz.M_PMap);
//  delete pform;
//  nodemap::iterator nit;
//  for (nit = elz.M_PMap.begin();nit!=elz.M_PMap.end();nit++)
//  {
//    (*nit).second->Normalize(645.0);
//  }
  int m_DefState = -1;
  DimServer::start("FarmStatus");
  DimServer::autoStartOn();
//  int low,high;
//  fprintf(outf,"Low/high options %d %d\n",elz.m_low,elz.m_high);
  ExclInfo exclInfo((char*)"HLT/ExcludedNodes",&elz.m_exclNodes);
  DimInfo *defstate=new DimInfo("RunInfo/LHCb/DeferHLT",m_DefState);
  elz.m_DefStateInfo = defstate;
  ExitCommand EnableandExit("FarmStatus/EnableAndExit",(char*)"I",&elz.m_AllNodes,&elz);
  LHCb1RunStatus LHCb1runstatus((char*)"RunInfo/LHCb1/RunStatus",-1,&elz);
  elz.m_StatusService = new DimService("FarmStatus/FarmStatus", "C",(void*)"\0",1);
  fflush(outf);
  while (1)
  {
    sleep (30);
    elz.Analyze();
  }
  return 0;
}


