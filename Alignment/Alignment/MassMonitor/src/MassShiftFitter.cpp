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
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraphErrors.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>

// ROOFIT INCLUDES
#include <RooRealVar.h>
#include <RooConstVar.h>
#include <RooFormulaVar.h>
#include <RooDataHist.h>
#include <RooPlot.h>
#include <RooArgSet.h> 
#include <RooGaussian.h>
#include <RooCBShape.h>
#include <RooGenericPdf.h>
#include <RooFFTConvPdf.h>
using namespace RooFit;

namespace {

  // THIS BOOLEAN CONTROLS HOW MUCH INFO IS PRINTED
  const bool verbose = true;  

  // INPUT / OUTPUT
  const string RootExt = ".root";
  const string FitExt = ".fitvar";
  
  // RESONANCES TO SMEAR WITH
  const int N = 35;
  const int Nbins = 1000;

  // DECAY CONSTANTS

  const int NumRes = 5;
  int res = 0; // The particular decay for this execution; initialized at the command line
  
  const string ID[NumRes] =
    {
      "Jpsi"
      , "Psi2S"
      , "Ups1S"
      , "Z0"
      , "D0"
    }; // Short form decay string, e.g. Jpsi2MuMu  
  const string Particle[NumRes] =
    {
      "J/psi(1S)"
      , "psi(2S)"
      , "Upsilon(1S)"
      , "Z0"
      , "D0"
    }; // Long form LHCb particle name, e.g. J/psi(1S)
  
  // MC doesn't use *quite* these numbers, but should be close enough
  const double Mass[NumRes] = {
    3.096916
    , 3.686109
    , 9.4603025
    , 91.1876
    , 1.86484
  };

  const double MinMass[NumRes] = {
    2.9
    , 3.4
    , 8.9
    , 60
    , 1.5
  };
  
  const double MaxMass[NumRes] = {
    3.2
    , 3.77
    , 10.0
    , 120
    , 2.1
  };  

  const double smear_mult[NumRes] = {
    0.001
    , 0.001
    , 0.002
    , 0.2
    , 0.001
  };
}


// MAIN

int main( int argc, char** argv) {

  // ----------------------------------------------------------------------------------------------------
  // SETUP
  // ----------------------------------------------------------------------------------------------------

  /* This takes several arguments:
     1) Name of resonanace (Jpsi, Ups1S, etc)
     2) Name of ROOT file containing TTree
     3) Location of TTree inside the ROOT file
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

  // SETTING UP TREE
  TTree* tree;
  inputFile.GetObject(rootDirectoryFile.c_str(),tree);
  Double_t mm;
  string mmstr = ID[res] + "_MM";
  tree->SetBranchAddress(mmstr.c_str(),&mm);
  Long64_t nentries = tree->GetEntries();

  // ----------------------------------------------------------------------------------------------------
  // OUTPUT FILE
  // ----------------------------------------------------------------------------------------------------

  string graph_filename = OutputDirectory + "/" + ID[res] + "_smears" + RootExt;
  TFile fout(graph_filename.c_str(),"recreate");  

  
  // ----------------------------------------------------------------------------------------------------
  // FIT MODEL
  // ----------------------------------------------------------------------------------------------------

  RooRealVar mass ("mass","mass", MinMass[res], MaxMass[res]);
  RooConstVar truemean ("truemean","truemean",Mass[res]);
  RooRealVar deltam ("deltam","deltam", -5, 5);
  RooFormulaVar shmean ("shmean",
                        "truemean - deltam",
                        RooArgList(truemean, deltam));
  RooRealVar width ("width","width", 0.0);
  RooRealVar z ("z","z",2.0,1.0,3.0);
  RooConstVar n ("n","n",1.0);
  RooCBShape cbmodel ("model","model",mass,shmean,width,z,n);

  RooConstVar gamma("gamma","gamma",2.495);
  RooGenericPdf rbw("rbw",
                    "2 / 3.14159 * gamma^4 * shmean^2 / ((mass^2 - shmean^2)^2 + mass^4* gamma^2 / shmean^2)",
                    RooArgList(mass,shmean,gamma));
  RooConstVar bias ("bias","bias",0);
  RooConstVar zn ("zn","zn",1);
  RooConstVar zz ("zz","zz",2);
  //  RooCBShape sm ("sm","sm",mass,bias,width,zz,zn);
  RooGaussian sm ("sm","sm",mass,bias,width);
  RooFFTConvPdf rbwmodel ("rbwmodel","rbwmodel",mass,rbw,sm);
  rbwmodel.setInterpolationOrder(3);

  double smears[N];
  double smearerrs[N];
  double dvals[N];
  double derrs[N];
  double zvals[N];
  double zerrs[N];

  // LOOP
  for (int nstep = 1; nstep <= N; nstep++) {
    
    // RESOLUTION
    double smear = nstep*smear_mult[res];

    // SMEAR
    TH1D* hist = new TH1D("hist","hist",Nbins,MinMass[res],MaxMass[res]);
    TRandom3 R;
    TF1 cb("cb","exp(-0.5*x*x/([0]*[0])) * (x/[0] > -2) + 0.5*exp(-2) / (-1.5 - x/[0]) * (x/[0] <= -2)",-10*smear,10*smear);
    cb.SetParameter(0,smear);
    for (Long64_t i = 0; i < nentries; i++) {
      tree->GetEntry(i);
      Double_t mm_smeared = mm;
      //      if (ID[res] != "Z0")
      mm_smeared += R.Gaus(0,smear);
      //       else
      //         mm_smeared += cb.GetRandom();
      hist->Fill(mm_smeared);
    }

    RooAbsPdf* model = (ID[res] == "Z0") ? (RooAbsPdf*)&rbwmodel : (RooAbsPdf*)&cbmodel;

    // FIT
    RooDataHist rdhist("rdhist","rdhist",mass,hist);
    width.setVal(smear);
    width.setConstant(true);
    model->fitTo(rdhist,RooFit::Binned(true));

    // PLOT
    RooPlot *frame = mass.frame(mass.getBins());
    frame->SetTitle("MM (minus FSR) Fit");
    frame->SetXTitle("MM");
    rdhist.plotOn(frame,RooFit::Name("data"));
    model->plotOn(frame,RooFit::Name("fit"));

    // MAKE IT PRETTY
    RooPlot* pulls = mass.frame(mass.getBins());
    pulls->SetTitle("");
    pulls->SetXTitle("MM");
    pulls->SetYTitle("Pulls");
    RooHist* pullshist = frame->pullHist("data","fit");
    pulls->addObject((TObject*)pullshist,"XP");
    pulls->SetMinimum(-5);
    pulls->SetMaximum(5);
    
    char canstr[10]; sprintf(canstr,"canvas%d",nstep);
    TCanvas* canvas = new TCanvas(canstr,canstr,800,800);
    canvas->cd();
    char hpstr[15]; sprintf(hpstr,"histpad%d",nstep);
    TPad* histpad = new TPad(hpstr,hpstr,0.0,0.25,1.0,1.0);
    histpad->SetLogy();
    histpad->Draw();
    histpad->SetTopMargin(0.1);
    char ppstr[15]; sprintf(ppstr,"pullpad%d",nstep);
    TPad* pullpad = new TPad(ppstr,ppstr,0.0,0.0,1.0,0.25);
    pullpad->Draw();
    pullpad->SetTopMargin(0.1);
    histpad->cd();
    frame->SetTitleFont(63,"XY");
    frame->SetTitleSize(18,"XY");
    frame->SetTitleOffset(2,"XY");
    frame->SetLabelFont(63,"XY");
    frame->SetLabelSize(14,"XY");
    frame->Draw();
    histpad->Update();
    pullpad->cd();
    pulls->SetTitleFont(63,"XY");
    pulls->SetTitleSize(18,"XY");
    pulls->SetTitleOffset(2,"XY");
    pulls->SetLabelFont(63,"XY");
    pulls->SetLabelSize(14,"XY");
    pulls->Draw();
    pullpad->Update();
    canvas->Update();
    canvas->Write();
    
    // SAVE INFO
    smears[nstep-1] = smear;
    smearerrs[nstep-1] = smear_mult[res];
    if (ID[res] != "Z0") {
      zvals[nstep-1] = z.getValV();
      zerrs[nstep-1] = z.getError();
    }
    dvals[nstep-1] = deltam.getValV();
    derrs[nstep-1] = deltam.getError();

    // DELETE
    delete histpad;
    delete pullpad;
    delete canvas;    
  }

  TGraphErrors *dgraph = new TGraphErrors(N,smears,dvals,smearerrs,derrs);
  dgraph->SetName("dgraph");
  dgraph->SetTitle("dgraph");
  dgraph->Write();

  if (ID[res] != "Z0") {
    TGraphErrors *zgraph = new TGraphErrors(N,smears,zvals,smearerrs,zerrs);
    zgraph->SetName("zgraph");
    zgraph->SetTitle("zgraph");
    zgraph->Write();
  }
  
  // CLOSE FILE
  fout.Close();

  // RETURN
  return 0;
}
