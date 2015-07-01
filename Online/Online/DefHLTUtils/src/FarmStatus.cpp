#include "FarmStatus.h"
#include <math.h>
#include <time.h>
#include "ROMon/Utilities.h"
#include "RTL/strdef.h"
#include <algorithm>
#include "stdlib.h"
static FILE *outf;
static bool inhibit_act;



FarmStatus::FarmStatus()
{
//  DimUpdatedInfo *sfinfo;
  DimInfo *sfinfo;
  m_InfoHandler = new StatusInfoHandler(this);
  m_TSInfoHandler = new TaskSummaryInfoHandler(this);
//  m_MBMInfoHandler = new MBMInfoHandler(this);
  static int NoLink=-1;
  char sf[128];
  string srvcstring;
  char row;
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;
  m_low = 10;
  m_high = 20;
  m_enabledFarm.clear();
  string dnam = "FarmStatus/TaskDictionary";
  m_TaskDict = new Dictionary(dnam);
  dnam = "FarmStatus/ConnectionDictionary";
  m_ConnDict = new Dictionary(dnam);
  dnam = "FarmStatus/ProjectDictionary";
  m_ProjDict = new Dictionary(dnam);
  for (row='A';row<='E';row++)
  {
    for (int rack=1;rack<=10;rack++)
    {
      sprintf(sf,"HLT%c%02d",row,rack);
      srvcstring = "/RO/";
      srvcstring += sf;
      srvcstring += "/TaskSupervisor/Summary";
      sfinfo = new DimInfo(srvcstring.c_str(),(void*)(&NoLink),sizeof(int),m_InfoHandler);
      m_infoMap.insert(make_pair(srvcstring,sfinfo));
      srvcstring = "/RO/";
      srvcstring += sf;
      srvcstring += "/TaskSupervisor/Status";
      sfinfo = new DimInfo(srvcstring.c_str(),(void*)(&NoLink),sizeof(int),m_TSInfoHandler);
      m_TaskMap.insert(make_pair(srvcstring,sfinfo));
      m_DimMap.insert( make_pair(sfinfo    ,string(sf)));
      fprintf(outf,"%s\n",sf);
      m_AllpFarms.insert(string(sf));
    }
  }
  row = 'F';
  for (int rack=1;rack<=12;rack++)
  {
    sprintf(sf,"HLT%c%02d",row,rack);
    srvcstring = "/RO/";
    srvcstring += sf;
    srvcstring += "/TaskSupervisor/Summary";
    sfinfo = new DimInfo(srvcstring.c_str(),(void*)(&NoLink),sizeof(int),m_InfoHandler);
    m_infoMap.insert(make_pair(srvcstring,sfinfo));
    srvcstring = "/RO/";
    srvcstring += sf;
    srvcstring += "/TaskSupervisor/Status";
    sfinfo = new DimInfo(srvcstring.c_str(),(void*)(&NoLink),sizeof(int),m_TSInfoHandler);
    m_TaskMap.insert(make_pair(srvcstring,sfinfo));
    m_DimMap.insert( make_pair(sfinfo    ,string(sf)));
    fprintf(outf,"%s\n",sf);
    m_AllpFarms.insert(string(sf));
  }
  string otherNodes[]={"ECS03","ECS04","MONA08","MONA09","STORECTL01","CALD07"};
  int nothers = sizeof(otherNodes)/sizeof(string*);
  for (int N=0;N<nothers;N++)
  {
    srvcstring = "/RO/";
    srvcstring += otherNodes[N]+"/TaskSupervisor/Summary";
    sfinfo = new DimInfo(srvcstring.c_str(),(void*)(&NoLink),sizeof(int),m_InfoHandler);
    m_infoMap.insert(make_pair(srvcstring,sfinfo));
    srvcstring = "/RO/";
    srvcstring += otherNodes[N]+"/TaskSupervisor/Status";
    sfinfo = new DimInfo(srvcstring.c_str(),(void*)(&NoLink),sizeof(int),m_TSInfoHandler);
    m_TaskMap.insert(make_pair(srvcstring,sfinfo));
    m_DimMap.insert( make_pair(sfinfo    ,string(sf)));
    fprintf(outf,"%s\n",otherNodes[N].c_str());
    m_AllpFarms.insert(otherNodes[N]);
  }
}
namespace
{
//  inline void toLowerCase(string &s)
//  {
//    for (unsigned int i=0;i<s.size();i++)
//    {
//      s[i] = tolower(s[i]);
//    }
//  }
  inline void toUpperCase(string &s)
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
  osStatus.str("");
  osDetails.str("");
  m_ConnDict->Update();
  m_TaskDict->Update();
  m_ProjDict->Update();
  for (NodeSet::iterator nit=m_everrecvd.begin();nit != m_everrecvd.end();nit++)
  {
    char Line[1024];
    MyNode *nod;
    string nname= *nit;
    nod = (*m_AllNodes.find(nname)).second;
    vector<int>  badIndices[3];
    badIndices[0].clear();
    badIndices[1].clear();
    badIndices[2].clear();
    int badies = 0;
    if (m_recvNodes.find(nname)!= m_recvNodes.end())
    {
      sprintf(Line,"%s %d/%d/%d|",nname.c_str(),nod->m_state,nod->m_badtasks,nod->m_badconns);
      osStatus << nname << " "<< nod->m_state << "/" << nod->m_badtasks << "/" << nod->m_badconns << "|";
      Cluster *clu = this->m_TSInfoHandler->m_CluMap[nod->m_name];
      Cluster::Node &n = clu->nodes[nod->m_name];
      if (nod->m_badtasks+nod->m_badconns+n.projects.size() > 0)
      {
        if (nod->m_badconns != 0)
        {
          for (size_t k=0;k<n.conns.size();k++)
          {
            if (!n.conns[k].second)
            {
              badIndices[1].push_back(m_ConnDict->Find(n.conns[k].first));
              badies++;
            }
          }
        }
        if (nod->m_badtasks != 0)
        {
          for (size_t k=0;k<n.tasks.size();k++)
          {
            if (!n.tasks[k].second)
            {
              badIndices[0].push_back(m_TaskDict->Find(n.tasks[k].first));
              badies++;
            }
          }
        }
        for (size_t k=0;k<n.projects.size();k++)
        {
          if (!n.projects[k].dataMgr)
          {
            badIndices[2].push_back(m_ProjDict->Find(n.projects[k].name));
            badies++;
          }
        }
        if (badies>0)
        {
          if (osDetails.str().size()>0)
          {
            osDetails << "|"<<n.name<<" ";
          }
          else
          {
            osDetails <<n.name<<" ";
          }
          char nums[32];
          for (size_t k = 0;k<3;k++)
          {
            if (badIndices[k].size() > 0)
            {
              for (size_t l=0;l<badIndices[k].size();l++)
              {
                if (l == 0)
                {
                  osDetails <<badIndices[k][l];
                  sprintf(nums,"%d",badIndices[k][l]);
                }
                else
                {
                  osDetails <<"," << badIndices[k][l];
                  sprintf(nums,",%d",badIndices[k][l]);
                }
              }
            }
            if (k!=2)
            {
              osDetails <<"/";
            }
          }
        }
      }
    }
    else
    {
      osStatus << nname << " -1/"<< nod->m_badtasks << "/" << nod->m_badconns << "|";
    }
  }
  osStatus << '\0';
  osDetails << '\0';
  m_StatusService->setData((void*)osStatus.str().c_str(),osStatus.str().size());
  m_StatusService->updateService();
  m_recvNodes.clear();
  m_Nodes.clear();
  m_DetailService->setData((void*)osDetails.str().c_str(),osDetails.str().size());
  m_DetailService->updateService();
  dim_unlock();
}
StatusInfoHandler::StatusInfoHandler(FarmStatus *e):m_subfarm(0),m_sfstatus(0),m_bufsiz(0)
{
  m_Equalizer = e;
//      m_subfarm = sf;
}


void StatusInfoHandler::infoHandler()
{
  typedef _SFSumm::Nodes _N;
  size_t siz;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz <= sizeof(int)) return;
  m_sfstatus = (_SFSumm*)this->itsService->getData();
  const _SFSumm* stats = m_sfstatus;
  const _N& nodes = (stats->nodes);
//      _N::const_iterator i;
  for (_N::const_iterator i = nodes.begin();i!= nodes.end();i=nodes.next(i))
  {
    string nname = RTL::str_lower((*i).name);
    myNodeMap::iterator nit;
    myNodeMap::iterator anit;
    m_Equalizer->m_recvNodes.insert(nname);
    m_Equalizer->m_everrecvd.insert(nname);
    anit = m_Equalizer->m_AllNodes.find(nname);
    MyNode* nod;
    if (anit == m_Equalizer->m_AllNodes.end())
    {
      nod = new MyNode(nname);
      m_Equalizer->m_AllNodes.insert(make_pair(nname,nod));
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

TaskSummaryInfoHandler::TaskSummaryInfoHandler(FarmStatus *e)
{
  this->m_bufsiz = 0;
  this->m_Equalizer = e;
}

void TaskSummaryInfoHandler::infoHandler()
{
  size_t siz;
  XML::TaskSupervisorParser parser;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz <= sizeof(int)) return;
  this->itsService->getName();
  void *ptr = this->itsService->getData();
  DimInfo *svc = this->getInfo();
  map<DimInfo*,string>::iterator i =m_Equalizer->m_DimMap.find(svc);
  Cluster *clu = 0;
  if (i != m_Equalizer->m_DimMap.end())
  {
    map<DimInfo*,Cluster*>::iterator ii;
    ii = this->m_ClusterMap.find(svc);
    if (ii == m_ClusterMap.end())
    {
      clu = new Cluster();
      m_ClusterMap.insert(make_pair(svc,clu));
    }
    else
    {
      clu = (*ii).second;
    }
    parser.parseBuffer((*i).second,ptr,siz);
    parser.getClusterNodes(*clu);
//    parser.getInventory(*inv);
  }
  Cluster::Nodes::iterator ni;
  for (ni=clu->nodes.begin();ni!=clu->nodes.end();ni++)
  {
    Cluster::Node &n=(*ni).second;
    m_CluMap[n.name] = clu;
    for (size_t j=0;j<n.connCount;j++)
    {
      m_Equalizer->m_ConnDict->Add(n.conns[j].first);
    }
    for (size_t j=0;j<n.taskCount;j++)
    {
      m_Equalizer->m_TaskDict->Add(n.tasks[j].first);
    }
    for (size_t j=0;j<n.projects.size();j++)
    {
      m_Equalizer->m_ProjDict->Add(n.projects[j].name);
    }
  }
//  status =1;
  return;
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
  DimServer::start("FarmStatus");
  DimServer::autoStartOn();
  elz.m_StatusService = new DimService("FarmStatus/FarmStatus", "C",(void*)"\0",1);
  elz.m_DetailService = new DimService("FarmStatus/FarmStatusDetails", "C",(void*)"\0",1);
  fflush(outf);
  while (1)
  {
    sleep (30);
    elz.Analyze();
  }
  return 0;
}


