//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineRootHist.cpp,v 1.3 2007-07-10 13:50:00 ggiacomo Exp $
#include "OnlineHistDB/OnlineRootHist.h"

OnlineRootHist::OnlineRootHist(OnlineHistDBEnv& Env,
		 std::string Identifier,
		 std::string Page,
		 int Instance) : OnlineHistogram(Env,Identifier,Page,Instance)
{ 
  setFromDB();
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
  
}

void OnlineRootHist::setDrawOptions() {
  int iopt=0;
  //float fopt=0.;
  //std::string sopt="";
  if(getDisplayOption("STATS"  ,   &iopt)) {
    this->SetStats(true);
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

bool OnlineRootHist::setDisplayOption(std::string ParameterName, 
					      void* value) {
  bool out;
  out = OnlineHistogram::setDisplayOption(ParameterName,value);
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

bool OnlineRootHist::unsetHistoSetDisplayOption(std::string ParameterName) {
  bool out;
  out = OnlineHistogram::unsetHistoSetDisplayOption(ParameterName);
  if(out) setFromDB();
  return out;
}

bool OnlineRootHist::unsetDisplayOption(std::string ParameterName) {
  bool out;
  out = OnlineHistogram::unsetDisplayOption(ParameterName);
  if(out) setFromDB();
  return out;
}

bool OnlineRootHist::unsetHistoPageDisplayOption(std::string ParameterName) {
  bool out;
  out = OnlineHistogram::unsetHistoPageDisplayOption(ParameterName);
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
  m_Histenv(Env) {}

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
    (*ih)->update(); 
}


OnlineRootHist* OnlineRootHistStorage::getRootHist(std::string Identifier, 
					    std::string Page,
					    int Instance) {
  OnlineRootHist* h=0;
  // see if the histogram object exists already
  std::vector<OnlineRootHist*>::iterator ih;
  for (ih = m_myHist.begin(); ih != m_myHist.end(); ++ih) {
    if ((*ih)->identifier() == Identifier && (*ih)->page() == Page && 
	(*ih)->instance() == Instance ) {
      h = *ih;
      break;
    }
  }
  if (!h) {
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


