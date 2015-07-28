/*
 * hist.cpp
 *
 *  Created on: Mar 29, 2010
 *      Author: beat
 */
#include "Gaucho/MonHist.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatEntity.h"
#include "TObject.h"
#include "Gaucho/RootHists.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IProfile2D.h"
#include "AIDA/IProfile1D.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "Gaucho/dimhist.h"
#include "Gaucho/CntrMgr.h"
#include "Gaucho/Utilities.h"
//#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)
std::string MonHist::optsep="/:/:/:";



void MonHist::_clear()
{
 m_type = H_ILLEGAL;
 m_rootobj = 0;
 m_rootdeser = 0;
 m_cntrmgr = 0;
 m_xmitbuffersize = sizeof(DimBuffBase);
 m_hdrlen = 0;
 m_buffersize = 0;
 m_namelen = 0;
 m_titlen = 0;
 m_nx = 0;           /** Number of x-bins **/
 m_xmin = 0;     /** Minimum x **/
 m_xmax = 0;     /** Maximum y **/
 m_ny = 0;           /* Number of y bins */
 m_ymin = 0;     /* Minimum y */
 m_ymax = 0;     /* Maximum y */
 m_addoff = 0;

 m_hentries = 0;
 m_hsumw2 = 0;
 m_hsumw = 0;
 m_hbinsumw2 = 0;
 m_blocksize = 0;
 m_Xaxis = 0;
 m_Yaxis = 0;
 m_Xlabels.clear();
 m_Ylabels.clear();
 m_xlablen = 0;
 m_ylablen = 0;
  m_msgsvc = 0;
 TH1D::SetDefaultSumw2();
 TH2D::SetDefaultSumw2();
 TProfile::SetDefaultSumw2();

}
MonHist::MonHist()
{
  _clear();
}
MonHist::MonHist(IMessageSvc* msgs, const std::string& source, CntrMgr *cm)
{
// Premordial Setup....
  _clear();
  m_msgsvc = msgs;
  m_cntrmgr = cm;
  m_addoff = 9;
  m_type = H_RATE;
  m_name = source;
  this->m_xmitbuffersize = 0;
}

MonHist::MonHist(IMessageSvc* msgSvc, const std::string& source, const std::string &desc, const StatEntity *se)
{
// Premordial Setup....
  _clear();
  m_msgsvc = msgSvc;
  setup(m_msgsvc,source,desc,se);
  m_cntrmgr = 0;
}
MonHist::MonHist(IMessageSvc* msgs, const std::string& source, const AIDA::IBaseHistogram *aidahist)
{
  m_msgsvc = msgs;
  setup(m_msgsvc, source,aidahist);
  m_cntrmgr = 0;
}
void MonHist::makeCounters()
{
  Bool_t dirstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  TProfile *tp;
//  MsgStream msg(m_msgsvc,"MonitorSvc");
  if (m_cntrmgr != 0)
  {
    if (m_cntrmgr->m_newcounter)
    {
      deletePtr(m_rootobj);
//      if (m_rootobj != 0)
//      {
//        delete m_rootobj;
//      }
      m_rootobj = new TProfile("COUNTER_TO_RATE", "Counter to rate Profile",
          m_cntrmgr->m_counterMap.size()+8,0.0,m_cntrmgr->m_counterMap.size()+8);
    }
    tp = (TProfile*)m_rootobj;
    if (tp == 0)
    {
      TH1::AddDirectory(dirstat);
      return;
    }
    tp->Reset();
    tp->Fill(0.50, m_cntrmgr->offsetTimeFirstEvInRun(), 1.00);
    tp->Fill(1.50, m_cntrmgr->offsetTimeLastEvInCycle(), 1.00);
    tp->Fill(2.50, m_cntrmgr->offsetGpsTimeLastEvInCycle(), 1.00);
    tp->Fill(3.50, 1.00, 1.00);
    tp->Fill(4.50, (double) (m_cntrmgr->runNumber()), 1.00);
    tp->Fill(5.50, (double) (m_cntrmgr->triggerConfigurationKey()), 1.00);
    tp->Fill(6.50, (double) (m_cntrmgr->cycleNumber()), 1.00);
    tp->Fill(7.50, (double) (m_cntrmgr->deltaT()), 1.00);

    int i = 9;
    CntrMgr::counterMapIt it;
    for (it= m_cntrmgr->m_counterMap.begin(); it != m_cntrmgr->m_counterMap.end(); it++)
    {
       if (it->second.second.first == "int" )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is int ");
         tp->Fill((double)i - 0.5, (*(int*)(it->second.second.second)), 1.00);
       }
       else if (it->second.second.first.compare("double") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is double ");
//         msg <<MSG::DEBUG<<"Counter :" << m_cntrmgr->m_counterMapIt->first << " is double "<< endreq;
//   msg <<MSG::DEBUG<<"Counter :" << (*(double*)(m_cntrmgr->m_counterMapIt->second.second.second)) << endreq;
         tp->Fill((double)i - 0.5, (*(double*)(it->second.second.second)), 1.00);
       }
       else if (it->second.second.first.compare("long") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is long ");
//         msg <<MSG::DEBUG<<"Counter :" << m_cntrmgr->m_counterMapIt->first << " is long  "<< endreq;
         tp->Fill((double)i - 0.5, (*(long*)(it->second.second.second)), 1.00);
       }
       else if (it->second.second.first.compare("StatEntityflag") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is  StatEntity (flag) ");
//         msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (flag) "<< endreq;
         StatEntity counter = (*(StatEntity*) it->second.second.second);
         tp->Fill((double)i - 0.5, counter.flag(), 1.00);
//         msg <<MSG::DEBUG<<"Counter :" << counter.flag() << endreq;
       }
       else if (it->second.second.first.compare("StatEntitynEntries") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is  StatEntity (nEntries) ");
//         msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (nEntries) "<< endreq;
         StatEntity counter = (*(StatEntity*) it->second.second.second);
         tp->Fill((double)i - 0.5, counter.nEntries(), 1.00);
//         msg <<MSG::DEBUG<<"Counter :" << counter.nEntries() << endreq;
       }
       else
       {
//         msg <<MSG::ERROR<<"Incompatible counter type for MonRate.." << endreq;
       }
       i++;
     }
     TAxis *ax=tp->GetXaxis();
     if (ax != 0)
     {
       ax->SetBinLabel(1, "OffsetTimeFirstEvInRun");
       ax->SetBinLabel(2, "OffsetTimeLastEvInCycle");
       ax->SetBinLabel(3, "OffsetGpsTimeLastEvInCycle");
       ax->SetBinLabel(4, "NumberofProcesses"); // actually identical to 1 for each process. adders sum it up to give the number of processes...
       ax->SetBinLabel(5, "RunNumber");
       ax->SetBinLabel(6, "TCK");
       ax->SetBinLabel(7, "CycleNumber");
       ax->SetBinLabel(8, "deltaT");

       i = 9;
       for (it = m_cntrmgr->m_counterMap.begin(); it != m_cntrmgr->m_counterMap.end(); it++)
       {
  //       msg <<MSG::DEBUG<<"label description: " << (*(it->second.first)).c_str() << endreq;
         if (it->second.first.c_str() != 0)
         {
           ax->SetBinLabel(i, it->second.first.c_str());
         }
         else
         {
           ax->SetBinLabel(i,"Bad label");
         }
  //       printf("Rate Axis Label for bin %d %s\n",i, (*(it->second.first)).c_str());
         i++;
       }
     }
     this->setup(m_msgsvc);
   }
  TH1::AddDirectory(dirstat);
}
void MonHist::setup(IMessageSvc* msgs, const std::string& source, const AIDA::IBaseHistogram *aidahist)
{
  MsgStream msg(msgs,"MonitorSvc");
  m_cntrmgr = 0;
  m_rootdeser = 0;
  m_rootobj = 0;
  if( 0 != dynamic_cast<const AIDA::IProfile1D* >(aidahist) )
    {

      m_type = H_PROFILE;
      MyTProfile *rhist = (MyTProfile*)Gaudi::Utils::Aida2ROOT::aida2root(const_cast<AIDA::IProfile1D *>(dynamic_cast<const AIDA::IProfile1D* >(aidahist)));
      rhist->SetName(source.c_str());
      m_rootobj = rhist;
    }
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(aidahist) )
    {
      m_type = H_1DIM;
      MyTH1D *rhist = (MyTH1D*)Gaudi::Utils::Aida2ROOT::aida2root(const_cast<AIDA::IHistogram1D *>(dynamic_cast<const AIDA::IHistogram1D* >(aidahist)));
      rhist->SetName(source.c_str());
      m_rootobj = rhist;
    }
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(aidahist) )
    {
      m_type = H_2DIM;
      MyTH2D *rhist = (MyTH2D*)Gaudi::Utils::Aida2ROOT::aida2root(const_cast<AIDA::IHistogram2D *>(dynamic_cast<const AIDA::IHistogram2D* >(aidahist)));
      rhist->SetName(source.c_str());
      m_rootobj = rhist;
    }
  else if( 0 != dynamic_cast<const AIDA::IHistogram3D * >(aidahist) )
    {
      m_type = H_3DIM;
      MyTH3D *rhist = (MyTH3D*)Gaudi::Utils::Aida2ROOT::aida2root(const_cast<AIDA::IHistogram3D *>(dynamic_cast<const AIDA::IHistogram3D* >(aidahist)));
      rhist->SetName(source.c_str());
      m_rootobj = rhist;
    }
  else if( 0 != dynamic_cast<const AIDA::IProfile2D * >(aidahist) )
    {
      m_type = H_2DPROFILE;
      MyTProfile2D *rhist = (MyTProfile2D*)Gaudi::Utils::Aida2ROOT::aida2root(const_cast<AIDA::IProfile2D *>(dynamic_cast<const AIDA::IProfile2D* >(aidahist)));
      rhist->SetName(source.c_str());
      m_rootobj = rhist;
    }
  else
    {
      msg << MSG::ERROR << "Unknown histogram type. Source " << source << endreq;
      m_rootobj = 0;
      return;
    }
  setup(msgs);
}

void MonHist::setup(IMessageSvc* msgs, const std::string& source, const std::string &desc, const StatEntity *se)
{
  MsgStream msg(msgs,"MonitorSvc");
  m_rootdeser = 0;
  m_rootobj = 0;
  m_cntrmgr = 0;
  m_type = C_STATENT;
  m_rootobj = (TObject*)se;
  m_hentries = 0;
  m_hsumw2 = 0;
  m_hsumw = 0;
  m_name = source;
  m_namelen = m_name.length();
  m_title  = desc;
  m_titlen = m_title.length();
  m_hdrlen = sizeof(DimHistbuff1)+titlen()+1+namelength()+1;
  m_hdrlen = (m_hdrlen + 7)&~7;
  m_buffersize = 3*sizeof(double);
  m_blocksize = m_buffersize;
  m_hdrlen = sizeof(DimHistbuff1)+titlen()+1+namelength()+1;
  m_hdrlen = (m_hdrlen + 7)&(~7);
  m_xmitbuffersize = m_hdrlen+m_buffersize;
  this->m_addoff = 0;
  return;
}

void MonHist::resetup(void)
{
  switch(m_type)
  {
    case H_1DIM:
    case H_2DIM:
    case H_PROFILE:
    case H_3DIM:
    case H_2DPROFILE:
    {
      m_Xlabels.clear();
      m_Ylabels.clear();
      m_Zlabels.clear();
      m_xlablen = 0;
      m_ylablen = 0;
      m_zlablen = 0;
      setup(m_msgsvc);
    }
    case H_RATE:
    case C_INT:
    case C_LONGLONG:
    case C_FLOAT:
    case C_DOUBLE:
    case C_STATENT:
    case H_ILLEGAL:
    case C_VOIDSTAR:
    default:
      break;
  }
}
#define getProfileDataPtrs(h) \
    m_hentries = h->GetEntryArr();\
    m_hsumw2 = h->GetSumw2Arr();\
    m_hbinsumw2 = h->GetBinSumw2Arr();\
    m_hsumw = h->GetSumwArr();

void MonHist::setup(IMessageSvc* msgs)
{
  MsgStream msg(msgs,"MonitorSvc");
//  m_cntrmgr = 0;
  switch (m_type)
  {
    case H_PROFILE:
    case H_RATE:
    {
      MyTProfile *rhist = (MyTProfile*)m_rootobj;
      getProfileDataPtrs(rhist);
      m_rootobj = rhist;
      m_Xaxis = rhist->GetXaxis();
      m_name = rhist->GetName();
      m_namelen = m_name.length();
      m_title = rhist->GetTitle();
      m_bookopts = std::string(rhist->GetOption());
      m_title = m_title+MonHist::optsep+m_bookopts;
      m_titlen = m_title.length();
      m_hdrlen = sizeof(DimHistbuff1)+titlen()+1+namelength()+1;
      m_hdrlen = (m_hdrlen + 7)&~7;
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff1)+titlen()+1+namelength()+1;
      break;
    }
    case H_1DIM:
    {
      MyTH1D *rhist = (MyTH1D*)m_rootobj;
      m_hentries = rhist->GetEntryArr();
      m_hsumw2 = rhist->GetSumw2Arr();
      m_rootobj = rhist;
      m_Xaxis =  rhist->GetXaxis();
      m_name = rhist->GetName();
      m_namelen = m_name.length();
      m_title = rhist->GetTitle();
      m_bookopts = std::string(rhist->GetOption());
      m_title = m_title+MonHist::optsep+m_bookopts;
      m_titlen = m_title.length();
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff1)+titlen()+1+namelength()+1;
      break;
    }
    case H_2DIM:
    {
      MyTH2D *rhist = (MyTH2D*)m_rootobj;
      m_hentries = rhist->GetEntryArr();
      m_hsumw2 = rhist->GetSumw2Arr();
      m_rootobj = rhist;
      m_name = rhist->GetName();
      m_namelen = m_name.length();
      m_title = rhist->GetTitle();
      m_bookopts = std::string(rhist->GetOption());
//      printf("+++++++++++++++++++++++++++++++2Dim Histogram. Name %s, Title %s \n",m_name, m_title);
      m_title = m_title+MonHist::optsep+m_bookopts;
      m_titlen = m_title.length();
      m_Xaxis =  rhist->GetXaxis();
      m_Yaxis = rhist->GetYaxis();
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff2)+titlen()+1+namelength()+1;
      break;
    }
    case H_3DIM:
    {
      MyTH2D *rhist = (MyTH2D*)m_rootobj;
      m_hentries = rhist->GetEntryArr();
      m_hsumw2 = rhist->GetSumw2Arr();
      m_rootobj = rhist;
      m_name = rhist->GetName();
      m_namelen = m_name.length();
      m_title = rhist->GetTitle();
      m_bookopts = std::string(rhist->GetOption());
//      printf("+++++++++++++++++++++++++++++++2Dim Histogram. Name %s, Title %s \n",m_name, m_title);
      m_title = m_title+MonHist::optsep+m_bookopts;
      m_titlen = m_title.length();
      m_Xaxis =  rhist->GetXaxis();
      m_Yaxis = rhist->GetYaxis();
      m_Zaxis = rhist->GetZaxis();
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff3)+titlen()+1+namelength()+1;
      break;
    }
    case H_2DPROFILE:
    {
      MyTProfile2D *rhist = (MyTProfile2D*)m_rootobj;
//      m_hentries = rhist->GetEntryArr();
//      m_hsumw2 = rhist->GetSumw2Arr();
//      m_hbinsumw2 = rhist->GetBinSumw2Arr();
//      m_hsumw = rhist->GetSumwArr();
      getProfileDataPtrs(rhist);
      m_rootobj = rhist;
      m_name = rhist->GetName();
      m_namelen = m_name.length();
      m_title = rhist->GetTitle();
      m_bookopts = std::string(rhist->GetOption());
//      printf("+++++++++++++++++++++++++++++++2Dim Histogram. Name %s, Title %s \n",m_name, m_title);
      m_title = m_title+MonHist::optsep+m_bookopts;
      m_titlen = m_title.length();
      m_Xaxis =  rhist->GetXaxis();
      m_Yaxis = rhist->GetYaxis();
      m_Zaxis = rhist->GetZaxis();
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff3)+titlen()+1+namelength()+1;
      break;
    }
    default:
    {
      msg << MSG::ERROR << "MonHist Setup: Unknown histogram type" << endreq;
      return;
    }
  }
  m_nx = m_Xaxis->GetNbins();
  m_xmin = m_Xaxis->GetXmin();
  m_xmax = m_Xaxis->GetXmax();
  m_ny = 0;
  m_ylablen = 0;
  m_xlablen = 0;
  m_Xlabels.clear();
  m_Ylabels.clear();
  m_nz = 0;
  m_zlablen = 0;
  m_Zlabels.clear();
  switch (m_type)
  {
    case H_1DIM:
    {
      m_buffersize = 2*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,m_Xlabels);
      m_hdrlen += m_xlablen;
      break;
    }
    case H_PROFILE:
    case H_RATE:
    {
      m_buffersize = 4*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,m_Xlabels);
      m_hdrlen += m_xlablen;
      break;
    }
    case H_2DIM:
    {
      m_ny = m_Yaxis->GetNbins();
      m_ymin = m_Yaxis->GetXmin();
      m_ymax = m_Yaxis->GetXmax();
      m_hdrlen = sizeof(DimHistbuff2)+titlen()+1+namelength()+1;
      m_buffersize = 2*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,m_Xlabels);
      m_ylablen = GetBinLabels(m_Yaxis,m_Ylabels);
      m_hdrlen += m_xlablen;
      m_hdrlen += m_ylablen;
      break;
    }
    case H_3DIM:
    {
      m_ny = m_Yaxis->GetNbins();
      m_ymin = m_Yaxis->GetXmin();
      m_ymax = m_Yaxis->GetXmax();
      m_nz = m_Zaxis->GetNbins();
      m_zmin = m_Zaxis->GetXmin();
      m_zmax = m_Zaxis->GetXmax();
      m_hdrlen = sizeof(DimHistbuff3)+titlen()+1+namelength()+1;
      m_buffersize = 2*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,m_Xlabels);
      m_ylablen = GetBinLabels(m_Yaxis,m_Ylabels);
      m_zlablen = GetBinLabels(m_Zaxis,m_Zlabels);
      m_hdrlen += m_xlablen;
      m_hdrlen += m_ylablen;
      m_hdrlen += m_zlablen;
      break;
    }
    case H_2DPROFILE:
    {
      m_ny = m_Yaxis->GetNbins();
      m_ymin = m_Yaxis->GetXmin();
      m_ymax = m_Yaxis->GetXmax();
      m_nz = m_Zaxis->GetNbins();
      m_zmin = m_Zaxis->GetXmin();
      m_zmax = m_Zaxis->GetXmax();
      m_hdrlen = sizeof(DimHistbuff3)+titlen()+1+namelength()+1;
      m_buffersize = 4*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,m_Xlabels);
      m_ylablen = GetBinLabels(m_Yaxis,m_Ylabels);
      m_zlablen = GetBinLabels(m_Zaxis,m_Zlabels);
      m_hdrlen += m_xlablen;
      m_hdrlen += m_ylablen;
      m_hdrlen += m_zlablen;
      break;
    }
    default:
    {
      return;
    }
  }
  m_hdrlen = (m_hdrlen + 7)&(~7);
  m_xmitbuffersize = m_hdrlen+m_buffersize;
  return;
}

MonHist::~MonHist()
{
  Bool_t dirstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  m_Xlabels.clear();
  m_Ylabels.clear();
  m_xlablen = 0;
  m_ylablen = 0;
  if (m_rootobj !=0)
  {
    if (m_type == H_RATE) delete m_rootobj;
    m_rootobj = 0;
  }
  if (m_rootdeser != 0)
  {
    delete m_rootdeser;
    m_rootdeser = 0;
  }
  TH1::AddDirectory(dirstat);
}

int MonHist::GetBinLabels(TAxis *ax, std::vector<std::string> &labs)
{
  int l=0;
  int i;
  int nbin=ax->GetNbins();
  for (i = 1; i < (nbin+1) ; ++i)
  {
    char *binLab = (char*)ax->GetBinLabel(i);
    l += strlen(binLab);
  }
  if (l>0)
  {
    for (i=1;i<nbin+1;i++)
    {
      labs.push_back(ax->GetBinLabel(i));
    }
    l+= nbin+1;
  }
  return l;
}
void MonHist::SetBinLabels(TAxis *ax, char *labs)
{
  int nbin=ax->GetNbins();
  char *lab = labs;
  for (int i = 1; i < (nbin+1) ; ++i)
  {
    ax->SetBinLabel(i,lab);
    lab = (char*)AddPtr(lab,strlen(lab)+1);
  }
}

int MonHist::xmitbuffersize()
{
  resetup();
  return m_xmitbuffersize;
}

void *MonHist::cpyName(void *ptr)  const
{
  ::memcpy(ptr,m_name.c_str(),m_name.length()+1);
  return AddPtr(ptr,m_name.length());
}

void *MonHist::cpytitle(void *ptr)  const
{
  ::memcpy(ptr,m_title.c_str(),m_title.length()+1);
  return AddPtr(ptr,m_title.length());
}

void MonHist::clear()
{
  if (m_rootobj == 0)
    return;
  switch (m_type)
  {
    case H_1DIM:
    {
      ((TH1D*) m_rootobj)->Reset();
      break;
    }
    case H_2DIM:
    {
      ((TH2D*) m_rootobj)->Reset();
      break;
    }
    case H_3DIM:
    {
      ((TH3D*) m_rootobj)->Reset();
      break;
    }
    case H_PROFILE:
    case H_RATE:
    {
      ((TProfile*) m_rootobj)->Reset();
      break;
    }
    case H_2DPROFILE:
    {
      ((TProfile2D*) m_rootobj)->Reset();
      break;
    }
    case C_STATENT:
    {
      ((StatEntity*) m_rootobj)->reset();
      break;
    }
    default:
    {
      break;
    }
  }
}

int MonHist::serialize(void* &ptr)
{
  int siz;
//  int status;
  if (m_type == H_RATE)
  {
    this->makeCounters();
    if (this->m_rootobj == 0)
    {
      return 0;
    }
  }
  if (m_type == C_STATENT)
  {
    DimStatBuff *pp = (DimStatBuff*)ptr;
    pp->type = m_type;
    pp->dataoff = 0;
    pp->reclen = this->m_xmitbuffersize;
    pp->namelen = namelength();
    pp->titlen = titlen();
    StatEntity *s = (StatEntity*)this->m_rootobj;
    char *tit;
    char *nam;
    pp->nentries = s->nEntries();
    pp->m_sumw = s->flag();
    pp->m_sumw2 = s->flag2();
    pp->m_min = s->flagMin();
    pp->m_max = s->flagMax();
    pp->nameoff = sizeof(DimStatBuff);
    pp->titoff = pp->nameoff+pp->namelen;
    nam = (char*)AddPtr(pp,pp->nameoff);
    tit = (char*)AddPtr(pp,pp->titoff);
    cpyName(nam);
    cpytitle(tit);
    siz = m_xmitbuffersize;
    ptr = (void*)((char*)ptr+siz);
    return siz;
  }
  DimBuffBase *pill = (DimBuffBase*)ptr;
  if(m_type == H_ILLEGAL)
  {
    pill->type = (int)H_ILLEGAL;
    pill->reclen = sizeof(*pill);
    pill->nameoff=sizeof(*pill);
    pill->namelen = 0;
    pill->titoff = sizeof(*pill);
    pill->titlen = 0;
    pill->flags = 0;
    siz = m_xmitbuffersize;
    return siz;
  }

  DimHistbuff1 *pp = (DimHistbuff1*)ptr;
  DimHistbuff2 *pp2 = (DimHistbuff2*)ptr;
  DimHistbuff3 *pp3 = (DimHistbuff3*)ptr;
  pp->nxbin = m_nx;
  pp->xmin  = m_xmin;
  pp->xmax  = m_xmax;
  int titl = titlen();
  int naml = namelength();
  int hdrl = hdrlen();
  siz = m_xmitbuffersize;
  pp->dataoff = hdrl;
  pp->addoffset = m_addoff;
  pp->reclen = siz;
  pp->titlen = titl;
  pp->namelen = naml;
  pp->xlablen = m_xlablen;
  pp->type = m_type;
  char *tit=0;
  char *nam=0;
  char *xtits=0;
  char *ytits=0;
  char *ztits=0;
  double *ntries=0;
  double *errp=0;
//  int status;
//  double *sumbin;
  switch(m_type)
  {
    case H_3DIM:
    {
      pp3->nameoff = sizeof(DimHistbuff3);
      pp3->dim = 3;
      pp3->titoff  = pp->nameoff+namelength();
      pp3->xlaboff = pp->titoff+pp->titlen;
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      ntries = (double*)AddPtr(ptr,pp->dataoff);
      errp  = (double*)AddPtr(ntries,m_blocksize);
      xtits = (char*)AddPtr(pp,pp->xlaboff);
      pp3->nybin = m_ny;
      pp3->ymin  = m_ymin;
      pp3->ymax  = m_ymax;
      pp3->ylablen = m_ylablen;
      pp3->ylaboff = pp->xlaboff+pp2->xlablen;
      ytits = (char*)AddPtr(pp2,pp2->ylaboff);
      pp3->nzbin = m_nz;
      pp3->zmin  = m_zmin;
      pp3->zmax  = m_zmax;
      pp3->zlablen = m_zlablen;
      pp3->zlaboff = pp2->ylaboff+pp2->ylablen;
      ztits = (char*)AddPtr(pp3,pp3->zlaboff);
      break;
    }
    case H_2DPROFILE:
    {
      pp3->nameoff = sizeof(DimHistbuff3);
      pp3->dim = 3;
      pp3->titoff  = pp->nameoff+namelength();
      pp3->xlaboff = pp->titoff+pp->titlen;
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      ntries = (double*)AddPtr(ptr,pp->dataoff);
      errp  = (double*)AddPtr(ntries,m_blocksize);
      xtits = (char*)AddPtr(pp,pp->xlaboff);
      pp3->nybin = m_ny;
      pp3->ymin  = m_ymin;
      pp3->ymax  = m_ymax;
      pp3->ylablen = m_ylablen;
      pp3->ylaboff = pp->xlaboff+pp2->xlablen;
      ytits = (char*)AddPtr(pp2,pp2->ylaboff);
      pp3->nzbin = m_nz;
      pp3->zmin  = m_zmin;
      pp3->zmax  = m_zmax;
      pp3->zlablen = m_zlablen;
      pp3->zlaboff = pp2->ylaboff+pp2->ylablen;
      ztits = (char*)AddPtr(pp3,pp3->zlaboff);
      break;
    }
    case H_2DIM:
    {
      pp2->nameoff = sizeof(DimHistbuff2);
      pp2->dim = 2;
      pp2->titoff  = pp->nameoff+namelength();
      pp2->xlaboff = pp->titoff+pp->titlen;
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      ntries = (double*)AddPtr(ptr,pp->dataoff);
      errp  = (double*)AddPtr(ntries,m_blocksize);
      xtits = (char*)AddPtr(pp,pp->xlaboff);
      pp2->nybin = m_ny;
      pp2->ymin  = m_ymin;
      pp2->ymax  = m_ymax;
      pp2->ylablen = m_ylablen;
      pp2->ylaboff = pp->xlaboff+pp2->xlablen;
      ytits = (char*)AddPtr(pp2,pp2->ylaboff);
      break;
    }
    case H_1DIM:
    {
      pp->nameoff = sizeof(DimHistbuff1);
      pp->titoff  = pp->nameoff+namelength();
      pp->xlaboff = pp->titoff+pp->titlen;
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      ntries = (double*)AddPtr(ptr,pp->dataoff);
      errp  = (double*)AddPtr(ntries,m_blocksize);
      xtits = (char*)AddPtr(pp,pp->xlaboff);
      pp->dim = 1;
      break;
    }
    case H_PROFILE:
    case H_RATE:
    {
      pp->nameoff = sizeof(DimHistbuff1);
      pp->titoff  = pp->nameoff+namelength();
      pp->xlaboff = pp->titoff+pp->titlen;
      nam = (char*)AddPtr(pp,pp->nameoff);
      tit = (char*)AddPtr(pp,pp->titoff);
      xtits = (char*)AddPtr(pp,pp->xlaboff);
      pp->dim = 1;
     break;
    }
    default:
    {
      break;
    }
  }
  cpyName(nam);
  cpytitle(tit);
  switch (m_type)
  {
    case   H_1DIM:
    {
      cpyBinLabels(xtits,m_Xlabels,m_nx);
      MyTH1D *h=(MyTH1D*)m_rootobj;
      pp->nentries = h->GetEntries();
      h->movetodimbuffer(AddPtr(ptr,pp->dataoff));
//      status  = 0;
      break;
    }
    case H_2DIM:
    {
      cpyBinLabels(xtits,m_Xlabels,m_nx);
      MyTH2D *h=(MyTH2D*)m_rootobj;
      pp->nentries = h->GetEntries();
      h->movetodimbuffer(AddPtr(ptr,pp->dataoff));
      cpyBinLabels(ytits,m_Ylabels,m_ny);
//      status  = 0;
      break;
    }
    case H_3DIM:
    {
      cpyBinLabels(xtits,m_Xlabels,m_nx);
      MyTH3D *h=(MyTH3D*)m_rootobj;
      pp->nentries = h->GetEntries();
      h->movetodimbuffer(AddPtr(ptr,pp->dataoff));
      cpyBinLabels(ytits,m_Ylabels,m_ny);
      cpyBinLabels(ztits,m_Zlabels,m_nz);
//      status  = 0;
      break;
    }
    case H_PROFILE:
    case H_RATE:
    {
      cpyBinLabels(xtits,m_Xlabels,m_nx);
      MyTProfile *h =  (MyTProfile*)m_rootobj;
      pp->nentries = h->GetEntries();


      h->movetodimbuffer(AddPtr(ptr,pp->dataoff));
      pp->yminval = ((MyTProfile*)m_rootobj)->fYmin;
      pp->ymaxval = ((MyTProfile*)m_rootobj)->fYmax;
//      status  = 0;
      break;
    }

    case H_2DPROFILE:
    {
      MyTProfile2D *h =  (MyTProfile2D*)m_rootobj;
      cpyBinLabels(xtits,m_Xlabels,m_nx);
      pp->nentries = h->GetEntries();
      cpyBinLabels(ytits,m_Ylabels,m_ny);
      cpyBinLabels(ztits,m_Zlabels,m_nz);
      pp->nentries = h->GetEntries();
      h->movetodimbuffer(AddPtr(ptr,pp->dataoff));
      pp3->zmin = h->GetZmin();
      pp3->zmax = h->GetZmax();
      pp3->zminval = h->fZmin;
      pp3->zmaxval = h->fZmax;
//      status  = 0;
      break;
    }
    default:
    {
//      status  = -2;
      siz = 0;
      break;
    }
  }
  ptr = (void*)((char*)ptr+siz);
  return siz;
}
//void MonHist::cpyBinLabels(char* dst, char **src, int nlab)
//{
//  if (src == 0 ) return;
//  int i;
//  int leni;
//  for (i=0;i<nlab;i++)
//  {
//    leni = strlen(src[i]);
//    strcpy(dst,src[i]);
//    dst[leni] = 0;
//    dst += leni+1;
//  }
//  return;
//}

void MonHist::cpyBinLabels(char* dst, std::vector<std::string> &src, int nlab)
{
  if (src.empty() ) return;
  int i;
  int leni;
  for (i=0;i<nlab;i++)
  {
    leni = strlen(src[i].c_str());
    strcpy(dst,src[i].c_str());
    dst[leni] = 0;
    dst += leni+1;
  }
  return;
}

void MonHist::List()
{
  std::string typ;
  switch (m_type)
  {
    case H_1DIM:
    {
      typ="H_1DIM";
      break;
    }
    case H_2DIM:
    {
      typ="H_2DIM";
      break;
    }
    case H_3DIM:
    {
      typ="H_3DIM";
      break;
    }
    case H_PROFILE:
    {
      typ="H_PROFILE";
      break;
    }
    case H_2DPROFILE:
    {
      typ="H_2DPROFILE";
      break;
    }
    case H_RATE:
    {
      typ="H_RATE";
      break;
    }
    default:
    {
      typ="UNKNOWN";
    }
  }
//  printf("Histogram Type: %s Bins(%i,%i) Name: %s\n",typ.c_str(),m_nx, m_ny, name());
}


void *MonHist::de_serialize(void *ptr, char *nam)
{
  Bool_t dirstat = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  DimBuffBase *p = (DimBuffBase*)ptr;
  double *mhentries,*mhsumw2;
  int mblocksize;
//  this->m_type = (MONTYPE)p->type;
  if (nam == 0)
  {
    nam = (char*)AddPtr(p,p->nameoff);
  }
  switch (p->type)
  {
    case  H_1DIM:
    {
      DimHistbuff1 *b=(DimHistbuff1*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
      std::string titopt = tit;
      std::string bopt;
      size_t seppos;
      seppos = titopt.find(MonHist::optsep);
      if (seppos != std::string::npos)
      {
        bopt = titopt.substr(seppos+MonHist::optsep.length());
        titopt.erase(seppos);
      }
      MyTH1D *h = (MyTH1D*)new TH1D(nam,titopt.c_str(),b->nxbin,b->xmin,b->xmax);
      h->SetEntries( b->nentries);
      h->movefromdimbuffer(AddPtr(b,b->dataoff));
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
      TH1::AddDirectory(dirstat);
      return h;
      break;
    }
    case  H_2DIM:
    {
      DimHistbuff2 *b=(DimHistbuff2*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
      std::string titopt = tit;
      std::string bopt;
      size_t seppos;
      seppos = titopt.find(MonHist::optsep);
      if (seppos != std::string::npos)
      {
        bopt = titopt.substr(seppos+MonHist::optsep.length());
        titopt.erase(seppos);
      }
      MyTH2D *h = (MyTH2D*)new TH2D(nam,titopt.c_str(),b->nxbin,b->xmin,b->xmax,b->nybin,b->ymin,b->ymax);
      h->SetEntries( b->nentries);
      h->movefromdimbuffer(AddPtr(b,b->dataoff));
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
      if (b->ylablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->ylaboff);
        SetBinLabels(&h->fYaxis,xl);
      }
      TH1::AddDirectory(dirstat);
      return h;
      break;
    }
    case  H_3DIM:
    {
      DimHistbuff3 *b=(DimHistbuff3*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
      std::string titopt = tit;
      std::string bopt;
      size_t seppos;
      seppos = titopt.find(MonHist::optsep);
      if (seppos != std::string::npos)
      {
        bopt = titopt.substr(seppos+MonHist::optsep.length());
        titopt.erase(seppos);
      }
      MyTH3D *h = (MyTH3D*)new TH3D(nam,titopt.c_str(),b->nxbin,b->xmin,b->xmax,b->nybin,b->ymin,b->ymax,b->nzbin,b->zmin,b->zmax);
      h->SetEntries( b->nentries);
      h->movefromdimbuffer(AddPtr(b,b->dataoff));
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
      if (b->ylablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->ylaboff);
        SetBinLabels(&h->fYaxis,xl);
      }
      if (b->zlablen > 0)
      {
        char *zl = (char*)AddPtr(b,b->zlaboff);
        SetBinLabels(&h->fZaxis,zl);
      }
      TH1::AddDirectory(dirstat);
      return h;
      break;
    }
    case  H_PROFILE:
    case  H_RATE:
    {
      DimHistbuff1 *b=(DimHistbuff1*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
      std::string titopt = tit;
      std::string bopt;
      size_t seppos;
      seppos = titopt.find(MonHist::optsep);
      if (seppos != std::string::npos)
      {
        bopt = titopt.substr(seppos+MonHist::optsep.length());
        titopt.erase(seppos);
      }
      MyTProfile *h = (MyTProfile*)new TProfile(nam,titopt.c_str(),
          b->nxbin,b->xmin,b->xmax,bopt.c_str());
      h->movefromdimbuffer(AddPtr(b,b->dataoff));
      h->SetEntries( b->nentries);
      h->fYmin = b->yminval;
      h->fYmax = b->ymaxval;
      if (h->fYmin == h->fYmax) h->fYmax++;
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
      TH1::AddDirectory(dirstat);
      return h;
      break;
    }



    case  H_2DPROFILE:
    {
      DimHistbuff3 *b=(DimHistbuff3*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
      std::string titopt = tit;
      std::string bopt;
      size_t seppos;
      seppos = titopt.find(MonHist::optsep);
      if (seppos != std::string::npos)
      {
        bopt = titopt.substr(seppos+MonHist::optsep.length());
        titopt.erase(seppos);
      }
      MyTProfile2D *h = (MyTProfile2D*)new TProfile2D(nam,titopt.c_str(),
          b->nxbin,b->xmin,b->xmax,b->nybin,b->ymin,b->ymax,b->zmin,b->zmax,bopt.c_str());
      h->movefromdimbuffer(AddPtr(b,b->dataoff));
      h->SetEntries( b->nentries);
      h->fZmin = b->zminval;
      h->fZmax = b->zmaxval;
      if (h->fZmin == h->fZmax) h->fZmax++;
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
      if (b->ylablen > 0)
      {
        char *yl = (char*)AddPtr(b,b->ylaboff);
        SetBinLabels(&h->fYaxis,yl);
      }
      TH1::AddDirectory(dirstat);
      return h;
      break;
    }




    case  C_STATENT:
    {
      DimStatBuff *b = (DimStatBuff*)ptr;
      StatEntity *s = new StatEntity ( (unsigned long) b->nentries ,
                   b->m_sumw    ,
                   b->m_sumw2   ,
                   b->m_min ,
                   b->m_max ) ;
      TH1::AddDirectory(dirstat);
      return s;
      break;
    }
  }
  TH1::AddDirectory(dirstat);
  return 0;
}
