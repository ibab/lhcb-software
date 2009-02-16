// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMADivide.cpp,v 1.4 2009-02-16 10:38:21 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMADivide::OMADivide(OMAcommon* Env) : 
  OMAHcreatorAlg("Divide", Env) {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Ratio of two histograms";
}

TH1* OMADivide::exec( const std::vector<TH1*> *sources,
			  const std::vector<float> *params,
			  std::string &outName,
			  std::string &outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  params=params; // avoid compil. warning
  if (sources->size() >= 2)
    out = exec( sources->at(0), sources->at(1), outName, outTitle, existingHisto);
  return out;
}

TH1* OMADivide::exec(TH1* okH,
			 TH1* allH,
			 std::string &outName,
			 std::string &outTitle,
			 TH1* existingHisto) {  
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
  }
  if(outHist) {
    outHist->Sumw2();
    outHist->Divide(okH, allH, 1., 1.);
  }
  
  return  outHist;
}

