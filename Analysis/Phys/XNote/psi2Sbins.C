#include <string>
#include "JStyle.h"
gStyle->SetOptStat(0); 

TF1* fit(TGraph* graph) {
  graph->Fit("pol0");
  TF1* myfunc2 = graph->GetFunction("pol0");
  myfunc2->SetLineColor(2);
  myfunc2->SetLineStyle(1);
  return myfunc2;
}

std::string buildProb(TF1* fitfun){
  char pvalue[100]; 
  sprintf(pvalue, "%.2f",fitfun->GetProb()); 
  std::string probString = "P(#chi^{2}) = ";
  probString += pvalue;
  return probString;  
}

std::string buildResult(TF1* fitfun) {

  char xvalue[100]; 
  sprintf(xvalue, "%.2f", fitfun->GetParameter(0)); 
  std::string xString = "<#Deltam> = ";
  xString += xvalue; 
  xString += " #pm ";

  char xvalue[100]; 
  sprintf(xvalue, "%.2f", fitfun->GetParError(0));   
  xString += xvalue;
  return xString;
}

TGraphErrors* makeMeanGraph(TH1D* histo, double min, double max, std::string xTitle, std::string yTitle, double yOff ) {
  float x[6]; float y[6]; float ex[6] = {0, 0, 0, 0, 0, 0};  
  float ey[6]; 
  int n = 0;
  for (int i = 1 ; i < histo->GetNbinsX(); ++i){
    if (histo->GetBinContent(i) > 0){
      x[n] = histo->GetBinCenter(i)/1000;
      y[n] = histo->GetBinContent(i) - 3686.09;
      ey[n] = histo->GetBinError(i);
      ++n;
    }
  }  
  TGraphErrors* mean  = new TGraphErrors(6, x,y ,ex,ey); 
  StyleGraph(mean);
  mean->SetMarkerStyle(20);
  mean->SetMaximum(max);
  mean->SetMinimum(min);
  mean->GetYaxis()->SetTitleOffset(yOff);
  mean->GetXaxis()->SetTitle(xTitle.c_str());
  mean->GetYaxis()->SetTitle(yTitle.c_str());
  return mean;
}

TGraphErrors* makeSigmaGraph(TH1D* histo, double min, double max, std::string xTitle, std::string yTitle, double yOff ) {
  float x[6]; float y[6]; float ex[6] = {0, 0, 0, 0, 0, 0};  
  float ey[6]; 
  int n = 0;
  for (int i = 1 ; i < histo->GetNbinsX(); ++i){
    if (histo->GetBinContent(i) > 0){
      x[n] = histo->GetBinCenter(i)/1000;
      y[n] = histo->GetBinContent(i);
      ey[n] = histo->GetBinError(i);
      ++n;
    }
  }  
  TGraphErrors* sigma  = new TGraphErrors(6, x,y ,ex,ey); 
  StyleGraph(sigma);
  sigma->SetMarkerStyle(20);
  sigma->SetMaximum(max);
  sigma->SetMinimum(min);
  sigma->GetYaxis()->SetTitleOffset(yOff);
  sigma->GetXaxis()->SetTitle(xTitle.c_str());
  sigma->GetYaxis()->SetTitle(yTitle.c_str());
  return sigma;
}


TCanvas* makeCanvas(std::string name) {
  TCanvas *c2 = new TCanvas(name.c_str(),name.c_str(),10,44,800,400);
  c2->Divide(2);
  c2->cd(1);
  return c2 ;
}

void decorateAndDrawMean(TCanvas* c2, TGraph* mean, double x){

  TF1* fitfun = fit(mean);
  std::string probString = buildProb(fitfun);
  std::string xString = buildResult(fitfun);
  
  TLatex *   tex2 = new TLatex(x,0.8,xString.c_str());
  StyleTex(tex2);

  TLatex *   tex = new TLatex(x,0.6,probString.c_str());
  StyleTex(tex);

  mean->Draw("AP");
  tex->Draw();
  tex2->Draw();
}

void psi2Sbins() {

  gStyle->SetOptFit(0);

  // get the file
  TFile *file =new TFile("mass_systematics.root");

  // make the p plot
  TCanvas* c2 = makeCanvas("c2");
  TH1D* histo = (TH1D*) file->Get("Mean_P;1"); // get the mean
  TGraphErrors* mean = makeMeanGraph(histo, -1, 1, "p [GeV/c]","#Delta m_{#psi(2S)} [GeV/c^{2}]", 1.2 );
  decorateAndDrawMean(c2, mean, 70);

  TH1D* histo = (TH1D*) file->Get("Sigma_P;1"); // get the sigma
  TGraphErrors* sigma = makeSigmaGraph(histo, 0, 4, "p [GeV/c]","#sigma (m_{#psi(2S)}) [GeV/c^{2}]", 1.);
  c2->cd(2);
  sigma->Draw("AP");
  makeEps(c2,"P_bin.eps");

  // make pt plot
  TCanvas* c2 = makeCanvas("c3");
  TH1D* histo = (TH1D*) file->Get("Mean_PT;1"); // get the mean
  TGraphErrors* mean = makeMeanGraph(histo, -1, 1, "pt [GeV/c]","#Delta m_{#psi(2S)} [GeV/c^{2}]", 1.2 );
  decorateAndDrawMean(c2, mean,6);

  TH1D* histo = (TH1D*) file->Get("Sigma_PT;1"); // get the sigma
  TGraphErrors* sigma = makeSigmaGraph(histo, 0, 4, "Q [GeV/c]","#sigma (m_{#psi(2S)}) [GeV/c^{2}]", 1.);
  c2->cd(2);
  sigma->Draw("AP");
  makeEps(c2,"PT_bin.eps");

  TCanvas* c2 = makeCanvas("c4");
  TH1D* histo = (TH1D*) file->Get("Mean_Q;1"); // get the mean
  TGraphErrors* mean = makeMeanGraph(histo, -1, 1, "Q [GeV/c]","#Delta m_{#psi(2S)} [GeV/c^{2}]", 1.2 );
  decorateAndDrawMean(c2, mean,0.085);

  TH1D* histo = (TH1D*) file->Get("Sigma_Q;1"); // get the sigma
  TGraphErrors* sigma = makeSigmaGraph(histo, 0, 4, "Q [GeV/c]","#sigma (m_{#psi(2S)}) [GeV/c^{2}]", 1.);
  c2->cd(2);
  sigma->Draw("AP");
  makeEps(c2,"Q_bin.eps");

  // make the m23 plot
  TCanvas* c2 = makeCanvas("c5");
  TH1D* histo = (TH1D*) file->Get("Mean_m23;1"); // get the mean
  TGraphErrors* mean = makeMeanGraph(histo, -1, 1, "m_{#pi #pi} [GeV/c]","#Delta m_{#psi(2S)} [GeV/c^{2}]", 1.2 );
  decorateAndDrawMean(c2, mean,0.515);

  TH1D* histo = (TH1D*) file->Get("Sigma_m23;1"); // get the sigma
  TGraphErrors* sigma = makeSigmaGraph(histo, 0, 4, "m_{#pi #pi} [GeV/c]","#sigma (m_{#psi(2S)}) [GeV/c^{2}]", 1.);
  c2->cd(2);
  sigma->Draw("AP");
  makeEps(c2,"m23_bin.eps");


}
