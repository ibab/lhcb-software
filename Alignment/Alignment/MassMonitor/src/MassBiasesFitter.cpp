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
#include <RooTable.h>

namespace {

  // THIS BOOLEAN CONTROLS HOW MUCH INFO IS PRINTED
  bool verbose = true;  
  
  // INPUT / OUTPUT
  const string RootExt = ".root";
  const string FitExt = ".fitvar";
  
  // DECAY CONSTANTS

  const int NumRes = 4;
  int res = 0; // The particular decay for this execution; initialized at the command line
  
  const string ID[NumRes] =
    {
      "Jpsi2MuMu"
      , "Ups1S2MuMu"
      , "Z02MuMu"
      , "D02Kpi"
    }; // Short form decay string, e.g. Jpsi2MuMu  
  const string Head[NumRes] =
    {
      "Jpsi"
      , "Ups1S"
      , "Z0"
      , "D0"
    }; // Short form decay particle
  const string Decay[NumRes] =
    {
      "J/psi(1S) -> mu+ mu-"
      , "Upsilon(1S) -> mu+ mu-"
      , "Z0 -> mu+ mu-"
      , "[D*(2010)+ -> (D0 => K- pi+) pi+]cc"
    }; // Long form LHCb decay, e.g. J/psi(1S) -> mu+ mu-
  const string Particle[NumRes] =
    {
      "J/psi(1S)"
      , "Upsilon(1S)"
      , "Z0"
      , "D0"
    }; // Long form LHCb particle name, e.g. J/psi(1S)
  const double Mass[NumRes] = 
    {
      3.096916
      , 9.4603026
      , 91.1876
      , 1.86484
    }; // Resonance mass; starting value for fit
  const double Width[NumRes] =
    {
      0.08,
      0.05,
      10.00,
      0.08
    }; // Starting width for fit

  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------
  // MODEL PDFS
  // ----------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------

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

  // Cruijff (Symmetric, 1-sided)
  RooRealVar alpha("alpha","alpha",0.1,1E-10,1);
  const char* cr1_formula =
    "exp( - (mass-mean)*(mass-mean)"
    " / (2*width*width "
    "+ alpha*(mass-mean)*(mass-mean)*(mass<mean) ) )";
  RooGenericPdf signal_cr1("signal_cr1","signal (CR1)",cr1_formula,RooArgSet(mass,mean,width,alpha));

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
  RooAddPdf model_g_p0("model_g_p0","model_g_p0",RooArgList(signal_g,bkg_p0),RooArgList(frac)); // D0, KS, Lambda
  /* Create others as needed, such as 
     - phi : Voigtian / threshold
     - Others for testing purposes
  */

  RooAbsPdf* ResModel[NumRes] = {
    &model_cb_exp
    , &model_cb_exp
    , &model_cb_exp
    , &model_g_p0
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
    char meanLabelUnits[] = "#Deltam/m (#times 10^{3})";
    char widthLabel[] = "#sigma/m_{PDG}";
    char widthLabelUnits[] = "#sigma/m (#times 10^{3})";

    // CALCULATE VALUES
    int bins = hist->GetNbinsX();
    double* mean = new double[bins];
    double* meanErrs = new double[bins];
    double* width = new double[bins];
    double* widthErrs = new double[bins];
    double* mids = new double[bins];
    double* binwidths = new double[bins];
    for (int i = 0; i < bins; i++) {
      mean[i] = (data[0].vals[i]/(Mass[res]) - 1.0)*1000;
      meanErrs[i] = data[0].errs[i]/(Mass[res])*1000;
      width[i] = data[1].vals[i]/(Mass[res])*1000;
      widthErrs[i] = data[1].errs[i]/(Mass[res])*1000;
      mids[i] = hist->GetBinCenter(i+1);
      binwidths[i] = hist->GetBinWidth(i+1)/2.0;
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

    // SAVE TO FILE
    file.cd();
    means.Write();
    sigmas.Write();
  
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

  // OPENING ROOT FILE
  TFile inputFile(HistoFile.c_str(),"READ");
  if (inputFile.IsZombie()) {
    cout << "ERROR: input file not found" << endl;
    return 1;
  } else  {
    if (verbose) cout << "Input file opened successfully" << endl;
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
  RooArgList vars(mean,width);

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
    SplitFitter totFitter(&mass,model,&vars,OutputDirectory);
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
    SplitFitter momFitter(&mass,model,&vars,&momCat,OutputDirectory);
    momFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter ptFitter(&mass,model,&vars,&ptCat,OutputDirectory);
    ptFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
     SplitFitter momdiffFitter(&mass,model,&vars,&momdiffCat,OutputDirectory);
     momdiffFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
     string massVmomasymStr = rootDirectoryFile + "/massVersusMomAsymH2";
     TH2D* massVmomasym_th2 = (TH2D*)(inputFile.Get(massVmomasymStr.c_str()));
     RooCategory momasymCat ("momasymCat","momasymCat");
     RooDataHist Hist_momasym = splitDataHistFromTH2D("massVmomasymH2","massVmomasymH2",mass,momasymCat,massVmomasym_th2);
     
     // CREATE FITTER OBJECT; PERFORM FIT
     SplitFitter momasymFitter(&mass,model,&vars,&momasymCat,OutputDirectory);
     momasymFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter etaFitter(&mass,model,&vars,&etaCat,OutputDirectory);
    etaFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter txFitter(&mass,model,&vars,&txCat,OutputDirectory);
    txFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter tyFitter(&mass,model,&vars,&tyCat,OutputDirectory);
    tyFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter phiFitter(&mass,model,&vars,&phidCat,OutputDirectory);
    phiFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter phiFitter(&mass,model,&vars,&phiCat,OutputDirectory);
    phiFitter.fitpath.Add(new RooCmdArg(Range("all")));
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
    SplitFitter phiFitter(&mass,model,&vars,&thetaCat,OutputDirectory);
    phiFitter.fitpath.Add(new RooCmdArg(Range("all")));
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_theta,"fitSplit_theta",false);
    plotMakerAndSaver(fout,massVtheta_th2,data,"theta","#theta","#theta");
  }


  // ----------------------------------------------------------------------------------------------------
  // THETA-SPLIT SIMULTANEOUS FITS (opening angle)
  // ----------------------------------------------------------------------------------------------------
  
  
  bool split_time = true;
  if (split_time) {
    if (verbose) cout << "Fitting over each run bin separately:" << endl;
    
    // MAKE ROO DATA OBJECT
    string massVtimeStr = rootDirectoryFile + "/massVersusTimeH2";
    TH2D* massVtime_th2 = (TH2D*)(inputFile.Get(massVtimeStr.c_str()));
    RooCategory timeCat ("timeCat","timeCat");
    RooDataHist Hist_time = splitDataHistFromTH2D("massVtimeH2","massVtimeH2",mass,timeCat,massVtime_th2);
    
    // CREATE FITTER OBJECT; PERFORM FIT
    SplitFitter phiFitter(&mass,model,&vars,&timeCat,OutputDirectory);
    phiFitter.fitpath.Add(new RooCmdArg(Range("all")));
    vector<binValsAndErrs> data = phiFitter.fit(&Hist_time,"fitSplit_time",false);
    plotMakerAndSaver(fout,massVtime_th2,data,"run","run","run");
  }
  
  
  // CLOSE FILE
  fout.Close();
  
  // RETURN
  return 0;
}
