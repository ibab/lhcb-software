#include "ROMon/HLTFileEqualizer.h"

HLTFileEqualizer::HLTFileEqualizer()
{
  DimUpdatedInfo *sfinfo;
  m_InfoHandler = new DefHltInfoHandler(this);
  static int NoLink=-1;
  char sf[128];
  char row;
  m_nnodes = 0;
  m_nfiles = 0;
  m_enabledFarm.clear();
  for (row='b';row<='e';row++)
  {
    for (int rack=1;rack<=11;rack++)
    {
      if (rack==5) continue;
      sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLTDefer",row,rack);
      sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_InfoHandler);
      m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
      printf ("%s\n",sf);
    }
  }
  row = 'f';
  for (int rack=1;rack<=7;rack++)
  {
    if (rack==5) continue;
    sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLTDefer",row,rack);
    sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_InfoHandler);
    m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
    printf ("%s\n",sf);
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
  if (0 != m_nnodes)
  {
    av_files = float(this->m_nfiles)/this->m_nnodes;
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
  }
  printf("Analyzer: Average number of files per node: %f\n",av_files);
  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    if (nod->m_nofiles > av_files+20)
    {
      if (nod->m_state == 1)
      {
        std::string farm;
        farm = nod->m_name.substr(0,6);
        Actions[farm].push_back(std::make_pair(nod->m_name,0));
//        printf("Would DISable deferring on node %s. Av. number of files %f. Node file count %d\n",nod->m_name.c_str(),av_files,nod->m_nofiles);
        nod->m_state = 0;
      }
    }
    else if (nod->m_nofiles < av_files-5)
    {
      std::string farm;
      farm = nod->m_name.substr(0, 6);
//      printf(
//          "Would ENable deferring on node %s. Av. number of files %f. Node file count %d\n",
//          nod->m_name.c_str(), av_files, nod->m_nofiles);
      nod->m_state = 1;
      Actions[farm].push_back(std::make_pair(nod->m_name,1));
    }
  }
  m_nnodes = 0;
  m_nfiles = 0;
  char *olddns = DimClient::getDnsNode();
  printf("Analysis Summary:\n");
  myActionMap::iterator fit;
  for (fit = Actions.begin();fit!=Actions.end();fit++)
  {
//    DimClient::setDnsNode((*fit).first.c_str());
    if (!m_enabledFarm.empty() && (m_enabledFarm.find((*fit).first) == m_enabledFarm.end()))
    {
      continue;
    }
    printf("On Farm %s:\n",(*fit).first.c_str());
    std::list<std::pair<std::string,int> >::iterator i;
    for (i =(*fit).second.begin();i != (*fit).second.end();i++)
    {
      std::string svcname;
      std::string node = (*i).first;
      toUpperCase(node);
      svcname = node+"_MEPRx_01/setOverflow";
//      DimClient::sendCommand(svcname.c_str(), (*i).second);
      char cmd[1024];
      sprintf(cmd,"dim_send_command.exe %s %d -dns %s -s -i&",svcname.c_str(),(*i).second,(*fit).first.c_str());
      ::system(cmd);
      printf("\tMEPRX on Node %s (%s) value %d\n",(*i).first.c_str(),svcname.c_str(),(*i).second);
    }
  }
//  DimClient::setDnsNode(olddns);
  printf ("==================\n");
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
  int status;
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
//    printf("%s: ",(*i).name);
    int nfiles=0;
    for (_R::const_iterator j = runs.begin();j!= runs.end();j=runs.next(j))
    {
      nfiles += (*j).second;
    }
//    printf("%d Files\n",nfiles);
    myNodeMap::iterator nit;
    if (nfiles>0)
    {
      nit = m_Equalizer->m_Nodes.find(nname);
      if (nit == m_Equalizer->m_Nodes.end())
      {
        myNode* nod = new myNode(nname);
        m_Equalizer->m_Nodes.insert(std::make_pair(nname,nod));
      }
      m_Equalizer->m_Nodes[nname]->m_nofiles = nfiles;
      m_Equalizer->m_nfiles += nfiles;
      m_Equalizer->m_nnodes++;
    }
  }
  status =1;
  return;
}


int main(int argc, char **argv)
{
  HLTFileEqualizer elz;
  int m_DefState = 0;
  DimInfo defstate("RunInfo/LHCb/DeferHLT",m_DefState);
  while (1)
  {
    sleep (60);
    if (m_DefState != 1)
    {
      elz.Analyze();
    }
  }
  return 0;
}
