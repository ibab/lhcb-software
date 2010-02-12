// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMARebin.cpp,v 1.1 2010-02-12 14:25:39 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMARebin::OMARebin(OMAlib* Env) : 
  OMAHcreatorAlg("Rebin", Env) {
  m_ninput = 1;
  m_histSetFlag = false;
  m_npars = 1;
  m_parnames.push_back("Nbins"); m_parDefValues.push_back(2.);
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Rebin the histogram by merging each Nbins bins into a single bin";
  
}

TH1* OMARebin::exec( const std::vector<TH1*> *sources,
			  const std::vector<float> *params,
			  std::string &outName,
			  std::string &outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() <1) return out;
  int N=(int) (m_parDefValues[0]+.1);
  if (params) {
    if (params->size()>0) N= (int) (params->at(0)+.1);
  }
  TH1* H = sources->at(0);
  if (existingHisto) {
    existingHisto->Delete();
  }
  out = H->Rebin(N,outName.c_str());
  out->SetTitle(outTitle.c_str());
  
  return  out;
}



