//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineRootHist.cpp,v 1.10 2007-10-23 07:30:03 ggiacomo Exp $
#include "OnlineHistDB/OnlineRootHist.h"

OnlineRootHist::OnlineRootHist(std::string Identifier,
			       OnlineHistDB *Session,
			       std::string Page,
			       int Instance) :
  m_identifier(Identifier) , m_session(Session), m_dbHist(NULL), m_rootHist(NULL)
{
  if (Session) connectToDB(Session, Page, Instance);
}
	

OnlineRootHist::OnlineRootHist(OnlineHistogram*  oh) :
  m_identifier("") , m_session(NULL), m_dbHist(oh), m_rootHist(NULL)
{
  if(m_dbHist) 
    m_identifier = m_dbHist->identifier();
}


bool OnlineRootHist::setdbHist(OnlineHistogram*  oh) {
  bool out=false;
  bool isInit = (NULL == m_dbHist);
  if(oh) {
    if (oh->identifier() == m_identifier && false == oh->isAbort()) {
      m_dbHist = oh;
      if (m_rootHist && isInit) setTH1FromDB();
      out=true;
    }
    else {
      oh->errorMessage("provided OnlineHistogram object is not compatible");
    }
  }
  return out;
}


bool OnlineRootHist::setrootHist(TH1*  rh) {
  bool out=false;
  bool isInit = (NULL == m_rootHist);
  if(rh) {
    m_rootHist = rh;
    if (m_dbHist && isInit) setTH1FromDB();
    out=true;
  }
  return out;
}

bool OnlineRootHist::connectToDB(OnlineHistDB* Session,
		   std::string Page,
		   int Instance)
{
  bool out;
  if(Session) m_session = Session;
  if (m_session)
    out= setdbHist(m_session->getHistogram(m_identifier, Page, Instance));
  else
    out = false;
  return out;
}



void OnlineRootHist::setTH1FromDB() {
  if (m_dbHist && m_rootHist) {
    int iopt;
    float fopt;
    std::string sopt;
    if(m_dbHist->getDisplayOption("LABEL_X",   &sopt)) m_rootHist->SetXTitle (sopt.data());
    if(m_dbHist->getDisplayOption("LABEL_Y",   &sopt)) m_rootHist->SetYTitle (sopt.data());
    if(m_dbHist->getDisplayOption("LABEL_Z",   &sopt)) m_rootHist->SetZTitle (sopt.data());
    if(m_dbHist->getDisplayOption("YMIN",      &fopt)) m_rootHist->SetMinimum(fopt);
    if(m_dbHist->getDisplayOption("YMAX",      &fopt)) m_rootHist->SetMaximum(fopt);
    if(m_dbHist->getDisplayOption("FILLSTYLE", &iopt)) m_rootHist->SetFillStyle(iopt);
    if(m_dbHist->getDisplayOption("FILLCOLOR", &iopt)) m_rootHist->SetFillColor(iopt);
    if(m_dbHist->getDisplayOption("LINESTYLE", &iopt)) m_rootHist->SetLineStyle(iopt);
    if(m_dbHist->getDisplayOption("LINECOLOR", &iopt)) m_rootHist->SetLineColor(iopt);
    if(m_dbHist->getDisplayOption("LINEWIDTH", &iopt)) m_rootHist->SetLineWidth(iopt);
    if(m_dbHist->getDisplayOption("DRAWOPTS",  &sopt)) m_rootHist->SetOption(sopt.data());
    if(m_dbHist->getDisplayOption("STATS",     &iopt)) m_rootHist->SetStats(true);
  }
}

void OnlineRootHist::setDrawOptionsFromDB() {
  if(m_dbHist) {
    int iopt=0;
    //float fopt=0.;
    //std::string sopt="";
    if(m_dbHist->getDisplayOption("STATS"  ,   &iopt)) {
      gStyle->SetOptStat(iopt);
    }
  }
}


bool OnlineRootHist::saveTH1ToDB() {
  bool out = false;
  if (m_dbHist && m_rootHist) {
    int iopt=0;
    //float fopt=0.;
    std::string sopt="";
    out = true;
    sopt = m_rootHist->GetXaxis()->GetTitle();
    out &=m_dbHist->setDisplayOption("LABEL_X",     &sopt);
    sopt = m_rootHist->GetYaxis()->GetTitle();
    out &=m_dbHist->setDisplayOption("LABEL_Y",     &sopt);
    sopt = m_rootHist->GetZaxis()->GetTitle();
    out &=m_dbHist->setDisplayOption("LABEL_Z",     &sopt);
    // miminum and maximum should not be set in this way
    //fopt = m_rootHist->GetMinimum();
    //out &=m_dbHist->setDisplayOption("YMIN"   ,     &fopt);
    //fopt = m_rootHist->GetMaximum();
    //out &=m_dbHist->setDisplayOption("YMAX"   ,     &fopt);
    iopt=(int) m_rootHist->GetFillStyle();
    out &=m_dbHist->setDisplayOption("FILLSTYLE", &iopt);
    iopt=(int) m_rootHist->GetFillColor();
    out &=m_dbHist->setDisplayOption("FILLCOLOR", &iopt);
    iopt=(int) m_rootHist->GetLineStyle();
    out &=m_dbHist->setDisplayOption("LINESTYLE", &iopt);
    iopt=(int) m_rootHist->GetLineColor();
    out &=m_dbHist->setDisplayOption("LINECOLOR", &iopt);
    iopt=(int) m_rootHist->GetLineWidth();
    out &=m_dbHist->setDisplayOption("LINEWIDTH", &iopt);
    sopt = m_rootHist->GetDrawOption();
    out &=m_dbHist->setDisplayOption("DRAWOPTS",  &sopt);
  }
  return out;
}

bool OnlineRootHist::setHistoSetDisplayOption(std::string ParameterName, 
					      void* value) {
  bool out;
  out =m_dbHist->setHistoSetDisplayOption(ParameterName,value);
  if(out) setTH1FromDB();
  return out;
}

bool OnlineRootHist::setHistDisplayOption(std::string ParameterName, 
					  void* value) {
  bool out=false;
  if(m_dbHist) {
    out =m_dbHist->setHistDisplayOption(ParameterName,value);
    if(out && m_rootHist) setTH1FromDB();
  }
  return out;
}

bool OnlineRootHist::setHistoPageDisplayOption(std::string ParameterName, 
					       void* value,
					       std::string PageName,
					       int Instance) {
  bool out=false;
  if(m_dbHist) {
    out =m_dbHist->setHistoPageDisplayOption(ParameterName,value,PageName,Instance);
    if(out && m_rootHist) setTH1FromDB();
  }
  return out;
}

bool OnlineRootHist::setDisplayOption(std::string ParameterName, 
				      void* value) {
  bool out=false;
  if(m_dbHist) {
    out =m_dbHist->setDisplayOption(ParameterName,value);
    if(out && m_rootHist) setTH1FromDB();
  }
  return out;
}


void OnlineRootHist::Draw() {
  if(m_rootHist) {
    std::string option="";
    if(m_dbHist){
      m_dbHist->getDisplayOption("DRAWOPTS",     &option);
     }
    setDrawOptionsFromDB();
    m_rootHist->Draw(option.c_str());
  }
}


