#include "HLTFileEqualizer.h"
#include <math.h>
#include <time.h>
#include "ROMon/Utilities.h"
#include "RTL/strdef.h"

static FILE *outf;
static bool inhibit_act;



HLTFileEqualizer::HLTFileEqualizer()
{
  DimUpdatedInfo *sfinfo;
  m_DefHandler = new DefHltInfoHandler(this);
  m_HLT1Handler = new HLT1InfoHandler(this);
  m_MBMInfoHandler = new MBMInfoHandler(this);
  static int NoLink=-1;
  char sf[128];
  char row;
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;
  m_low = 10;
  m_high = 20;
  m_enabledFarm.clear();
  m_DefNodeList = 0;
  m_DefNodeListDiff = 0;
  for (row='a';row<='e';row++)
  {
    for (int rack=1;rack<=10;rack++)
    {
//      if (rack==5) continue;
      sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLTDefer",row,rack);
      sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_DefHandler);
      m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
      sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLT1",row,rack);
      sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_HLT1Handler);
      m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
      fprintf(outf,"%s\n",sf);
      sprintf(sf,"/RO/hlt%c%02d/ROpublish",row,rack);
      sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_MBMInfoHandler);
      m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
      fprintf(outf,"%s\n",sf);
      sprintf(sf,"hlt%c%02d",row,rack);
      m_AllpFarms.insert(std::string(sf));
      for (int j=1;j<=32;j++)
      {
        sprintf(sf,"hlt%c%02d%02d",row,rack,j);
        m_AllpNodes.insert(std::string(sf));
      }
//      for (int j=12;j<=32;j++)
//      {
//        sprintf(sf,"hlt%c%02d%02d",row,rack,j);
//        m_AllpNodes.insert(std::string(sf));
//      }
    }
  }
  row = 'f';
  for (int rack=1;rack<=12;rack++)
  {
//    if (rack==5) continue;
    sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLTDefer",row,rack);
    sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_DefHandler);
    m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
    sprintf(sf,"/RO/hlt%c%02d/ROpublish/HLT1",row,rack);
    sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_HLT1Handler);
    m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
    fprintf(outf,"%s\n",sf);
    sprintf(sf,"/RO/hlt%c%02d/ROpublish",row,rack);
    sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_MBMInfoHandler);
    m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
    fprintf(outf,"%s\n",sf);
  }
  sprintf(sf,"/RO/storectl01/ROpublish");
  sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_MBMInfoHandler);
  m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
  fprintf(outf,"%s\n",sf);
  sprintf(sf,"/RO/storectl01/ROpublish/HLT1");
  sfinfo = new DimUpdatedInfo(sf,(void*)&NoLink,sizeof(int),m_MBMInfoHandler);
  m_infoMap.insert(std::make_pair(std::string(sf),sfinfo));
  fprintf(outf,"%s\n",sf);
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
void HLTFileEqualizer::FillMBMBufferOccs(char *Line, std::string &nodn,PartMBMs &part)
{
  int lpos = 0;
  lpos += sprintf(&Line[lpos],"%s ",nodn.c_str());
  if (part.Events.name != "undefined")
  {
    lpos += sprintf(&Line[lpos],"%s/%d/%d/%d/%d,",
        part.Events.name.c_str(),part.Events.spused,part.Events.sptot,part.Events.evused,part.Events.evtot);
  }
  if (part.Send.name != "undefined")
  {
    lpos += sprintf(&Line[lpos],"%s/%d/%d/%d/%d",
        part.Send.name.c_str(),part.Send.spused,part.Send.sptot,part.Send.evused,part.Send.evtot);
  }
  Line[lpos]= '|';lpos++;
  Line[lpos] = 0;
}
void HLTFileEqualizer::FillMBMBufferRates(char *Line,std::string &nodn,PartMBMs &part)
{
  int lpos = 0;
  lpos += sprintf(&Line[lpos],"%s ",nodn.c_str());
//  if (part.Events.name != "undefined")
  {
    lpos += sprintf(&Line[lpos],"%s/%d/%d/%0.4f/%0.4f,",
        part.Events.name.c_str(),part.Events.produced,part.Events.seen,
        part.Events.p_rate,part.Events.s_rate);
  }
//  if (part.Overflow.name != "undefined")
  {
    lpos += sprintf(&Line[lpos],"%s/%d/%d/%0.4f/%0.4f,",
        part.Overflow.name.c_str(),part.Overflow.produced,part.Overflow.seen,
        part.Overflow.p_rate,part.Overflow.s_rate);
  }
//  if (part.Send.name != "undefined")
  {
    lpos += sprintf(&Line[lpos],"%s/%d/%d/%0.4f/%0.4f,",
        part.Send.name.c_str(),part.Send.produced,part.Send.seen,
        part.Send.p_rate,part.Send.s_rate);
  }
//  if (part.ProcPerf.name != "undefined")
  {
    lpos += sprintf(&Line[lpos],"%s/%d/%d/%0.4f/%0.4f,",
        part.ProcPerf.name.c_str(),part.ProcPerf.produced,part.ProcPerf.seen,
        part.ProcPerf.p_rate,part.ProcPerf.s_rate);
  }
  Line[lpos]= '|';lpos++;
  Line[lpos] = 0;
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
    if (nod->m_DefQ.m_excl)
    {
      continue;
    }
    float nfiles=nod->m_DefQ.m_nofiles;
    nodemap::iterator pit;
    if (M_PMap.find(nod->m_name) == M_PMap.end())
    {
      nod->m_DefQ.m_cfiles = nfiles;
      continue;
    }
    float fact = M_PMap[nod->m_name]->m_scalefactor;
    nfiles = nfiles/(fact==0.0?1.0:fact);
    nod->m_DefQ.m_cfiles = nfiles;
    m_nfiles += nfiles;
    m_nfiles2 += nfiles*nfiles;//nod->m_nofiles*nod->m_nofiles;
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
    if ((nod->m_DefQ.m_cfiles > av_files+2.0*rms) || (nod->m_DefQ.m_cfiles <av_files-2.0*rms))
    {
      continue;
    }
    if (nod->m_DefQ.m_excl)
    {
      continue;
    }
    nfiles += nod->m_DefQ.m_cfiles;
    nfiles2 += nod->m_DefQ.m_cfiles*nod->m_DefQ.m_cfiles;
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
    if (nod->m_DefQ.m_excl)
    {
      continue;
    }
    if (nod->m_DefQ.m_cfiles > av_files+m_high)
    {
      if (nod->m_DefQ.m_ROC_state == 'Y')
      {
        std::string farm;
        farm = nod->m_name.substr(0,6);
        Actions[farm].push_back(std::make_pair(nod->m_name,0));
        nod->m_DefQ.m_state = 0;
      }
      n_dis++;
    }
    else if (nod->m_DefQ.m_cfiles < av_files+m_low)
    {
      std::string farm;
      farm = nod->m_name.substr(0, 6);
      nod->m_DefQ.m_state = 1;
      Actions[farm].push_back(std::make_pair(nod->m_name,1));
      n_ena++;
    }
    if (nod->m_DefQ.m_state == 0) tot_dis++;
    else tot_ena++;
    if (nod->m_DefQ.m_ROC_state == 'Y') ROC_tot_ena++;
    else ROC_tot_dis++;
  }
  fprintf(outf,"Analyzer: Second round (within +/- 5 sigma) of analysis Average number of files per node: %f +/- %f\n",av_files,rms);
  fprintf(outf,"%d Nodes enabled; %d Nodes disabled\n",n_ena,n_dis);
  fprintf(outf,"Total %d Nodes enabled; Total %d Nodes disabled\n",tot_ena,tot_dis);
  fprintf(outf,"From ROcollect: Total %d Nodes enabled; Total %d Nodes disabled\n",ROC_tot_ena,ROC_tot_dis);
  m_nnodes = 0;
  m_nfiles = 0;
  m_nfiles2 = 0;
  if (act && !inhibit_act)
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
  m_Defservdat.erase();
  for (myNodeMap::iterator nit=m_AllNodes.begin();nit != m_AllNodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    char nfile[32];
    sprintf(nfile,"%s %d|",nod->m_name.c_str(),nod->m_DefQ.m_nofiles);
    m_Defservdat += nfile;
  }
  m_Defservdat += '\0';
  m_DefNodeList->setData((void*)m_Defservdat.c_str(),m_Defservdat.size());
  m_DefNodeList->updateService();
  m_DefservdatDiff.erase();
  m_DefservdatNodesRunsFiles.erase();
  for (NodeSet::iterator nit=m_recvNodes.begin();nit != m_recvNodes.end();nit++)
  {
    std::string nname;
    nname = *nit;
    myNodeMap::iterator nodeit = m_AllNodes.find(nname);
    myNode *nod = (*nodeit).second;
    char nfile[32];
    sprintf(nfile,"%s %d|",nod->m_name.c_str(),nod->m_DefQ.m_nofiles);
    m_DefservdatDiff += nfile;
    if (nod->m_DefQ.m_nofiles >0)
    {
      if (nod->m_DefQ.m_ROC_state == 'Y')
      {
        m_DefservdatNodesRunsFiles += nod->m_name+" 1 ";
      }
      else
      {
        m_DefservdatNodesRunsFiles += nod->m_name+" 0 ";
      }
      RunMap::iterator k;
      for (k = nod->m_DefQ.m_runmap.begin();k!=nod->m_DefQ.m_runmap.end();k++)
      {
        sprintf(nfile,"%d/%d,",(*k).first,(*k).second);
        m_DefservdatNodesRunsFiles += nfile;
      }
      m_DefservdatNodesRunsFiles += "|";
    }
    else
    {
      if (nod->m_DefQ.m_ROC_state == 'Y')
      {
        m_DefservdatNodesRunsFiles += nod->m_name+" 1 0/0,|";
      }
      else
      {
        m_DefservdatNodesRunsFiles += nod->m_name+" 0 0/0,|";
      }
    }
  }
  m_DefservdatNodesRunsFiles += '\0';
  m_DefservdatDiff += '\0';
  m_DefNodeListDiff->setData((void*)m_DefservdatDiff.c_str(),m_DefservdatDiff.size());
  m_DefNodeListDiff->updateService();
  m_DefNodesRunsFiles->setData((void*)m_DefservdatNodesRunsFiles.c_str(),m_DefservdatNodesRunsFiles.size());
  m_DefNodesRunsFiles->updateService();



  m_HLT1servdat.erase();
  for (myNodeMap::iterator nit=m_AllNodes.begin();nit != m_AllNodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    char nfile[32];
    sprintf(nfile,"%s %d|",nod->m_name.c_str(),nod->m_DefQ.m_nofiles);
    m_HLT1servdat += nfile;
  }
  m_HLT1servdat += '\0';
  m_HLT1NodeList->setData((void*)m_HLT1servdat.c_str(),m_HLT1servdat.size());
  m_HLT1NodeList->updateService();
  m_HLT1servdatDiff.erase();
  m_HLT1servdatNodesRunsFiles.erase();
  for (NodeSet::iterator nit=m_recvNodes.begin();nit != m_recvNodes.end();nit++)
  {
    std::string nname;
    nname = *nit;
    myNodeMap::iterator nodeit = m_AllNodes.find(nname);
    myNode *nod = (*nodeit).second;
    char nfile[32];
    sprintf(nfile,"%s %d|",nod->m_name.c_str(),nod->m_HLT1Q.m_nofiles);
    m_HLT1servdatDiff += nfile;
    if (nod->m_HLT1Q.m_nofiles >0)
    {
      if (nod->m_HLT1Q.m_ROC_state == 'Y')
      {
        m_HLT1servdatNodesRunsFiles += nod->m_name+" 1 ";
      }
      else
      {
        m_HLT1servdatNodesRunsFiles += nod->m_name+" 0 ";
      }
      RunMap::iterator k;
      for (k = nod->m_HLT1Q.m_runmap.begin();k!=nod->m_HLT1Q.m_runmap.end();k++)
      {
        sprintf(nfile,"%d/%d,",(*k).first,(*k).second);
        m_HLT1servdatNodesRunsFiles += nfile;
      }
      m_HLT1servdatNodesRunsFiles += "|";
    }
    else
    {
      if (nod->m_HLT1Q.m_ROC_state == 'Y')
      {
        m_HLT1servdatNodesRunsFiles += nod->m_name+" 1 0/0,|";
      }
      else
      {
        m_HLT1servdatNodesRunsFiles += nod->m_name+" 0 0/0,|";
      }
    }
  }
  m_HLT1servdatNodesRunsFiles += '\0';
  m_HLT1servdatDiff += '\0';
  m_HLT1NodeListDiff->setData((void*)m_HLT1servdatDiff.c_str(),m_HLT1servdatDiff.size());
  m_HLT1NodeListDiff->updateService();
  m_HLT1NodesRunsFiles->setData((void*)m_HLT1servdatNodesRunsFiles.c_str(),m_HLT1servdatNodesRunsFiles.size());
  m_HLT1NodesRunsFiles->updateService();




  float stat[2];
  stat[0] = av_files;
  stat[1] = rms;
  m_DefStatServ->setData(stat,sizeof(stat));
  m_DefStatServ->updateService();
  m_recvNodes.clear();

  m_DefservdatNodesBuffersEvents.erase();
  m_DefservdatNodesBuffersEvents_LHCb2.erase();
  for (NodeSet::iterator nit=m_BufferrecvNodes.begin();nit != m_BufferrecvNodes.end();nit++)
  {
    std::string nname;
    nname = *nit;
    myNodeMap::iterator nodeit = m_AllNodes.find(nname);
    myNode *nod = (*nodeit).second;
    char Line[2048];
    long dtime = nod->m_DefQ.ReadTime-nod->m_DefQ.ReadTime_prev;
    nod->AnyPart.ProcPerf.name = "Input";
    nod->AnyPart.ProcPerf.produced = nod->AnyPart.Events.produced+nod->AnyPart.Overflow.produced;
    nod->AnyPart.ProcPerf.seen = nod->AnyPart.Events.seen+nod->AnyPart.Overflow.seen;
    nod->AnyPart.Events.calcRate(nod->AnyPart.Events_prev,dtime);
    nod->AnyPart.Overflow.calcRate(nod->AnyPart.Overflow_prev,dtime);
    nod->AnyPart.Send.calcRate(nod->AnyPart.Send_prev,dtime);
    nod->AnyPart.ProcPerf.calcRate(nod->AnyPart.ProcPerf_prev,dtime);
    FillMBMBufferRates(Line,nod->m_name,nod->AnyPart);
//    sprintf(Line,"%s %s/%d/%d/%0.4f/%0.4f,%s/%d/%d/%0.4f/%0.4f,%s/%d/%d/%0.4f/%0.4f,%s/%d/%d/%0.4f/%0.4f|",
//        nod->m_name.c_str(),
//        nod->AnyPart.Events.name.c_str(),nod->AnyPart.Events.produced,nod->AnyPart.Events.seen,nod->AnyPart.Events.p_rate,nod->AnyPart.Events.s_rate,
//        nod->AnyPart.Overflow.name.c_str(),nod->AnyPart.Overflow.produced,nod->AnyPart.Overflow.seen,nod->AnyPart.Overflow.p_rate,nod->AnyPart.Overflow.s_rate,
//        nod->AnyPart.Send.name.c_str(),nod->AnyPart.Send.produced,nod->AnyPart.Send.seen,nod->AnyPart.Send.p_rate,nod->AnyPart.Send.s_rate,
//        nod->AnyPart.ProcPerf.name.c_str(),nod->AnyPart.ProcPerf.produced,nod->AnyPart.ProcPerf.seen,nod->AnyPart.ProcPerf.p_rate,nod->AnyPart.ProcPerf.s_rate
//        );
    m_DefservdatNodesBuffersEvents += Line;
    nod->m_DefQ.ReadTime_prev = nod->m_DefQ.ReadTime;
    nod->AnyPart.Events_prev = nod->AnyPart.Events;
    nod->AnyPart.Overflow_prev = nod->AnyPart.Overflow;
    nod->AnyPart.Send_prev = nod->AnyPart.Send;
    nod->AnyPart.ProcPerf_prev = nod->AnyPart.ProcPerf;
    if (nod->m_DefQ.m_ROC_state == 'Y')
    {
      if (nod->AnyPart.Overflow.p_rate>0.0)
      {
        nod->m_DefQ.m_nodePerformance = float(nod->AnyPart.Events.p_rate);
        nod->m_DefQ.m_active = true;
      }
      else
      {
        nod->m_DefQ.m_active = false;
      }
    }
    else
    {
      nod->m_DefQ.m_active = false;
    }




    nod->LHCb2.ProcPerf.name = "Input";
    nod->LHCb2.ProcPerf.produced = nod->LHCb2.Events.produced+nod->LHCb2.Overflow.produced;
    nod->LHCb2.ProcPerf.seen = nod->LHCb2.Events.seen+nod->LHCb2.Overflow.seen;
    nod->LHCb2.Events.calcRate(nod->LHCb2.Events_prev,dtime);
    nod->LHCb2.Overflow.calcRate(nod->LHCb2.Overflow_prev,dtime);
    nod->LHCb2.Send.calcRate(nod->LHCb2.Send_prev,dtime);
    nod->LHCb2.ProcPerf.calcRate(nod->LHCb2.ProcPerf_prev,dtime);
    FillMBMBufferRates(Line,nod->m_name,nod->LHCb2);
//    sprintf(Line,"%s %s/%d/%d/%0.4f/%0.4f,%s/%d/%d/%0.4f/%0.4f,%s/%d/%d/%0.4f/%0.4f,%s/%d/%d/%0.4f/%0.4f|",
//        nod->m_name.c_str(),
//        nod->LHCb2.Events.name.c_str(),nod->LHCb2.Events.produced,nod->LHCb2.Events.seen,nod->LHCb2.Events.p_rate,nod->LHCb2.Events.s_rate,
//        nod->LHCb2.Overflow.name.c_str(),nod->LHCb2.Overflow.produced,nod->LHCb2.Overflow.seen,nod->LHCb2.Overflow.p_rate,nod->LHCb2.Overflow.s_rate,
//        nod->LHCb2.Send.name.c_str(),nod->LHCb2.Send.produced,nod->LHCb2.Send.seen,nod->LHCb2.Send.p_rate,nod->LHCb2.Send.s_rate,
//        nod->LHCb2.ProcPerf.name.c_str(),nod->LHCb2.ProcPerf.produced,nod->LHCb2.ProcPerf.seen,nod->LHCb2.ProcPerf.p_rate,nod->LHCb2.ProcPerf.s_rate
//        );
    m_DefservdatNodesBuffersEvents_LHCb2 += Line;
    nod->m_DefQ.ReadTime_prev = nod->m_DefQ.ReadTime;
    nod->LHCb2.Events_prev = nod->LHCb2.Events;
    nod->LHCb2.Overflow_prev = nod->LHCb2.Overflow;
    nod->LHCb2.Send_prev = nod->LHCb2.Send;
    nod->LHCb2.ProcPerf_prev = nod->LHCb2.ProcPerf;
//    if (nod->m_ROC_state == 'Y')
//    {
//      if (nod->LHCb2.Overflow.p_rate>0.0)
//      {
//        nod->m_nodePerformance = float(nod->LHCb2.Events.p_rate);
//        nod->m_active = true;
//      }
//      else
//      {
//        nod->m_active = false;
//      }
//    }
//    else
//    {
//      nod->m_active = false;
//    }







  }
  m_DefservdatNodesBuffersEvents_LHCb2 += '\0';
  m_DefservdatNodesBuffersEvents += '\0';
  m_NodesBuffersEvents->setData((void*)m_DefservdatNodesBuffersEvents.c_str(),m_DefservdatNodesBuffersEvents.size());
  m_NodesBuffersEvents->updateService();
  m_NodesBuffersEvents_LHCb2->setData((void*)m_DefservdatNodesBuffersEvents_LHCb2.c_str(),m_DefservdatNodesBuffersEvents_LHCb2.size());
  m_NodesBuffersEvents_LHCb2->updateService();
//========================================================================
  m_DefservdatNodesBuffersOcc.erase();
  m_DefservdatNodesBuffersOcc_LHCb2.erase();
  for (NodeSet::iterator nit=m_BufferrecvNodes.begin();nit != m_BufferrecvNodes.end();nit++)
  {
    std::string nname;
    nname = *nit;
    myNodeMap::iterator nodeit = m_AllNodes.find(nname);
    myNode *nod = (*nodeit).second;
    char Line[2048];
    FillMBMBufferOccs(Line,nod->m_name,nod->AnyPart);
//    int lpos = 0;
//    lpos += sprintf(&Line[lpos],"%s ",
//        nod->m_name.c_str());
//    if (nod->AnyPart.Events.name != "undefined")
//    {
//      lpos += sprintf(&Line[lpos],"%s/%d/%d/%d/%d",
//          nod->AnyPart.Events.name.c_str(),nod->AnyPart.Events.spused,nod->AnyPart.Events.sptot,nod->AnyPart.Events.evused,nod->AnyPart.Events.evtot);
//    }
//    Line[lpos] = ',';
//    if (nod->AnyPart.Send.name != "undefined")
//    {
//      lpos++;
//      lpos += sprintf(&Line[lpos],"%s/%d/%d/%d/%d",
//          nod->AnyPart.Send.name.c_str(),nod->AnyPart.Send.spused,nod->AnyPart.Send.sptot,nod->AnyPart.Send.evused,nod->AnyPart.Send.evtot);
//    }
//    Line[lpos]= '|';lpos++;
//    Line[lpos] = 0;
    m_DefservdatNodesBuffersOcc += Line;


    FillMBMBufferOccs(Line,nod->m_name,nod->LHCb2);
//    lpos = 0;
//    lpos += sprintf(&Line[lpos],"%s ",
//        nod->m_name.c_str());
//    if (nod->LHCb2.Events.name != "undefined")
//    {
//      lpos += sprintf(&Line[lpos],"%s/%d/%d/%d/%d",
//          nod->LHCb2.Events.name.c_str(),nod->LHCb2.Events.spused,nod->LHCb2.Events.sptot,nod->LHCb2.Events.evused,nod->LHCb2.Events.evtot);
//    }
//    Line[lpos] = ',';
//    if (nod->LHCb2.Send.name != "undefined")
//    {
//      lpos++;
//      lpos += sprintf(&Line[lpos],"%s/%d/%d/%d/%d",
//          nod->LHCb2.Send.name.c_str(),nod->LHCb2.Send.spused,nod->LHCb2.Send.sptot,nod->LHCb2.Send.evused,nod->LHCb2.Send.evtot);
//    }
//    Line[lpos]= '|';lpos++;
//    Line[lpos] = 0;
//    sprintf(Line,"%s %s/%d/%d/%d/%d,%s/%d/%d/%d/%d|",
//        nod->m_name.c_str(),
//        nod->LHCb2.Events.name.c_str(),nod->AnyPart.Events.spused,nod->AnyPart.Events.sptot,nod->AnyPart.Events.evused,nod->AnyPart.Events.evtot,
//        nod->LHCb2.Send.name.c_str(),nod->AnyPart.Send.spused,nod->AnyPart.Send.sptot,nod->AnyPart.Send.evused,nod->AnyPart.Send.evtot
//        );
    m_DefservdatNodesBuffersOcc_LHCb2 += Line;
  }
  m_DefservdatNodesBuffersOcc_LHCb2 += '\0';
  m_DefservdatNodesBuffersOcc += '\0';
  m_NodesBuffersOcc->setData((void*)m_DefservdatNodesBuffersOcc.c_str(),m_DefservdatNodesBuffersOcc.size());
  m_NodesBuffersOcc->updateService();
  m_NodesBuffersOcc_LHCb2->setData((void*)m_DefservdatNodesBuffersOcc_LHCb2.c_str(),m_DefservdatNodesBuffersOcc_LHCb2.size());
  m_NodesBuffersOcc_LHCb2->updateService();
//========================================================================

  float av_perf=0.0;
  float rms_perf=1000.0;
  float max_perf=0.0;
  float min_perf=1000.0;
  float erate_sum=0.0;
  float erate_sum2=0.0;
  int nnod = 0;
  for (int ii =0;ii<2;ii++)
  {
    for (NodeSet::iterator nit=m_BufferrecvNodes.begin();nit != m_BufferrecvNodes.end();nit++)
    {
      std::string nname;
      nname = *nit;
      myNodeMap::iterator nodeit = m_AllNodes.find(nname);
      myNode *nod = (*nodeit).second;
      if (nod->m_DefQ.m_active)
      {
        if ((nod->m_DefQ.m_nodePerformance < av_perf+2.5*rms_perf) && (nod->m_DefQ.m_nodePerformance > av_perf-2.5*rms_perf))
        {
          erate_sum += nod->m_DefQ.m_nodePerformance;
          erate_sum2 += (nod->m_DefQ.m_nodePerformance*nod->m_DefQ.m_nodePerformance);
          nnod++;
          max_perf = max_perf<nod->m_DefQ.m_nodePerformance ?nod->m_DefQ.m_nodePerformance:max_perf;
          min_perf = min_perf>nod->m_DefQ.m_nodePerformance ?nod->m_DefQ.m_nodePerformance:min_perf;
        }
      }
    }
    if (nnod >0)
    {
      av_perf = erate_sum/nnod;
      rms_perf = sqrt(erate_sum2/nnod-av_perf*av_perf);
    }
    fprintf(outf,"Based on % d nodes.  Average performance %f +/- %f. Minimum performance %f. maximum performance %f\n",
        nnod,av_perf,rms_perf,min_perf,max_perf);
    fprintf(outf,"Based on % d nodes.  Average performance %f +/- %f. Minimum performance %f. maximum performance %f\n",
        nnod,13*av_perf,13*rms_perf,13*min_perf,13*max_perf);
    max_perf=0.0;
    min_perf=1000.0;
    erate_sum=0.0;
    erate_sum2=0.0;
    nnod = 0;
  }
//  BufferDump();
  m_BufferrecvNodes.clear();
  dim_unlock();
}
void HLTFileEqualizer::BufferDump()
{
  fprintf(outf,"Dump of Events Buffer Distribution at ");
  {
    time_t rawtime;
    time(&rawtime);
    fprintf(outf,"%s\n",asctime(localtime(&rawtime)));
  }
  fprintf(outf,"          ");
  for (int i=1;i<=32;i++)
  {
    fprintf(outf,"  %02i  ",i);

  }
  std::string cfarm="";
  std::string eline(256,' ');
  std::string line=eline;
  for (NodeSet::iterator nit=m_BufferrecvNodes.begin();nit != m_BufferrecvNodes.end();nit++)
  {
    std::string nname;
    nname = *nit;
    myNodeMap::iterator nodeit = m_AllNodes.find(nname);
    myNode *nod = (*nodeit).second;
    if (cfarm != nod->m_subfarm)
    {
      if (cfarm.size()>2)
      {
        fprintf(outf,"\n%s",line.substr(0,line.find_last_not_of(" ")+1).c_str());
      }
      line = eline;
      line.replace(0,nod->m_subfarm.size(),nod->m_subfarm);
//      fprintf(outf,"\n%s",nod->m_subfarm);
      cfarm = nod->m_subfarm;
    }
    int indx;
    sscanf(nod->m_name.substr(6,2).c_str(),"%d",&indx);
    char nfil[10];
    sprintf(nfil,"%4.3f",nod->AnyPart.Events.p_rate);
    line.replace(9+(indx-1)*6,5,nfil);
  }
  fprintf(outf,"\n%s\n",line.substr(0,line.find_last_not_of(" ")+1).c_str());
  fprintf(outf,"          ");
  for (int i=1;i<=32;i++)
  {
    fprintf(outf,"  %02i  ",i);

  }
  fprintf(outf,"\n");
  fflush(outf);
}
DefHltInfoHandler::DefHltInfoHandler(HLTFileEqualizer *e)
{
  m_subfarm = 0;
  m_sfstatus = 0;
  m_bufsiz = 0;
  m_Equalizer = e;
//      m_subfarm = sf;
}

HLT1InfoHandler::HLT1InfoHandler(HLTFileEqualizer *e)
{
  m_subfarm = 0;
  m_sfstatus = 0;
  m_bufsiz = 0;
  m_Equalizer = e;
//      m_subfarm = sf;
}

MBMInfoHandler::MBMInfoHandler(HLTFileEqualizer *e)
{
  m_sfstatus = 0;
  m_bufsiz = 0;
  m_Equalizer = e;
}
using namespace ROMon;
typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;

void MBMInfoHandler::infoHandler()
{
  int siz;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz == sizeof(int)) return;
  const char *psname, *pnname;
  m_sfstatus = (_MBMSF *)itsService->getData();
  const _MBMSF *ns = m_sfstatus;
  psname = ns->name;
  Nodes::const_iterator n;
    for (n=ns->nodes.begin();n!=ns->nodes.end(); n=ns->nodes.next(n))
    {
      std::string nname = (*n).name;
      pnname = nname.c_str();
      m_Equalizer->m_BufferrecvNodes.insert(nname);
      myNodeMap::iterator anit;
      anit = m_Equalizer->m_AllNodes.find(nname);
      myNode* nod;
      if (anit == m_Equalizer->m_AllNodes.end())
      {
        nod = new myNode(nname);
        m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
      }
      else
      {
        nod = (*anit).second;
      }
      if (nod->m_DefQ.ReadTime_prev == 0)
      {
        nod->m_DefQ.ReadTime_prev = nod->m_DefQ.ReadTime;
        nod->AnyPart.Events_prev = nod->AnyPart.Events;
        nod->AnyPart.Overflow_prev = nod->AnyPart.Overflow;
        nod->AnyPart.Send_prev = nod->AnyPart.Send;
        nod->AnyPart.ProcPerf.produced = nod->AnyPart.Events.produced+nod->AnyPart.Overflow.produced;
        nod->AnyPart.ProcPerf.seen = nod->AnyPart.Events.seen+nod->AnyPart.Overflow.seen;
        nod->LHCb2.Events_prev = nod->LHCb2.Events;
        nod->LHCb2.Overflow_prev = nod->LHCb2.Overflow;
        nod->LHCb2.Send_prev = nod->LHCb2.Send;
        nod->LHCb2.ProcPerf.produced = nod->LHCb2.Events.produced+nod->LHCb2.Overflow.produced;
        nod->LHCb2.ProcPerf.seen = nod->LHCb2.Events.seen+nod->LHCb2.Overflow.seen;
      }
      nod->m_DefQ.ReadTime = (*n).time;
      nod->m_DefQ.ReadTime *= 1000;
      nod->m_DefQ.ReadTime += (*n).millitm;
      const Buffers& buffs = *(*n).buffers();
      for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))
      {
        const Buffers::value_type::Control& c = (*ib).ctrl;
        std::string bnam = (*ib).name;
        if (bnam.find("LHCb2") == std::string::npos)
        { //Not LHCb2
          if (bnam.find("Events") != std::string::npos)
          {
  //          nod->Events_prev = nod->Events;
            nod->AnyPart.Events.name = bnam;
            nod->AnyPart.Events.produced = c.tot_produced;
            nod->AnyPart.Events.seen  = c.tot_seen;
            nod->AnyPart.Events.evtot = c.p_emax;
            nod->AnyPart.Events.evused = c.i_events;
            nod->AnyPart.Events.sptot  = c.buff_size;
            nod->AnyPart.Events.spused = c.buff_size-c.i_space;
          }
          else if (bnam.find("Overflow") != std::string::npos)
          {
  //          nod->Overflow_prev = nod->Overflow;
            nod->AnyPart.Overflow.name = bnam;
            nod->AnyPart.Overflow.produced = c.tot_produced;
            nod->AnyPart.Overflow.seen  = c.tot_seen;
            nod->AnyPart.Overflow.evtot = c.p_emax;
            nod->AnyPart.Overflow.evused = c.i_events;
            nod->AnyPart.Overflow.sptot  = c.buff_size;
            nod->AnyPart.Overflow.spused = c.buff_size-c.i_space;
          }
          else if (bnam.find("Output") != std::string::npos)
          {
  //          nod->Send_prev = nod->Send;
            nod->AnyPart.Send.name = bnam;
            nod->AnyPart.Send.produced = c.tot_produced;
            nod->AnyPart.Send.seen  = c.tot_seen;
            nod->AnyPart.Send.evtot = c.p_emax;
            nod->AnyPart.Send.evused = c.i_events;
            nod->AnyPart.Send.sptot  = c.buff_size;
            nod->AnyPart.Send.spused = c.buff_size-c.i_space;
          }
        }
        else
        { //LHCb2
          if (bnam.find("Events") != std::string::npos)
          {
  //          nod->Events_prev = nod->Events;
            nod->LHCb2.Events.name = bnam;
            nod->LHCb2.Events.produced = c.tot_produced;
            nod->LHCb2.Events.seen  = c.tot_seen;
            nod->LHCb2.Events.evtot = c.p_emax;
            nod->LHCb2.Events.evused = c.i_events;
            nod->LHCb2.Events.sptot  = c.buff_size;
            nod->LHCb2.Events.spused = c.buff_size-c.i_space;
          }
          else if (bnam.find("Overflow") != std::string::npos)
          {
  //          nod->Overflow_prev = nod->Overflow;
            nod->LHCb2.Overflow.name = bnam;
            nod->LHCb2.Overflow.produced = c.tot_produced;
            nod->LHCb2.Overflow.seen  = c.tot_seen;
            nod->LHCb2.Overflow.evtot = c.p_emax;
            nod->LHCb2.Overflow.evused = c.i_events;
            nod->LHCb2.Overflow.sptot  = c.buff_size;
            nod->LHCb2.Overflow.spused = c.buff_size-c.i_space;
          }
          else if (bnam.find("Output") != std::string::npos)
          {
  //          nod->Send_prev = nod->Send;
            nod->LHCb2.Send.name = bnam;
            nod->LHCb2.Send.produced = c.tot_produced;
            nod->LHCb2.Send.seen  = c.tot_seen;
            nod->LHCb2.Send.evtot = c.p_emax;
            nod->LHCb2.Send.evused = c.i_events;
            nod->LHCb2.Send.sptot  = c.buff_size;
            nod->LHCb2.Send.spused = c.buff_size-c.i_space;
          }
        }
      }
    }
}

void DefHltInfoHandler::infoHandler()
{
  typedef _DHLTSF::Nodes               _N;
  typedef _DHLTSF::Node::Runs          _R;
  int siz;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz == sizeof(int)) return;
  m_sfstatus = (_DHLTSF*)this->itsService->getData();
  const _DHLTSF* stats = m_sfstatus;
  const _N& nodes = *(stats->nodes());
//      _N::const_iterator i;
  for (_N::const_iterator i = nodes.begin();i!= nodes.end();i=nodes.next(i))
  {
    const _R& runs = (*i).runs;
    std::string nname = RTL::str_lower((*i).name);
    myNodeMap::iterator nit;
    myNodeMap::iterator anit;
    m_Equalizer->m_recvNodes.insert(nname);
    anit = m_Equalizer->m_AllNodes.find(nname);
    myNode* nod;
    if (anit == m_Equalizer->m_AllNodes.end())
    {
      nod = new myNode(nname);
      m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
    }
    else
    {
      nod = (*anit).second;
    }
    NodeSet::iterator en = m_Equalizer->m_exclNodes.find(nod->m_name);
    nod->m_DefQ.m_excl = (en != m_Equalizer->m_exclNodes.end());
    nod->m_DefQ.m_runmap.clear();
//    fprintf(outf,"%s: ",(*i).name);
    int nfiles=0;
    for (_R::const_iterator j = runs.begin();j!= runs.end();j=runs.next(j))
    {
      nfiles += (*j).second;
      nod->m_DefQ.m_runmap[(*j).first] = (*j).second;
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
      m_Equalizer->m_Nodes[nname]->m_DefQ.m_nofiles = nfiles;
      m_Equalizer->m_Nodes[nname]->m_DefQ.m_ROC_state = (*i).overflowState;
//      m_Equalizer->m_Nodes[nname]->m_state *= (*i).overflowState;
      m_Equalizer->m_nfiles += nfiles;
      m_Equalizer->m_nfiles2 += nfiles*nfiles;
      m_Equalizer->m_nnodes++;
    }
    else
    {
      m_Equalizer->m_AllNodes[nname]->m_DefQ.m_nofiles = nfiles;
      m_Equalizer->m_AllNodes[nname]->m_DefQ.m_ROC_state = (*i).overflowState;
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

void HLT1InfoHandler::infoHandler()
{
  typedef _DHLTSF::Nodes               _N;
  typedef _DHLTSF::Node::Runs          _R;
  int siz;
//  int status;
  siz = this->itsService->getSize();
//      gettimeofday()
  if (siz == sizeof(int)) return;
  m_sfstatus = (_DHLTSF*)this->itsService->getData();
  const _DHLTSF* stats = m_sfstatus;
  const _N& nodes = *(stats->nodes());
//      _N::const_iterator i;
  for (_N::const_iterator i = nodes.begin();i!= nodes.end();i=nodes.next(i))
  {
    const _R& runs = (*i).runs;
    std::string nname = RTL::str_lower((*i).name);
    myNodeMap::iterator nit;
    myNodeMap::iterator anit;
    m_Equalizer->m_recvNodes.insert(nname);
    anit = m_Equalizer->m_AllNodes.find(nname);
    myNode* nod;
    if (anit == m_Equalizer->m_AllNodes.end())
    {
      nod = new myNode(nname);
      m_Equalizer->m_AllNodes.insert(std::make_pair(nname,nod));
    }
    else
    {
      nod = (*anit).second;
    }
    NodeSet::iterator en = m_Equalizer->m_exclNodes.find(nod->m_name);
    nod->m_HLT1Q.m_excl = (en != m_Equalizer->m_exclNodes.end());
    nod->m_HLT1Q.m_runmap.clear();
//    fprintf(outf,"%s: ",(*i).name);
    int nfiles=0;
    for (_R::const_iterator j = runs.begin();j!= runs.end();j=runs.next(j))
    {
      nfiles += (*j).second;
      nod->m_HLT1Q.m_runmap[(*j).first] = (*j).second;
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
      m_Equalizer->m_Nodes[nname]->m_HLT1Q.m_nofiles = nfiles;
      m_Equalizer->m_Nodes[nname]->m_HLT1Q.m_ROC_state = (*i).overflowState;
//      m_Equalizer->m_Nodes[nname]->m_state *= (*i).overflowState;
      m_Equalizer->m_nfiles += nfiles;
      m_Equalizer->m_nfiles2 += nfiles*nfiles;
      m_Equalizer->m_nnodes++;
    }
    else
    {
      m_Equalizer->m_AllNodes[nname]->m_HLT1Q.m_nofiles = nfiles;
      m_Equalizer->m_AllNodes[nname]->m_HLT1Q.m_ROC_state = (*i).overflowState;
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
  std::string cfarm="";
  std::string eline(256,' ');
  std::string line=eline;
//  int maxidx = -1;
  for (myNodeMap::iterator nit=m_Nodes.begin();nit != m_Nodes.end();nit++)
  {
    myNode *nod = (*nit).second;
    if (cfarm != nod->m_subfarm)
    {
      if (cfarm.size()>2)
      {
        fprintf(outf,"\n%s",line.substr(0,line.find_last_not_of(" ")+1).c_str());
      }
      line = eline;
      line.replace(0,nod->m_subfarm.size(),nod->m_subfarm);
//      fprintf(outf,"\n%s",nod->m_subfarm);
      cfarm = nod->m_subfarm;
    }
    int indx;
    sscanf(nod->m_name.substr(6,2).c_str(),"%d",&indx);
    char nfil[10];
    if (nod->m_DefQ.m_ROC_state == 'Y')
    {
      sprintf(nfil,"%4d",nod->m_DefQ.m_nofiles);
      line.replace(9+(indx-1)*5,4,nfil);
    }
    else
    {
      sprintf(nfil,"%4d*",nod->m_DefQ.m_nofiles);
      line.replace(9+(indx-1)*5,5,nfil);
    }
  }
  fprintf(outf,"\n%s\n",line.c_str());
  fflush(outf);
}

namespace {
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
}

LHCb1RunStatus::LHCb1RunStatus(char *name, int nolink,HLTFileEqualizer *e) : DimInfo(name,nolink)
{
  m_nolink = nolink;
  m_equalizer = e;
}

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
}

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

int main(int argc, char **argv)
{
  char *ofile = getenv("HLTEQ_LOGF");
  if (ofile == 0)
  {
    outf = fopen("/group/online/HLTFileEqualizer.log","a+");
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
  HLTFileEqualizer elz;
  HLTPerfFormatter *pform = new HLTPerfFormatter();
  pform->Extract(elz.M_PMap);
  delete pform;
  nodemap::iterator nit;
  for (nit = elz.M_PMap.begin();nit!=elz.M_PMap.end();nit++)
  {
    (*nit).second->Normalize(645.0);
  }
  int m_DefState = -1;
  DimServer::start("HLTFileEqualizer");
  DimServer::autoStartOn();
  int low,high;
  if (argc > 1)
  {
    sscanf(argv[1],"%d",&low);
    elz.m_low = low;
  }
  if (argc > 2)
  {
    sscanf(argv[2],"%d",&high);
    elz.m_high = high;
  }
  fprintf(outf,"Low/high options %d %d\n",elz.m_low,elz.m_high);
  ExclInfo exclInfo((char*)"HLT/ExcludedNodes",&elz.m_exclNodes);
  DimInfo *defstate=new DimInfo("RunInfo/LHCb/DeferHLT",m_DefState);
  elz.m_DefStateInfo = defstate;
  ExitCommand EnableandExit("HLTFileEqualizer/EnableAndExit",(char*)"I",&elz.m_AllNodes,&elz);
  LHCb1RunStatus LHCb1runstatus((char*)"RunInfo/LHCb1/RunStatus",-1,&elz);
  DimService *m_NodeService = new DimService("HLTFileEqualizer/NodeList", "C",(void*)"\0",1);
  elz.m_DefNodeList= m_NodeService;
  DimService *m_NodeServiceDiff = new DimService("HLTFileEqualizer/NodeListDiff", "C",(void*)"\0",1);
  elz.m_DefNodeListDiff = m_NodeServiceDiff;
  DimService *m_NodesRunsFiles= new DimService("HLTFileEqualizer/NodesRunsFiles", "C",(void*)"\0",1);
  elz.m_DefNodesRunsFiles = m_NodesRunsFiles;
  DimService *m_NodesBuffersEvents = new DimService("HLTFileEqualizer/NodesBuffersEvents", "C",(void*)"\0",1);
  elz.m_NodesBuffersEvents = m_NodesBuffersEvents;
  m_NodeService = new DimService("HLTFileEqualizer/HLT1/NodeList", "C",(void*)"\0",1);
  elz.m_HLT1NodeList= m_NodeService;
  m_NodeServiceDiff = new DimService("HLTFileEqualizer/HLT1/NodeListDiff", "C",(void*)"\0",1);
  elz.m_HLT1NodeListDiff = m_NodeServiceDiff;
  m_NodesRunsFiles= new DimService("HLTFileEqualizer/HLT1/NodesRunsFiles", "C",(void*)"\0",1);
  elz.m_HLT1NodesRunsFiles = m_NodesRunsFiles;
  DimService *m_NodesBuffersEvents_LHCb2 = new DimService("HLTFileEqualizer/LHCb2/NodesBuffersEvents", "C",(void*)"\0",1);
  elz.m_NodesBuffersEvents_LHCb2 = m_NodesBuffersEvents_LHCb2;
  DimService *m_NodesBuffersOcc_LHCb2 = new DimService("HLTFileEqualizer/LHCb2/NodesBuffersOccupancies", "C",(void*)"\0",1);
  elz.m_NodesBuffersOcc_LHCb2 = m_NodesBuffersOcc_LHCb2;
  DimService *m_NodesBuffersOcc = new DimService("HLTFileEqualizer/NodesBuffersOccupancies", "C",(void*)"\0",1);
  elz.m_NodesBuffersOcc = m_NodesBuffersOcc;
  float stat[2];
  stat[0] = -1.0;
  stat[1] = 0.0;
  DimService *m_Statistics = new DimService("HLTFileEqualizer/Statistics", "F",stat,sizeof(stat));
  elz.m_DefStatServ = m_Statistics;
  fflush(outf);
  while (1)
  {
    sleep (30);
    elz.Analyze();
  }
  return 0;
}


