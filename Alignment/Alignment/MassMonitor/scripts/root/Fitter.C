/* Fitter.C, a script to aid use of the MassMonitor package
   MassBiasesMonitor uses empirical shapes of distributions to make *roughly* even distributions
   This script will help make simple fits to these distributions, and print the parameters
   This assumes that a trial run of MassBiasesMonitor has been run, and looks at the distributions included in that output
   Open the root output containing the MassBiasesMonitor output and runt his script
   folder should be the folder containing MassBiasesMonitor histograms
   varName should be the variable you want to fit, i.e. momentum, pt, momasym, etc.
   fitName should be the fit model you want to use, i.e. GAUS, EXPO, GAUSEXPO, etc.
*/


#include "TFile.h"
#include "TTree.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooExponential.h"
#include "RooGenericPdf.h"
#include "RooGamma.h"
#include "RooCBShape.h"

#include <string>
using std::string;

RooPlot* Fitter(string folder = "Biases/JPSIPREMON", string varName = "momasym",
                string fitName = "gamma", double min = 0, double max = 0) 
{
  // VARIABLE
  RooRealVar var("var","var",0.0);

  // FIT MODELS
  RooFormulaVar absvar("absvar","abs(@0)",RooArgSet(var));

  RooRealVar decay("decay","decay",0.5,0,100);
  RooFormulaVar negdecay("negdecay","-1*@0",RooArgSet(decay));
  RooExponential exp("exp","exp",absvar,negdecay);

  RooRealVar onat("onat","onat",20,-10,100);
  onat.setConstant(true);
  RooRealVar rise("rise","rise",0.5,1e-15,100);
  RooGenericPdf rfexpo("rfexpo","(@0>@1)*(1-exp(-@2*(@0-@1)))*exp(-@3*@0)",RooArgList(absvar,onat,rise,decay));

  RooRealVar turn("turn","turn",20,0,100);
  RooRealVar turnwidth("turnwidth","turnwidth",10,0,25);
  RooGenericPdf gaussexp("rfexpo","(1+TMath::Erf((@0-@1)/(sqrt(2.0)*@2)))*exp(@3*@0)",RooArgList(absvar,turn,turnwidth,negdecay));

  RooRealVar g("g","g",48,1e-5,50);
  RooRealVar beta("beta","beta",0.02,1e-5,5);
  RooRealVar mu("mu","mu",0.0,0.0,10);
  mu.setConstant(true);
  RooGamma gamma("gamma","gamma",absvar,g,beta,mu);

  RooFormulaVar asym("asym","asym","@0*@0",RooArgList(var));  
  RooRealVar a1("a1","a1",1,-100,100);
  RooRealVar a2("a2","a2",1,-100,100);
  RooRealVar a3("a3","a3",1,-100,100);
  RooGenericPdf asympdf("asympdf","(1-@0*@0)*(1+@1*@0^2 + @2*@0^4 + @3*@0^6)",RooArgList(var,a1,a2,a3));

  // CHOOSE PDF
  RooAbsPdf* pdf;
  switch (fitName) {
  case "EXPO" : pdg = &exp; break;
  case "RFEXPO" : pdg = &rfexp; break;
  case "GAUSEXPO" : pdg = &gaussexp; break;
  case "GAMMA" : pdg = &gamma; break;
  case "ASYM" : pdg = &asym; break;
  }
  
  // FIT
  string str = folder + modeName + "/" + varName;
  cout << str << endl;
  TH1D* h = (TH1D*)_file0->Get(str.c_str());
  RooDataHist hist("hist","hist",RooArgSet(var),h);
  var.setBins(20);
  var.setRange(min,max);
  pdf->fitTo(hist,RooFit::Range(min,max));
  RooPlot* f = var.frame();
  hist.plotOn(f);
  pdf->plotOn(f,RooFit::Range(min,max));
  f->Draw();
  return f;
}
