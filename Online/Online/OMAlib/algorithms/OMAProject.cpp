// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAProject.cpp,v 1.2 2010-05-17 11:05:35 ggiacomo Exp $
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
  int axis= intParam(m_parDefValues[0]);
  int firstbin = intParam(m_parDefValues[1]);
  int lastbin = intParam(m_parDefValues[2]);
  if (params) {
    if (params->size()>0) axis = intParam(params->at(0));
    if (params->size()>1) firstbin = intParam(params->at(1));
    if (params->size()>2) {
      if (params->at(2) > 0) lastbin = intParam(params->at(2));
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



