//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineRootHist.cpp,v 1.5 2007-07-13 17:19:28 ggiacomo Exp $
#include "OnlineHistDB/OnlineRootHist.h"

OnlineRootHist::OnlineRootHist(OnlineHistDBEnv& Env,
		 std::string Identifier,
		 std::string Page,
		 int Instance) : OnlineHistogram(Env,Identifier,Page,Instance)
{ 
  if (!isAbort()) setFromDB();
}


void OnlineRootHist::setFromDB() {
  int iopt;
  float fopt;
  std::string sopt;
  if(getDisplayOption("LABEL_X",   &sopt)) this->SetXTitle (sopt.data());
  if(getDisplayOption("LABEL_Y",   &sopt)) this->SetYTitle (sopt.data());
  if(getDisplayOption("LABEL_Z",   &sopt)) this->SetZTitle (sopt.data());
  if(getDisplayOption("YMIN",      &fopt)) this->SetMinimum(fopt);
  if(getDisplayOption("YMAX",      &fopt)) this->SetMaximum(fopt);
  if(getDisplayOption("FILLSTYLE", &iopt)) this->SetFillStyle(iopt);
  if(getDisplayOption("FILLCOLOR", &iopt)) this->SetFillColor(iopt);
  if(getDisplayOption("LINESTYLE", &iopt)) this->SetLineStyle(iopt);
  if(getDisplayOption("LINECOLOR", &iopt)) this->SetLineColor(iopt);
  if(getDisplayOption("LINEWIDTH", &iopt)) this->SetLineWidth(iopt);
  if(getDisplayOption("STATS",     &iopt)) this->SetStats(true);
  
}

void OnlineRootHist::setDrawOptions() {
  int iopt=0;
  //float fopt=0.;
  //std::string sopt="";
  if(getDisplayOption("STATS"  ,   &iopt)) {
    gStyle->SetOptStat(iopt);
  }
}


bool OnlineRootHist::saveToDB() {
  int iopt=0;
  //float fopt=0.;
  std::string sopt="";
  sopt = this->GetXaxis()->GetTitle();
  this->setDisplayOption("LABEL_X",     &sopt);
  sopt = this->GetYaxis()->GetTitle();
  this->setDisplayOption("LABEL_Y",     &sopt);
  sopt = this->GetZaxis()->GetTitle();
  this->setDisplayOption("LABEL_Z",     &sopt);
  // miminum and maximum should not be set in this way
  //fopt = this->GetMinimum();
  //this->setDisplayOption("YMIN"   ,     &fopt);
  //fopt = this->GetMaximum();
  //this->setDisplayOption("YMAX"   ,     &fopt);
  iopt=(int) this->GetFillStyle();
  this->setDisplayOption("FILLSTYLE", &iopt);
  iopt=(int) this->GetFillColor();
  this->setDisplayOption("FILLCOLOR", &iopt);
  iopt=(int) this->GetLineStyle();
  this->setDisplayOption("LINESTYLE", &iopt);
  iopt=(int) this->GetLineColor();
  this->setDisplayOption("LINECOLOR", &iopt);
  iopt=(int) this->GetLineWidth();
  this->setDisplayOption("LINEWIDTH", &iopt);
  return true;
}

bool OnlineRootHist::setHistoSetDisplayOption(std::string ParameterName, 
					      void* value) {
  bool out;
  out = OnlineHistogram::setHistoSetDisplayOption(ParameterName,value);
  if(out) setFromDB();
  return out;
}

bool OnlineRootHist::setHistDisplayOption(std::string ParameterName, 
					  void* value) {
  bool out;
  out = OnlineHistogram::setHistDisplayOption(ParameterName,value);
  if(out) setFromDB();
  return out;
}

bool OnlineRootHist::setHistoPageDisplayOption(std::string ParameterName, 
					       void* value,
					       std::string PageName,
					       int Instance) {
  bool out;
  out = OnlineHistogram::setHistoPageDisplayOption(ParameterName,value,PageName,Instance);
  if(out) setFromDB();
  return out;
}

bool OnlineRootHist::setDisplayOption(std::string ParameterName, 
				      void* value) {
  bool out;
  out = OnlineHistogram::setDisplayOption(ParameterName,value);
  if(out) setFromDB();
  return out;
}


void OnlineRootHist::Draw(Option_t* option) {
  setDrawOptions();
  TH1::Draw(option);
}
void OnlineRootHist::DrawPanel() {
  setDrawOptions();
  TH1::DrawPanel();
}
TH1* OnlineRootHist::myDrawCopy(Option_t* option) {
  setDrawOptions();
  return TH1::DrawCopy(option);
}


OnlineRootHistStorage::OnlineRootHistStorage(OnlineHistDBEnv* Env) :
  m_Histenv(Env), m_avoid_dup(true) {}

OnlineRootHistStorage::~OnlineRootHistStorage() 
{
  if (m_Histenv->debug() > 2) cout << "Deleting "<<
    m_myHist.size() << " OnlineRootHist objects"<<endl;
  std::vector<OnlineRootHist*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    delete *ih; 
}


void OnlineRootHistStorage::updateRootHistograms() {
  std::vector<OnlineRootHist*>::iterator ih;
  for (ih = m_myHist.begin();ih != m_myHist.end(); ++ih) 
    (*ih)->checkServiceName(); 
}


OnlineRootHist* OnlineRootHistStorage::getRootHist(std::string Identifier, 
					    std::string Page,
					    int Instance) {
  OnlineRootHist* h=0;
  if (m_avoid_dup) {
    // see if the histogram object exists already
    std::vector<OnlineRootHist*>::iterator ih;
    for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
      if ((*ih)->identifier() == Identifier && (*ih)->page() == Page && 
	  (*ih)->instance() == Instance ) {
	h = *ih;
	break;
      }
    }
  }
  if (!h) {
    if (Instance > 999) Instance=1;
    h= new OnlineRootHist(*m_Histenv,Identifier,Page,Instance);
    if (h->isAbort()) {
      cout<<"Error from OnlineHistDB::getRootHist : cannot create histogram object " 
	  << Identifier <<endl;
      delete h;
      h=0;
    }
    else 
      m_myHist.push_back(h);
  }
  return h;
}

OnlineRootHist* OnlineRootHistStorage::getNewRootHist(std::string Identifier, 
					    std::string Page,
					    int Instance) {
  OnlineRootHist* h=0;
  m_avoid_dup = false;
  h = getRootHist(Identifier, Page, Instance);
  m_avoid_dup = true;
  return h;
}



bool OnlineRootHistStorage::removeRootHistogram(OnlineRootHist* h,
				   bool RemoveWholeSet) {
  bool out=h->remove(RemoveWholeSet);
  if (out) {
    std::vector<OnlineRootHist*>::iterator ih = m_myHist.begin();
    while (ih != m_myHist.end()) {
      if((RemoveWholeSet && (*ih)->hsid() == h->hsid()) ||
	 (*ih)->hid() == h->hid() ) {
	if (h != (*ih)) delete *ih;
	ih=m_myHist.erase(ih);
      }
      else {
	ih++;
      }
    }
    delete h;
  }
  return out;  
}


