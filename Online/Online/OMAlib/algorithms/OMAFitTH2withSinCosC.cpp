//=============================================================================
// Include files 
// Online
#include "OMAlib/OMAFitFunction.h"
// Root
#include "TF1.h"
#include "TDirectory.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

//=============================================================================
// Algorithm for RICH global alignment : FitTH2withSinCosC
// 2009-02-25 : Christopher Blanks
//=============================================================================

OMAFitTH2withSinCosC::OMAFitTH2withSinCosC() :
  OMAFitFunction("FitTH2withSinCosC") {
  m_parNames.push_back("SinAmp");
  m_parNames.push_back("CosAmp");
  m_parNames.push_back("Shift");

  m_mustInit=true;
  m_doc="custom fit for RICH 2d histograms";
  m_predef=false;

  m_funcString= "[0]*sin(x) + [1]*cos(x) + [2]";
  initfun();
  
  GausP1 = new TF1("GausP1","[0]*TMath::Gaus(x,[1],[2]) + [3]*x + [4]");
  GausP1->SetParName(0, "Gaus_weight");
  GausP1->SetParName(1, "Gaus_mean");
  GausP1->SetParName(2, "Gaus_sigma");
  GausP1->SetParName(3, "P1_gradient");
  GausP1->SetParName(4, "P1_intercept");
}

OMAFitTH2withSinCosC::~OMAFitTH2withSinCosC() {
  if(GausP1) delete GausP1;
}

void OMAFitTH2withSinCosC::fit(TH1* histo, std::vector<float>* initValues) {
  //int startrun;
  //std::string DataType;
  initValues= initValues; // avoid compilation warning
//=============================================================================
  //Definition of Histogram, RICH.
  TH2F* inHist = dynamic_cast<TH2F*>(histo);

  if (!inHist) return;
  if(inHist->GetDimension()!=2 ) return;
  
  TDirectory* dir = inHist->GetDirectory();
  TString rich = dir->GetName(); // RichAlignCheckR1
  rich.Remove(0, 14); // R1

//=============================================================================
  //Conversion of 2D Hist to 1D Stripe Hists

  int xbins = (int)inHist->GetNbinsX();
  int ybins = (int)inHist->GetNbinsY();
  TH1F* Stripe[xbins];

  for (int i=0; i<xbins; i++)
  {
    TString title = "Stripe";
    title += i;
    Stripe[i] = new TH1F(title, title, ybins, -0.005, 0.005); //"new" saves THF1 to heap to persist outside this loop
    for (int j=0; j<ybins; j++)
    {
      Double_t k = inHist->GetBinContent(i+1, j);
      Stripe[i]->SetBinContent(j, k);
    }
  }

//=============================================================================
  //Fitting 1D Stripe Hists, Creating Reduced 2DHist (TGraphErrors)

  TGraphErrors Plot;
  Double_t TDRsigma=0.0, fitmin=0.0, fitmax=0.0;
  if(rich=="R1"){TDRsigma = 0.00145; fitmin = -0.0038; fitmax = 0.0038;}
  if(rich=="R2"){TDRsigma = 0.00058; fitmin = -0.0038; fitmax = 0.0024;}
  
  for (int i=0; i<xbins; i++)
  {
    Double_t NoiseEst, SlopeEst, Lsum = 0.0, Rsum = 0.0; //Estimate fit parameters
    for(int j=1; j<4; j++){Lsum += Stripe[i]->GetBinContent(j);}
    for(int j=ybins-4; j<ybins-1; j++){Rsum += Stripe[i]->GetBinContent(j);}
    NoiseEst = (Lsum+Rsum)/6.0;
    SlopeEst = ((Rsum/3.0)-(Lsum/3.0))
             / (Stripe[i]->GetBinCenter(ybins-2) - Stripe[i]->GetBinCenter(2));
    
    GausP1->SetParameter(0, Stripe[i]->GetBinContent(Stripe[i]->GetMaximumBin())-NoiseEst);
    GausP1->SetParLimits(0, 0, Stripe[i]->GetBinContent(Stripe[i]->GetMaximumBin())*1000); //i.e. no maximum
    GausP1->SetParameter(1, Stripe[i]->GetBinCenter(Stripe[i]->GetMaximumBin()));
    GausP1->SetParameter(2, TDRsigma); //TDR single Photon Precision
    GausP1->SetParameter(3, SlopeEst);
    GausP1->SetParameter(4, NoiseEst);
    Stripe[i]->Fit(GausP1, "", "", fitmin, fitmax);
      
    Plot.SetPoint(i, (i+0.5)*2.0*TMath::Pi()/xbins, GausP1->GetParameter(1));
    Plot.SetPointError(i, 0.0, GausP1->GetParError(1));
    delete Stripe[i];
  }
  

//=============================================================================
  //Fitting Reduced 2DHist (TGraphErrors)
    
  
  Double_t SinAmpEst; //estimate SinAmp
  Double_t x, y1, y2, sy1, sy2;
  Plot.GetPoint((int)xbins/4, x, y1);
  Plot.GetPoint((int)xbins*3/4, x, y2);
  sy1 = Plot.GetErrorY((int)xbins/4);
  sy2 = Plot.GetErrorY((int)xbins*3/4);
  SinAmpEst = ((y1/sy1)-(y2/sy2))/((1/sy1)+(1/sy2));
  m_fitfun->SetParameter(0, SinAmpEst);
  
  Double_t CosAmpEst; //estimate CosAmp
  Double_t y3, y4, y5, sy3, sy4, sy5;
  Plot.GetPoint(0, x, y3);
  Plot.GetPoint((int)xbins/2, x, y4);
  Plot.GetPoint(xbins, x, y5);
  sy3 = Plot.GetErrorY(0);
  sy4 = Plot.GetErrorY((int)xbins/2);
  sy5 = Plot.GetErrorY(xbins);
  CosAmpEst = ((y3/sy3)-(y4/sy4)+(y5/sy5))/((1/sy3)+(1/sy4)+(1/sy5));
  m_fitfun->SetParameter(1, CosAmpEst);

  Double_t ShiftEst; //estimate Shift
  Double_t y, ysum = 0.0;
  for (int i=0; i<xbins; i++)
  {
    Plot.GetPoint(i, x, y);
    ysum += y;
  }
  ShiftEst = ysum/xbins;
  m_fitfun->SetParameter(2, ShiftEst);

  Plot.Fit(m_fitfun);

  return;
}
