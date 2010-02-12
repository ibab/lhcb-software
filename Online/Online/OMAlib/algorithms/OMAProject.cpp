// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAProject.cpp,v 1.1 2010-02-12 14:25:39 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAProject::OMAProject(OMAlib* Env) : 
  OMAHcreatorAlg("Project", Env) {
  m_ninput = 1;
  m_histSetFlag = false;
  m_npars = 3;
  m_parnames.push_back("axis"); m_parDefValues.push_back(1.);
  m_parnames.push_back("firstbin"); m_parDefValues.push_back(0.);
  m_parnames.push_back("lastbin"); m_parDefValues.push_back(-1.);
  m_outHType = OnlineHistDBEnv::H1D;
  m_doc = "X (axis=1) or Y (axis=2) projection of a 2d histogram in the specified Y(X) bin range";
  m_doc += " (leave default for full range)";
}

TH1* OMAProject::exec( const std::vector<TH1*> *sources,
			  const std::vector<float> *params,
			  std::string &outName,
			  std::string &outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() <1) return out;
  int axis=(int) (m_parDefValues[0]+0.1);
  int firstbin = 0;
  int lastbin = -1;
  if (params) {
    if (params->size()>0) axis = (int) (params->at(0) + 0.1);
    if (params->size()>1) firstbin = (int) (params->at(1) + 0.1);
    if (params->size()>2) {
      if (params->at(2) > 0) lastbin = (int) (params->at(2) + 0.1);
    }
  }
  TH2* H = dynamic_cast<TH2*>(sources->at(0));
  if (!H)  return out;
  
  TH1* projHist=existingHisto;

  if (axis == 2) {
    projHist=H->ProjectionY(outName.data(),firstbin,lastbin);
  }
  else {
    projHist=H->ProjectionX(outName.data(),firstbin,lastbin);
  }

  projHist->SetNameTitle(outName.data(),outTitle.data() );
  return projHist;
}



