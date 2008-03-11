// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAScale.cpp,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAScale::OMAScale() : OMAHcreatorAlg("Scale") {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Divide all bins of the first histogram by the content of the first bin of the second histogram.";
  m_doc += " If the second histogram is equal to the first, the first bin is removed from output";  
}

TH1* OMAScale::exec( const std::vector<TH1*> *sources,
			  const std::vector<float> *params,
			  std::string outName,
			  std::string outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  params=params; // avoid compil. warning
  if (sources->size() >= 2)
    out = exec( sources->at(0), sources->at(1), outName, outTitle, existingHisto);
  return out;
}

TH1* OMAScale::exec(TH1* H,
			 TH1* scalefactorH,
			 std::string outName,
			 std::string outTitle,
			 TH1* existingHisto) {  
  TH1* scaledHist=existingHisto;
  bool copied=false;
  int firstbin=1,shift=0;
  if(!scaledHist) {
    if(scalefactorH->GetName() != H->GetName() ||
       dynamic_cast<TH2*>(H)) {
      scaledHist=(TH1*) H->Clone();
      scaledHist->SetNameTitle(outName.data(),outTitle.data() );
      copied = true;
    }
    else {
      scaledHist= (TH1*) ( new TH1D 
                           (outName.data(), outTitle.data(), 
                            H->GetNbinsX()-1, 
                            H->GetXaxis()->GetBinLowEdge(2),
                            H->GetXaxis()->GetXmax()) );  
      scaledHist->Sumw2();
      firstbin=2;
      shift=-1;
    }
  }
  if (scaledHist && !copied) {
    for (int ib=firstbin; ib<=H->GetNbinsX(); ib++) {
      scaledHist->SetBinContent(ib+shift,  H->GetBinContent(ib));
    }
  }
  double normK= scalefactorH->GetBinContent(1);
  
  if (scaledHist && normK > 0.)
    scaledHist->Scale(1./normK);
  
  return  scaledHist;
}

