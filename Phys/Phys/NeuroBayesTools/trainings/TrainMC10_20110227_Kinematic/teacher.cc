// access to data in nutple
#include "readNtuple_UnbiasedLoose.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>

// ROOT 
#include <TVector3.h>
#include <TH1F.h>
#include <TFile.h>

// NeuroBayes
#include "NeuroBayesTeacher.hh"
#include "NeuroBayesExpert.hh"

// declare functions
void teacher();

// ---------------------------------------------------------------
void teacher() 
{
  std::cout << "Start NeuroBayes Setup" << std::endl
            << "======================" << std::endl
            << std::endl;
 
  double nSig = 0;
  double nBg  = 0;
  double WT   = 1.0;  // weight

  double nTrueBs = 0;
  double nTrueBd = 0;

  double nCut = 0;


  double nSigTrue = 0; // true Signal
  double nSkipped = 0;

  double sideband_low_min  = 5.0 ;
  double sideband_low_max  = 5.30;
  
  double signal_min        = 5.30;
  double signal_max        = 5.44; 
  
  double sideband_high_min = 5.44;
  double sideband_high_max = 5.6;
  
  //
  // plot input variables 
  //
  TFile output("inputvars.root","recreate");
  TH1F* hKPlusPt     = new TH1F("hKPlusPt"     , "K+ pt"                   , 120,    0.0,  30.0);
  TH1F* hKPlusMinPt  = new TH1F("hKPlusMinPt"  , "K+ pt"                   , 120,    0.0,  30.0);
  TH1F* hKPlusMaxPt  = new TH1F("hKPlusMaxPt"  , "K+ pt"                   , 120,    0.0,  30.0);
  TH1F* hKPlusP      = new TH1F("hKPlusP"      , "K+ p"                    , 120,    0.0, 300.0);
  TH1F* hKPlusD0     = new TH1F("hKPlusD0"     , "K+ impact parameter"     , 120,   -8.0,  10.0);
  TH1F* hKPlusD0SV   = new TH1F("hKPlusD0SV"   , "K+ impact parameter (SV)", 120,   -0.5,   8.0);
  TH1F* hKPlusDllK   = new TH1F("hKPlusDllK"   , "K+ DllK"                 , 5000, -1000, 200.0);
  TH1F* hKPlusDllE   = new TH1F("hKPusDllE"    , "K+ DllE"                 , 5000, -1000,  25.0);
  TH1F* hKPlusDllP   = new TH1F("hKPlusDllP"   , "K+ DllP"                 , 5000, -1000, 120.0);
  TH1F* hKPlusDllMu  = new TH1F("hKPlusDllMu"  , "K+ DllMu"                , 5000, -1000,  25.0);
  TH1F* hKPlusE      = new TH1F("hKPlusE"      , "K+ calo E"               , 5000, -1000.0,8000.0);
  TH1F* hBsChi2      = new TH1F("hBsChi2"      , "Bs chi2"                 , 120,    0.0,  25.0);
  TH1F* hBsP         = new TH1F("hBsP "        , "Bs P "                   , 120,    0.0, 300.0);
  TH1F* hBsMassErr   = new TH1F("hBsMassErr"   , "Bs massErr"              , 120,    0.0,   0.1);
  TH1F* hBsCTauErr   = new TH1F("hBsCTauErr"   , "Bs cTauErr"              , 120,    0.0,   0.1);
  TH1F* hThetaStar   = new TH1F("hThetaStar"   , "|cos(theta*)|"           , 120,    0.0,   1.0);
  TH1F* hNCand       = new TH1F("hNCand"       , "n BsCand"                ,  40,    0.0,  40.0);

  TH1F* hWT          = new TH1F("hWT"          , "event weight"            , 120,    0.0, 10.0); 
  //
  // setup NeuroBayes
  //
	const int nvar = 12;                  // number of input variables 
	//create NeuroBayes instance
  NeuroBayesTeacher* teacher = NeuroBayesTeacher::Instance();

  teacher->NB_DEF_NODE1(nvar+1);        // nodes in input layer
  teacher->NB_DEF_NODE2(nvar);          	// nodes in hidden layer
  teacher->NB_DEF_NODE3(1);          	  // nodes in output layer
  

  teacher->NB_DEF_TASK("CLA");          // binominal classification
  teacher->NB_DEF_SHAPE("DIAG");        // force to diagonal  
  teacher->NB_DEF_REG("ALL");           // 'OFF','REG' (def) ,'ARD','ASR','ALL'
  teacher->NB_DEF_LOSS("ENTROPY");      // 'ENTROPY'(def),'QUADRATIC'

 	teacher->NB_DEF_ITER(100);            // number of training iteration
  teacher->NB_DEF_METHOD("BFGS");	     // will stop automatically if there's nothing more to learn
  teacher->NB_DEF_LEARNDIAG( 1 );	     // BFGS
  teacher->NB_DEF_PRE(612);		         // flatten and de-correlate input variables, keep only variables with >3 sigma significance
  
  //teacher->NB_DEF_SPEED(0.1);
  //teacher->NB_DEF_MAXLEARN(0.1);
  //teacher->NB_DEF_MOM(0.9);

  //int i= 49801;
  //int j= 21; 
  //teacher->NB_RANVIN(i,j,2);      //different random seed
 
	char ExpertiseFile[256];
	sprintf(ExpertiseFile,"bshhet.nb");

  std::cout << "Will put the Expertise in " << ExpertiseFile << std::endl;
  teacher->SetOutputFile(ExpertiseFile);  // expert file

  //
  // individual prepro-flags
  //
  // 14: flatten input variable, regularised spline fit to mean of target, no delta function
  // 15: same as 14 but monotonous fit
  // 18: flatten input variable,no delta func, unordered class 
  // 19: same, but ordered class
  //
  teacher->SetIndividualPreproFlag( 0,34); 
  teacher->SetIndividualPreproFlag( 1,34); 
  teacher->SetIndividualPreproFlag( 2,34);
  teacher->SetIndividualPreproFlag( 3,34);
  teacher->SetIndividualPreproFlag( 4,34); 
  teacher->SetIndividualPreproFlag( 5,34);
  teacher->SetIndividualPreproFlag( 6,34);
  teacher->SetIndividualPreproFlag( 7,34);
  teacher->SetIndividualPreproFlag( 8,34);
  teacher->SetIndividualPreproFlag( 9,34);
  teacher->SetIndividualPreproFlag(10,34);
  teacher->SetIndividualPreproFlag(11,34);
  //teacher->SetIndividualPreproFlag(12,34);
  //teacher->SetIndividualPreproFlag(13,34);
  

  //
  // acceess to training sample
  //
  Long64_t nbytes = 0, nb = 0;
  readNtuple_UnbiasedLoose ntuple;
  Long64_t nentries = ntuple.fChain->GetEntries();
  std::cout << "read #entries " << nentries << std::endl;

  float* InputArray = new float[nvar]; // array to hold input data

  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if (jentry % (int)((double)nentries/10.0) == 0)
      std::cout << "now read entry " << jentry << " of " << nentries <<  std::endl;
    Long64_t ientry = ntuple.LoadTree(jentry);
    if (ientry < 0) break;
    nb = ntuple.fChain->GetEntry(jentry);
    nbytes += nb;
 
    //
    // soft precuts to remove extreme outliers
    //
    double hPlusPt         = ntuple.Kplus_PT/1000;
    double hMinusPt        = ntuple.Kminus_PT/1000;  
    double hPlusP          = ntuple.Kplus_P/1000;
    double hMinusP         = ntuple.Kminus_P/1000;  
    double hPlusMinIP      = ntuple.Kplus_MINIP;
    double hMinusMinIP     = ntuple.Kminus_MINIP;
    double hPlusCaloE      = ntuple.Kplus_CaloEcalE;
    double hMinusCaloE     = ntuple.Kminus_CaloEcalE;
    double hPlusDllK       = ntuple.Kplus_PIDK;
    double hMinusDllK      = ntuple.Kminus_PIDK;
    double hPlusDllE       = ntuple.Kplus_PIDe;
    double hMinusDllE      = ntuple.Kminus_PIDe;
    double hPlusDllP       = ntuple.Kplus_PIDp;
    double hMinusDllP      = ntuple.Kminus_PIDp;
    double hPlusDllMu      = ntuple.Kplus_PIDmu;
    double hMinusDllMu     = ntuple.Kminus_PIDmu;
    double hPlusTrackChi2  = ntuple.Kplus_TRACK_CHI2NDOF;
    double hMinusTrackChi2 = ntuple.Kminus_TRACK_CHI2NDOF;
    double hPlusGhost      = ntuple.Kplus_TRACK_GhostProb;
    double hMinusGhost     = ntuple.Kminus_TRACK_GhostProb; 
    double hPlusNNPIDK     = ntuple.Kplus_NNPIDk;
    double hMinusNNPIDK    = ntuple.Kminus_NNPIDk;
    double hPlusNNPIDpi    = ntuple.Kplus_NNPIDpi;
    double hMinusNNPIDpi   = ntuple.Kminus_NNPIDpi;
    double hPlusNNPIDe     = ntuple.Kplus_NNPIDe;
    double hMinusNNPIDe    = ntuple.Kminus_NNPIDe;
    double hPlusNNPIDmu    = ntuple.Kplus_NNPIDmu;
    double hMinusNNPIDmu   = ntuple.Kminus_NNPIDmu;
    double hPlusNNPIDp     = ntuple.Kplus_NNPIDp;
    double hMinusNNPIDp    = ntuple.Kminus_NNPIDp;


    double mB              = ntuple.B_s0_MM/1000;
    double mErrB           = ntuple.B_s0_MMERR/1000;
    double ptB             = ntuple.B_s0_PT/1000;
    double pB              = ntuple.B_s0_P/1000;
    double tauB            = ntuple.B_s0_TAU*1000;         //TAU is in ns, convert to ps
    double tauErrB         = ntuple.B_s0_TAUERR*1000;
    double cTauB           = ntuple.B_s0_TAU*299.792458;   //in mm: units by Gaudi
    double chi2B           = ntuple.B_s0_ENDVERTEX_CHI2;
    double docaB           = ntuple.B_s0_DOCAMAX;


    double minPt           = TMath::Min(hPlusPt    , hMinusPt);
    double maxPt           = TMath::Max(hPlusPt    , hMinusPt);

    double minP            = TMath::Min(hPlusP     , hMinusP);
    double maxP            = TMath::Max(hPlusP     , hMinusP);
                          
    double minMinIP        = TMath::Min(hPlusMinIP , hMinusMinIP);
    double maxMinIP        = TMath::Max(hPlusMinIP , hMinusMinIP);

    double minDllK         = TMath::Min(hPlusDllK  , hMinusDllK);
    double maxDllK         = TMath::Max(hPlusDllK  , hMinusDllK);
    
    double minNNPIDK       = TMath::Min(hPlusNNPIDK    , hMinusNNPIDK);
    double maxNNPIDK       = TMath::Max(hPlusNNPIDK    , hMinusNNPIDK);

    double minNNPIDpi      = TMath::Min(hPlusNNPIDpi    , hMinusNNPIDpi);
    double maxNNPIDpi      = TMath::Max(hPlusNNPIDpi    , hMinusNNPIDpi);

    double mMin            = sideband_low_min;
    double mMax            = sideband_high_max;
    //
    // soft precuts
    // (reject obvious background without loosing much signal)

    //
    // train on MC: BdPiK, BsKK signal, incl B Background
    //

    // softer precuts
    if (mB               < mMin     ||
        mB               > mMax     ||
        hPlusTrackChi2   >  5       ||
        hMinusTrackChi2  >  5       ||
        minP             < 10.0     ||
        minPt            <  1.0     ||
        docaB            > 0.10     ||
        chi2B            > 25.0     ||
        maxPt            <  1.5     )
      continue;


    TString fileName =  ntuple.fChain->GetCurrentFile()->GetName();

    bool isSignal = false;
    if  ( (fileName.Contains("BsKK") && ntuple.B_s0_BKGCAT == 0) || (fileName.Contains("BdPiK") && ntuple.B_s0_BKGCAT == 30) ) {
      isSignal = true;
      nSigTrue += 1;
      if (fileName.Contains("BsKK"))
        nTrueBs += 1;
      if (fileName.Contains("BdPiK"))
        nTrueBd += 1;
      
    }else {
      if (fileName.Contains("BsKK") || (fileName.Contains("BdPiK")))
        continue;
    }

//    // upper sideband only
//    if (fileName.Contains("Reco8Strip12") && mB < signal_max)
//      continue;


    //with pre-cuts -- MC10
//    //const double nBGCut =  15720.0;   // MC10: BG incl B
//    //const double nBGCut = 18640.0; // MC10: BG inclB + signal tapes
//    //const double nBGCut = 904770.0; // MC10: BG inclB + inclJPsi + signal tapes
//    //const double nBGCut = 570252.0; // data - loose - full mass spectrum 
//    //const double nBGCut = 137138; // data - loose - uppper sideband
//    if (fileName.Contains("BsKK")  && nTrueBs > nBGCut/2.0)
//      continue;
//    if (fileName.Contains("BdPiK") && nTrueBd > nBGCut/2.0)
//      continue;

    // #remaining Bd, Bs signal events after pre-cuts
    //const double nSigCut = 411089;
    //if (!isSignal && nBg > nSigCut)
    //  continue;

    nCut += 1;

    //
    // move delta functions to -999
    //
    
    if (hPlusCaloE > -0.01 && hPlusCaloE < 0.01)
      hPlusCaloE = -999;
    if (hMinusCaloE > -0.01 && hMinusCaloE < 0.01)
      hMinusCaloE = -999;
    if (hPlusCaloE < -999)
      hPlusCaloE = -999;
    if (hMinusCaloE < -999)
      hMinusCaloE = -999;


    
    if (hPlusDllK > -0.01 && hPlusDllK < 0.01)
      hPlusDllK = -999;
    if (hMinusDllK > -0.01 && hMinusDllK < 0.01)
      hMinusDllK = -999;

    
    if (hPlusDllE > -0.1 && hPlusDllE < 0.01)
      hPlusDllE = -999;
    if (hMinusDllE > -0.1 && hMinusDllE < 0.01)
      hMinusDllE = -999;

    
    if (hPlusDllP > -0.01 && hPlusDllP < 0.01)
      hPlusDllP = -999;
    if (hMinusDllP > -0.01 && hMinusDllP < 0.01)
      hMinusDllP = -999;

    
    if (hPlusDllMu > -0.1 && hPlusDllMu < 0.01)
      hPlusDllK = -999;
    if (hMinusDllMu > -0.1 && hMinusDllMu < 0.01)
      hMinusDllMu = -999;

    if (tauErrB<-999)
      tauErrB =-999;
    
    if (hPlusGhost < 0)
      hPlusGhost = -999;

    if (hMinusGhost < 0)
      hMinusGhost = -999;

    // fill input variables
    //InputArray[  0 ] =  ntuple.Kplus_isMuon;                     
    //InputArray[  1 ] =  ntuple.Kminus_isMuon;                    
    //InputArray[  2 ] =  hPlusCaloE;
    //InputArray[  3 ] =  hMinusCaloE;
    InputArray[  0 ] =  minPt;     
    InputArray[  1 ] =  maxPt;     
    InputArray[  2 ] =  hPlusP;    
    InputArray[  3 ] =  hMinusP;                         
    InputArray[  4 ] =  chi2B;     
    InputArray[  5 ] =  mErrB;     
    InputArray[  6 ] =  pB;        
    InputArray[  7 ] =  docaB;
    InputArray[  8 ] =  hPlusTrackChi2;
    InputArray[  9 ] =  hMinusTrackChi2;
    InputArray[ 10 ] =  TMath::Abs(ntuple.B_s0_CosTheta); 
    InputArray[ 11 ] =  ptB; //doesn't help

    if ( (nSig+nBg) < 1000000) {
      if (isSignal) {
        nSig++;
        teacher->SetTarget(1);
        teacher->SetNextInput(nvar,InputArray);
      } else {
        nBg++;
        teacher->SetTarget(-1);
        teacher->SetNextInput(nvar,InputArray);
      }
    }//if nSig

    //hWT ->Fill(WT);
    //teacher->SetWeight(WT);
  } // for

  output.Write();
  output.Close();
  std::cout << "past precut " << nCut << " #Sig " << nSig << "(true " << nSigTrue << ", skipped " << nSkipped << ") #nBg " << nBg << std::endl;

  //
	// perform training
  //
  
  std::cout << "To see NeuroBayes output have a look at \"nb_teacher.log\"" << std::endl;
	//stdout is redirected to the file "nb_teacher.log"
	int original = dup(fileno(stdout));
  fflush(stdout);
  freopen("nb_teacher.log", "w", stdout);
  
  teacher->TrainNet();
  
  fflush(stdout);
  dup2(original, fileno(stdout));
  close(original);

} // void teacher


//----------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
	teacher();
}
