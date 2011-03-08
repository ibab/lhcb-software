// access to data in nutple - same as sWeights.cc
#include "readNtuple_MuMuHigh.h"

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


  int nSigTrue = 0; // true Signal
  int nSkipped = 0;

  //
  // plot input variables 
  //
  TFile output("inputvars.root","recreate");
  TH1F* hmuplusPt     = new TH1F("hmuplusPt"     , "mu+ pt"              , 120,    0.0,  30.0);
  TH1F* hmuminusPt    = new TH1F("hmuminusPt"    , "mu- pt"              , 120,    0.0,  30.0);
  TH1F* hAlpha        = new TH1F("hAlpha"        , "Alpha"               , 120,   -1.0,   1.0);
  TH1F* hAngle        = new TH1F("hAngle"        , "Angle"               , 120,    0.0,   1.0);
  TH1F* hmuplusD0     = new TH1F("hmuplusD0"     , "Mu+ impact parameter", 120,    0.0,   4.0);
  TH1F* hmuminusD0    = new TH1F("hmuminusD0"    , "Mu- impact parameter", 120,    0.0,   4.0);
  TH1F* hmuplusChi2   = new TH1F("hmuplusChi2"   , "Mu+ Chi2"            , 120,    0.0, 500.0);
  TH1F* hmuminusChi2  = new TH1F("hmuminusChi2"  , "Mu- Chi2"            , 120,    0.0, 500.0);
  TH1F* hmuplusPIDK   = new TH1F("hmuplusPIDK"   , "Mu+ PIDK"            , 120, -200  , 100.0);
  TH1F* hmuminusPIDK  = new TH1F("hmuminusPIDK"  , "Mu- PIDK"            , 120, -200  , 100.0);
  TH1F* hmuplusPIDE   = new TH1F("hMuPusPIDE"    , "Mu+ PIDE"            , 120,  -25  ,  25.0);
  TH1F* hmuminusPIDE  = new TH1F("hmuminusPIDE"  , "Mu- PIDE"            , 120,  -25  ,  25.0);
  TH1F* hmuplusPIDP   = new TH1F("hmuplusPIDP"   , "Mu+ PIDP"            , 120, -200  , 200.0);
  TH1F* hmuminusPIDP  = new TH1F("hmuminusPIDP"  , "Mu- PIDP"            , 120, -200  , 200.0);
  TH1F* hmuplusPIDMu  = new TH1F("hmuplusPIDMu"  , "Mu+ PIDMu"           , 120,  -25  ,  25.0);
  TH1F* hmuminusPIDMu = new TH1F("hmuminusPIDMu" , "Mu- PIDMu"           , 120,  -25  ,  25.0);
  TH1F* hJPsiChi2     = new TH1F("hJPsiChi2"     , "J/psi chi2"          , 120,    0.0,  25.0);
  TH1F* hJPsiPt       = new TH1F("hJPsiPt"       , "J/psi Pt"            , 120,    0.0,  50.0);
  TH1F* hJPsiMassErr  = new TH1F("hJPsiMassErr"  , "J/psi massErr"       , 120,    0.0,   0.1);
  TH1F* hmuplusE      = new TH1F("hmuplusE"      , "Mu+ calo E"          , 120,    0.0,5000.0);
  TH1F* hmuminusE     = new TH1F("hmuminusE"     , "Mu- calo E"          , 120,    0.0,5000.0);

  //
  // setup NeuroBayes
  //
  // full
  //const int nvar = 24;                  // number of input variables 

  // no IP chi2
  const int nvar = 19;                  // number of input variables 

  //kinematics
  //const int nvar = 9;                  // number of input variables 


	//create NeuroBayes instance
  NeuroBayesTeacher* teacher = NeuroBayesTeacher::Instance();

  teacher->NB_DEF_NODE1(nvar+1);        // nodes in input layer
  teacher->NB_DEF_NODE2(nvar);        	// nodes in hidden layer
  teacher->NB_DEF_NODE3(1);          	  // nodes in output layer
  

  teacher->NB_DEF_TASK("CLA");          // binominal classification
  //teacher->NB_DEF_SHAPE("DIAG");        // force to diagonal  
  teacher->NB_DEF_REG("REG");           // 'OFF','REG' (def) ,'ARD','ASR','ALL'
  teacher->NB_DEF_LOSS("QUADRATIC");      // 'ENTROPY'(def),'QUADRATIC'

 	//teacher->NB_DEF_ITER(0);            // number of training iteration
  teacher->NB_DEF_METHOD("BFGS");	     // will stop automatically if there's nothing more to learn
  teacher->NB_DEF_LEARNDIAG( 1 );	     // BFGS
  teacher->NB_DEF_PRE(612);		         // flatten and de-correlate input variables, keep only variables with >3 sigma significance
  
  //teacher->NB_DEF_SPEED(0.1);
  //teacher->NB_DEF_MAXLEARN(0.1);
  //teacher->NB_DEF_MOM(0.9);

  //int i= 4301;
  //int j= 21; 
  //teacher->NB_RANVIN(i,j,2);      //different random seed
 
	char ExpertiseFile[256];
	sprintf(ExpertiseFile,"mumuhighmass_noip.nb");

  std::cout << "Will put the Expertise in " << ExpertiseFile << std::endl;
  teacher->SetOutputFile(ExpertiseFile);  // expert file

  //
  // individual prepro-flags
  //
  // 14: flatten input variable, regularised spline fit to mean of target, no delta function
  // 15: same as 14 but monotonous fit
  //

  // full
  //teacher->SetIndividualPreproFlag( 0,14); 
  //teacher->SetIndividualPreproFlag( 1,14); 
  //teacher->SetIndividualPreproFlag( 2,14); 
  //teacher->SetIndividualPreproFlag( 3,14); 
  //teacher->SetIndividualPreproFlag( 4,14); 
  //teacher->SetIndividualPreproFlag( 5,14); 
  //teacher->SetIndividualPreproFlag( 6,14);
  //teacher->SetIndividualPreproFlag( 7,14);
  //teacher->SetIndividualPreproFlag( 8,15);
  //teacher->SetIndividualPreproFlag( 9,14);
  //teacher->SetIndividualPreproFlag(10,14);
  //teacher->SetIndividualPreproFlag(11,14);
  //teacher->SetIndividualPreproFlag(12,14);
  //teacher->SetIndividualPreproFlag(13,14);
  //teacher->SetIndividualPreproFlag(14,34);
  //teacher->SetIndividualPreproFlag(15,34);
  //teacher->SetIndividualPreproFlag(16,34); 
  //teacher->SetIndividualPreproFlag(17,34);
  //teacher->SetIndividualPreproFlag(18,34);
  //teacher->SetIndividualPreproFlag(19,34);
  //teacher->SetIndividualPreproFlag(20,34);
  //teacher->SetIndividualPreproFlag(21,34);
  //teacher->SetIndividualPreproFlag(22,34);
  //teacher->SetIndividualPreproFlag(23,34);


  // no IP chi2
  teacher->SetIndividualPreproFlag( 0,14); 
  teacher->SetIndividualPreproFlag( 1,14); 
  teacher->SetIndividualPreproFlag( 2,14); 
  teacher->SetIndividualPreproFlag( 3,14); 
  teacher->SetIndividualPreproFlag( 4,14);
  teacher->SetIndividualPreproFlag( 5,14);
  teacher->SetIndividualPreproFlag( 6,15);
  teacher->SetIndividualPreproFlag( 7,14);
  teacher->SetIndividualPreproFlag( 8,14);
  teacher->SetIndividualPreproFlag( 9,34);
  teacher->SetIndividualPreproFlag(10,34);
  teacher->SetIndividualPreproFlag(11,34); 
  teacher->SetIndividualPreproFlag(12,34);
  teacher->SetIndividualPreproFlag(13,34);
  teacher->SetIndividualPreproFlag(14,34);
  teacher->SetIndividualPreproFlag(15,34);
  teacher->SetIndividualPreproFlag(16,34);
  teacher->SetIndividualPreproFlag(17,34);
  teacher->SetIndividualPreproFlag(18,34);

  // kinematics
  //teacher->SetIndividualPreproFlag( 0,14); 
  //teacher->SetIndividualPreproFlag( 1,14); 
  //teacher->SetIndividualPreproFlag( 2,14); 
  //teacher->SetIndividualPreproFlag( 3,14); 
  //teacher->SetIndividualPreproFlag( 4,15);
  //teacher->SetIndividualPreproFlag( 5,14);
  //teacher->SetIndividualPreproFlag( 6,14);
  //teacher->SetIndividualPreproFlag( 7,34);
  //teacher->SetIndividualPreproFlag( 8,34);


  //
  // acceess to training sample
  //
  Long64_t nbytes = 0, nb = 0;
  readNtuple_MuMuHigh data;
  Long64_t nentries = data.fChain->GetEntries();
  std::cout << "read #entries " << nentries << std::endl;

  float* InputArray = new float[nvar]; // array to hold input data

  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if (jentry % (int)((double)nentries/10.0) == 0)
      std::cout << "now read entry " << jentry << " of " << nentries <<  std::endl;
    Long64_t ientry = data.LoadTree(jentry);
    if (ientry < 0) break;
    nb = data.fChain->GetEntry(jentry);
    nbytes += nb;

//  if (fabs(data.muplusTruth_PID)  ==  13 && fabs(data.muminusTruth_PID) ==  13 &&
//      data.muplusTruth_MotherPID  == 443 && data.muminusTruth_MotherPID == 443)
//    nSigTrue++;

    float alpha = 0;
    if (data.muplus_P < data.muminus_P)
      alpha = 1 - data.muplus_P/data.muminus_P;
    else
      alpha = -(1 - data.muminus_P/data.muplus_P);

    TVector3 v1(data.muplus_PX,
                data.muplus_PY,
                data.muplus_PZ);
    TVector3 v2(data.muminus_PX,
                data.muminus_PY,
                data.muminus_PZ);

    //
    // apply the same soft pre-cuts as
    // during the evaluation of the sWeights
    //

    //
    // move delta functions to -999
    //
    double muplusE = data.muplus_CaloEcalE;
    if ( (muplusE>-0.1 && muplusE<0.1) ||
         muplusE < 0.0)
      muplusE = -999;

    double muminusE = data.muminus_CaloEcalE;
    if ( (muminusE>-0.1 && muminusE<0.1) ||
         muminusE < 0.0)
      muminusE = -999;

    double muplusPIDK = data.muplus_PIDK;
    if (muplusPIDK > -0.01 && muplusPIDK < 0.01)
      muplusPIDK = -999;
    double muminusPIDK = data.muminus_PIDK;
    if (muminusPIDK > -0.01 && muminusPIDK < 0.01)
      muminusPIDK = -999;

    double muplusPIDe = data.muplus_PIDe;
    if (muplusPIDe > -0.5 && muplusPIDe < 0.5)
      muplusPIDe = -999;
    double muminusPIDe = data.muminus_PIDe;
    if (muminusPIDe > -0.5 && muminusPIDe < 0.5)
      muminusPIDe = -999;

    double muplusPIDp = data.muplus_PIDp;
    if (muplusPIDp > -0.01 && muplusPIDp < 0.01)
      muplusPIDp = -999;
    double muminusPIDp = data.muminus_PIDp;
    if (muminusPIDp > -0.01 && muminusPIDp < 0.01)
      muminusPIDp = -999;

    double muplusChi2 = data.muplus_TRACK_PCHI2;
    if (muplusChi2 < 0.001) {
      muplusChi2 = -999;
    }

    double muminusChi2 = data.muminus_TRACK_PCHI2;
    if (muminusChi2 < 0.001) {
      muminusChi2 = -999;
    }

    // fill input variables
    // full
    //InputArray[  0 ] =  data.muplus_PT/1000;           hmuplusPt    ->Fill(InputArray[  0 ]);
    //InputArray[  1 ] =  data.muminus_PT/1000;          hmuminusPt   ->Fill(InputArray[  1 ]);
    //InputArray[  2 ] =  alpha;                    hAlpha       ->Fill(InputArray[  2 ]);
    //InputArray[  3 ] =  v1.Angle(v2);             hAngle       ->Fill(InputArray[  3 ]);
    //InputArray[  4 ] =  data.muplus_MINIPCHI2;        hmuplusD0    ->Fill(InputArray[  4 ]);
    //InputArray[  5 ] =  data.muminus_MINIPCHI2;       hmuminusD0   ->Fill(InputArray[  5 ]);
    //InputArray[  6 ] =  data.muplus_PIDmu;        hmuplusPIDMu ->Fill(InputArray[ 14 ]); 
    //InputArray[  7 ] =  data.muminus_PIDmu;       hmuminusPIDMu->Fill(InputArray[ 15 ]);
    //InputArray[  8 ] =  data.J_psi_1S_ENDVERTEX_CHI2;       hJPsiChi2    ->Fill(InputArray[ 16 ]);
    //InputArray[  9 ] =  data.J_psi_1S_PT/1000;         hJPsiPt      ->Fill(InputArray[ 17 ]);
    //InputArray[ 10 ] =  data.J_psi_1S_MMERR/1000;      hJPsiMassErr ->Fill(InputArray[ 18 ]);
    //InputArray[ 11 ] =  data.J_psi_1S_MINIPCHI2;
    //InputArray[ 12 ] =  data.J_psi_1S_DIRA_OWNPV;
    //InputArray[ 13 ] =  data.J_psi_1S_P/1000;
    //InputArray[ 14 ] =  muplusChi2;               hmuplusChi2  ->Fill(InputArray[  6 ]);
    //InputArray[ 15 ] =  muminusChi2;              hmuminusChi2 ->Fill(InputArray[  7 ]);
    //InputArray[ 16 ] =  muplusPIDK;               hmuplusPIDK  ->Fill(InputArray[  8 ]);
    //InputArray[ 17 ] =  muminusPIDK;              hmuminusPIDK ->Fill(InputArray[  9 ]);
    //InputArray[ 18 ] =  muplusPIDe;               hmuplusPIDE  ->Fill(InputArray[ 10 ]); 
    //InputArray[ 19 ] =  muminusPIDe;              hmuminusPIDE ->Fill(InputArray[ 11 ]);
    //InputArray[ 20 ] =  muplusPIDp;               hmuplusPIDP  ->Fill(InputArray[ 12 ]);
    //InputArray[ 21 ] =  muminusPIDp;              hmuminusPIDP ->Fill(InputArray[ 13 ]);
    //InputArray[ 22 ] =  muplusE;                  hmuplusE     ->Fill(InputArray[ 19 ]);
    //InputArray[ 23 ] =  muminusE;                 hmuminusE    ->Fill(InputArray[ 20 ]); 

    // no IP chi2
    InputArray[  0 ] =  data.muplus_PT/1000;   
    InputArray[  1 ] =  data.muminus_PT/1000;  
    InputArray[  2 ] =  alpha;                 
    InputArray[  3 ] =  v1.Angle(v2);          
    InputArray[  4 ] =  data.muplus_PIDmu;      
    InputArray[  5 ] =  data.muminus_PIDmu;      
    InputArray[  6 ] =  data.J_psi_1S_ENDVERTEX_CHI2; 
    InputArray[  7 ] =  data.J_psi_1S_MMERR/1000;     
    InputArray[  8 ] =  data.J_psi_1S_P/1000;
    InputArray[  9 ] =  muplusChi2;               
    InputArray[ 10 ] =  muminusChi2;              
    InputArray[ 11 ] =  muplusPIDK;               
    InputArray[ 12 ] =  muminusPIDK;              
    InputArray[ 13 ] =  muplusPIDe;               
    InputArray[ 14 ] =  muminusPIDe;              
    InputArray[ 15 ] =  muplusPIDp;               
    InputArray[ 16 ] =  muminusPIDp;              
    InputArray[ 17 ] =  muplusE;                  
    InputArray[ 18 ] =  muminusE;                 


    //kinematics
    //InputArray[  0 ] =  data.muplus_PT/1000;   
    //InputArray[  1 ] =  data.muminus_PT/1000;  
    //InputArray[  2 ] =  alpha;                 
    //InputArray[  3 ] =  v1.Angle(v2);          
    //InputArray[  4 ] =  data.J_psi_1S_ENDVERTEX_CHI2; 
    //InputArray[  5 ] =  data.J_psi_1S_PT/1000;        
    //InputArray[  6 ] =  data.J_psi_1S_P/1000;
    //InputArray[  7 ] =  muplusChi2;               
    //InputArray[  8 ] =  muminusChi2;              



    //
    // set training target
    //
    double mass = data.J_psi_1S_MM/1000;



    //
    // define signal and background 
    // using the same mass window as in the calculation of the sWeights
    // and use the sWeights derived from the fit
    //

    // data 2010
    //
    //
    //sWeights sP_sig(bin 1) = -0.220434 sP_sig(bin 2) = 1.00746
    //  sP_bg (bin 1) = 1.22043 sP_bg (bin 2) = -0.0074646
    //
    // all data
    //sWeights sP_sig(bin 1) = -0.219721 sP_sig(bin 2) = 1.00476
    //  sP_bg (bin 1) = 1.21972 sP_bg (bin 2) = -0.00475792


    const double sP_sig_1 = -0.219721;
    const double sP_sig_2 =  1.00476;

    if ( (mass > 8.5  && mass < 9.3) ||  
         (mass > 9.6  && mass < 9.9) ){
      nBg += 1.0;
      teacher->SetTarget(sP_sig_1) ; // event is a BACKGROUND event, sWeight calculated separately
      teacher->SetNextInput(nvar,InputArray);     
    } else if (mass > 9.3 && mass < 9.6) {
      nSig += 1.0;
      teacher->SetTarget(sP_sig_2) ;  // event is a SIGNAL event, sWeight  calculated separately
      teacher->SetNextInput(nvar,InputArray);     
    } //if mass


  } // for

  output.Write();
  output.Close();
  std::cout << "#Sig " << nSig << "(true " << nSigTrue << ", skipped " << nSkipped << ") #nBg " << nBg << std::endl;

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
