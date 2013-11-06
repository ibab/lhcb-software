// Project INCLUDES
#include "MassMonitor/IOVDefinitions.hh"
#include "MassMonitor/SplitFitter.hh"

// C INCLUDES
#include <cstring>
#include <cassert>

// C++ INCLUDES
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdio>
using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;

// ROOT INCLUDES
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TPaveStats.h>
#include <TLegend.h>
#include <TList.h>
#include <TText.h>
#include <TMath.h>

// ROOFIT INCLUDES
#include <RooConstVar.h>
#include <RooFormulaVar.h>
#include <RooAddPdf.h>
#include <RooArgSet.h> 
#include <RooGaussian.h>
#include <RooCBShape.h>
#include <RooVoigtian.h>
#include <RooExponential.h>
#include <RooGenericPdf.h>
#include <RooFFTConvPdf.h>
#include <RooChebychev.h>
#include <RooCmdArg.h>
#include <RooTable.h>

namespace {

  // THIS BOOLEAN CONTROLS HOW MUCH INFO IS PRINTED
  const bool verbose = false;  

  // INPUT / OUTPUT
  const string RootExt = ".root";
  const string FitExt = ".fitvar";
  
  // SOME EXTRA LABELS
  const char meanLabel[] = "#Deltam/m_{PDG}";
  const char meanLabelUnits[] = "#Deltam/m (10^{-3})";
  const char widthLabel[] = "#sigma/m_{PDG}";
  const char widthLabelUnits[] = "#sigma/m (10^{-3})";
  const char fracLabel[] = "signal %";
  const char fracLabelUnits[] = "signal %";
  const char pchi2Label[] = "Fit ln(P(#chi^{2}))";
  const char pchi2LabelUnits[] = "ln(P(#chi^{2}))";
  
  // DECAY CONSTANTS

  const int NumRes = 11;
  int res = 0; // The particular decay for this execution; initialized at the command line
  
  const string ID[NumRes] =
    {
      "Jpsi"
      , "Jpsi2CB"
      , "JpsiCru"
      , "Psi2S"
      , "Psi2S2CB"
      , "Psi2SCru"
      , "Ups1S"
      , "Ups1S2CB"
      , "Ups1SCru"
      , "Z0"
      , "D0"
    }; // Short form decay string, e.g. Jpsi2MuMu  
  const string Particle[NumRes] =
    {
      "J/#psi(1S)"
      , "J/#psi(1S)"
      , "J/#psi(1S)"
      , "#psi(2S)"
      , "#psi(2S)"
      , "#psi(2S)"
      , "#Upsilon(1S)"
      , "#Upsilon(1S)"
      , "#Upsilon(1S)"
      , "Z"
      , "D^{0}"
    }; // Long form LHCb particle name, e.g. J/psi(1S)
  
  const double Mass[NumRes] = {
    3.096916
    , 3.096916
    , 3.096916
    , 3.686109
    , 3.686109
    , 3.686109
    , 9.4603025
    , 9.4603025
    , 9.4603025
    , 91.1876
    , 1.86484
  };
    
  // Jpsi mass 3.09616, width ~ 1.5e-2
  // Ups1s mass 9.4603025, width ~ 5e-2
  // Z0 mass 91.1876, width ~ 10
  // D0 mass 1.86484, width ~ 8e-3

  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------
  // MODEL PDFS
  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------

  // JPSI
  /* The following deserves some explanation
     True J/psi distribution is a sharp breit-wigner + tail
     Gaussian resolution -> crystal ball with shifted mean
     Double gaussian resolution -> double crystal ball with separate, but related, shifted means
     Shifted mean shouldn't depend so much on crystal ball fit, so I'll use it for Cruijff fit as well
     For more detail, see LHCb-ANA-2011-027 Appendix B
  */

  RooRealVar jpsi_mass ("jpsi_mass","jpsi_mass", 3.02, 3.16);
  RooRealVar jpsi_mean ("jpsi_mean","jpsi_mean", 3.09, 3.02, 3.16);
  RooRealVar jpsi_width ("jpsi_width","jpsi_width", 0.015, 0.001, 0.1);

  // 1CB (default)
  RooFormulaVar jpsi_shmean ("jpsi_shmean",
                             "@0 - (-2.287e-06 + 0.04979*@1 + 0.08928*@1*@1)",
                             RooArgList(jpsi_mean, jpsi_width));
  RooFormulaVar jpsi_z ("jpsi_z",
                        "2.015 + 0.9992*sqrt(@0) + 0.8885*@0 - 6.601*@0*sqrt(@0)",
                        RooArgList(jpsi_width));
  RooConstVar jpsi_n ("jpsi_n","jpsi_n",1.0);
  RooCBShape jpsi_sig ("jpsi_sig","jpsi_sig",jpsi_mass,jpsi_shmean,jpsi_width,jpsi_z,jpsi_n);

  // 2CB
  RooRealVar jpsi_widthmult("jpsi_widthmult","jpsi_widthmult",1.1,1,3);
  RooFormulaVar jpsi_width2("jpsi_width2","jpsi_width2","jpsi_width*jpsi_widthmult",RooArgList(jpsi_width,jpsi_widthmult));
  RooFormulaVar jpsi_shmean2 ("jpsi_shmean2",
                              "jpsi_mean - 0.055 * jpsi_width2 + 0.00026 * jpsi_width2 * jpsi_width2",
                              RooArgList(jpsi_mean, jpsi_width2));
  RooFormulaVar jpsi_z2 ("jpsi_z2",
                         "1.975 - 0.011 * jpsi_width2 - 0.00018 * jpsi_width2 * jpsi_width2",
                         RooArgList(jpsi_width2));
  RooConstVar jpsi_n2 ("jpsi_n2","jpsi_n2",1.0);
  RooCBShape jpsi_cb2("jpsi_cb2","jpsi_cb2",jpsi_mass,jpsi_shmean2,jpsi_width2,jpsi_z2,jpsi_n2);
  RooRealVar jpsi2cb_cbfrac("jpsi2cb_cbfrac","jpsi2cb_cbfrac",0.7,0,1);
  RooAddPdf jpsi2cb_sig("jpsi2cb_sig","jpsi2cb_sig",RooArgList(jpsi_sig,jpsi_cb2),RooArgList(jpsi2cb_cbfrac));
  RooFormulaVar jpsi2cb_effwidth("jpsi2cb_effwidth",
                                 "sqrt( (@0^2*@1^2*(@2*@1 + (1-@2)*@0)) / (@2*@1^3 + (1-@2)*@0^3) )",
                                 RooArgList(jpsi_width,jpsi_width2,jpsi2cb_cbfrac));

  // CRU
  RooRealVar jpsi_alphaL("jpsi_alphaL","jpsi_alphaL",0.1,1E-10,1);
  RooRealVar jpsi_alphaR("jpsi_alphaR","jpsi_alphaR",0.1,1E-10,1);
  const char* jpsicru_formula =
    "exp( - (jpsi_mass-jpsi_shmean)*(jpsi_mass-jpsi_shmean)"
    " / (2*jpsi_width*jpsi_width "
    "+ jpsi_alphaL*(jpsi_mass-jpsi_shmean)*(jpsi_mass-jpsi_shmean)*(jpsi_mass<jpsi_shmean) "
    "+ jpsi_alphaR*(jpsi_mass-jpsi_shmean)*(jpsi_mass-jpsi_shmean)*(jpsi_mass>jpsi_shmean) ) )";
  RooGenericPdf jpsicru_sig("jpsicru_sig","jpsicru_sig",
                            jpsicru_formula,
                            RooArgSet(jpsi_mass,jpsi_shmean,jpsi_width,jpsi_alphaL,jpsi_alphaR));

  // EXP BKG
  RooRealVar jpsi_decay("jpsi_decay","jpsi_decay",0,-5,5);
  RooExponential jpsi_bkg("jpsi_bkg","jpsi_bkg",jpsi_mass,jpsi_decay);

  // TOTAL MODEL
  RooRealVar jpsi_frac("jpsi_frac","jpsi_frac",0.7,0,1);
  RooAddPdf jpsi_model("jpsi_model","jpsi_model",RooArgList(jpsi_sig,jpsi_bkg),RooArgList(jpsi_frac));
  RooAddPdf jpsi2cb_model("jpsi2cb_model","jpsi2cb_model",RooArgList(jpsi2cb_sig,jpsi_bkg),RooArgList(jpsi_frac));
  RooAddPdf jpsicru_model("jpsicru_model","jpsicru_model",RooArgList(jpsicru_sig,jpsi_bkg),RooArgList(jpsi_frac));

  // PSI2S
  /* The following deserves some explanation
     True J/psi distribution is a sharp breit-wigner + tail
     Gaussian resolution -> crystal ball with shifted mean
     Double gaussian resolution -> double crystal ball with separate, but related, shifted means
     Shifted mean shouldn't depend so much on crystal ball fit, so I'll use it for Cruijff fit as well
     For more detail, see LHCb-ANA-2011-027 Appendix B
  */

  RooRealVar psi2s_mass ("psi2s_mass","psi2s_mass", 3.61, 3.76);
  RooRealVar psi2s_mean ("psi2s_mean","psi2s_mean", 3.69, 3.61, 3.76);
  RooRealVar psi2s_width ("psi2s_width","psi2s_width", 0.015, 0.001, 0.1);

  // 1CB (default)
  RooFormulaVar psi2s_shmean ("psi2s_shmean",
                              "@0 - (-4.235e-06 + 0.05164*@1 + 0.1256*@1*@1)",
                              RooArgList(psi2s_mean, psi2s_width));
  RooFormulaVar psi2s_z ("psi2s_z",
                         "1.973 + 1.403*sqrt(@0) - 4.223*@0 + 9.594*@0*sqrt(@0)",
                         RooArgList(psi2s_width));
  RooConstVar psi2s_n ("psi2s_n","psi2s_n",1.0);
  RooCBShape psi2s_sig ("psi2s_sig","psi2s_sig",psi2s_mass,psi2s_shmean,psi2s_width,psi2s_z,psi2s_n);

  // 2CB
  RooRealVar psi2s_widthmult("psi2s_widthmult","psi2s_widthmult",1.1,1,3);
  RooFormulaVar psi2s_width2("psi2s_width2","psi2s_width2","psi2s_width*psi2s_widthmult",RooArgList(psi2s_width,psi2s_widthmult));
  RooFormulaVar psi2s_shmean2 ("psi2s_shmean2",
                               "psi2s_mean",
                               RooArgList(psi2s_mean, psi2s_width2));
  RooFormulaVar psi2s_z2 ("psi2s_z2",
                          "2",
                          RooArgList(psi2s_width2));
  RooConstVar psi2s_n2 ("psi2s_n2","psi2s_n2",1.0);
  RooCBShape psi2s_cb2("psi2s_cb2","psi2s_cb2",psi2s_mass,psi2s_shmean2,psi2s_width2,psi2s_z2,psi2s_n2);
  RooRealVar psi2s2cb_cbfrac("psi2s2cb_cbfrac","psi2s2cb_cbfrac",0.7,0,1);
  RooAddPdf psi2s2cb_sig("psi2s2cb_sig","psi2s2cb_sig",RooArgList(psi2s_sig,psi2s_cb2),RooArgList(psi2s2cb_cbfrac));
  RooFormulaVar psi2s2cb_effwidth("psi2s2cb_effwidth",
                                  "sqrt( (@0^2*@1^2*(@2*@1 + (1-@2)*@0)) / (@2*@1^3 + (1-@2)*@0^3) )",
                                  RooArgList(psi2s_width,psi2s_width2,psi2s2cb_cbfrac));

  // CRU
  RooRealVar psi2s_alphaL("psi2s_alphaL","psi2s_alphaL",0.1,1E-10,1);
  RooRealVar psi2s_alphaR("psi2s_alphaR","psi2s_alphaR",0.1,1E-10,1);
  const char* psi2scru_formula =
    "exp( - (psi2s_mass-psi2s_shmean)*(psi2s_mass-psi2s_shmean)"
    " / (2*psi2s_width*psi2s_width "
    "+ psi2s_alphaL*(psi2s_mass-psi2s_shmean)*(psi2s_mass-psi2s_shmean)*(psi2s_mass<psi2s_shmean) "
    "+ psi2s_alphaR*(psi2s_mass-psi2s_shmean)*(psi2s_mass-psi2s_shmean)*(psi2s_mass>psi2s_shmean) ) )";
  RooGenericPdf psi2scru_sig("psi2scru_sig","psi2scru_sig",
                             psi2scru_formula,
                             RooArgSet(psi2s_mass,psi2s_shmean,psi2s_width,psi2s_alphaL,psi2s_alphaR));

  // EXP BKG
  RooRealVar psi2s_decay("psi2s_decay","psi2s_decay",0,-5,5);
  RooExponential psi2s_bkg("psi2s_bkg","psi2s_bkg",psi2s_mass,psi2s_decay);

  // TOTAL MODEL
  RooRealVar psi2s_frac("psi2s_frac","psi2s_frac",0.7,0,1);
  RooAddPdf psi2s_model("psi2s_model","psi2s_model",RooArgList(psi2s_sig,psi2s_bkg),RooArgList(psi2s_frac));
  RooAddPdf psi2s2cb_model("psi2s2cb_model","psi2s2cb_model",RooArgList(psi2s2cb_sig,psi2s_bkg),RooArgList(psi2s_frac));
  RooAddPdf psi2scru_model("psi2scru_model","psi2scru_model",RooArgList(psi2scru_sig,psi2s_bkg),RooArgList(psi2s_frac));

  // Upsilon(1S)
  /* The following deserves some explanation
     True Upsilon(1S) distribution is a sharp breit-wigner + tail
     Gaussian resolution -> crystal ball with shifted mean
     Double gaussian resolution -> double crystal ball with separate, but related, shifted means
     For more detail, see LHCb-ANA-2011-027 Appendix B
  */

  RooRealVar ups1s_mass ("ups1s_mass","ups1s_mass", 9.1, 9.8);
  RooRealVar ups1s_mean ("ups1s_mean","ups1s_mean", 9.5, 9.1, 9.8);
  RooRealVar ups1s_width ("ups1s_width","ups1s_width", 0.05, 0.005, 0.5);

  // 1CB (default)
  RooFormulaVar ups1s_shmean ("ups1s_shmean",
                              "@0 - (-1.928e-05 + 0.06835*@1 + 0.02235*@1*@1)",
                              RooArgList(ups1s_mean, ups1s_width));
  RooFormulaVar ups1s_z ("ups1s_z",
                         "1.83 + 1.315*sqrt(@0) - 4.161*@0 + 7.153*@0*sqrt(@0)",
                         RooArgList(ups1s_width));
  RooConstVar ups1s_n ("ups1s_n","ups1s_n",1.0);
  RooCBShape ups1s_sig ("ups1s_sig","ups1s_sig",ups1s_mass,ups1s_shmean,ups1s_width,ups1s_z,ups1s_n);
  
  // 2CB
  RooRealVar ups1s_widthmult("ups1s_widthmult","ups1s_widthmult",1.1,1,3);
  RooFormulaVar ups1s_width2("ups1s_width2","ups1s_width2","ups1s_width*ups1s_widthmult",RooArgList(ups1s_width,ups1s_widthmult));
  RooFormulaVar ups1s_shmean2 ("ups1s_shmean2",
                               "ups1s_mean - 0.068 * ups1s_width2",
                               RooArgList(ups1s_mean, ups1s_width2));
  RooConstVar ups1s_z2 ("ups1s_z2","ups1s_z2",2.0);
  RooConstVar ups1s_n2 ("ups1s_n2","ups1s_n2",1.0);
  RooCBShape ups1s_cb2("ups1s_cb2","ups1s_cb2",ups1s_mass,ups1s_shmean2,ups1s_width2,ups1s_z2,ups1s_n2);
  RooRealVar ups1s2cb_cbfrac("ups1s2cb_cbfrac","ups1s2cb_cbfrac",0.7,0,1);
  RooAddPdf ups1s2cb_sig("ups1s2cb_sig","ups1s2cb_sig",RooArgList(ups1s_sig,ups1s_cb2),RooArgList(ups1s2cb_cbfrac));
  RooFormulaVar ups1s2cb_effwidth("ups1s2cb_effwidth",
                                  "sqrt( (@0^2*@1^2*(@2*@1 + (1-@2)*@0)) / (@2*@1^3 + (1-@2)*@0^3) )",
                                  RooArgList(ups1s_width,ups1s_width2,ups1s2cb_cbfrac));

  // CRU
  RooRealVar ups1s_alphaL("ups1s_alphaL","ups1s_alphaL",0.1,1E-10,1);
  RooRealVar ups1s_alphaR("ups1s_alphaR","ups1s_alphaR",0.1,1E-10,1);
  const char* ups1scru_formula =
    "exp( - (ups1s_mass-ups1s_shmean)*(ups1s_mass-ups1s_shmean)"
    " / (2*ups1s_width*ups1s_width "
    "+ ups1s_alphaL*(ups1s_mass-ups1s_shmean)*(ups1s_mass-ups1s_shmean)*(ups1s_mass<ups1s_shmean) "
    "+ ups1s_alphaR*(ups1s_mass-ups1s_shmean)*(ups1s_mass-ups1s_shmean)*(ups1s_mass>ups1s_shmean) ) )";
  RooGenericPdf ups1scru_sig("ups1scru_sig","ups1scru_sig",
                             ups1scru_formula,
                             RooArgSet(ups1s_mass,ups1s_shmean,ups1s_width,ups1s_alphaL,ups1s_alphaR));

  // EXP BKG
  RooRealVar ups1s_decay("ups1s_decay","ups1s_decay",0,-5,5);
  RooExponential ups1s_bkg("ups1s_bkg","ups1s_bkg",ups1s_mass,ups1s_decay);

  // TOTAL MODEL
  RooRealVar ups1s_frac("ups1s_frac","ups1s_frac",0.7,0,1);
  RooAddPdf ups1s_model("ups1s_model","ups1s_model",RooArgList(ups1s_sig,ups1s_bkg),RooArgList(ups1s_frac));
  RooAddPdf ups1s2cb_model("ups1s2cb_model","ups1s2cb_model",RooArgList(ups1s2cb_sig,ups1s_bkg),RooArgList(ups1s_frac));
  RooAddPdf ups1scru_model("ups1scru_model","ups1scru_model",RooArgList(ups1scru_sig,ups1s_bkg),RooArgList(ups1s_frac));

  // Z0
  /* The true shape is a relativistic Breit-Wigner
     This is convolved with a Crystal Ball, representing detector resolution / energy loss
     The RooFFTConvPdf introduces a bias, which I attempt to correct for
     However, I don't trust my correction yet
  */
  
  RooRealVar z0_mass ("z0_mass","z0_mass", 60, 120);
  RooRealVar z0_mean ("z0_mean","z0_mean", 90, 60, 120);
  RooRealVar z0_width ("z0_width","z0_width", 5, 0.5, 50);
  
  // RBW
  RooFormulaVar z0_shmean("z0_shmean",
                          "z0_mean + 0.3", // ???
                          RooArgList(z0_mean,z0_width));
  RooConstVar z0_gamma("z0_gamma","z0_gamma",2.495);
  RooGenericPdf z0_rbw("z0_rbw",
                       "2 / 3.14159 * z0_gamma^4 * z0_shmean^2 / ((z0_mass^2 - z0_shmean^2)^2 + z0_mass^4* z0_gamma^2 / z0_shmean^2)",
                       RooArgList(z0_mass,z0_shmean,z0_gamma));
  RooConstVar z0_bias ("z0_bias","z0_bias",0);
  RooRealVar z0_n ("z0_n","z0_n", 1, 0.1, 2.0);
  RooRealVar z0_z ("z0_z","z0_z", 2, 1, 3);
  RooCBShape z0_sm ("z0_sm","z0_sm",z0_mass,z0_bias,z0_width,z0_z,z0_n);
  RooFFTConvPdf z0_sig ("z0_sig","z0_sig",z0_mass,z0_rbw,z0_sm);

  // EXP BKG
  RooRealVar z0_decay("z0_decay","z0_decay",0,-5,5);
  RooExponential z0_bkg("z0_bkg","z0_bkg",z0_mass,z0_decay);

  // TOTAL MODEL
  RooRealVar z0_frac("z0_frac","z0_frac",0.7,0,1);
  RooAddPdf z0_model("z0_model","z0_model",RooArgList(z0_sig,z0_bkg),RooArgList(z0_frac));

  // D0
  /* Default is double crystal ball
   */
  RooRealVar d0_mass ("d0_mass","d0_mass", 1.82, 1.91);
  RooRealVar d0_mean ("d0_mean","d0_mean", 1.87, 1.82, 1.91);
  RooRealVar d0_width ("d0_width","d0_width", 0.015, 0.001, 0.1);
  
  // 1CB (default)
  RooFormulaVar d0_shmean ("d0_shmean",
                           "@0 - (-2.411e-05 + 0.03082*@1 - 0.04584*@1*@1)",
                           RooArgList(d0_mean, d0_width));
  RooFormulaVar d0_z ("d0_z",
                      "2.305 + 1.984*sqrt(@0) - 6.034*@0 + 14.9*@0*sqrt(@0)",
                      RooArgList(d0_width));
  RooConstVar d0_n ("d0_n","d0_n",1.0);
  RooCBShape d0_sig ("d0_sig","d0_sig",d0_mass,d0_shmean,d0_width,d0_z,d0_n);

  // 2CB
  RooRealVar d0_widthmult("d0_widthmult","d0_widthmult",1.1,1,3);
  RooFormulaVar d0_width2("d0_width2","d0_width2","d0_width*d0_widthmult",RooArgList(d0_width,d0_widthmult));
  RooFormulaVar d0_shmean2 ("d0_shmean2",
                            "d0_mean",
                            RooArgList(d0_mean, d0_width2));
  RooRealVar d0_z2 ("d0_z2","d0_z2",2.0,1.0,3.0);
  RooConstVar d0_n2 ("d0_n2","d0_n2",1.0);
  RooCBShape d0_cb2("d0_cb2","d0_cb2",d0_mass,d0_shmean2,d0_width2,d0_z2,d0_n2);
  RooRealVar d02cb_cbfrac("d02cb_cbfrac","d02cb_cbfrac",0.7,0,1);
  RooAddPdf d02cb_sig("d02cb_sig","d02cb_sig",RooArgList(d0_sig,d0_cb2),RooArgList(d02cb_cbfrac));
  RooFormulaVar d02cb_effwidth("d02cb_effwidth",
                               "sqrt( (@0^2*@1^2*(@2*@1 + (1-@2)*@0)) / (@2*@1^3 + (1-@2)*@0^3) )",
                               RooArgList(d0_width,d0_width2,d02cb_cbfrac));
  
  // CRU
  RooRealVar d0_alphaL("d0_alphaL","d0_alphaL",0.1,1E-10,1);
  RooRealVar d0_alphaR("d0_alphaR","d0_alphaR",0.1,1E-10,1);
  const char* d0cru_formula =
    "exp( - (d0_mass-d0_shmean)*(d0_mass-d0_shmean)"
    " / (2*d0_width*d0_width "
    "+ d0_alphaL*(d0_mass-d0_shmean)*(d0_mass-d0_shmean)*(d0_mass<d0_shmean) "
    "+ d0_alphaR*(d0_mass-d0_shmean)*(d0_mass-d0_shmean)*(d0_mass>d0_shmean) ) )";
  RooGenericPdf d0cru_sig("d0cru_sig","d0cru_sig",
                          d0cru_formula,
                          RooArgSet(d0_mass,d0_shmean,d0_width,d0_alphaL,d0_alphaR));
  
  // EXP BKG
  RooRealVar d0_decay("d0_decay","d0_decay",-5,5);
  RooExponential d0_bkg("d0_bkg","d0_bkg",d0_mass,d0_decay);
//   RooRealVar d0_p1("d0_p1","d0_p1",0,-100,100);
//   RooRealVar d0_p2("d0_p2","d0_p2",0,-100,100);
//   RooChebychev d0_bkg("d0_bkg","d0_bkg",d0_mass,RooArgList(d0_p2));
  
  // TOTAL MODEL
  RooRealVar d0_frac("d0_frac","d0_frac",0.7,0,1);
  RooAddPdf d0_model("d0_model","d0_model",RooArgList(d0_sig,d0_bkg),RooArgList(d0_frac));
  RooAddPdf d02cb_model("d02cb_model","d02cb_model",RooArgList(d02cb_sig,d0_bkg),RooArgList(d0_frac));
  RooAddPdf d0cru_model("d0cru_model","d0cru_model",RooArgList(d0cru_sig,d0_bkg),RooArgList(d0_frac));

  // SWITCH VARIABLES
  RooAbsPdf* ResModel[NumRes] = {
    &jpsi_model
    , &jpsi2cb_model
    , &jpsicru_model
    , &psi2s_model
    , &psi2s2cb_model
    , &psi2scru_model
    , &ups1s_model
    , &ups1s2cb_model
    , &ups1scru_model
    , &z0_model
    , &d02cb_model
  };

  RooRealVar* ResMass[NumRes] = {
    &jpsi_mass
    , &jpsi_mass
    , &jpsi_mass
    , &psi2s_mass
    , &psi2s_mass
    , &psi2s_mass
    , &ups1s_mass
    , &ups1s_mass
    , &ups1s_mass
    , &z0_mass
    , &d0_mass
  };
  
  RooAbsReal* ResMean[NumRes] = {
    &jpsi_mean
    , &jpsi_mean
    , &jpsi_mean
    , &psi2s_mean
    , &psi2s_mean
    , &psi2s_mean
    , &ups1s_mean
    , &ups1s_mean
    , &ups1s_mean
    , &z0_mean
    , &d0_mean
  };

  RooAbsReal* ResWidth[NumRes] = {
    &jpsi_width
    , &jpsi2cb_effwidth
    , &jpsi_width
    , &psi2s_width
    , &psi2s2cb_effwidth
    , &psi2s_width
    , &ups1s_width
    , &ups1s2cb_effwidth
    , &ups1s_width
    , &z0_width
    , &d02cb_effwidth
  };
  
  RooAbsReal* ResPurity[NumRes] = {
    &jpsi_frac
    , &jpsi_frac
    , &jpsi_frac
    , &psi2s_frac
    , &psi2s_frac
    , &psi2s_frac
    , &ups1s_frac
    , &ups1s_frac
    , &ups1s_frac
    , &z0_frac
    , &d0_frac
  };

  struct PlotOptions
  {
    const char* type;
    const char* label;
    const char* labelUnits;
    const char* meanfit;
    double mean_minFrac;
    double mean_maxFrac;
    const char* sigmafit;
    double sigma_minFrac;
    double sigma_maxFrac;
    const char* fracfit;
    double frac_minFrac;
    double frac_maxFrac;
  };

  // Doesn't seem to be a native function for this, sadly
  RooDataHist splitDataHistFromTH2D(const char* name, const char* title,
                                    const RooRealVar& var, RooCategory& indexCat, TH2D* hist)
  {
    int numbins = hist->GetNbinsX();
    map<string,TH1*> histMap;
    TH1* temp;
    for (int i = 1; i <= numbins; i++) {
      char dig[5];
      sprintf(dig,"_%d",i);
      string tname = string(name) + dig;
      indexCat.defineType(tname.c_str());
      temp = (TH1*)(hist->ProjectionY(tname.c_str(),i,i));
      histMap[tname] = temp;
    }
    RooDataHist rdh (name,title,RooArgList(var),indexCat,histMap);
    
    return rdh;
  }
  
  // GRAPH STYLE
  void SetGraphStyle(TGraphErrors& g, const char* name, const char* title, const char* xlabel, const char* ylabel) 
  {
    // Set Draw Options
    g.SetName(name);
    g.SetTitle(title);
    g.Draw("AP");
    g.GetHistogram()->SetXTitle(xlabel);
    g.GetHistogram()->SetYTitle(ylabel);
    g.SetMarkerStyle(8);
    g.Draw("AP");
  }
  
  void SetGraphTrend(TGraphErrors &g, const char* fit, double min=0, double max=0) 
  {
    g.Fit(fit,"W","",min,max);
    g.Fit(fit,"","",min,max);
    gStyle->SetOptFit(0111);
    g.Draw("AP");
  }
  
  
  // Ugly, but saves LOTS of repeated lines
  void plotMakerAndSaver(TFile& file, TH2D* hist, vector<binValsAndErrs> data, PlotOptions opts, vector<bool> *keep = 0)
  {

    // CALCULATE VALUES
    int bins = 0;
    vector<int> m;
    if (keep == 0) {
      bins = hist->GetNbinsX();
      for (int k = 0; k < bins; k++) {
        m.push_back(k);
      }
    } else {
      int k = 0;
      for (vector<bool>::iterator it = keep->begin(); it != keep->end(); ++it) {
        if ((*it) == true) {
          bins++;
          m.push_back(k);
        }
        k++;
      }
    }
    double* mean = new double[bins];
    double* meanErrs = new double[bins];
    double* width = new double[bins];
    double* widthErrs = new double[bins];
    double* frac = new double[bins];
    double* fracErrs = new double[bins];
    double* pchi2 = new double[bins];
    double* pchi2Errs = new double[bins];
    double* mids = new double[bins];
    double* binwidths = new double[bins];
    for (int i = 0; i < bins; i++) {
      mean[i] = (data[0].vals[m[i]]/(Mass[res]) - 1.0)*1000;
      meanErrs[i] = data[0].errs[m[i]]/(Mass[res])*1000;
      width[i] = data[1].vals[m[i]]/(Mass[res])*1000;
      widthErrs[i] = data[1].errs[m[i]]/(Mass[res])*1000;
      frac[i] = data[2].vals[m[i]]*100;
      fracErrs[i] = data[2].errs[m[i]]*100;
      pchi2[i] = data[3].vals[m[i]];
      pchi2Errs[i] = data[3].errs[m[i]];
      mids[i] = hist->GetBinCenter(m[i]+1);
      binwidths[i] = hist->GetBinWidth(m[i]+1)/2.0;
    }
    
    // CREATE GRAPH OF MEANS
    TGraphErrors means(bins,mids,mean,binwidths,meanErrs);
    string title = Particle[res] + " " + string(meanLabel) + " vs. " + string(opts.label);
    string name = string(opts.type) + "_means";
    SetGraphStyle(means,name.c_str(),title.c_str(),opts.labelUnits,meanLabelUnits);
    double meanfit_min = 0;
    double meanfit_max = 0;
    if (opts.mean_maxFrac > opts.mean_minFrac) {
      double total_min = mids[0] - binwidths[0];
      double total_max = mids[bins-1] + binwidths[bins-1];
      double width = total_max - total_min;
      meanfit_min = total_min + opts.mean_minFrac * width;
      meanfit_max = total_min + opts.mean_maxFrac * width;
    }
    SetGraphTrend(means,opts.meanfit,meanfit_min,meanfit_max);
    
    // CREATE GRAPH OF WIDTHS
    TGraphErrors sigmas(bins,mids,width,binwidths,widthErrs);
    title = Particle[res] + " " + string(widthLabel) + " vs. " + string(opts.label);
    name = string(opts.type) + "_sigmas";
    SetGraphStyle(sigmas,name.c_str(),title.c_str(),opts.labelUnits,widthLabelUnits);
    double sigmafit_min = 0;
    double sigmafit_max = 0;
    if (opts.sigma_maxFrac > opts.sigma_minFrac) {
      double total_min = mids[0] - binwidths[0];
      double total_max = mids[bins-1] + binwidths[bins-1];
      double width = total_max - total_min;
      sigmafit_min = total_min + opts.sigma_minFrac * width;
      sigmafit_max = total_min + opts.sigma_maxFrac * width;
    }
    SetGraphTrend(sigmas,opts.sigmafit,sigmafit_min,sigmafit_max);

    // CREATE GRAPH OF PURITY
    TGraphErrors fracs(bins,mids,frac,binwidths,fracErrs);
    title = Particle[res] + " " + string(fracLabel) + " vs. " + string(opts.label);
    name = string(opts.type) + "_purities";
    SetGraphStyle(fracs,name.c_str(),title.c_str(),opts.labelUnits,fracLabelUnits);
    double fracfit_min = 0;
    double fracfit_max = 0;
    if (opts.frac_maxFrac > opts.frac_minFrac) {
      double total_min = mids[0] - binwidths[0];
      double total_max = mids[bins-1] + binwidths[bins-1];
      double width = total_max - total_min;
      fracfit_min = total_min + opts.frac_minFrac * width;
      fracfit_max = total_min + opts.frac_maxFrac * width;
    }
    SetGraphTrend(fracs,opts.fracfit,fracfit_min,fracfit_max);

    // CREATE GRAPH OF PCHI2
    TGraphErrors pchi2s(bins,mids,pchi2,binwidths,pchi2Errs);
    title = Particle[res] + " " + string(pchi2Label) + " vs. " + string(opts.label);
    name = string(opts.type) + "_pchi2";
    SetGraphStyle(pchi2s,name.c_str(),title.c_str(),opts.labelUnits,pchi2LabelUnits);

    // SAVE TO FILE
    file.cd();
    means.Write();
    sigmas.Write();
    fracs.Write();
    pchi2s.Write();
  
    // MEMORY MANAGEMENT
    delete mean;
    delete meanErrs;
    delete width;
    delete widthErrs;
    delete pchi2;
    delete pchi2Errs;
    delete mids;
    delete binwidths;
  }

  void UpDownCombiner(TFile& file, const char* up_name, const char* down_name, const char* name, const char* label) 
  {
    // Why doesn't this work?
    //     gROOT->Reset();
    //     gStyle->SetOptStat(0);
    //     gStyle->SetOptFit(0);

    // MEAN
    string upmeans = string(up_name) + "_means";
    string downmeans = string(down_name) + "_means";
    TGraphErrors* up_mean = (TGraphErrors*)file.Get(upmeans.c_str());
    TGraphErrors* down_mean = (TGraphErrors*)file.Get(downmeans.c_str());
    up_mean->SetTitle("up");
    up_mean->SetLineColor(kBlue);
    up_mean->SetMarkerColor(kBlue);
    up_mean->SetMarkerStyle(22);
    up_mean->SetMarkerSize(2.0);
    up_mean->GetFunction("pol1")->SetLineColor(kBlue);
    down_mean->SetTitle("down");
    down_mean->SetLineColor(kRed);
    down_mean->SetMarkerColor(kRed);
    down_mean->SetMarkerStyle(23);
    down_mean->SetMarkerSize(2.0);
    down_mean->GetFunction("pol1")->SetLineColor(kRed);
    string means = string(name) + "_means";
    TCanvas means_canvas(means.c_str(),means.c_str(),800,600);
    means_canvas.SetGrid();
    string mean_title = Particle[res] + " " + string(meanLabel) + " vs. " + string(label);
    TMultiGraph mean_mg(name,mean_title.c_str());
    mean_mg.Add(up_mean);
    mean_mg.Add(down_mean);
    mean_mg.Draw("AP");
    mean_mg.GetHistogram()->SetXTitle("run");
    mean_mg.GetHistogram()->SetYTitle(meanLabelUnits);
    means_canvas.BuildLegend(0.9,0.9,1.0,1.0,"");
    //     TPaveStats *upmeans_stats = (TPaveStats*)up_mean->GetListOfFunctions()->FindObject("stats");
    //     TPaveStats *downmeans_stats = (TPaveStats*)down_mean->GetListOfFunctions()->FindObject("stats");
    //     upmeans_stats->SetTextColor(kBlue);
    //     downmeans_stats->SetTextColor(kRed);
    //     upmeans_stats->SetX1NDC(0.47); upmeans_stats->SetX2NDC(0.67); upmeans_stats->SetY1NDC(0.75);
    //     downmeans_stats->SetX1NDC(0.72); downmeans_stats->SetX2NDC(0.92); downmeans_stats->SetY1NDC(0.75);
    mean_mg.Fit("pol1","W","",0,0);
    mean_mg.Fit("pol1","","",0,0);
    mean_mg.GetFunction("pol1")->SetLineColor(kGray+1);
    mean_mg.GetFunction("pol1")->SetLineStyle(kDashed);
    //     TPaveStats *meanps = (TPaveStats*)mean_mg.GetListOfFunctions()->FindObject("stats");
    //     std::cout << meanps << std::endl;
    //     TList *meanstats_list = meanps->GetListOfLines();
    //     TText *meanstats_text = meanps->GetLineWith("p0");
    //     meanstats_list->Remove(meanstats_text);
    //     meanps->SetX1NDC(0.72); meanps->SetX2NDC(0.92); meanps->SetY1NDC(0.75); meanps->SetY2NDC(0.65);
    means_canvas.Modified();

    // SIGMA
    string upsigmas = string(up_name) + "_sigmas";
    string downsigmas = string(down_name) + "_sigmas";
    TGraphErrors* up_sigma = (TGraphErrors*)file.Get(upsigmas.c_str());
    TGraphErrors* down_sigma = (TGraphErrors*)file.Get(downsigmas.c_str());
    up_sigma->SetTitle("up");
    up_sigma->SetLineColor(kBlue);
    up_sigma->SetMarkerColor(kBlue);
    up_sigma->SetMarkerStyle(22);
    up_sigma->SetMarkerSize(2.0);
    up_sigma->GetFunction("pol1")->SetLineColor(kBlue);
    down_sigma->SetTitle("down");
    down_sigma->SetLineColor(kRed);
    down_sigma->SetMarkerColor(kRed);
    down_sigma->SetMarkerStyle(23);
    down_sigma->SetMarkerSize(2.0);
    down_sigma->GetFunction("pol1")->SetLineColor(kRed);
    string sigmas = string(name) + "_sigmas";
    TCanvas sigmas_canvas(sigmas.c_str(),sigmas.c_str(),800,600);
    sigmas_canvas.SetGrid();
    string sigma_title = Particle[res] + " " + string(widthLabel) + " vs. " + string(label);
    TMultiGraph sigma_mg(name,sigma_title.c_str());
    sigma_mg.Add(up_sigma);
    sigma_mg.Add(down_sigma);
    sigma_mg.Draw("AP");
    sigma_mg.GetHistogram()->SetXTitle("run");
    sigma_mg.GetHistogram()->SetYTitle(widthLabelUnits);
    sigmas_canvas.BuildLegend(0.9,0.9,1.0,1.0,"");
    //     TPaveStats *upsigmas_stats = (TPaveStats*)up_sigma->GetListOfFunctions()->FindObject("stats");
    //     TPaveStats *downsigmas_stats = (TPaveStats*)down_sigma->GetListOfFunctions()->FindObject("stats");
    //     upsigmas_stats->SetTextColor(kBlue);
    //     downsigmas_stats->SetTextColor(kRed);
    //     upsigmas_stats->SetX1NDC(0.42); upsigmas_stats->SetX2NDC(0.62); upsigmas_stats->SetY1NDC(0.75);
    //     downsigmas_stats->SetX1NDC(0.72); downsigmas_stats->SetX2NDC(0.92); downsigmas_stats->SetY1NDC(0.75);
    sigma_mg.Fit("pol1","W","",0,0);
    sigma_mg.Fit("pol1","","",0,0);
    sigma_mg.GetFunction("pol1")->SetLineColor(kGray+1);
    sigma_mg.GetFunction("pol1")->SetLineStyle(kDashed);
    //     TPaveStats *sigmaps = (TPaveStats*)sigma_mg.GetListOfFunctions()->FindObject("stats");
    //     std::cout << sigmaps << std::endl;
    //     TList *sigmastats_list = sigmaps->GetListOfLines();
    //     TText *sigmastats_text = sigmaps->GetLineWith("p0");
    //     sigmastats_list->Remove(sigmastats_text);
    //     sigmaps->SetX1NDC(0.72); sigmaps->SetX2NDC(0.92); sigmaps->SetY1NDC(0.75); sigmaps->SetY2NDC(0.65);
    sigmas_canvas.Modified();

    // FRAC
    string upfracs = string(up_name) + "_purities";
    string downfracs = string(down_name) + "_purities";
    TGraphErrors* up_frac = (TGraphErrors*)file.Get(upfracs.c_str());
    TGraphErrors* down_frac = (TGraphErrors*)file.Get(downfracs.c_str());
    up_frac->SetTitle("up");
    up_frac->SetLineColor(kBlue);
    up_frac->SetMarkerColor(kBlue);
    up_frac->SetMarkerStyle(22);
    up_frac->SetMarkerSize(2.0);
    up_frac->GetFunction("pol1")->SetLineColor(kBlue);
    down_frac->SetTitle("down");
    down_frac->SetLineColor(kRed);
    down_frac->SetMarkerColor(kRed);
    down_frac->SetMarkerStyle(23);
    down_frac->SetMarkerSize(2.0);
    down_frac->GetFunction("pol1")->SetLineColor(kRed);
    string fracs = string(name) + "_fracs";
    TCanvas fracs_canvas(fracs.c_str(),fracs.c_str(),800,600);
    fracs_canvas.SetGrid();
    string frac_title = Particle[res] + " " + string(fracLabel) + " vs. " + string(label);
    TMultiGraph frac_mg(name,frac_title.c_str());
    frac_mg.Add(up_frac);
    frac_mg.Add(down_frac);
    frac_mg.Draw("AP");
    frac_mg.GetHistogram()->SetXTitle("run");
    frac_mg.GetHistogram()->SetYTitle(fracLabelUnits);
    fracs_canvas.BuildLegend(0.9,0.9,1.0,1.0,"");
    //     TPaveStats *upfracs_stats = (TPaveStats*)up_frac->GetListOfFunctions()->FindObject("stats");
    //     TPaveStats *downfracs_stats = (TPaveStats*)down_frac->GetListOfFunctions()->FindObject("stats");
    //     upfracs_stats->SetTextColor(kBlue);
    //     downfracs_stats->SetTextColor(kRed);
    //     upfracs_stats->SetX1NDC(0.42); upfracs_stats->SetX2NDC(0.62); upfracs_stats->SetY1NDC(0.75);
    //     downfracs_stats->SetX1NDC(0.72); downfracs_stats->SetX2NDC(0.92); downfracs_stats->SetY1NDC(0.75);
    frac_mg.Fit("pol1","W","",0,0);
    frac_mg.Fit("pol1","","",0,0);
    frac_mg.GetFunction("pol1")->SetLineColor(kGray+1);
    frac_mg.GetFunction("pol1")->SetLineStyle(kDashed);
    //     TPaveStats *fracps = (TPaveStats*)frac_mg.GetListOfFunctions()->FindObject("stats");
    //     std::cout << fracps << std::endl;
    //     TList *fracstats_list = fracps->GetListOfLines();
    //     TText *fracstats_text = fracps->GetLineWith("p0");
    //     fracstats_list->Remove(fracstats_text);
    //     fracps->SetX1NDC(0.72); fracps->SetX2NDC(0.92); fracps->SetY1NDC(0.75); fracps->SetY2NDC(0.65);
    fracs_canvas.Modified();

    // SAVE TO FILE
    file.cd();
    means_canvas.Write();
    file.cd();
    sigmas_canvas.Write();
    file.cd();
    fracs_canvas.Write();
  }
}

// MAIN

int main( int argc, char** argv) {

  // ----------------------------------------------------------------------------------------------------
  // SETUP
  // ----------------------------------------------------------------------------------------------------

  /* This takes several arguments:
     1) Name of resonanace (Jpsi2MuMu, Ups1S2MuMu, etc)
     2) Name of ROOT file containing histogram
     3) Location of histograms inside ROOT file (e.g. Track/TrackParticleMonitor)
     4) Name of output directory
  */

  // CHECK THAT THERE ARE ENOUGH
  if (verbose) cout << "Starting SETUP" << endl;
  if (argc < 5) {
    cout << "ERROR: missing arguments" << endl;
    return 1;
  }

  // RESONANCE NUMBER FROM NAME
  string resname = string(argv[1]);
  res = -1;
  for (int i = 0; i < NumRes; i++)
    if (resname.compare(ID[i]) == 0)
      res = i;
  assert(res >= 0);
  if (verbose) cout << "Resonance # " << res << ": " << ID[res] << endl;
  
  // STRING ARGUMENTS
  string HistoFile = argv[2];
  if (verbose) cout << HistoFile << endl;
  string rootDirectoryFile = string(argv[3]);
  if (verbose) cout << rootDirectoryFile << endl;
  string OutputDirectory = string(argv[4]);
  if (verbose) cout << OutputDirectory << endl;

  // OPENING ROOT FILE
  TFile inputFile(HistoFile.c_str(),"READ");
  if (inputFile.IsZombie()) {
    cout << "ERROR: input file not found" << endl;
    return 1;
  } else  {
    if (verbose) cout << "Input file opened successfully" << endl;
  }

  // Other
  gStyle->SetOptStat();
  gStyle->SetOptFit();

  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------
  // FITS
  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------

  // Get model
  RooAbsPdf* model = ResModel[res];
  RooRealVar* mass = ResMass[res];
  RooAbsReal* mean = ResMean[res];
  RooAbsReal* width = ResWidth[res];
  RooAbsReal* purity = ResPurity[res];
  
  RooArgList vars(*mean,*width,*purity);

  //   z0_rbw1.setInterpolationOrder(2);
  //   z0_rbw2.setInterpolationOrder(2);

  // Other tweaks
  //   if (ID[res] == "D02KPi")
  //     c1.setVal(-0.3);

  // ----------------------------------------------------------------------------------------------------
  // OUTPUT FILE
  // ----------------------------------------------------------------------------------------------------

  string graph_filename = OutputDirectory + "/" + ID[res] + "_graphs" + RootExt;
  TFile fout(graph_filename.c_str(),"recreate");
  
  // ----------------------------------------------------------------------------------------------------
  // TOTAL FIT
  // ----------------------------------------------------------------------------------------------------
  
  bool totalfit = true;
  if (totalfit) {
    
    // MAKE ROO DATA OBJECT
    string massStr = rootDirectoryFile + "/mass";
    cout << massStr << endl;
    TH1D* mass_th1 = (TH1D*)(inputFile.Get(massStr.c_str()));
    RooDataHist Hist ("massH1","massH1",RooArgList(*mass),mass_th1);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    Hist.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter totFitter(mass,model,0,&vars,OutputDirectory);
    if (verbose) cout << "Fitting over all data" << endl;
    vector<binValsAndErrs> data = totFitter.fit(&Hist,"fitTotal",false);
  }
  
  // ----------------------------------------------------------------------------------------------------
  // MOM-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------

  bool split_mom = true;
  if (split_mom) {
    if (verbose) cout << "Fitting over each mom bin separately:" << endl;

    // MAKE ROO DATA OBJECT
    string massVmomStr = rootDirectoryFile + "/massVersusMomH2";
    TH2D* massVmom_th2 = (TH2D*)(inputFile.Get(massVmomStr.c_str()));
    RooCategory momCat ("momCat","momCat");
    RooDataHist Hist_mom = splitDataHistFromTH2D("massVmomH2","massVmomH2",*mass,momCat,massVmom_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    Hist_mom.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter momFitter(mass,model,0,&vars,&momCat,OutputDirectory);
    vector<binValsAndErrs> data = momFitter.fit(&Hist_mom,"fitSplit_mom",false);
    PlotOptions plotopts = { "mom", "P", "P [GeV]", "pol0",0,0, "pol1",0,0, "pol0",0,0 };
    plotMakerAndSaver(fout,massVmom_th2,data,plotopts);
  }

  // ----------------------------------------------------------------------------------------------------
  // PT-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------

  bool split_pt = true;
  if (split_pt) {
    if (verbose) cout << "Fitting over each pt bin separately:" << endl;

    // MAKE ROO DATA OBJECT
    string massVptStr = rootDirectoryFile + "/massVersusPtH2";
    TH2D* massVpt_th2 = (TH2D*)(inputFile.Get(massVptStr.c_str()));
    RooCategory ptCat ("ptCat","ptCat");
    RooDataHist Hist_pt = splitDataHistFromTH2D("massVptH2","massVptH2",*mass,ptCat,massVpt_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter ptFitter(mass,model,0,&vars,&ptCat,OutputDirectory);
    vector<binValsAndErrs> data = ptFitter.fit(&Hist_pt,"fitSplit_pt",false);
    PlotOptions plotopts = { "pt", "PT", "PT [GeV]", "pol0",0,0, "pol1",0,0, "pol0", 0, 0 };
    plotMakerAndSaver(fout,massVpt_th2,data,plotopts);
  }

  // ----------------------------------------------------------------------------------------------------
  // MOMDIFF-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------
   
  bool split_momdiff = true;
  if (split_momdiff) {
    if (verbose) cout << "Fitting over each momdiffs bin separately:" << endl;
     
    // MAKE ROO DATA OBJECT
    string massVmomdiffStr = rootDirectoryFile + "/massVersusMomDifH2";
    TH2D* massVmomdiff_th2 = (TH2D*)(inputFile.Get(massVmomdiffStr.c_str()));
    RooCategory momdiffCat ("momdiffCat","momdiffCat");
    RooDataHist Hist_momdiff = splitDataHistFromTH2D("massVmomdiffH2","massVmomdiffH2",*mass,momdiffCat,massVmomdiff_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
     
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter momdiffFitter(mass,model,0,&vars,&momdiffCat,OutputDirectory);
    vector<binValsAndErrs> data = momdiffFitter.fit(&Hist_momdiff,"fitSplit_momdiff",false);
    PlotOptions plotopts = { "momdiff", "p^{+}-p^{-}", "p^{+}-p^{-} [GeV]", "pol1",0,0, "pol0",0,0, "pol0",0,0 };
    plotMakerAndSaver(fout,massVmomdiff_th2,data,plotopts);
  }
  
  // ----------------------------------------------------------------------------------------------------
  // MOMASYM-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------
  
  bool split_momasym = true;
  if (split_momasym) {
    if (verbose) cout << "Fitting over each momasym bin separately:" << endl;
     
    // MAKE ROO DATA OBJECT
    string massVmomasymStr = rootDirectoryFile + "/massVersusAsymH2";
    TH2D* massVmomasym_th2 = (TH2D*)(inputFile.Get(massVmomasymStr.c_str()));
    RooCategory momasymCat ("momasymCat","momasymCat");
    RooDataHist Hist_momasym = splitDataHistFromTH2D("massVmomasymH2","massVmomasymH2",*mass,momasymCat,massVmomasym_th2); 
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter momasymFitter(mass,model,0,&vars,&momasymCat,OutputDirectory);
    vector<binValsAndErrs> data = momasymFitter.fit(&Hist_momasym,"fitSplit_momasym",false);
    PlotOptions plotopts = { "momasym", "(p^{+}-p^{-})/(p^{+}+p^{-})", "(p^{+}-p^{-})/(p^{+}+p^{-}) [GeV]", "pol1",0.3,0.7, "pol2",0,0, "pol0",0,0 };
    plotMakerAndSaver(fout,massVmomasym_th2,data,plotopts);
  }
  
  // ----------------------------------------------------------------------------------------------------
  // ETA-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------
  
  bool split_eta = true;
  if (split_eta) {
    if (verbose) cout << "Fitting over eta bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massVetaStr = rootDirectoryFile + "/massVersusEtaH2";
    TH2D* massVeta_th2 = (TH2D*)(inputFile.Get(massVetaStr.c_str()));
    RooCategory etaCat ("etaCat","etaCat");
    RooDataHist Hist_eta = splitDataHistFromTH2D("massVetaH2","massVetaH2",*mass,etaCat,massVeta_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter etaFitter(mass,model,0,&vars,&etaCat,OutputDirectory);
    vector<binValsAndErrs> data = etaFitter.fit(&Hist_eta,"fitSplit_eta",false);
    PlotOptions plotopts = { "eta","#eta","#eta", "pol0", 0, 0, "pol1", 0, 0, "pol0", 0, 0 };
    plotMakerAndSaver(fout,massVeta_th2,data,plotopts);
  }
  
  // ----------------------------------------------------------------------------------------------------
  // TX-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------

  bool split_tx = true;
  if (split_tx) {
    if (verbose) cout << "Fitting over each tx bin separately:" << endl;

    // MAKE ROO DATA OBJECT
    string massVtxStr = rootDirectoryFile + "/massVersusTxH2";
    TH2D* massVtx_th2 = (TH2D*)(inputFile.Get(massVtxStr.c_str()));
    RooCategory txCat ("txCat","txCat");
    RooDataHist Hist_tx = splitDataHistFromTH2D("massVtxH2","massVtxH2",*mass,txCat,massVtx_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter txFitter(mass,model,0,&vars,&txCat,OutputDirectory);
    vector<binValsAndErrs> data = txFitter.fit(&Hist_tx,"fitSplit_tx",false);
    PlotOptions plotopts = { "tx","Tx","Tx", "pol0", 0, 0, "pol0", 0, 0, "pol0", 0, 0 };
    plotMakerAndSaver(fout,massVtx_th2,data,plotopts);
  }

  // ----------------------------------------------------------------------------------------------------
  // TY-SPLIT SIMULTANEOUS FITS
  // ----------------------------------------------------------------------------------------------------

  bool split_ty = true;
  if (split_ty) {
    if (verbose) cout << "Fitting over each ty bin separately:" << endl;

    // MAKE ROO DATA OBJECT
    string massVtyStr = rootDirectoryFile + "/massVersusTyH2";
    TH2D* massVty_th2 = (TH2D*)(inputFile.Get(massVtyStr.c_str()));
    RooCategory tyCat ("tyCat","tyCat");
    RooDataHist Hist_ty = splitDataHistFromTH2D("massVtyH2","massVtyH2",*mass,tyCat,massVty_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter tyFitter(mass,model,0,&vars,&tyCat,OutputDirectory);
    vector<binValsAndErrs> data = tyFitter.fit(&Hist_ty,"fitSplit_ty",false);
    PlotOptions plotopts = { "ty","Ty","Ty", "pol0", 0, 0, "pol0", 0, 0, "pol0", 0, 0 };
    plotMakerAndSaver(fout,massVty_th2,data, plotopts);
  }

  // ----------------------------------------------------------------------------------------------------
  // PHID-SPLIT SIMULTANEOUS FITS (decay plane angle w.r.t. magnetic field)
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_phid = true;
  if (split_phid) {
    if (verbose) cout << "Fitting over each phids bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massVphidStr = rootDirectoryFile + "/massVersusPhiMattH2";
    TH2D* massVphid_th2 = (TH2D*)(inputFile.Get(massVphidStr.c_str()));
    RooCategory phidCat ("phidCat","phidCat");
    RooDataHist Hist_phid = splitDataHistFromTH2D("massVphidH2","massVphidH2",*mass,phidCat,massVphid_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(mass,model,0,&vars,&phidCat,OutputDirectory);
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_phid,"fitSplit_phid",false);
    PlotOptions plotopts = { "phid","#phi_{d}","#phi_{d}", "pol0", 0, 0, "pol0", 0, 0, "pol0", 0, 0 };
    plotMakerAndSaver(fout,massVphid_th2,data,plotopts);
  }
  
  
  // ----------------------------------------------------------------------------------------------------
  // PHI-SPLIT SIMULTANEOUS FITS (decay plane azimuthal angle)
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_phi = true;
  if (split_phi) {
    if (verbose) cout << "Fitting over each phi bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massVphiStr = rootDirectoryFile + "/massVersusPhiH2";
    TH2D* massVphi_th2 = (TH2D*)(inputFile.Get(massVphiStr.c_str()));
    RooCategory phiCat ("phiCat","phiCat");
    RooDataHist Hist_phi = splitDataHistFromTH2D("massVphiH2","massVphiH2",*mass,phiCat,massVphi_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(mass,model,0,&vars,&phiCat,OutputDirectory);
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_phi,"fitSplit_phi",false);
    PlotOptions plotopts = { "phi","#phi","#phi", "pol0", 0, 0, "pol0", 0, 0, "pol0", 0, 0 };
    plotMakerAndSaver(fout,massVphi_th2,data, plotopts);
  }
  
  
  // ----------------------------------------------------------------------------------------------------
  // THETA-SPLIT SIMULTANEOUS FITS (opening angle)
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_theta = true;
  if (split_theta) {
    if (verbose) cout << "Fitting over each theta bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massVthetaStr = rootDirectoryFile + "/massVersusOpenAngleH2";
    TH2D* massVtheta_th2 = (TH2D*)(inputFile.Get(massVthetaStr.c_str()));
    RooCategory thetaCat ("thetaCat","thetaCat");
    RooDataHist Hist_theta = splitDataHistFromTH2D("massVthetaH2","massVthetaH2",*mass,thetaCat,massVtheta_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(mass,model,0,&vars,&thetaCat,OutputDirectory);
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_theta,"fitSplit_theta",false);
    PlotOptions plotopts = { "theta","#theta","#theta", "pol1", 0, 0, "pol1", 0, 0, "pol1", 0, 0 };
    plotMakerAndSaver(fout,massVtheta_th2,data,plotopts);
  }


  // ----------------------------------------------------------------------------------------------------
  // 2011 TIME SPLIT DATA
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_iov2011 = true;
  if (split_iov2011) {
    if (verbose) cout << "Fitting over each run bin separately:" << endl;

    // Get MagnetUp / MagnetDown
    vector<bool> magnetUp;
    vector<bool> magnetDown;
    std::stringstream input;
    input << iov_data_2011;
    string line;
    while (std::getline(input, line))
    {
      std::istringstream ss(line);
      string condition, startDate, endDate, fillRange;
      int startRun, endRun;
      double luminosity;
      ss >> condition >> startDate >> endDate >> fillRange >> startRun >> endRun >> luminosity;
      if (condition == "MagUp") {
        magnetUp.push_back(true);
        magnetDown.push_back(false);
      } else {
        magnetUp.push_back(false);
        magnetDown.push_back(true);
      }
    }

    // MAKE ROO DATA OBJECT
    string massViov2011Str = rootDirectoryFile + "/massVersus2011iovH2";
    TH2D* massViov2011_th2 = (TH2D*)(inputFile.Get(massViov2011Str.c_str()));
    RooCategory iovCat ("iov2011Cat","iov2011Cat");
    RooDataHist Hist_iov2011 = splitDataHistFromTH2D("massViov2011H2","massViov2011H2",*mass,iovCat,massViov2011_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter iov2011Fitter(mass,model,0,&vars,&iovCat,OutputDirectory);
    vector<binValsAndErrs> data = iov2011Fitter.fit(&Hist_iov2011,"fitSplit_iov2011",false);
    PlotOptions plotopts_Up = { "iov2011_Up","2011 UP IOV","run", "pol1", 0, 0, "pol1", 0, 0, "pol1", 0, 0 };
    PlotOptions plotopts_Down = { "iov2011_Down","2011 DOWN IOV","run", "pol1", 0, 0, "pol1", 0, 0, "pol1", 0, 0 };
    plotMakerAndSaver(fout,massViov2011_th2,data,plotopts_Up,&magnetUp);
    plotMakerAndSaver(fout,massViov2011_th2,data,plotopts_Down,&magnetDown);
    UpDownCombiner(fout,"iov2011_Up","iov2011_Down","iov2011","2011 IOV");
  }

  // ----------------------------------------------------------------------------------------------------
  // 2012 TIME SPLIT DATA
  // ----------------------------------------------------------------------------------------------------
  
  bool split_iov2012 = true;
  if (split_iov2012) {
    if (verbose) cout << "Fitting over each run bin separately:" << endl;
    
    // Get MagnetUp / MagnetDown
    vector<bool> magnetUp;
    vector<bool> magnetDown;
    std::stringstream input;
    input << iov_data_2012;
    string line;
    int counter = 0; // to ignore first two
    while (std::getline(input, line))
    {
      std::istringstream ss(line);
      string condition, startDate, endDate, fillRange;
      int startRun, endRun;
      double luminosity;
      ss >> condition >> startDate >> endDate >> fillRange >> startRun >> endRun >> luminosity;
      cout << condition << endl;
      if (counter < 2) {
        magnetUp.push_back(false);
        magnetDown.push_back(false);
        counter++;
      } else {
        if (condition == "MagUp") {
          magnetUp.push_back(true);
          magnetDown.push_back(false);
        } else {
          magnetUp.push_back(false);
          magnetDown.push_back(true);
        }    
      } 
    }

    // MAKE ROO DATA OBJECT
    string massViov2012Str = rootDirectoryFile + "/massVersus2012iovH2";
    TH2D* massViov2012_th2 = (TH2D*)(inputFile.Get(massViov2012Str.c_str()));
    RooCategory iovCat ("iov2012Cat","iov2012Cat");
    RooDataHist Hist_iov2012 = splitDataHistFromTH2D("massViov2012H2","massViov2012H2",*mass,iovCat,massViov2012_th2);
    if (ID[res] == string("Z0"))
      mass->setBins(1000);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter iov2012Fitter(mass,model,0,&vars,&iovCat,OutputDirectory);
    vector<binValsAndErrs> data = iov2012Fitter.fit(&Hist_iov2012,"fitSplit_iov2012",false);
    PlotOptions plotopts_Up = { "iov2012_Up","2012 UP IOV","run","pol1", 0, 0, "pol1", 0, 0, "pol1", 0, 0 };
    PlotOptions plotopts_Down = { "iov2012_Down","2012 DOWN IOV","run","pol1", 0, 0, "pol1", 0, 0, "pol1", 0, 0 };
    plotMakerAndSaver(fout,massViov2012_th2,data,plotopts_Up,&magnetUp);
    plotMakerAndSaver(fout,massViov2012_th2,data,plotopts_Down,&magnetDown);
    UpDownCombiner(fout,"iov2012_Up","iov2012_Down","iov2012","2012 IOV");
  }
  
  
  // CLOSE FILE
  fout.Close();
  
  // RETURN
  return 0;
}
