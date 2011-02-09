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
/*
#include "TProfile.h"
#include "TH1D.h"
#include "TH2D.h"
*/
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "Gaucho/dimhist.h"
#include "Gaucho/CntrMgr.h"
#define AddPtr(ptr,offs) (void*)((char*)ptr +offs)

//#include "AIDA/IProfile1D.h"
MonHist::MonHist()
{
  m_msgsvc = 0;
  m_cntrmgr = 0;
  m_addoff = 0;
  m_type = H_ILLEGAL;
  m_rootobj = 0;
  m_rootdeser = 0;
  m_name = 0;
  m_xmitbuffersize = 0;
  m_Xlabels = 0;
  m_Ylabels = 0;
  m_title = 0;
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();

}
MonHist::MonHist(IMessageSvc* msgs, const std::string& source, CntrMgr *cm)
{
//  setup(msgs, source,aidahist);
  m_msgsvc = msgs;
  m_cntrmgr = cm;
  m_addoff = 9;
  m_type = H_RATE;
  m_rootobj = 0;
  m_rootdeser = 0;
  m_name = (char*)source.c_str();
  this->m_xmitbuffersize = 0;
}

MonHist::MonHist(IMessageSvc* msgSvc, const std::string& source, const std::string &desc, const StatEntity *se)
{
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
  TProfile *tp;
//  MsgStream msg(m_msgsvc,"MonitorSvc");
  if (m_cntrmgr != 0)
  {
    if (m_cntrmgr->m_newcounter)
    {
      if (m_rootobj != 0)
      {
        delete m_rootobj;
      }
      m_rootobj = tp = new TProfile("COUNTER_TO_RATE", "Counter to rate Profile",
          m_cntrmgr->m_counterMap.size()+8,0.0,m_cntrmgr->m_counterMap.size()+8);
    }
    tp = (TProfile*)m_rootobj;
    if (tp == 0)
    {
      return;
    }
    tp->Reset();

//    printf("makeCounters: m_cntrmgr->offsetTimeFirstEvInRun() = %X %f\n",m_cntrmgr->m_offsetTimeFirstEvInRun,m_cntrmgr->offsetTimeFirstEvInRun());
//    printf("makeCounters: m_cntrmgr->offsetTimeLastEvInCycle() = %X %f\n",m_cntrmgr->m_offsetTimeLastEvInCycle,m_cntrmgr->offsetTimeLastEvInCycle());
//    printf("makeCounters: m_cntrmgr->offsetGpsTimeLastEvInCycle() = %X %f\n",m_cntrmgr->m_offsetGpsTimeLastEvInCycle,m_cntrmgr->offsetGpsTimeLastEvInCycle());
//    printf("makeCounters: m_cntrmgr->runNumber() = %X %d\n",m_cntrmgr->m_runNumber,m_cntrmgr->runNumber());
//    printf("makeCounters: m_cntrmgr->triggerConfigurationKey() = %X %x\n",m_cntrmgr->m_triggerConfigurationKey,m_cntrmgr->triggerConfigurationKey());
//    printf("makeCounters: m_cntrmgr->cycleNumber() = %X %d\n",m_cntrmgr->m_cycleNumber,m_cntrmgr->cycleNumber());
//    printf("makeCounters: m_cntrmgr->deltaT() = %X %f\n",m_cntrmgr->m_deltaT,m_cntrmgr->deltaT());



    tp->Fill(0.50, m_cntrmgr->offsetTimeFirstEvInRun(), 1.00);
    tp->Fill(1.50, m_cntrmgr->offsetTimeLastEvInCycle(), 1.00);
    tp->Fill(2.50, m_cntrmgr->offsetGpsTimeLastEvInCycle(), 1.00);
    tp->Fill(3.50, 1.00, 1.00);
    tp->Fill(4.50, (double) (m_cntrmgr->runNumber()), 1.00);
    tp->Fill(5.50, (double) (m_cntrmgr->triggerConfigurationKey()), 1.00);
    tp->Fill(6.50, (double) (m_cntrmgr->cycleNumber()), 1.00);
    tp->Fill(7.50, (double) (m_cntrmgr->deltaT()), 1.00);

    int i = 9;
    for (m_cntrmgr->m_counterMapIt = m_cntrmgr->m_counterMap.begin(); m_cntrmgr->m_counterMapIt != m_cntrmgr->m_counterMap.end(); m_cntrmgr->m_counterMapIt++)
    {
       if (m_cntrmgr->m_counterMapIt->second.second.first.compare("int") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is int ");
         tp->Fill((double)i - 0.5, (*(int*)(m_cntrmgr->m_counterMapIt->second.second.second)), 1.00);
       }
       else if (m_cntrmgr->m_counterMapIt->second.second.first.compare("double") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is double ");
//         msg <<MSG::DEBUG<<"Counter :" << m_cntrmgr->m_counterMapIt->first << " is double "<< endreq;
//   msg <<MSG::DEBUG<<"Counter :" << (*(double*)(m_cntrmgr->m_counterMapIt->second.second.second)) << endreq;
         tp->Fill((double)i - 0.5, (*(double*)(m_cntrmgr->m_counterMapIt->second.second.second)), 1.00);
       }
       else if (m_cntrmgr->m_counterMapIt->second.second.first.compare("long") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is long ");
//         msg <<MSG::DEBUG<<"Counter :" << m_cntrmgr->m_counterMapIt->first << " is long  "<< endreq;
         tp->Fill((double)i - 0.5, (*(long*)(m_cntrmgr->m_counterMapIt->second.second.second)), 1.00);
       }
       else if (m_cntrmgr->m_counterMapIt->second.second.first.compare("StatEntityflag") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is  StatEntity (flag) ");
//         msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (flag) "<< endreq;
         StatEntity counter = (*(StatEntity*) m_cntrmgr->m_counterMapIt->second.second.second);
         tp->Fill((double)i - 0.5, counter.flag(), 1.00);
//         msg <<MSG::DEBUG<<"Counter :" << counter.flag() << endreq;
       }
       else if (m_cntrmgr->m_counterMapIt->second.second.first.compare("StatEntitynEntries") ==0 )
       {
//         printf("%s %s %s\n","Counter :", m_cntrmgr->m_counterMapIt->first.c_str(), " is  StatEntity (nEntries) ");
//         msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (nEntries) "<< endreq;
         StatEntity counter = (*(StatEntity*) m_cntrmgr->m_counterMapIt->second.second.second);
         tp->Fill((double)i - 0.5, counter.nEntries(), 1.00);
//         msg <<MSG::DEBUG<<"Counter :" << counter.nEntries() << endreq;
       }
       else
       {
//         msg <<MSG::ERROR<<"Incompatible counter type for MonRate.." << endreq;
       }
       i++;
     }

     tp->GetXaxis()->SetBinLabel(1, "OffsetTimeFirstEvInRun");
     tp->GetXaxis()->SetBinLabel(2, "OffsetTimeLastEvInCycle");
     tp->GetXaxis()->SetBinLabel(3, "OffsetGpsTimeLastEvInCycle");
     tp->GetXaxis()->SetBinLabel(4, "NumberofProcesses"); // actually identical to 1 for each process. adders sum it up to give the number of processes...
     tp->GetXaxis()->SetBinLabel(5, "RunNumber");
     tp->GetXaxis()->SetBinLabel(6, "TCK");
     tp->GetXaxis()->SetBinLabel(7, "CycleNumber");
     tp->GetXaxis()->SetBinLabel(8, "deltaT");

     i = 9;
     for (m_cntrmgr->m_counterMapIt = m_cntrmgr->m_counterMap.begin(); m_cntrmgr->m_counterMapIt != m_cntrmgr->m_counterMap.end(); m_cntrmgr->m_counterMapIt++)
     {
//       msg <<MSG::DEBUG<<"label description: " << (*(m_cntrmgr->m_counterMapIt->second.first)).c_str() << endreq;
       tp->GetXaxis()->SetBinLabel(i, (*(m_cntrmgr->m_counterMapIt->second.first)).c_str());
//       printf("Rate Axis Label for bin %d %s\n",i, (*(m_cntrmgr->m_counterMapIt->second.first)).c_str());
       i++;
     }
     this->setup(m_msgsvc);
   }
}
void MonHist::setup(IMessageSvc* msgs, const std::string& source, const AIDA::IBaseHistogram *aidahist)
{
  MsgStream msg(msgs,"MonitorSvc");
  m_cntrmgr = 0;
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
	else
	{
	  msg << MSG::ERROR << "Unknown histogram type. Source " << source << endreq;
	  m_rootobj = 0;
	  return;
	}
  setup(msgs);
	return;
}
void MonHist::setup(IMessageSvc* msgs, const std::string& source, const std::string &desc, const StatEntity *se)
{
  MsgStream msg(msgs,"MonitorSvc");
  m_cntrmgr = 0;
  m_type = C_STATENT;
  m_rootobj = (TObject*)se;
  m_hentries = 0;
  m_hsumw2 = 0;
  m_hsumw = 0;
  m_name = (char*)source.c_str();
  m_namelen = strlen(m_name);
  m_name = (char*)malloc(m_namelen);
  strcpy(m_name,source.c_str());
  m_title = (char*)desc.c_str();
  m_titlen = strlen(m_title);
  m_title = (char*)malloc(m_titlen);
  strcpy(m_title,desc.c_str());
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
    case H_RATE:
    case C_INT:
    case C_LONGLONG:
    case C_FLOAT:
    case C_DOUBLE:
    case C_STATENT:
    case H_ILLEGAL:
    {
      return;
      break;
    }
    case H_1DIM:
    case H_2DIM:
    case H_PROFILE:
    {
      if (m_Xlabels != 0)
      {
        free (m_Xlabels);
        m_Xlabels = 0;
      }
      if (m_Ylabels != 0)
      {
        free (m_Ylabels);
        m_Ylabels = 0;
      }
      m_xlablen = 0;
      m_ylablen = 0;
      setup(this->m_msgsvc);
    }
  }
}

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
      m_hentries = rhist->GetEntryArr();
      m_hsumw2 = rhist->GetSumw2Arr();
      m_hbinsumw2 = rhist->GetBinSumw2Arr();
      m_hsumw = rhist->GetSumwArr();
      m_rootobj = rhist;
      m_Xaxis = rhist->GetXaxis();
      m_name = (char*)rhist->GetName();
      m_namelen = strlen(m_name);
      m_title = (char*)rhist->GetTitle();
      m_titlen = strlen(m_title);
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
      m_name = (char*)rhist->GetName();
      m_namelen = strlen(m_name);
      m_title = (char*)rhist->GetTitle();
      m_titlen = strlen(m_title);
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff1)+titlen()+1+namelength()+1;
      break;
    }
    case H_2DIM:
    {
      m_type = H_2DIM;
      MyTH2D *rhist = (MyTH2D*)m_rootobj;
      m_hentries = rhist->GetEntryArr();
      m_hsumw2 = rhist->GetSumw2Arr();
      m_rootobj = rhist;
      m_name = (char*)rhist->GetName();
      m_namelen = strlen(m_name);
      m_title = (char*)rhist->GetTitle();
//      printf("+++++++++++++++++++++++++++++++2Dim Histogram. Name %s, Title %s \n",m_name, m_title);
      m_titlen = strlen(m_title);
      m_Xaxis =  rhist->GetXaxis();
      m_Yaxis = rhist->GetYaxis();
      m_blocksize = rhist->fN*sizeof(double);
      m_hdrlen = sizeof(DimHistbuff2)+titlen()+1+namelength()+1;
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
  m_Xlabels = 0;
  m_Ylabels = 0;
  switch (m_type)
  {
    case H_1DIM:
    {
      m_buffersize = 2*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,&m_Xlabels);
      m_hdrlen += m_xlablen;
      break;
    }
    case H_PROFILE:
    case H_RATE:
    {
      m_buffersize = 4*m_blocksize;
      m_xlablen = GetBinLabels(m_Xaxis,&m_Xlabels);
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
      m_xlablen = GetBinLabels(m_Xaxis,&m_Xlabels);
      m_ylablen = GetBinLabels(m_Yaxis,&m_Ylabels);
      m_hdrlen += m_xlablen;
      m_hdrlen += m_ylablen;
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
  if (m_Xlabels != 0) free(m_Xlabels);
  if (m_Ylabels != 0) free(m_Ylabels);
  if (m_type == C_STATENT)
  {
    if (m_name != 0)
    {
      free(m_name);
      m_name = 0;
    }
    if (m_title != 0)
    {
      free(m_title);
      m_title = 0;
    }
  }
  if (m_rootobj !=0)
  {
    delete m_rootobj;
    m_rootobj = 0;
  }
  if (m_rootdeser != 0)
  {
    delete m_rootdeser;
    m_rootdeser = 0;
  }
}

int MonHist::GetBinLabels(TAxis *ax, char ***labs)
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
    *labs = (char**)malloc(nbin*sizeof(char*));
    char **lab = *labs;
    for (i=1;i<nbin+1;i++)
    {
      (lab)[i-1] = (char*)ax->GetBinLabel(i);
//      printf("Bin %d Label %s\n",i,lab[i-1]);
    }
    l+= nbin+1;
  }
  return l;
}
void MonHist::SetBinLabels(TAxis *ax, char *labs)
{
  int i;
  int nbin=ax->GetNbins();
  char *lab = labs;
  for (i = 1; i < (nbin+1) ; ++i)
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

void *MonHist::cpyName(void *ptr)
{
  strncpy((char*)ptr,m_name,this->m_namelen);
  ((char*)ptr)[m_namelen]=0;
  ptr = (char*)ptr+m_namelen;
  return ptr;
}

void *MonHist::cpytitle(void *ptr)
{
  memcpy((char*)ptr,m_title,m_titlen);
  ((char*)ptr)[m_titlen] = 0;
  ptr = (char*)ptr+m_titlen;
  return ptr;
}

void MonHist::clear()
{
	switch(m_type)
	{
		case H_1DIM:
		{
			((TH1D*)m_rootobj)->Reset();
			break;
		}
		case H_2DIM:
		{
			((TH2D*)m_rootobj)->Reset();
			break;
		}
		case H_PROFILE:
		case H_RATE:
		{
			((TProfile*) m_rootobj)->Reset();
			break;
		}
		case C_STATENT:
		{
		  ((StatEntity*)m_rootobj)->reset();
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
  int status;
  if (m_type == H_RATE)
  {
//    printf("Serializing a RATE... calling makeCounters\n");
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
  DimHistbuff1 *pp = (DimHistbuff1*)ptr;
  DimHistbuff2 *pp2 = (DimHistbuff2*)ptr;
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
  char *tit;
  char *nam;
  char *xtits;
  char *ytits;
  double *ntries;
  double *errp;
//  double *sumbin;
  switch(m_type)
  {
    case H_2DIM:
    {
      pp->nameoff = sizeof(DimHistbuff2);
      pp->dim = 2;
      pp->titoff  = pp->nameoff+namelength();
      pp->xlaboff = pp->titoff+pp->titlen;
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
//      errp  = (double*)AddPtr(ntries,m_blocksize);
      xtits = (char*)AddPtr(pp,pp->xlaboff);
      pp->dim = 1;
//      if (m_type == H_RATE)
//      {
//        printf ("Array Size %d Blocksize = %d ",((TProfile*)m_rootobj)->fN,m_blocksize);
//        ((TProfile*)m_rootobj)->Print("range");
//      }
     break;
    }
    default:
    {
      break;
    }
  }
  cpyName(nam);
  cpytitle(tit);
  cpyBinLabels(xtits,m_Xlabels,m_nx);
  switch (m_type)
  {
    case   H_1DIM:
    {
      pp->nentries = ((TH1D*)m_rootobj)->GetEntries();
      memcpy(ntries,m_hentries,m_blocksize);
      memcpy(errp,m_hsumw2,m_blocksize);
      status  = 0;
      break;
    }
    case H_2DIM:
    {
      pp->nentries = ((TH2D*)m_rootobj)->GetEntries();
      memcpy(ntries,m_hentries,m_blocksize);
      memcpy(errp,m_hsumw2,m_blocksize);
      status  = 0;
      break;
    }
    case H_PROFILE:
    case H_RATE:
    {
      MyTProfile *h =  (MyTProfile*)m_rootobj;
      pp->nentries = ((TProfile*)m_rootobj)->GetEntries();


      double *ents = (double*)AddPtr(pp,pp->dataoff); //0
      double *sum = (double*)AddPtr(ents,m_blocksize); //1
      double *sum2 = (double*)AddPtr(sum,m_blocksize); //2
      double *bsum2 = (double*)AddPtr(sum2,m_blocksize); //3
      double *hsum=h->fArray;
      double *hsum2 = h->fSumw2.fArray;
      double *hbines = h->fBinEntries.fArray;
      double *hbinsum2 = h->fBinSumw2.fArray;
      memcpy(ents,hbines,m_blocksize);
      memcpy(sum,hsum,m_blocksize);
      memcpy(sum2,hsum2,m_blocksize);
      memcpy(bsum2,hbinsum2,m_blocksize);





      pp->yminval = ((MyTProfile*)m_rootobj)->fYmin;
      pp->ymaxval = ((MyTProfile*)m_rootobj)->fYmax;
//      if (m_type == H_RATE)
//      {
//        MyTProfile *hh = (MyTProfile*)m_rootobj;
//        hh->Dump();
//        MyTArrayD* a;
//        a = (MyTArrayD*)&(hh->fBinEntries);
//        a->Dump((char*)"fBinEntries");
//        a = (MyTArrayD*)&(hh->fSumw2);
//        a->Dump((char*)"fSumw2");
//        a = (MyTArrayD*)&(hh->fBinSumw2);
//        a->Dump((char*)"fBinSumw2");
//      }
      status  = 0;
      break;
    }

    default:
    {
      status  = -2;
      siz = 0;
      break;
    }
  }
  ptr = (void*)((char*)ptr+siz);
  return siz;
}
void MonHist::cpyBinLabels(char* dst, char **src, int nlab)
{
  if (src == 0 ) return;
  int i;
  int leni;
  for (i=0;i<nlab;i++)
  {
    leni = strlen(src[i]);
    strcpy(dst,src[i]);
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
    case H_PROFILE:
    {
      typ="H_PROFILE";
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
//      if (m_rootdeser != 0)
//      {
//        delete m_rootdeser;
//        m_rootdeser = 0;
//      }
      MyTH1D *h = (MyTH1D*)new TH1D(nam,tit,b->nxbin,b->xmin,b->xmax);
//      this->m_rootdeser = h;
      mhentries = h->GetEntryArr();
      mhsumw2 = h->GetSumw2Arr();
//      m_hsumw = h->GetSumwArr();
      h->SetEntries( b->nentries);
      double *ents = (double*)AddPtr(b,b->dataoff);
      mblocksize = (b->nxbin+2)*sizeof(double);
      double *errp = (double*)AddPtr(ents,mblocksize);
      memcpy(mhentries,ents,mblocksize);
      memcpy(mhsumw2,errp,mblocksize);
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
      return h;
      break;
    }
    case  H_2DIM:
    {
      DimHistbuff2 *b=(DimHistbuff2*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
//      if (m_rootdeser != 0)
//      {
//        delete m_rootdeser;
//        m_rootdeser = 0;
//      }
      MyTH2D *h = (MyTH2D*)new TH2D(nam,tit,b->nxbin,b->xmin,b->xmax,b->nybin,b->ymin,b->ymax);
//      this->m_rootdeser  = h;
      mhentries = h->GetEntryArr();
      mhsumw2 = h->GetSumw2Arr();
//      m_hsumw = h->GetSumwArr();
      h->SetEntries( b->nentries);
      double *ents = (double*)AddPtr(b,b->dataoff);
      mblocksize = (b->nxbin+2)*(b->nybin+2)*sizeof(double);
      double *errp = (double*)AddPtr(ents,mblocksize);
      memcpy(mhentries,ents,mblocksize);
      memcpy(mhsumw2,errp,mblocksize);
      return h;
      break;
    }
    case  H_PROFILE:
    case  H_RATE:
    {
      DimHistbuff1 *b=(DimHistbuff1*)ptr;
      char *tit = (char*)AddPtr(b,b->titoff);
//      if (m_rootdeser != 0)
//      {
//        delete m_rootdeser;
//        m_rootdeser = 0;
//      }
      MyTProfile *h = (MyTProfile*)new TProfile(nam,tit,b->nxbin,b->xmin,b->xmax);
//      this->m_rootdeser  = h;
      mblocksize = (b->nxbin+2)*sizeof(double);
      double *ents = (double*)AddPtr(b,b->dataoff); //0
      double *sum = (double*)AddPtr(ents,mblocksize); //1
      double *sum2 = (double*)AddPtr(sum,mblocksize); //2
      double *bsum2 = (double*)AddPtr(sum2,mblocksize); //3
      double *hsum=h->fArray;
      double *hsum2 = h->fSumw2.fArray;
      double *hbines = h->fBinEntries.fArray;
      double *hbinsum2 = h->fBinSumw2.fArray;
      memcpy(hbines,ents,mblocksize);
      memcpy(hsum,sum,mblocksize);
      memcpy(hsum2,sum2,mblocksize);
      memcpy(hbinsum2,bsum2,mblocksize);
//      h->fBinEntries.Set(b->nxbin,ents);
//      h->fBinSumw2.Set(b->nxbin,sum2);
      h->SetEntries( b->nentries);
      h->fYmin = b->yminval;
      h->fYmax = b->ymaxval;
      if (h->fYmin == h->fYmax) h->fYmax++;
      if (b->xlablen > 0)
      {
        char *xl = (char*)AddPtr(b,b->xlaboff);
        SetBinLabels(&h->fXaxis,xl);
      }
//      if (p->type == H_RATE)
//      {
//        MyTProfile *hh = (MyTProfile*)m_rootdeser;
//        hh->Dump();
//        MyTArrayD* a;
//        a = (MyTArrayD*)&(hh->fBinEntries);
//        a->Dump((char*)"fBinEntries");
//        a = (MyTArrayD*)&(hh->fSumw2);
//        a->Dump((char*)"fSumw2");
//        a = (MyTArrayD*)&(hh->fBinSumw2);
//        a->Dump((char*)"fBinSumw2");
//      }

      return h;
      break;
    }
    case  C_STATENT:
    {
      DimStatBuff *b = (DimStatBuff*)ptr;
//      mtype = (MONTYPE)b->type;
      char *tit;
//      if (m_rootdeser != 0)
//      {
//        delete m_rootdeser;
//        m_rootdeser = 0;
//      }
      StatEntity *s = new StatEntity ( (const unsigned long) b->nentries ,
                   b->m_sumw    ,
                   b->m_sumw2   ,
                   b->m_min ,
                   b->m_max ) ;
      /// destructor
//      this->m_rootdeser = (TObject*)s;
      tit = (char*)AddPtr(b,b->titoff);
//      cpyName(nam);
//      cpytitle(tit);
      return s;
      break;
    }
  }
  return 0;
}
