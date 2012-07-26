#include "HLTFileEqualizer.h"
#include <math.h>
#include <time.h>

static FILE *outf;
HLTFileEqualizer::HLTFileEqualizer()
{
  DimUpdatedInfo *sfinfo;
  m_InfoHandler = new DefHltInfoHandler(this);
  static int NoLink=-1;
  char sf[128];
  char row;
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;
  m_low = 0;
  m_high = 20;
  m_enabledFarm.clear();
  for (row='b';row<='e';row++)
  {
    for (int rack=1;rack<=11;rack++)
    {
      if (rack==5) continue;
      sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLTDefer",row,rack);
      sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_InfoHandler);
      m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
      fprintf(outf,"%s\n",sf);
    }
  }
  row = 'f';
  for (int rack=1;rack<=7;rack++)
  {
    if (rack==5) continue;
    sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLTDefer",row,rack);
    sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_InfoHandler);
    m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
    fprintf(outf,"%s\n",sf);
  }
//  m_enabledFarm.insert(std::string("hltb01"));
//  m_enabledFarm.insert(std::string("hltb02"));
}
namespace
{
  inline void toLowerCase(std::string &s)
  {
    for (unsigned int i=0;i<s.size();i++)
    {
      s[i] = tolower(s[i]);
    }
  }
  inline void toUpperCase(std::string &s)
  {
    for (unsigned int i=0;i<s.size();i++)
    {
      s[i] = toupper(s[i]);
    }
  }
}
void HLTFileEqualizer::Analyze()
{
  myActionMap Actions;
  dim_lock();
  float av_files;
  float rms;
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;

  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    m_nfiles += nod->m_nofiles;
    m_nfiles2 += nod->m_nofiles*nod->m_nofiles;
    m_nnodes++;
  }
  if (0 != m_nnodes)
  {
    av_files = float(this->m_nfiles)/this->m_nnodes;
    rms = double(m_nfiles2)/m_nnodes-double(av_files)*double(av_files);
    if (0.0<rms)
    {
      rms = sqrt(rms);
    }
    else
    {
      rms = 0.0;
    }
  }
  else
  {
    if (0 != m_nfiles)
    {
      av_files = 100000000.0;
    }
    else
    {
      av_files = 0.0;
    }
    rms = double(m_nfiles2)/m_nnodes-double(av_files)*double(av_files);
    if (0.0<rms)
    {
      rms = ::sqrt(rms);
    }
    else
    {
      rms = 0.0;
    }
  }
  fprintf(outf,"Analysis Summary based on %d nodes: ",m_nnodes);
  {
    time_t rawtime;
    time(&rawtime);
    fprintf(outf,"%s",asctime(localtime(&rawtime)));
  }
  int n_ena = 0;
  int n_dis = 0;
  int nfiles = 0;
  long nfiles2 = 0;
  int nnodes = 0;
  int tot_ena = 0;
  int tot_dis = 0;
  int ROC_tot_ena = 0;
  int ROC_tot_dis = 0;
  bool act = (m_DefStateInfo->getInt() == 1);
  fprintf(outf,"Analyzer: First round of analysis Average number of files per node: %f +/- %f\n",av_files,rms);
  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    if ((nod->m_nofiles > av_files+5.0*rms) || (nod->m_nofiles <av_files-5.0*rms))
    {
      continue;
    }
    nfiles += nod->m_nofiles;
    nfiles2 += nod->m_nofiles*nod->m_nofiles;
    nnodes++;
  }
  if (nnodes >0)
  {
    av_files = float(nfiles)/nnodes;
    rms = double(nfiles2)/nnodes - double(av_files*av_files);
    if (rms>0)
    {
      rms = ::sqrt(rms);
    }
    else
    {
      rms = 0.0;
    }
  }
  else
  {
    av_files = 0;
    rms = 0.0;
  }
  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    if (nod->m_nofiles > av_files+m_high)
    {
      if (nod->m_ROC_state == 'Y')
      {
        std::string farm;
        farm = nod->m_name.substr(0,6);
        Actions[farm].push_back(std::make_pair(nod->m_name,0));
        nod->m_state = 0;
      }
      n_dis++;
    }
    else if (nod->m_nofiles < av_files+m_low)
    {
      std::string farm;
      farm = nod->m_name.substr(0, 6);
      nod->m_state = 1;
      Actions[farm].push_back(std::make_pair(nod->m_name,1));
      n_ena++;
    }
    if (nod->m_state == 0) tot_dis++;
    else tot_ena++;
    if (nod->m_ROC_state == 'Y') ROC_tot_ena++;
    else ROC_tot_dis++;
  }
  fprintf(outf,"Analyzer: Second round (within +/- 5 sigma) of analysis Average number of files per node: %f +/- %f\n",av_files,rms);
  fprintf(outf,"%d Nodes enabled; %d Nodes disabled\n",n_ena,n_dis);
  fprintf(outf,"Total %d Nodes enabled; Total %d Nodes disabled\n",tot_ena,tot_dis);
  fprintf(outf,"From ROcollect: Total %d Nodes enabled; Total %d Nodes disabled\n",ROC_tot_ena,ROC_tot_dis);
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;
  if (act)
  {
    myActionMap::iterator fit;
    for (fit = Actions.begin(); fit != Actions.end(); fit++)
    {
      //    DimClient::setDnsNode((*fit).first.c_str());
      if (!m_enabledFarm.empty() && (m_enabledFarm.find((*fit).first)
          == m_enabledFarm.end()))
      {
        continue;
      }
      //    fprintf(outf,"On Farm %s:\n",(*fit).first.c_str());
      std::list<std::pair<std::string, int> >::iterator i;
      std::string sf_mesg = "";
      std::string endisSvc;
      endisSvc = (*fit).first + "_HLTDefBridge/EnDisCommand";
      for (i = (*fit).second.begin(); i != (*fit).second.end(); i++)
      {
        std::string svcname;
        std::string node = (*i).first;
        toUpperCase(node);
        svcname = node + "_MEPRx_01/setOverflow";
        char cmd[1024];
        sprintf(cmd, "%s %d|", svcname.c_str(), (*i).second);
        sf_mesg.append(cmd);

      }
      DimClient::sendCommandNB(endisSvc.c_str(), (char*) sf_mesg.c_str());
    }
  }
  else
  {
    fprintf(outf,"Defered HLT disabled. Not acting...\n");
  }
  fprintf(outf,"==================\n");
  fflush(outf);
  dim_unlock();
}

DefHltInfoHandler::DefHltInfoHandler(HLTFileEqualizer *e)
{
  m_Equalizer = e;
//      m_subfarm = sf;
}
void DefHltInfoHandler::infoHandler()
{
  int siz;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz == sizeof(int)) return;
  m_sfstatus = (_S*)this->itsService->getData();
  const _S* stats = m_sfstatus;
  const _N& nodes = *(stats->nodes());
//      _N::const_iterator i;
  for (_N::const_iterator i = nodes.begin();i!= nodes.end();i=nodes.next(i))
  {
    const _R& runs = (*i).runs;
    std::string nname;
    nname = (*i).name;
    myNodeMap::iterator nit;
    myNodeMap::iterator anit;
    anit = m_Equalizer->m_AllNodes.find(nname);
    if (anit == m_Equalizer->m_AllNodes.end())
    {
      myNode* nod = new myNode(nname);
      m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
    }
//    fprintf(outf,"%s: ",(*i).name);
    int nfiles=0;
    for (_R::const_iterator j = runs.begin();j!= runs.end();j=runs.next(j))
    {
      nfiles += (*j).second;
    }
//    fprintf(outf,"%d Files\n",nfiles);
    if (nfiles>0)
    {
      nit = m_Equalizer->m_Nodes.find(nname);
      if (nit == m_Equalizer->m_Nodes.end())
      {
        anit = m_Equalizer->m_AllNodes.find(nname);
        myNode* nod;
        if (anit == m_Equalizer->m_AllNodes.end())
        {
          nod = new myNode(nname);
          m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
          m_Equalizer->m_Nodes.insert(std::make_pair(nname,nod));
        }
        else
        {
          nod = (*anit).second;
          m_Equalizer->m_Nodes.insert(std::make_pair(nname,nod));
        }
      }
      m_Equalizer->m_Nodes[nname]->m_nofiles = nfiles;
      m_Equalizer->m_Nodes[nname]->m_ROC_state = (*i).overflowState;
//      m_Equalizer->m_Nodes[nname]->m_state *= (*i).overflowState;
      m_Equalizer->m_nfiles += nfiles;
      m_Equalizer->m_nfiles2 += nfiles*nfiles;
      m_Equalizer->m_nnodes++;
    }
    else
    {
      nit = m_Equalizer->m_Nodes.find(nname);
      if (nit != m_Equalizer->m_Nodes.end())
      {
//        myNode* nod;
//        nod = (*nit).second;
        m_Equalizer->m_Nodes.erase(nit);
      }
    }
  }
//  status =1;
  return;
}




void HLTFileEqualizer::Dump()
{
  fprintf(outf,"Dump of File Distribution at ");
  {
    time_t rawtime;
    time(&rawtime);
    fprintf(outf,"%s\n",asctime(localtime(&rawtime)));
  }
  fprintf(outf,"          ");
  for (int i=1;i<=32;i++)
  {
    fprintf(outf," %02i  ",i);

  }
//  fprintf(outf,"\n");
  std::string cfarm;
  std::string eline(256,' ');
  std::string line=eline;
  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    if (cfarm != nod->m_subfarm)
    {
      fprintf(outf,"\n%s",line.c_str());
      line = eline;
      line.replace(0,nod->m_subfarm.size(),nod->m_subfarm);
//      fprintf(outf,"\n%s",nod->m_subfarm);
      cfarm = nod->m_subfarm;
    }
    int indx;
    sscanf(nod->m_name.substr(6,2).c_str(),"%d",&indx);
    char nfil[10];
    sprintf(nfil,"%4d",nod->m_nofiles);
    line.replace(10+indx*5,4,nfil);
  }
  fflush(outf);
};






class ExitCommand : public DimCommand
{
public:
  myNodeMap *m_nodemap;
  HLTFileEqualizer *m_equl;
  ExitCommand(const char *name, char *format, myNodeMap *nodm,HLTFileEqualizer *elz):  DimCommand(name, format)
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
//      std::string svcname;
//      std::string node = (*nit).first;
//      toUpperCase(node);
//      svcname = node+"_MEPRx_01/setOverflow";
////      DimClient::sendCommand(svcname.c_str(), (*i).second);
//      char cmd[1024];
//      sprintf(cmd,"dim_send_command.exe %s %d -dns %s -s -i&",svcname.c_str(),1,(*nit).second->m_subfarm.c_str());
//      ::system(cmd);
  }
  for (fit = Actions.begin();fit!=Actions.end();fit++)
  {
//      if (!m_enabledFarm.empty() && (m_enabledFarm.find((*fit).first) == m_enabledFarm.end()))
//      {
//        continue;
//      }
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
//  ::sleep(5);
//  ::exit(0);
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
    int command = getInt();
    switch (command)
    {
      case 0:
      {
        ableAll(0);
        break;
      }
      case 1:
      {
        ableAll(1);
        break;
      }
      case 2:
      {
        ableAll(1);
        ::sleep(5);
        ::exit(0);
      }
      case 3:
      {
        this->m_equl->Dump();
        break;
      }

    }
  }
};

LHCb1RunStatus::LHCb1RunStatus(char *name, int nolink,HLTFileEqualizer *e) : DimInfo(name,nolink)
{
  m_nolink = nolink;
  m_equalizer = e;
};

void LHCb1RunStatus::infoHandler()
{
#define ALLOCATING 1
  int data;
  data = getInt();
  if (data == ALLOCATING)
  {
    m_equalizer->Dump();
  }
};

int main(int argc, char **argv)
{
  outf = fopen("/group/online/HLTFileEqualizer.log","a+");
  fprintf(outf,"HLTFileEqualizer starting at...");
  {
    time_t rawtime;
    time(&rawtime);
    fprintf(outf,"%s",asctime(localtime(&rawtime)));
  }

  DimClient::setDnsNode("ecs03");
  DimServer::setDnsNode("ecs03");
  HLTFileEqualizer elz;
  int m_DefState = -1;
  DimServer::start("HLTFileEqualizer");
  DimServer::autoStartOn();
  int low,high;
  if (argc == 3)
  {
    sscanf(argv[1],"%d",&low);
    sscanf(argv[2],"%d",&high);
    elz.m_low = low;
    elz.m_high = high;
  }
  DimInfo *defstate=new DimInfo("RunInfo/LHCb/DeferHLT",m_DefState);
  elz.m_DefStateInfo = defstate;
  ExitCommand EnableandExit("HLTFileEqualizer/EnableAndExit",(char*)"I",&elz.m_AllNodes,&elz);
  fflush(outf);
  while (1)
  {
    sleep (60);
    elz.Analyze();
  }
  return 0;
}


