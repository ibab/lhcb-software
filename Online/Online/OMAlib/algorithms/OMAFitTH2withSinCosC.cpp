//=============================================================================
// Include files 
// Online
#include "OMAlib/OMAFitFunction.h"
// Root
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TList.h"
#include "TCanvas.h"
//#include "TFile.h"

//=============================================================================
// Algorithm for RICH global alignment : FitTH2withSinCosC
// 2009-04-16 : Christopher Blanks
//=============================================================================

OMAFitTH2withSinCosC::OMAFitTH2withSinCosC() :
  OMAFitFunction("Rich2DSinCosC"),
  GausP1("GausP1","[0]*TMath::Gaus(x,[1],[2]) + [3]*x + [4]")
{
  m_parNames.push_back("SinAmp");
  m_parNames.push_back("CosAmp");
  m_parNames.push_back("Shift");

  m_mustInit=false;
  m_doc="custom fit for RICH 2d histograms";
  m_predef=false;

  m_funcString= "[0]*sin(x) + [1]*cos(x) + [2]";
  initfun();
  
  GausP1.SetParName(0, "Gaus_weight");
  GausP1.SetParName(1, "Gaus_mean");
  GausP1.SetParName(2, "Gaus_sigma");
  GausP1.SetParName(3, "P1_gradient");
  GausP1.SetParName(4, "P1_intercept");
}

OMAFitTH2withSinCosC::~OMAFitTH2withSinCosC() {
}

void OMAFitTH2withSinCosC::fit(TH1* histo, std::vector<float>* ) {
  //int startrun;
  //std::string DataType;

  //Definition of Histogram, RICH.
  TH2* inHist = dynamic_cast<TH2*>(histo);
  if(!inHist) return;
  
  //Read RICH from histo name
  TString name = histo->GetName(); // RICH/RichAlignMoniR1/dThetavphiRecSide0
  bool R1 = name.Contains("R1"); 
  SetName(name+"_fitted"); //
  
  //define variables
  TGraphErrors cleanPlot;
  double TDRsigma, fitmin, fitmax;
  if(R1){TDRsigma = 0.00145; fitmin = -0.0038; fitmax = 0.0038;}
  else{TDRsigma = 0.00058; fitmin = -0.0038; fitmax = 0.0024;}
  int xbins = (int)inHist->GetNbinsX();
  int ybins = (int)inHist->GetNbinsY();
  double xwidth = 2.0*TMath::Pi()/xbins;
   
  //loop over TH2 phi-bins, create TH1s, fit & write to TGraphErrors
  for (int i=0; i<xbins; i++)
  {
    //create TH1
    TString title = "Stripe";
    title += i;
    TH1D Stripe(title, title, ybins, -0.005, 0.005);
    
    //fill TH1
    for (int j=0; j<ybins; j++)
    {
      double binValue = inHist->GetBinContent(i+1, j+1);
      Stripe.SetBinContent(j+1, binValue);
    }
    
    //estimate fit parameters
    double NoiseEst, SlopeEst, Lsum = 0.0, Rsum = 0.0;
    for(int j=1; j<4; j++){Lsum += Stripe.GetBinContent(j);}
    for(int j=ybins-4; j<ybins-1; j++){Rsum += Stripe.GetBinContent(j);}
    NoiseEst = (Lsum+Rsum)/6.0;
    SlopeEst = ((Rsum/3.0)-(Lsum/3.0))
             / (Stripe.GetBinCenter(ybins-2) - Stripe.GetBinCenter(2));
    
    //initialise function and fit
    GausP1.SetParameter(0, Stripe.GetBinContent(Stripe.GetMaximumBin())-NoiseEst);
    GausP1.SetParLimits(0, 0, Stripe.GetBinContent(Stripe.GetMaximumBin())*1000); //i.e. no maximum
    GausP1.SetParameter(1, Stripe.GetBinCenter(Stripe.GetMaximumBin()));
    GausP1.SetParameter(2, TDRsigma); //TDR single Photon Precision
    GausP1.SetParameter(3, SlopeEst);
    GausP1.SetParameter(4, NoiseEst);
    Stripe.Fit(&GausP1, "", "", fitmin, fitmax);
    
    //output fit mean to TGraphErrors
    cleanPlot.SetPoint(i, (i+0.5)*xwidth, GausP1.GetParameter(1));
    cleanPlot.SetPointError(i, 0.5*xwidth/sqrt(Stripe.GetEntries()), GausP1.GetParError(1));    
  }
   
  //Fitting Reduced 2DHist (TGraphErrors)
  //estimate SinAmp
  double SinAmpEst;
  double x, y1, y2, sy1, sy2;
  cleanPlot.GetPoint(int(xbins/4), x, y1);
  cleanPlot.GetPoint(int(xbins*3/4), x, y2);
  sy1 = cleanPlot.GetErrorY(int(xbins/4));
  sy2 = cleanPlot.GetErrorY(int(xbins*3/4));
  SinAmpEst = ((y1/sy1)-(y2/sy2))/((1/sy1)+(1/sy2));
  SetParameter(0, SinAmpEst);
  
  //estimate CosAmp
  double CosAmpEst;
  double y3, y4, y5, sy3, sy4, sy5;
  cleanPlot.GetPoint(0, x, y3);
  cleanPlot.GetPoint(int(xbins/2), x, y4);
  cleanPlot.GetPoint(xbins-1, x, y5);
  sy3 = cleanPlot.GetErrorY(0);
  sy4 = cleanPlot.GetErrorY(int(xbins/2));
  sy5 = cleanPlot.GetErrorY(xbins-1);
  CosAmpEst = ((y3/sy3)-(y4/sy4)+(y5/sy5))/((1/sy3)+(1/sy4)+(1/sy5));
  SetParameter(1, CosAmpEst);
  
  //estimate Shift
  double ShiftEst;
  double y, ysum = 0.0;
  for (int i=0; i<xbins; i++)
  {
    cleanPlot.GetPoint(i, x, y);
    ysum += y;
  }
  ShiftEst = ysum/xbins;
  SetParameter(2, ShiftEst);

  cleanPlot.Fit((TF1*) this);  

  TF1* clone = (TF1*) (((TF1*) this)->Clone());
  clone->SetName(name + "fit");
  inHist->GetListOfFunctions()->Add(clone);
  clone->SetParent(inHist);
  //inHist->GetListOfFunctions()->ls();
  
  //TFile outFile("outFile.root", "UPDATE");
  //inHist->Write();
  //outFile.Close();

  return;
}
