// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAMultiply.cpp,v 1.2 2010-06-11 13:00:10 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAMultiply::OMAMultiply(OMAlib* Env) : 
  OMAHcreatorAlg("Multiply", Env) {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 2;
  m_parnames.push_back("k1"); m_parDefValues.push_back(1.);
  m_parnames.push_back("k2"); m_parDefValues.push_back(1.);
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Bin-by-bin product of two histograms (k1*h1) * (k2*h2)";
}

TH1* OMAMultiply::exec( const std::vector<TH1*> *sources,
                        const std::vector<float> *params,
                        std::string &outName,
                        std::string &outTitle,
                        TH1* existingHisto,
                        TH1* ) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() <2) return out;
  double k1=m_parDefValues[0],k2=m_parDefValues[1];
  if (params) {
    if (params->size()>0) k1=params->at(0);
    if (params->size()>1) k2=params->at(1);
  }
  TH1* okH = sources->at(0);
  TH1* allH = sources->at(1);
  TH1* outHist=0;
  if (existingHisto) {
    outHist= dynamic_cast<TH1D*>(existingHisto);
    if (!outHist)
      outHist= dynamic_cast<TH2D*>(existingHisto);
  }
  if (!outHist) {
    if ( 1 == okH->GetDimension() ) 
      outHist = (TH1*) ( new TH1D (outName.data(), outTitle.data(), 
				   okH->GetNbinsX(), 
				   okH->GetXaxis()->GetXmin(),
				   okH->GetXaxis()->GetXmax()) ); 
    else if(2 == okH->GetDimension())
      outHist = (TH1*) ( new TH2D (outName.data(), outTitle.data(), 
				   okH->GetNbinsX(), 
				   okH->GetXaxis()->GetXmin(),
				   okH->GetXaxis()->GetXmax(),
				   okH->GetNbinsY(), 
				   okH->GetYaxis()->GetXmin(),
				   okH->GetYaxis()->GetXmax()) ); 
    outHist->Sumw2();
  }
  if(outHist) {
    outHist->Multiply(okH, allH, k1, k2);
    outHist->SetBit(TH1::kIsAverage);
  }
  
  return  outHist;
}

