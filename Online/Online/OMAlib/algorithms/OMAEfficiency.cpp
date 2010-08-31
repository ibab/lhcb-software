// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAEfficiency.cpp,v 1.11 2010-08-31 16:54:11 ggiacomo Exp $
#include <cmath>
#include <cstdlib>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TGraphAsymmErrors.h>
#include <TList.h>
#include "OMAlib/OMAAlgorithms.h"


OMAEfficiency::OMAEfficiency(OMAlib* Env) : 
  OMAHcreatorAlg("Efficiency", Env) {
  m_ninput = 2;
  m_histSetFlag = false;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Ratio of two histograms with binomial errors";
}

TH1* OMAEfficiency::exec( const std::vector<TH1*> *sources,
                          const std::vector<float> *,
                          std::string &outName,
                          std::string &outTitle,
                          TH1* existingHisto,
                          TH1* ) {
  TH1* out=NULL;
  if (! sourceVerified(sources) ) return out;
  if (sources->size() >= 2)
    out = exec( sources->at(0), sources->at(1), outName, outTitle, existingHisto);
  return out;
}

TH1* OMAEfficiency::exec(TH1* okH,
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
    outHist = (TH1*) allH->Clone(outName.data());
    if(outHist) {
      outHist->SetTitle(outTitle.data());
    }
  }
  if(outHist) {
    if (okH->GetNbinsX() != allH->GetNbinsX() ||
        okH->GetNbinsY() != allH->GetNbinsY() ) return NULL;
    if (outHist->GetDimension()>1) {
      outHist->Divide(okH, allH, 1., 1., "B");
    }
    else {
      // attach a TGraphAsymmErrors for asymmetric errors
      int nb=okH->GetNbinsX();
      Float_t* x= (Float_t*) malloc(nb * sizeof(Float_t));
      Float_t* y= (Float_t*) malloc(nb * sizeof(Float_t));  
      Float_t* exl= (Float_t*) malloc(nb * sizeof(Float_t)); 
      Float_t* eyl= (Float_t*) malloc(nb * sizeof(Float_t)); 
      Float_t* exh= (Float_t*) malloc(nb * sizeof(Float_t)); 
      Float_t* eyh= (Float_t*) malloc(nb * sizeof(Float_t)); 
      double central;
      for (Int_t i=0;i<nb;i++) {
        *(x+i) = outHist->GetXaxis()->GetBinCenter(i+1);
        *(exl+i) = *(exh+i) = (outHist->GetBinWidth(i+1))/2.05;
        double c = okH->GetBinContent(i+1);
        double n = allH->GetBinContent(i+1);
        if (n==0.) {
          central = *(eyl+i) = *(eyh+i) =.5; 
        }
        else {
          central = c/n;
          // use the score confidence interval Agresti-Coull formula for more reasonable binomial errors  
          double pstar= (c+2.)/(n+4.);
          double error = sqrt(pstar*(1-pstar)/(n+4.));
          *(eyl+i) = TMath::Max(0.,central-(pstar-error));
          *(eyh+i) = TMath::Max(0.,(pstar+error)-central);
        }
        *(y+i)=central;
        outHist->SetBinContent(i+1,central);
        outHist->SetBinError(i+1,0);
      }
      TGraphAsymmErrors* gr = new TGraphAsymmErrors(nb,x,y,exl,exh,eyl,eyh);
      outHist->GetListOfFunctions()->Add(gr,"p");
      free(x); free(y); free(exl); free(exh); free(eyl); free(eyh);
    }
    outHist->SetEntries(okH->GetEntries());
  }
  return  outHist;
}

