// Project INCLUDES
#include "MassMonitor/SplitFitter.hh"

// C INCLUDES
#include <cstring>
#include <cassert>

// C++ INCLUDES
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;

// ROOT INCLUDES
//#include <TAxis.h>
//#include <TChain.h>
#include <TFile.h>
#include <TGraphErrors.h>
//#include <TH1F.h>
#include <TH1D.h>
#include <TH2D.h>

// ROOFIT INCLUDES
#include <RooAddPdf.h>
#include <RooArgSet.h> 
#include <RooGaussian.h>
#include <RooCBShape.h>
#include <RooExponential.h>
#include <RooGenericPdf.h>
#include <RooChebychev.h>
#include <RooCmdArg.h>
#include <RooTable.h>

namespace {

  // THIS BOOLEAN CONTROLS HOW MUCH INFO IS PRINTED
  const bool verbose = true;  

  // DO MEAN CORRECTION?
  bool meanCorrection = true; // do this by default
  
  // INPUT / OUTPUT
  const string RootExt = ".root";
  const string FitExt = ".fitvar";
  
  // DECAY CONSTANTS

  const int NumRes = 6;
  int res = 0; // The particular decay for this execution; initialized at the command line
  
  const string ID[NumRes] =
    {
      "Jpsi2MuMu"
      , "Jpsi2MuMu_Double"
      , "Jpsi2MuMu_Cruijff"
      , "Ups1S2MuMu"
      , "Z02MuMu"
      , "D02KPi"
    }; // Short form decay string, e.g. Jpsi2MuMu  
  const string Particle[NumRes] =
    {
      "J/psi(1S)"
      , "J/psi(1S)"
      , "J/psi(1S)"
      , "Upsilon(1S)"
      , "Z0"
      , "D0"
    }; // Long form LHCb particle name, e.g. J/psi(1S)
  const double Mass[NumRes] = 
    {
      3.096916
      , 3.096916
      , 3.096916
      , 9.4603026
      , 91.1876
      , 1.86484
    }; // Resonance mass; starting value for fit
  const double Width[NumRes] =
    {
      0.08
      , 0.0133
      , 0.08
      , 0.05
      , 10.00
      , 0.008
    }; // Starting width for fit

  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------
  // MODEL PDFS
  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------
  
  double AlphaCor[NumRes] = {
    0.055
    , 0.055
    , 0
    , 0
    , 0
  };
  
  double Alpha2Cor[NumRes] = {
    -2.6E-4
    , -2.6E-4
    , 0
    , 0
    , 0
  };
    
  // Signal mean and width (common to all shapes, basically)
  RooRealVar mass ("mass","mass", 0.0);
  RooRealVar mean("mean","mass mean", 0.0);
  RooRealVar width("width","mass width", 0.0);

  // Gaussian
  RooGaussian signal_g("signal_g","signal (G)",mass,mean,width);

  // Crystal Ball
  RooRealVar z("z","z",1.5,0.01,5);
  RooRealVar n("n","n",1);
  RooCBShape signal_cb("signal_cb","signal (CB)",mass,mean,width,z,n);

  // Double Crystal Ball FOR JPSI
  /* Adding gaussians f Gaus(mu,s) + (1-f) Gaus(mu,t), what is the effective width r?
     (Note: means here are different, but not that different)
     Add them, take logarithm, find second derivative at mean
     This gives 1/r^2 = f/s^2 + (1-f)/t^2
     So, given r (width), a = s/r (subwidth1_ratio), and b = t/r (subwidth2_ratio),
     we can find f (sigratio):
     f = a^2 * (b^2 - 1) / (b^2 - a^2)
     (a and b must both be in range 0 to 1, because combined width is always bigger)
     For this to be well defined, we clearly need 1 > b > a > 0. How to do this?
     It means that 1 < 1/b < 1/a < infinity, so we can say with impunity
     1/a = 1/b + d => a = b / (1 + bd)
   */
//   RooRealVar subwidth2_ratio("subwidth2_ratio","mass subwidth2_ratio",1.5,0.1,2);
//   RooRealVar subwidth12_diff("subwidth12_diff","mass subwidth12_diff",0.5,1E-4,10);
//   RooFormulaVar subwidth1_ratio("subwidth1_ratio","mass subwidth1_ratio","@0/(1+@0*@1)",
//                                 RooArgList(subwidth2_ratio,subwidth12_diff));
//   RooFormulaVar subwidth1("subwidth1","mass subwidth1","width*subwidth1_ratio",RooArgList(width,subwidth1_ratio));
//   RooFormulaVar subwidth2("subwidth2","mass subwidth2","width*subwidth2_ratio",RooArgList(width,subwidth2_ratio));
//   RooFormulaVar sigratio("sigratio","sigratio","@0*@0 * (@1*@1-1) / (@1*@1 - @0*@0)",
//                          RooArgList(subwidth1_ratio,subwidth2_ratio));

  /* The following deserves some explanation
     True J/psi distribution is a sharp breit-wigner + tail
     Gaussian resolution -> crystal ball with shifted mean
     Dobule gaussian resolution -> double crystal ball with separate, but related, shifted means
     For more detail, see LHCb-ANA-2011-027 Appendix B
  */
  RooRealVar extrawidth("extrawidth","extrawidth",1.1,1,3);
  RooFormulaVar width2("width2","mass width2","width*extrawidth",RooArgList(width,extrawidth));
  RooRealVar alphacor("alphacor","alphacor",AlphaCor[res]);
  RooRealVar alpha2cor("alpha2cor","alpha2cor",Alpha2Cor[res]);  
  string mean2_formula = "@0 + @3 * (@1-@2) + @4 * (@1*@1-@2*@2)";
  //  RooFormulaVar mean2("mean2","mass mean2",mean2_formula.c_str(), RooArgList(mean,subwidth1,subwidth2,alphacor,alpha2cor));
  RooFormulaVar mean2("mean2","mass mean2",mean2_formula.c_str(), RooArgList(mean,width,width2,alphacor,alpha2cor));
  RooRealVar z2("z2","z2",1.5,0.01,5);
  RooRealVar n2("n2","n2",1);
//   RooCBShape subcb1("subcb1","subcb1",mass,mean,subwidth1,z,n);
//   RooCBShape subcb2("subcb2","subcb2",mass,mean2,subwidth2,z2,n2);
  RooCBShape subcb1("subcb1","subcb1",mass,mean,width,z,n);
  RooCBShape subcb2("subcb2","subcb2",mass,mean2,width2,z2,n2);
  RooRealVar sigratio("sigratio","sigratio",0.7,0,1);
  RooAddPdf signal_2cb("signal_2cb","signal_2cb",RooArgList(subcb1,subcb2),RooArgList(sigratio));

  // Cruijff Shapes
  RooRealVar alpha("alpha","alpha",0.1,1E-10,1);
  RooRealVar alphaR("alphaR","alphaR",0.1,1E-10,1);
  const char* cr1_formula =
    "exp( - (mass-mean)*(mass-mean)"
    " / (2*width*width "
    "+ alpha*(mass-mean)*(mass-mean)*(mass<mean) ) )";
  RooGenericPdf signal_cr1("signal_cr1","signal (CR1)",cr1_formula,RooArgSet(mass,mean,width,alpha));

  const char* cr2_formula =
    "exp( - (mass-mean)*(mass-mean)"
    " / (2*width*width "
    "+ alpha*(mass-mean)*(mass-mean)*(mass<mean) "
    "+ alphaR*(mass-mean)*(mass-mean)*(mass>mean) ) )";
  RooGenericPdf signal_cr2("signal_cr2","signal (CR2)",cr2_formula,RooArgSet(mass,mean,width,alpha,alphaR));

  // Exponential background
  RooRealVar decay("decay","decay",0,-10,10);
  RooExponential bkg_exp("bkg_exp","bkg (EXP)",mass,decay);

  // Polynomial backgrounds
  RooRealVar c1("c1","c1",0,-10,10);
  RooRealVar c2("c2","c2",0,-10,10);
  RooChebychev bkg_p0("bkg_p0","bkg (P0)",mass,RooArgList());
  RooChebychev bkg_p1("bkg_p1","bkg (P1)",mass,RooArgList(c1));
  RooChebychev bkg_p2("bkg_p2","bkg (P2)",mass,RooArgList(c1,c2));  

  // Combinations
  RooRealVar frac("frac","frac",0.7,0,1);
  RooAddPdf model_cb_exp("model_cb_exp","model_cb_exp",RooArgList(signal_cb,bkg_exp),RooArgList(frac)); // J/psi, Ups(#s), Z0
  RooAddPdf model_2cb_exp("model_2cb_exp","model_2cb_exp",RooArgList(signal_2cb,bkg_exp),RooArgList(frac)); // J/psi, Ups(#s), Z0
  RooAddPdf model_cr2_exp("model_cr2_exp","model_cr2_exp",RooArgList(signal_cr2,bkg_exp),RooArgList(frac));
  RooAddPdf model_cr2_p1("model_cr2_p1","model_cr2_p1",RooArgList(signal_cr2,bkg_p1),RooArgList(frac)); // D0, above didn't work
  RooAddPdf model_g_p1("model_g_p1","model_g_p1",RooArgList(signal_g,bkg_p1),RooArgList(frac)); // D0, KS, Lambda
  /* Create others as needed, such as 
     - phi : Voigtian / threshold
     - Others for testing purposes
  */

  RooAbsPdf* ResModel[NumRes] = {
    &model_cb_exp
    , &model_2cb_exp
    , &model_cr2_exp
    , &model_cb_exp
    , &model_cb_exp
    , &model_g_p1
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
    g.Fit("pol0");
    g.Draw("AP");
  }

  // Ugly, but saves LOTS of repeated lines
  void plotMakerAndSaver(TFile& file, TH2D* hist, vector<binValsAndErrs> data,
                         const char* type, const char* label, const char* labelUnits)
  {

    // SOME EXTRA LABELS
    char meanLabel[] = "#Deltam/m_{PDG}";
    char meanLabelUnits[] = "#Deltam/m (10^{-3})";
    char widthLabel[] = "#sigma/m_{PDG}";
    char widthLabelUnits[] = "#sigma/m (10^{-3})";
    char fracLabel[] = "signal %";
    char fracLabelUnits[] = "#signal %";

    // CALCULATE VALUES
    int bins = hist->GetNbinsX();
    double* mean = new double[bins];
    double* meanErrs = new double[bins];
    double* width = new double[bins];
    double* widthErrs = new double[bins];
    double* frac = new double[bins];
    double* fracErrs = new double[bins];
    double* mids = new double[bins];
    double* binwidths = new double[bins];
    for (int i = 0; i < bins; i++) {
      mean[i] = (data[0].vals[i]/(Mass[res]) - 1.0)*1000;
      meanErrs[i] = data[0].errs[i]/(Mass[res])*1000;
      width[i] = data[1].vals[i]/(Mass[res])*1000;
      widthErrs[i] = data[1].errs[i]/(Mass[res])*1000;
      frac[i] = data[2].vals[i]*100;
      fracErrs[i] = data[2].errs[i]*100;
      mids[i] = hist->GetBinCenter(i+1);
      binwidths[i] = hist->GetBinWidth(i+1)/2.0;

      // MEAN CORRECTION
      if (meanCorrection) {
        double cor = AlphaCor[res]*width[i] + Alpha2Cor[res]*width[i]*width[i];
        mean[i] += cor;
      }
    }

    // CREATE GRAPH OF MEANS
    TGraphErrors means(bins,mids,mean,binwidths,meanErrs);
    string title = Particle[res] + " " + string(meanLabel) + " vs. " + string(label);
    string name = string(type) + "_means";
    SetGraphStyle(means,name.c_str(),title.c_str(),labelUnits,meanLabelUnits);
    
    // CREATE GRAPH OF WIDTHS
    TGraphErrors sigmas(bins,mids,width,binwidths,widthErrs);
    title = Particle[res] + " " + string(widthLabel) + " vs. " + string(label);
    name = string(type) + "_sigmas";
    SetGraphStyle(sigmas,name.c_str(),title.c_str(),labelUnits,widthLabelUnits);

    // CREATE GRAPH OF PURITY
    TGraphErrors fracs(bins,mids,frac,binwidths,fracErrs);
    title = Particle[res] + " " + string(fracLabel) + " vs. " + string(label);
    name = string(type) + "_purities";
    SetGraphStyle(fracs,name.c_str(),title.c_str(),labelUnits,fracLabelUnits);

    // SAVE TO FILE
    file.cd();
    means.Write();
    sigmas.Write();
    fracs.Write();
  
    // MEMORY MANAGEMENT
    delete mean;
    delete meanErrs;
    delete width;
    delete widthErrs;
    delete mids;
    delete binwidths;
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

  if (argc >= 6) {
    if (string(argv[5]) == "correct")
      meanCorrection = true;
  }
  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------
  // FITS
  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------

  // Get model
  RooAbsPdf* model = ResModel[res];
  
  // Set values and ranges
  mean.setVal(Mass[res]);
  mean.setRange(0.95*Mass[res],1.05*Mass[res]);
  mean.setConstant(false);
  width.setVal(Width[res]);
  width.setRange(0.1*Width[res],10*Width[res]);
  width.setConstant(false);
  RooArgList vars(mean,width,frac);

  // Tweak range
  RooCmdArg range = RooFit::Range("all");
  if (ID[res] == "Jpsi2MuMu" or ID[res] == "Jpsi2MuMu_Double" or ID[res] == "Jpsi2MuMu_Cruijff") {
    range = RooFit::Range(3.02,3.16);
    mass.setRange(3.02,3.16);
  } else if (ID[res] == "D02KPi") {
    range = RooFit::Range(1.82,1.91);
    mass.setRange(1.82,1.91);
  }

  // Other tweaks
  if (ID[res] == "D02KPi")
    c1.setVal(-0.3);

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
    RooDataHist Hist ("massH1","massH1",RooArgList(mass),mass_th1);
    Hist.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter totFitter(&mass,model,&range,&vars,OutputDirectory);
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
    RooDataHist Hist_mom = splitDataHistFromTH2D("massVmomH2","massVmomH2",mass,momCat,massVmom_th2);
    Hist_mom.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter momFitter(&mass,model,&range,&vars,&momCat,OutputDirectory);
    vector<binValsAndErrs> data = momFitter.fit(&Hist_mom,"fitSplit_mom",false);
    plotMakerAndSaver(fout,massVmom_th2,data,"mom","P","P [GeV]");
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
    RooDataHist Hist_pt = splitDataHistFromTH2D("massVptH2","massVptH2",mass,ptCat,massVpt_th2);
    Hist_pt.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter ptFitter(&mass,model,&range,&vars,&ptCat,OutputDirectory);
    vector<binValsAndErrs> data = ptFitter.fit(&Hist_pt,"fitSplit_pt",false);
    plotMakerAndSaver(fout,massVpt_th2,data,"pt","PT","PT [GeV]");
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
     RooDataHist Hist_momdiff = splitDataHistFromTH2D("massVmomdiffH2","massVmomdiffH2",mass,momdiffCat,massVmomdiff_th2);
     
     // CREATE FITTER OBJECT; PERFORM FIT
     SplitFitter momdiffFitter(&mass,model,&range,&vars,&momdiffCat,OutputDirectory);
     vector<binValsAndErrs> data = momdiffFitter.fit(&Hist_momdiff,"fitSplit_momdiff",false);
     plotMakerAndSaver(fout,massVmomdiff_th2,data,"momdiff","p^{+}-p^{-}","p^{+}-p^{-} [GeV]");
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
     RooDataHist Hist_momasym = splitDataHistFromTH2D("massVmomasymH2","massVmomasymH2",mass,momasymCat,massVmomasym_th2);
     
     // CREATE FITTER OBJECT; PERFORM FIT
     SplitFitter momasymFitter(&mass,model,&range,&vars,&momasymCat,OutputDirectory);
     vector<binValsAndErrs> data = momasymFitter.fit(&Hist_momasym,"fitSplit_momasym",false);
     plotMakerAndSaver(fout,massVmomasym_th2,data,"momasym","(p^{+}-p^{-})/(p^{+}+p^{-})","(p^{+}-p^{-})/(p^{+}+p^{-})");
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
    RooDataHist Hist_eta = splitDataHistFromTH2D("massVetaH2","massVetaH2",mass,etaCat,massVeta_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter etaFitter(&mass,model,&range,&vars,&etaCat,OutputDirectory);
    vector<binValsAndErrs> data = etaFitter.fit(&Hist_eta,"fitSplit_eta",false);
    plotMakerAndSaver(fout,massVeta_th2,data,"eta","#eta","#eta");
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
    RooDataHist Hist_tx = splitDataHistFromTH2D("massVtxH2","massVtxH2",mass,txCat,massVtx_th2);
    Hist_tx.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter txFitter(&mass,model,&range,&vars,&txCat,OutputDirectory);
    vector<binValsAndErrs> data = txFitter.fit(&Hist_tx,"fitSplit_tx",false);
    plotMakerAndSaver(fout,massVtx_th2,data,"tx","Tx","Tx");
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
    RooDataHist Hist_ty = splitDataHistFromTH2D("massVtyH2","massVtyH2",mass,tyCat,massVty_th2);
    Hist_ty.Print("v");
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter tyFitter(&mass,model,&range,&vars,&tyCat,OutputDirectory);
    vector<binValsAndErrs> data = tyFitter.fit(&Hist_ty,"fitSplit_ty",false);
    plotMakerAndSaver(fout,massVty_th2,data,"ty","Ty","Ty");
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
    RooDataHist Hist_phid = splitDataHistFromTH2D("massVphidH2","massVphidH2",mass,phidCat,massVphid_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(&mass,model,&range,&vars,&phidCat,OutputDirectory);
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_phid,"fitSplit_phid",false);
    plotMakerAndSaver(fout,massVphid_th2,data,"phid","#phi_{d}","#phi_{d}");
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
    RooDataHist Hist_phi = splitDataHistFromTH2D("massVphiH2","massVphiH2",mass,phiCat,massVphi_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(&mass,model,&range,&vars,&phiCat,OutputDirectory);
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_phi,"fitSplit_phi",false);
    plotMakerAndSaver(fout,massVphi_th2,data,"phi","#phi","#phi");
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
    RooDataHist Hist_theta = splitDataHistFromTH2D("massVthetaH2","massVthetaH2",mass,thetaCat,massVtheta_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(&mass,model,&range,&vars,&thetaCat,OutputDirectory);
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_theta,"fitSplit_theta",false);
    plotMakerAndSaver(fout,massVtheta_th2,data,"theta","#theta","#theta");
  }


  // ----------------------------------------------------------------------------------------------------
  // 2011 TIME SPLIT DATA
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_iov2011 = true;
  if (split_iov2011) {
    if (verbose) cout << "Fitting over each run bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massViov2011Str = rootDirectoryFile + "/massVersus2011iovH2";
    TH2D* massViov2011_th2 = (TH2D*)(inputFile.Get(massViov2011Str.c_str()));
    RooCategory iovCat ("iov2011Cat","iov2011Cat");
    RooDataHist Hist_iov2011 = splitDataHistFromTH2D("massViov2011H2","massViov2011H2",mass,iovCat,massViov2011_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter iov2011Fitter(&mass,model,&range,&vars,&iovCat,OutputDirectory);
    vector<binValsAndErrs> data = iov2011Fitter.fit(&Hist_iov2011,"fitSplit_iov2011",false);
    plotMakerAndSaver(fout,massViov2011_th2,data,"iov2011","iov","iov");
  }
  // ----------------------------------------------------------------------------------------------------
  // 2012 TIME SPLIT DATA
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_iov2012 = true;
  if (split_iov2012) {
    if (verbose) cout << "Fitting over each run bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massViov2012Str = rootDirectoryFile + "/massVersus2012iovH2";
    TH2D* massViov2012_th2 = (TH2D*)(inputFile.Get(massViov2012Str.c_str()));
    RooCategory iovCat ("iov2012Cat","iov2012Cat");
    RooDataHist Hist_iov2012 = splitDataHistFromTH2D("massViov2012H2","massViov2012H2",mass,iovCat,massViov2012_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter iov2012Fitter(&mass,model,&range,&vars,&iovCat,OutputDirectory);
    vector<binValsAndErrs> data = iov2012Fitter.fit(&Hist_iov2012,"fitSplit_iov2012",false);
    plotMakerAndSaver(fout,massViov2012_th2,data,"iov2012","iov","iov");
  }
  
  
  // CLOSE FILE
  fout.Close();
  
  // RETURN
  return 0;
}
