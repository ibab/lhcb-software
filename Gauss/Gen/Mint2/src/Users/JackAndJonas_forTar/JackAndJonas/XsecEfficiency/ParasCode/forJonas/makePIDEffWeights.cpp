// $Id: $
// Include files 
#include "PIDPerfTools/EvtTrackDataSet.h"
#include "PIDPerfTools/DataBinCuts.h"
#include "PIDPerfTools/PerfCalculator.h"
#include "TFile.h"

#include "TTree.h"
#include "TSystem.h"
#include "RooRealVar.h"
#include "RooBinning.h"
#include "TCut.h"
#include "RooWorkspace.h"
#include <list>
#include <boost/lexical_cast.hpp>
#include <cstdlib>

using namespace boost;
using namespace RooFit;

void printUsage(const char* fname)
{
  cout << "usage: " << fname <<
    " <perfFilename> <nCode> <inFile1> <outFile1> <tupleName> <tupleCdDir> <parentPrefix> <inFile2> <outfile2>" << 
    endl << endl;
  cout << 
    "nCode is the 3-digit code for how many kaons, pions, and protons you'd like to use" << endl <<
    "First Digit = NUMBER OF KAONS" << endl <<
    "Second Digit = NUMBER OF PIONS" << endl <<
    "Third Digit = NUMBER OF PROTONS" << endl <<
    " Example Codes: {K-pi+ = 110, K-pi+pi+pi- = 130, K-pi+p = 111}" << 
    endl << endl;
  cout <<
    "set tupleCdDir as \"\" if you don't want to create a subdirectory" << 
    endl << endl;
  cout <<
    "parentPrefix should usually be \"\" but you could provide your own (such as \"D_s_\" or \"D0_\" " << 
    endl << endl;
  cout <<
    "set inFile2 as \"\" if you only have one inFile" << 
    endl << endl;
  cout <<
    "* For compatibility with Conor's D_s tuples, choose mode 1210" << endl << 
    "* (this is code 210, with a flag 1 in the fourth digit to indicate to switch to Conor's naming scheme" << 
    endl << endl; 

  exit(0);
}

// Function: getDigitFromNum returns a digit at a given index of a integer.
// Goes from right to left with the starting index of 0.
int getDigitFromNum( int num, int digit )
{
  num /= pow( 10, digit );
  return num % 10;
}

int main(int argc, char *argv[]) 
{
  if (argc!=(9+1))    // 8 for the arguments and 1 for the name of this compiled program
  {
    printUsage(argv[0]);
  }

  const char* argv_perfFilename = (argv[1]);

  int argv_nCode = 0;
  try 
  {                                                
    argv_nCode = lexical_cast<int>(argv[2]);
  }
  catch (bad_lexical_cast& e) 
  {
    cout << "Option " << argv[3] << " cannot be converted to int" << endl;
    exit(1);
  }

  const char* argv_inFile1 = (argv[3]);
  const char* argv_outFile1 = (argv[4]);
  const char* argv_tupleName = (argv[5]);
  const char* argv_tupleCdDir = (argv[6]);
  const char* argv_parentPrefix = (argv[7]);
  const char* argv_inFile2 = (argv[8]);
  const char* argv_outFile2 = (argv[9]);

	gSystem->Load("libRooStats.so");

	int nK = getDigitFromNum(argv_nCode,2) ; // number of kaons
	int nPi = getDigitFromNum(argv_nCode,1) ; // number of pions
	int nP = getDigitFromNum(argv_nCode,0) ; // number of protons

  //	TString workingDir = "~/pidcalib/";

  //	TString perfFilename = "~/pidcalib/perfHists.root";
	TString perfFilename = argv_perfFilename ;

  // These are standard agreed choices at the moment, thus hard-coded
	TString in_pt = "pt";
	TString in_p = "p";
	TString in_eta = "eta";
	TString in_pdg = "pdg"; // takes the PDG number, we just use the sign of it later to determine the charge

  //	TString inFile1 = workingDir + "mc_forpid.root"; 
  //	TString outFile1 = workingDir + "mc_forpid_withpid.root"; 
	TString inFile1 = argv_inFile1; 
	TString outFile1 = argv_outFile1;

  //	TString tupleName = "mc_k3pi";
  //	TString tupleCdDir = "";
	TString tupleName = argv_tupleName;
	TString tupleCdDir = argv_tupleCdDir;
  
  //	TString inParent = ""; // It's a D0, D_s, etc... but we don't need to label it
	TString inParent = argv_parentPrefix;

  //	TString inFile2 = ""; 
  //	TString outFile2 = ""; 
	TString inFile2 = argv_inFile2; 
	TString outFile2 = argv_outFile2;


  //Now we have the TH3's containing the eff in pT, p, eta. 
  //Need to load our MC, loop on every candidate, find out what bin 
  //each falls into and create 2 new columns for each daughter and the 
  //parent particle: eff, err. 
  
  //Note that the following are the "default" names for daughter particles 
  //you should have in your tuple if you'd like to use this code

	TString kaon0 = "K0_";    
	TString kaon1 = "K1_";    
	TString kaon2 = "K2_";    
	TString kaon3 = "K3_";    
	TString kaon4 = "K4_";    

	TString pion0 = "Pi0_";    
	TString pion1 = "Pi1_";    
	TString pion2 = "Pi2_";    
	TString pion3 = "Pi3_";    
	TString pion4 = "Pi4_";    

	TString proton0 = "P0_";    
	TString proton1 = "P1_";    
	TString proton2 = "P2_";    
	TString proton3 = "P3_";    
	TString proton4 = "P4_";    

	TString pt = in_pt;
	TString p = in_p;
	TString eta = in_eta;
	TString pdg = in_pdg;

  ////	Checks flag to forcibly change things here to make
  ////  sure this works with Conor's tuples
  if (getDigitFromNum(argv_nCode,3) == 1) // 'secret' flag to switch to Conor's tuples
  {
  	kaon0 = "kaonplus_";
  	kaon1 = "kaonminus_";
  	pion0 = "pion_";
        pt = "LOKI_PT";
        p = "LOKI_P";
        eta = "LOKI_ETA";
        pdg = "ID";
  }

	TString PidEff = "pidcalib_eff";
	TString PidErr = "pidcalib_err";
	TString magnetPol = "magnet";

	TString inFilename = inFile1; 
	TString outFilename = outFile1;
  
	TString parent = inParent;

  // This loop allows for two input files, but if there is only one, that's ok too.
	for(UInt_t j = 0; j<2; j++)
  {                 
		if(strcmp(inFile2,"") && (j==1))
		{
      cout << "Second file processing..." << endl; 
			inFilename = inFile2;
			outFilename = outFile2;
		}
		else if (j==1)
		{
			cout << "There was only one file." << endl;
      return 0;
    }
    else
    {
      cout << "First file processing..." << endl;
    }
    
		TFile* mcCandFile = TFile::Open(inFilename);
		TTree* mcCandTuple = (TTree*)mcCandFile->Get(tupleName);
		TFile* outFile = new TFile(outFilename,"RECREATE");
		outFile->mkdir(tupleCdDir);
		outFile->cd(tupleCdDir);
		TTree *sOutTree = mcCandTuple->CloneTree(-1);

// Branches we need to WRITE

    // Default Values

    // -- Eff    
		Float_t parentPidEffBr = -9999.9;
		Float_t kaon0PidEffBr = -9999.9;
		Float_t kaon1PidEffBr = -9999.9;
		Float_t kaon2PidEffBr = -9999.9;
		Float_t kaon3PidEffBr = -9999.9;
		Float_t kaon4PidEffBr = -9999.9;
		Float_t pion0PidEffBr = -9999.9;
		Float_t pion1PidEffBr = -9999.9;
		Float_t pion2PidEffBr = -9999.9;
		Float_t pion3PidEffBr = -9999.9;
		Float_t pion4PidEffBr = -9999.9;
		Float_t proton0PidEffBr = -9999.9;
		Float_t proton1PidEffBr = -9999.9;
		Float_t proton2PidEffBr = -9999.9;
		Float_t proton3PidEffBr = -9999.9;
		Float_t proton4PidEffBr = -9999.9;

    // -- Err

		Float_t parentPidErrBr = -9999.9;
		Float_t kaon0PidErrBr = -9999.9;
		Float_t kaon1PidErrBr = -9999.9;
		Float_t kaon2PidErrBr = -9999.9;
		Float_t kaon3PidErrBr = -9999.9;
		Float_t kaon4PidErrBr = -9999.9;
		Float_t pion0PidErrBr = -9999.9;
		Float_t pion1PidErrBr = -9999.9;
		Float_t pion2PidErrBr = -9999.9;
		Float_t pion3PidErrBr = -9999.9;
		Float_t pion4PidErrBr = -9999.9;
		Float_t proton0PidErrBr = -9999.9;
		Float_t proton1PidErrBr = -9999.9;
		Float_t proton2PidErrBr = -9999.9;
		Float_t proton3PidErrBr = -9999.9;
		Float_t proton4PidErrBr = -9999.9;

    // TBranch creation

    // -- Eff

		TBranch *parentPidEffBranch = sOutTree->Branch(parent+PidEff, &parentPidEffBr, parent+PidEff+"/F");

		TBranch *kaon0PidEffBranch = 0;
		TBranch *kaon1PidEffBranch = 0;
		TBranch *kaon2PidEffBranch = 0;
		TBranch *kaon3PidEffBranch = 0;
		TBranch *kaon4PidEffBranch = 0;
		TBranch *pion0PidEffBranch = 0;
		TBranch *pion1PidEffBranch = 0;
		TBranch *pion2PidEffBranch = 0;
		TBranch *pion3PidEffBranch = 0;
		TBranch *pion4PidEffBranch = 0;
		TBranch *proton0PidEffBranch = 0;
		TBranch *proton1PidEffBranch = 0;
		TBranch *proton2PidEffBranch = 0;
		TBranch *proton3PidEffBranch = 0;
		TBranch *proton4PidEffBranch = 0;

    // -- Err

		TBranch *parentPidErrBranch = sOutTree->Branch(parent+PidErr, &parentPidErrBr, parent+PidErr+"/F");

		TBranch *kaon0PidErrBranch = 0;
		TBranch *kaon1PidErrBranch = 0;
		TBranch *kaon2PidErrBranch = 0;
		TBranch *kaon3PidErrBranch = 0;
		TBranch *kaon4PidErrBranch = 0;
		TBranch *pion0PidErrBranch = 0;
		TBranch *pion1PidErrBranch = 0;
		TBranch *pion2PidErrBranch = 0;
		TBranch *pion3PidErrBranch = 0;
		TBranch *pion4PidErrBranch = 0;
		TBranch *proton0PidErrBranch = 0;
		TBranch *proton1PidErrBranch = 0;
		TBranch *proton2PidErrBranch = 0;
		TBranch *proton3PidErrBranch = 0;
		TBranch *proton4PidErrBranch = 0;

    // Activate Non-parent Branches

		if (nK >= 1)
    {
      kaon0PidEffBranch = sOutTree->Branch(kaon0+PidEff, &kaon0PidEffBr, kaon0+PidEff+"/F");
      kaon0PidErrBranch = sOutTree->Branch(kaon0+PidErr, &kaon0PidErrBr, kaon0+PidErr+"/F");
    }
		if (nK >= 2) 
    {
      kaon1PidEffBranch = sOutTree->Branch(kaon1+PidEff, &kaon1PidEffBr, kaon1+PidEff+"/F");
      kaon1PidErrBranch = sOutTree->Branch(kaon1+PidErr, &kaon1PidErrBr, kaon1+PidErr+"/F");
    }
		if (nK >= 3)
    {
      kaon2PidEffBranch = sOutTree->Branch(kaon2+PidEff, &kaon2PidEffBr, kaon2+PidEff+"/F");
      kaon2PidErrBranch = sOutTree->Branch(kaon2+PidErr, &kaon2PidErrBr, kaon2+PidErr+"/F");
    }
		if (nK >= 4) 
    {
      kaon3PidEffBranch = sOutTree->Branch(kaon3+PidEff, &kaon3PidEffBr, kaon3+PidEff+"/F");
      kaon3PidErrBranch = sOutTree->Branch(kaon3+PidErr, &kaon3PidErrBr, kaon3+PidErr+"/F");

    }
		if (nK >= 5) 
    {
      kaon4PidEffBranch = sOutTree->Branch(kaon4+PidEff, &kaon4PidEffBr, kaon4+PidEff+"/F");
      kaon4PidErrBranch = sOutTree->Branch(kaon4+PidErr, &kaon4PidErrBr, kaon4+PidErr+"/F");
    }

		if (nPi >= 1)
    {
      pion0PidEffBranch = sOutTree->Branch(pion0+PidEff, &pion0PidEffBr, pion0+PidEff+"/F");
      pion0PidErrBranch = sOutTree->Branch(pion0+PidErr, &pion0PidErrBr, pion0+PidErr+"/F");
    }
		if (nPi >= 2) 
    {
      pion1PidEffBranch = sOutTree->Branch(pion1+PidEff, &pion1PidEffBr, pion1+PidEff+"/F");
      pion1PidErrBranch = sOutTree->Branch(pion1+PidErr, &pion1PidErrBr, pion1+PidErr+"/F");
    }
		if (nPi >= 3)
    {
      pion2PidEffBranch = sOutTree->Branch(pion2+PidEff, &pion2PidEffBr, pion2+PidEff+"/F");
      pion2PidErrBranch = sOutTree->Branch(pion2+PidErr, &pion2PidErrBr, pion2+PidErr+"/F");
    }
		if (nPi >= 4) 
    {
      pion3PidEffBranch = sOutTree->Branch(pion3+PidEff, &pion3PidEffBr, pion3+PidEff+"/F");
      pion3PidErrBranch = sOutTree->Branch(pion3+PidErr, &pion3PidErrBr, pion3+PidErr+"/F");

    }
		if (nPi >= 5) 
    {
      pion4PidEffBranch = sOutTree->Branch(pion4+PidEff, &pion4PidEffBr, pion4+PidEff+"/F");
      pion4PidErrBranch = sOutTree->Branch(pion4+PidErr, &pion4PidErrBr, pion4+PidErr+"/F");
    }

		if (nP >= 1)
    {
      proton0PidEffBranch = sOutTree->Branch(proton0+PidEff, &proton0PidEffBr, proton0+PidEff+"/F");
      proton0PidErrBranch = sOutTree->Branch(proton0+PidErr, &proton0PidErrBr, proton0+PidErr+"/F");
    }
		if (nP >= 2) 
    {
      proton1PidEffBranch = sOutTree->Branch(proton1+PidEff, &proton1PidEffBr, proton1+PidEff+"/F");
      proton1PidErrBranch = sOutTree->Branch(proton1+PidErr, &proton1PidErrBr, proton1+PidErr+"/F");
    }
		if (nP >= 3)
    {
      proton2PidEffBranch = sOutTree->Branch(proton2+PidEff, &proton2PidEffBr, proton2+PidEff+"/F");
      proton2PidErrBranch = sOutTree->Branch(proton2+PidErr, &proton2PidErrBr, proton2+PidErr+"/F");
    }
		if (nP >= 4) 
    {
      proton3PidEffBranch = sOutTree->Branch(proton3+PidEff, &proton3PidEffBr, proton3+PidEff+"/F");
      proton3PidErrBranch = sOutTree->Branch(proton3+PidErr, &proton3PidErrBr, proton3+PidErr+"/F");

    }
		if (nP >= 5) 
    {
      proton4PidEffBranch = sOutTree->Branch(proton4+PidEff, &proton4PidEffBr, proton4+PidEff+"/F");
      proton4PidErrBranch = sOutTree->Branch(proton4+PidErr, &proton4PidErrBr, proton4+PidErr+"/F");
    }
    

// Branches we need to READ

    // Default Values

    Int_t magnetPolBr = 0;

    Int_t kaon0pdgBr = 0;
    Float_t kaon0pt = -9999.9;
    Float_t kaon0p = -9999.9;
    Float_t kaon0eta = -9999.9;
    Int_t kaon1pdgBr = 0;
    Float_t kaon1pt = -9999.9;
    Float_t kaon1p = -9999.9;
    Float_t kaon1eta = -9999.9;
    Int_t kaon2pdgBr = 0;
    Float_t kaon2pt = -9999.9;
    Float_t kaon2p = -9999.9;
    Float_t kaon2eta = -9999.9;
    Int_t kaon3pdgBr = 0;
    Float_t kaon3pt = -9999.9;
    Float_t kaon3p = -9999.9;
    Float_t kaon3eta = -9999.9;
    Int_t kaon4pdgBr = 0;
    Float_t kaon4pt = -9999.9;
    Float_t kaon4p = -9999.9;
    Float_t kaon4eta = -9999.9;

    Int_t pion0pdgBr = 0;
    Float_t pion0pt = -9999.9;
    Float_t pion0p = -9999.9;
    Float_t pion0eta = -9999.9;
    Int_t pion1pdgBr = 0;
    Float_t pion1pt = -9999.9;
    Float_t pion1p = -9999.9;
    Float_t pion1eta = -9999.9;
    Int_t pion2pdgBr = 0;
    Float_t pion2pt = -9999.9;
    Float_t pion2p = -9999.9;
    Float_t pion2eta = -9999.9;
    Int_t pion3pdgBr = 0;
    Float_t pion3pt = -9999.9;
    Float_t pion3p = -9999.9;
    Float_t pion3eta = -9999.9;
    Int_t pion4pdgBr = 0;
    Float_t pion4pt = -9999.9;
    Float_t pion4p = -9999.9;
    Float_t pion4eta = -9999.9;

    Int_t proton0pdgBr = 0;
    Float_t proton0pt = -9999.9;
    Float_t proton0p = -9999.9;
    Float_t proton0eta = -9999.9;
    Int_t proton1pdgBr = 0;
    Float_t proton1pt = -9999.9;
    Float_t proton1p = -9999.9;
    Float_t proton1eta = -9999.9;
    Int_t proton2pdgBr = 0;
    Float_t proton2pt = -9999.9;
    Float_t proton2p = -9999.9;
    Float_t proton2eta = -9999.9;
    Int_t proton3pdgBr = 0;
    Float_t proton3pt = -9999.9;
    Float_t proton3p = -9999.9;
    Float_t proton3eta = -9999.9;
    Int_t proton4pdgBr = 0;
    Float_t proton4pt = -9999.9;
    Float_t proton4p = -9999.9;
    Float_t proton4eta = -9999.9;

    // Set Branch Addresses

    sOutTree->SetBranchAddress(magnetPol,&magnetPolBr);

    // -- Kaons

    if (nK >= 1)
    {
      sOutTree->SetBranchAddress(kaon0+pdg,&kaon0pdgBr);
      sOutTree->SetBranchAddress(kaon0+pt,&kaon0pt);
      sOutTree->SetBranchAddress(kaon0+p,&kaon0p);
      sOutTree->SetBranchAddress(kaon0+eta,&kaon0eta);
		}
    if (nK >= 2)
    {
      sOutTree->SetBranchAddress(kaon1+pdg,&kaon1pdgBr);
      sOutTree->SetBranchAddress(kaon1+pt,&kaon1pt);
      sOutTree->SetBranchAddress(kaon1+p,&kaon1p);
      sOutTree->SetBranchAddress(kaon1+eta,&kaon1eta);
		}
    if (nK >= 3)
    {
      sOutTree->SetBranchAddress(kaon2+pdg,&kaon2pdgBr);
      sOutTree->SetBranchAddress(kaon2+pt,&kaon2pt);
      sOutTree->SetBranchAddress(kaon2+p,&kaon2p);
      sOutTree->SetBranchAddress(kaon2+eta,&kaon2eta);
		}
    if (nK >= 4)
    {
      sOutTree->SetBranchAddress(kaon3+pdg,&kaon3pdgBr);
      sOutTree->SetBranchAddress(kaon3+pt,&kaon3pt);
      sOutTree->SetBranchAddress(kaon3+p,&kaon3p);
      sOutTree->SetBranchAddress(kaon3+eta,&kaon3eta);
		}
    if (nK >= 5)
    {
      sOutTree->SetBranchAddress(kaon4+pdg,&kaon4pdgBr);
      sOutTree->SetBranchAddress(kaon4+pt,&kaon4pt);
      sOutTree->SetBranchAddress(kaon4+p,&kaon4p);
      sOutTree->SetBranchAddress(kaon4+eta,&kaon4eta);
		}
    
    // -- Pions

    if (nPi >= 1)
    {
      sOutTree->SetBranchAddress(pion0+pdg,&pion0pdgBr);
      sOutTree->SetBranchAddress(pion0+pt,&pion0pt);
      sOutTree->SetBranchAddress(pion0+p,&pion0p);
      sOutTree->SetBranchAddress(pion0+eta,&pion0eta);
		}
    if (nPi >= 2)
    {
      sOutTree->SetBranchAddress(pion1+pdg,&pion1pdgBr);
      sOutTree->SetBranchAddress(pion1+pt,&pion1pt);
      sOutTree->SetBranchAddress(pion1+p,&pion1p);
      sOutTree->SetBranchAddress(pion1+eta,&pion1eta);
		}
    if (nPi >= 3)
    {
      sOutTree->SetBranchAddress(pion2+pdg,&pion2pdgBr);
      sOutTree->SetBranchAddress(pion2+pt,&pion2pt);
      sOutTree->SetBranchAddress(pion2+p,&pion2p);
      sOutTree->SetBranchAddress(pion2+eta,&pion2eta);
		}
    if (nPi >= 4)
    {
      sOutTree->SetBranchAddress(pion3+pdg,&pion3pdgBr);
      sOutTree->SetBranchAddress(pion3+pt,&pion3pt);
      sOutTree->SetBranchAddress(pion3+p,&pion3p);
      sOutTree->SetBranchAddress(pion3+eta,&pion3eta);
		}
    if (nPi >= 5)
    {
      sOutTree->SetBranchAddress(pion4+pdg,&pion4pdgBr);
      sOutTree->SetBranchAddress(pion4+pt,&pion4pt);
      sOutTree->SetBranchAddress(pion4+p,&pion4p);
      sOutTree->SetBranchAddress(pion4+eta,&pion4eta);
		}
    
    // -- Protons

    if (nP >= 1)
    {
      sOutTree->SetBranchAddress(proton0+pdg,&proton0pdgBr);
      sOutTree->SetBranchAddress(proton0+pt,&proton0pt);
      sOutTree->SetBranchAddress(proton0+p,&proton0p);
      sOutTree->SetBranchAddress(proton0+eta,&proton0eta);
		}
    if (nP >= 2)
    {
      sOutTree->SetBranchAddress(proton1+pdg,&proton1pdgBr);
      sOutTree->SetBranchAddress(proton1+pt,&proton1pt);
      sOutTree->SetBranchAddress(proton1+p,&proton1p);
      sOutTree->SetBranchAddress(proton1+eta,&proton1eta);
		}
    if (nP >= 3)
    {
      sOutTree->SetBranchAddress(proton2+pdg,&proton2pdgBr);
      sOutTree->SetBranchAddress(proton2+pt,&proton2pt);
      sOutTree->SetBranchAddress(proton2+p,&proton2p);
      sOutTree->SetBranchAddress(proton2+eta,&proton2eta);
		}
    if (nP >= 4)
    {
      sOutTree->SetBranchAddress(proton3+pdg,&proton3pdgBr);
      sOutTree->SetBranchAddress(proton3+pt,&proton3pt);
      sOutTree->SetBranchAddress(proton3+p,&proton3p);
      sOutTree->SetBranchAddress(proton3+eta,&proton3eta);
		}
    if (nP >= 5)
    {
      sOutTree->SetBranchAddress(proton4+pdg,&proton4pdgBr);
      sOutTree->SetBranchAddress(proton4+pt,&proton4pt);
      sOutTree->SetBranchAddress(proton4+p,&proton4p);
      sOutTree->SetBranchAddress(proton4+eta,&proton4eta);
		}



    //Loop, set hists based on magnet pol, get p, pt, eta of daughters, set pid. 

    UInt_t total = sOutTree->GetEntries();

    TH3* kNegEffhist; 
    TH3* kPosEffhist;
    TH3* pNegEffhist;
    TH3* pPosEffhist;
    TH3* PRONegEffhist; // for future proton name TBD
    TH3* PROPosEffhist; // for future proton name TBD

    // some of these will load a null pointer, it's ok so long as you don't access those
    TFile* perfFile = TFile::Open(perfFilename, "READ");
    TH3* KPosMdownEff = (TH3F*)gDirectory->Get("Ratio_KaonPos_MagDown");
    TH3* KNegMdownEff = (TH3F*)gDirectory->Get("Ratio_KaonNeg_MagDown");
    TH3* KPosMupEff = (TH3F*)gDirectory->Get("Ratio_KaonPos_MagUp");
    TH3* KNegMupEff = (TH3F*)gDirectory->Get("Ratio_KaonNeg_MagUp");
    TH3* PPosMdownEff = (TH3F*)gDirectory->Get("Ratio_PionPos_MagDown");
    TH3* PNegMdownEff = (TH3F*)gDirectory->Get("Ratio_PionNeg_MagDown");
    TH3* PPosMupEff = (TH3F*)gDirectory->Get("Ratio_PionPos_MagUp");
    TH3* PNegMupEff = (TH3F*)gDirectory->Get("Ratio_PionNeg_MagUp");
    TH3* PROPosMdownEff = (TH3F*)gDirectory->Get("Ratio_ProtonPos_MagDown");
    TH3* PRONegMdownEff = (TH3F*)gDirectory->Get("Ratio_ProtonNeg_MagDown");
    TH3* PROPosMupEff = (TH3F*)gDirectory->Get("Ratio_ProtonPos_MagUp");
    TH3* PRONegMupEff = (TH3F*)gDirectory->Get("Ratio_ProtonNeg_MagUp");    
    
    outFile->cd();
		for(UInt_t i = 0; i<total; i++)
    {
			sOutTree->GetEntry(i);
			if(magnetPolBr<0)
      {
				kNegEffhist=KNegMdownEff; 
				kPosEffhist=KPosMdownEff;
				pNegEffhist=PNegMdownEff;
				pPosEffhist=PPosMdownEff;
        PRONegEffhist=PRONegMdownEff; 
        PROPosEffhist=PROPosMdownEff;
			}
      else
      {
				kNegEffhist=KNegMupEff; 
				kPosEffhist=KPosMupEff;
				pNegEffhist=PNegMupEff;
				pPosEffhist=PPosMupEff;	
        PRONegEffhist=PRONegMupEff;
        PROPosEffhist=PROPosMupEff; 
			}
      
			// Kaons
      if (nK >= 1)
			{
				if(kaon0pdgBr>0)
        {
					kaon0PidEffBr = kPosEffhist->GetBinContent(kPosEffhist->FindBin(kaon0p,kaon0pt,kaon0eta));
					kaon0PidErrBr = kPosEffhist->GetBinError(kPosEffhist->FindBin(kaon0p,kaon0pt,kaon0eta));
        }
        else
        {
					kaon0PidEffBr = kNegEffhist->GetBinContent(kNegEffhist->FindBin(kaon0p,kaon0pt,kaon0eta));
					kaon0PidErrBr = kNegEffhist->GetBinError(kNegEffhist->FindBin(kaon0p,kaon0pt,kaon0eta));
				}
			}
      if (nK >= 2)
			{
				if(kaon1pdgBr>0)
        {
					kaon1PidEffBr = kPosEffhist->GetBinContent(kPosEffhist->FindBin(kaon1p,kaon1pt,kaon1eta));
					kaon1PidErrBr = kPosEffhist->GetBinError(kPosEffhist->FindBin(kaon1p,kaon1pt,kaon1eta));
				}
        else
        {
					kaon1PidEffBr = kNegEffhist->GetBinContent(kNegEffhist->FindBin(kaon1p,kaon1pt,kaon1eta));
					kaon1PidErrBr = kNegEffhist->GetBinError(kNegEffhist->FindBin(kaon1p,kaon1pt,kaon1eta));
				}
			}
      if (nK >= 3)
			{
				if(kaon2pdgBr>0)
        {
					kaon2PidEffBr = kPosEffhist->GetBinContent(kPosEffhist->FindBin(kaon2p,kaon2pt,kaon2eta));
					kaon2PidErrBr = kPosEffhist->GetBinError(kPosEffhist->FindBin(kaon2p,kaon2pt,kaon2eta));
				}
        else
        {
					kaon2PidEffBr = kNegEffhist->GetBinContent(kNegEffhist->FindBin(kaon2p,kaon2pt,kaon2eta));
					kaon2PidErrBr = kNegEffhist->GetBinError(kNegEffhist->FindBin(kaon2p,kaon2pt,kaon2eta));
				}
			}
      if (nK >= 4)
			{
				if(kaon3pdgBr>0)
        {
					kaon3PidEffBr = kPosEffhist->GetBinContent(kPosEffhist->FindBin(kaon3p,kaon3pt,kaon3eta));
					kaon3PidErrBr = kPosEffhist->GetBinError(kPosEffhist->FindBin(kaon3p,kaon3pt,kaon3eta));
				}
        else
        {
					kaon3PidEffBr = kNegEffhist->GetBinContent(kNegEffhist->FindBin(kaon3p,kaon3pt,kaon3eta));
					kaon3PidErrBr = kNegEffhist->GetBinError(kNegEffhist->FindBin(kaon3p,kaon3pt,kaon3eta));
				}
			}
      if (nK >= 5)
      {
				if(kaon4pdgBr>0)
        {
					kaon4PidEffBr = kPosEffhist->GetBinContent(kPosEffhist->FindBin(kaon4p,kaon4pt,kaon4eta));
					kaon4PidErrBr = kPosEffhist->GetBinError(kPosEffhist->FindBin(kaon4p,kaon4pt,kaon4eta));
				}
        else
        {
					kaon4PidEffBr = kNegEffhist->GetBinContent(kNegEffhist->FindBin(kaon4p,kaon4pt,kaon4eta));
					kaon4PidErrBr = kNegEffhist->GetBinError(kNegEffhist->FindBin(kaon4p,kaon4pt,kaon4eta));
				}
			}

			// Pions
      if (nPi >= 1)
			{
				if(pion0pdgBr>0)
        {
					pion0PidEffBr = pPosEffhist->GetBinContent(pPosEffhist->FindBin(pion0p,pion0pt,pion0eta));
					pion0PidErrBr = pPosEffhist->GetBinError(pPosEffhist->FindBin(pion0p,pion0pt,pion0eta));
				}
        else
        {
					pion0PidEffBr = pNegEffhist->GetBinContent(pNegEffhist->FindBin(pion0p,pion0pt,pion0eta));
					pion0PidErrBr = pNegEffhist->GetBinError(pNegEffhist->FindBin(pion0p,pion0pt,pion0eta));
				}
			}
      if (nPi >= 2)
			{
				if(pion1pdgBr>0)
        {
					pion1PidEffBr = pPosEffhist->GetBinContent(pPosEffhist->FindBin(pion1p,pion1pt,pion1eta));
					pion1PidErrBr = pPosEffhist->GetBinError(pPosEffhist->FindBin(pion1p,pion1pt,pion1eta));
				}
        else
        {
					pion1PidEffBr = pNegEffhist->GetBinContent(pNegEffhist->FindBin(pion1p,pion1pt,pion1eta));
					pion1PidErrBr = pNegEffhist->GetBinError(pNegEffhist->FindBin(pion1p,pion1pt,pion1eta));
				}
			}
      if (nPi >= 3)
			{
				if(pion2pdgBr>0)
        {
					pion2PidEffBr = pPosEffhist->GetBinContent(pPosEffhist->FindBin(pion2p,pion2pt,pion2eta));
					pion2PidErrBr = pPosEffhist->GetBinError(pPosEffhist->FindBin(pion2p,pion2pt,pion2eta));
				}
        else
        {
					pion2PidEffBr = pNegEffhist->GetBinContent(pNegEffhist->FindBin(pion2p,pion2pt,pion2eta));
					pion2PidErrBr = pNegEffhist->GetBinError(pNegEffhist->FindBin(pion2p,pion2pt,pion2eta));
				}
			}
      if (nPi >= 4)
			{
				if(pion3pdgBr>0)
        {
					pion3PidEffBr = pPosEffhist->GetBinContent(pPosEffhist->FindBin(pion3p,pion3pt,pion3eta));
					pion3PidErrBr = pPosEffhist->GetBinError(pPosEffhist->FindBin(pion3p,pion3pt,pion3eta));
				}
        else
        {
					pion3PidEffBr = pNegEffhist->GetBinContent(pNegEffhist->FindBin(pion3p,pion3pt,pion3eta));
					pion3PidErrBr = pNegEffhist->GetBinError(pNegEffhist->FindBin(pion3p,pion3pt,pion3eta));
				}
			}
      if (nPi >= 5)
			{
				if(pion4pdgBr>0)
        {
					pion4PidEffBr = pPosEffhist->GetBinContent(pPosEffhist->FindBin(pion4p,pion4pt,pion4eta));
					pion4PidErrBr = pPosEffhist->GetBinError(pPosEffhist->FindBin(pion4p,pion4pt,pion4eta));
				}
        else
        {
					pion4PidEffBr = pNegEffhist->GetBinContent(pNegEffhist->FindBin(pion4p,pion4pt,pion4eta));
					pion4PidErrBr = pNegEffhist->GetBinError(pNegEffhist->FindBin(pion4p,pion4pt,pion4eta));
				}
			}

			// Protons
      if (nP >= 1)
      {
				if(proton0pdgBr>0)
        {
					proton0PidEffBr = PROPosEffhist->GetBinContent(PROPosEffhist->FindBin(proton0p,proton0pt,proton0eta));
					proton0PidErrBr = PROPosEffhist->GetBinError(PROPosEffhist->FindBin(proton0p,proton0pt,proton0eta));
				}
        else
        {
					proton0PidEffBr = PRONegEffhist->GetBinContent(PRONegEffhist->FindBin(proton0p,proton0pt,proton0eta));
					proton0PidErrBr = PRONegEffhist->GetBinError(PRONegEffhist->FindBin(proton0p,proton0pt,proton0eta));
				}
			}
      if (nP >= 2)
			{
				if(proton1pdgBr>0)
        {
					proton1PidEffBr = PROPosEffhist->GetBinContent(PROPosEffhist->FindBin(proton1p,proton1pt,proton1eta));
					proton1PidErrBr = PROPosEffhist->GetBinError(PROPosEffhist->FindBin(proton1p,proton1pt,proton1eta));
				}
        else
        {
					proton1PidEffBr = PRONegEffhist->GetBinContent(PRONegEffhist->FindBin(proton1p,proton1pt,proton1eta));
					proton1PidErrBr = PRONegEffhist->GetBinError(PRONegEffhist->FindBin(proton1p,proton1pt,proton1eta));
				}
			}
      if (nP >= 3)
			{
				if(proton2pdgBr>0)
        {
					proton2PidEffBr = PROPosEffhist->GetBinContent(PROPosEffhist->FindBin(proton2p,proton2pt,proton2eta));
					proton2PidErrBr = PROPosEffhist->GetBinError(PROPosEffhist->FindBin(proton2p,proton2pt,proton2eta));
				}
        else
        {
					proton2PidEffBr = PRONegEffhist->GetBinContent(PRONegEffhist->FindBin(proton2p,proton2pt,proton2eta));
					proton2PidErrBr = PRONegEffhist->GetBinError(PRONegEffhist->FindBin(proton2p,proton2pt,proton2eta));
				}
			}
      if (nP >= 4)
			{
				if(proton3pdgBr>0)
        {
					proton3PidEffBr = PROPosEffhist->GetBinContent(PROPosEffhist->FindBin(proton3p,proton3pt,proton3eta));
					proton3PidErrBr = PROPosEffhist->GetBinError(PROPosEffhist->FindBin(proton3p,proton3pt,proton3eta));
				}
        else
        {
					proton3PidEffBr = PRONegEffhist->GetBinContent(PRONegEffhist->FindBin(proton3p,proton3pt,proton3eta));
					proton3PidErrBr = PRONegEffhist->GetBinError(PRONegEffhist->FindBin(proton3p,proton3pt,proton3eta));
				}
			}
      if (nP >= 5)
			{
				if(proton4pdgBr>0)
        {
					proton4PidEffBr = PROPosEffhist->GetBinContent(PROPosEffhist->FindBin(proton4p,proton4pt,proton4eta));
					proton4PidErrBr = PROPosEffhist->GetBinError(PROPosEffhist->FindBin(proton4p,proton4pt,proton4eta));
				}
        else
        {
					proton4PidEffBr = PRONegEffhist->GetBinContent(PRONegEffhist->FindBin(proton4p,proton4pt,proton4eta));
					proton4PidErrBr = PRONegEffhist->GetBinError(PRONegEffhist->FindBin(proton4p,proton4pt,proton4eta));
				}
			}

			// Parent
			parentPidEffBr =
        (nK >= 1 ? kaon0PidEffBr : 1) * 
        (nK >= 2 ? kaon1PidEffBr : 1) * 
        (nK >= 3 ? kaon2PidEffBr : 1) * 
        (nK >= 4 ? kaon3PidEffBr : 1) * 
        (nK >= 5 ? kaon4PidEffBr : 1) * 
        (nPi >= 1 ? pion0PidEffBr : 1) * 
        (nPi >= 2 ? pion1PidEffBr : 1) * 
        (nPi >= 3 ? pion2PidEffBr : 1) * 
        (nPi >= 4 ? pion3PidEffBr : 1) * 
        (nPi >= 5 ? pion4PidEffBr : 1) * 
        (nP >= 1 ? proton0PidEffBr : 1) * 
        (nP >= 2 ? proton1PidEffBr : 1) * 
        (nP >= 3 ? proton2PidEffBr : 1) * 
        (nP >= 4 ? proton3PidEffBr : 1) * 
        (nP >= 5 ? proton4PidEffBr : 1) ; 
			parentPidErrBr = 
        parentPidEffBr * 
        sqrt( 
             (nK >= 1 ? ((kaon0PidErrBr/kaon0PidEffBr)*(kaon0PidErrBr/kaon0PidEffBr)) : 0 ) +  
             (nK >= 2 ? ((kaon1PidErrBr/kaon1PidEffBr)*(kaon1PidErrBr/kaon1PidEffBr)) : 0 ) +  
             (nK >= 3 ? ((kaon2PidErrBr/kaon2PidEffBr)*(kaon2PidErrBr/kaon2PidEffBr)) : 0 ) +  
             (nK >= 4 ? ((kaon3PidErrBr/kaon3PidEffBr)*(kaon3PidErrBr/kaon3PidEffBr)) : 0 ) +  
             (nK >= 5 ? ((kaon4PidErrBr/kaon4PidEffBr)*(kaon4PidErrBr/kaon4PidEffBr)) : 0 ) +  
             (nPi >= 1 ? ((pion0PidErrBr/pion0PidEffBr)*(pion0PidErrBr/pion0PidEffBr)) : 0 ) +  
             (nPi >= 2 ? ((pion1PidErrBr/pion1PidEffBr)*(pion1PidErrBr/pion1PidEffBr)) : 0 ) +  
             (nPi >= 3 ? ((pion2PidErrBr/pion2PidEffBr)*(pion2PidErrBr/pion2PidEffBr)) : 0 ) +  
             (nPi >= 4 ? ((pion3PidErrBr/pion3PidEffBr)*(pion3PidErrBr/pion3PidEffBr)) : 0 ) +  
             (nPi >= 5 ? ((pion4PidErrBr/pion4PidEffBr)*(pion4PidErrBr/pion4PidEffBr)) : 0 ) +  
             (nP >= 1 ? ((proton0PidErrBr/proton0PidEffBr)*(proton0PidErrBr/proton0PidEffBr)) : 0 ) +  
             (nP >= 2 ? ((proton1PidErrBr/proton1PidEffBr)*(proton1PidErrBr/proton1PidEffBr)) : 0 ) +  
             (nP >= 3 ? ((proton2PidErrBr/proton2PidEffBr)*(proton2PidErrBr/proton2PidEffBr)) : 0 ) +  
             (nP >= 4 ? ((proton3PidErrBr/proton3PidEffBr)*(proton3PidErrBr/proton3PidEffBr)) : 0 ) +  
             (nP >= 5 ? ((proton4PidErrBr/proton4PidEffBr)*(proton4PidErrBr/proton4PidEffBr)) : 0 )   
             );


			if(kaon0PidErrBr==0.0){ 
        cout << "kaon0pid error is zero here: (p = " << kaon0p 
             << ", pt = " << kaon0pt << ", eta = " << kaon0eta << ")" << endl;}
			if(kaon1PidErrBr==0.0){
        cout << "kaon1pid error is zero here: (p = " << kaon1p 
             << ", pt = " << kaon1pt << ", eta = " << kaon1eta << ")" << endl;}
			if(kaon2PidErrBr==0.0){ 
        cout << "kaon2pid error is zero here: (p = " << kaon2p
             << ", pt = " << kaon2pt << ", eta = " << kaon2eta << ")" << endl;}
			if(kaon3PidErrBr==0.0){ 
        cout << "kaon3pid error is zero here: (p = " << kaon3p
             << ", pt = " << kaon3pt << ", eta = " << kaon3eta << ")" << endl;}
			if(kaon4PidErrBr==0.0){ 
        cout << "kaon4pid error is zero here: (p = " << kaon4p
             << ", pt = " << kaon4pt << ", eta = " << kaon4eta << ")" << endl;}

			if(pion0PidErrBr==0.0){ 
        cout << "pion0pid error is zero here: (p = " << pion0p 
             << ", pt = " << pion0pt << ", eta = " << pion0eta << ")" << endl;}
			if(pion1PidErrBr==0.0){ 
        cout << "pion1pid error is zero here: (p = " << pion1p 
             << ", pt = " << pion1pt << ", eta = " << pion1eta << ")" << endl;}
			if(pion2PidErrBr==0.0){
        cout << "pion2pid error is zero here: (p = " << pion2p
             << ", pt = " << pion2pt << ", eta = " << pion2eta << ")" << endl;}
			if(pion3PidErrBr==0.0){
        cout << "pion3pid error is zero here: (p = " << pion3p
             << ", pt = " << pion3pt << ", eta = " << pion3eta << ")" << endl;}
			if(pion4PidErrBr==0.0){
        cout << "pion4pid error is zero here: (p = " << pion4p
             << ", pt = " << pion4pt << ", eta = " << pion4eta << ")" << endl;}

			if(proton0PidErrBr==0.0){
        cout << "proton0pid error is zero here: (p = " << proton0p
             << ", pt = " << proton0pt << ", eta = " << proton0eta << ")" << endl;}
			if(proton1PidErrBr==0.0){ 
        cout << "proton1pid error is zero here: (p = " << proton1p
             << ", pt = " << proton1pt << ", eta = " << proton1eta << ")" << endl;}
			if(proton2PidErrBr==0.0){ 
        cout << "proton2pid error is zero here: (p = " << proton2p 
             << ", pt = " << proton2pt << ", eta = " << proton2eta << ")" << endl;}
			if(proton3PidErrBr==0.0){ 
        cout << "proton3pid error is zero here: (p = " << proton3p
             << ", pt = " << proton3pt << ", eta = " << proton3eta << ")" << endl;}
			if(proton4PidErrBr==0.0){ 
        cout << "proton4pid error is zero here: (p = " << proton4p
             << ", pt = " << proton4pt << ", eta = " << proton4eta << ")" << endl;}

      // Fill Branches

			parentPidEffBranch->Fill();
			parentPidErrBranch->Fill();

      // -- Kaon

			if (nK >= 1)
      {
        kaon0PidEffBranch->Fill();
        kaon0PidErrBranch->Fill();
      }
			if (nK >= 2) 
      {
        kaon1PidEffBranch->Fill();
        kaon1PidErrBranch->Fill();
      }
			if (nK >= 3) 
      {
        kaon2PidEffBranch->Fill();
        kaon2PidErrBranch->Fill();
			}
			if (nK >= 4) 
      {
        kaon3PidEffBranch->Fill();
        kaon3PidErrBranch->Fill();
      }
			if (nK >= 5) 
      {
        kaon4PidEffBranch->Fill();
        kaon4PidErrBranch->Fill();
      }
    

      // -- Pion

			if (nPi >= 1)
      {
        pion0PidEffBranch->Fill();
        pion0PidErrBranch->Fill();
      }
			if (nPi >= 2) 
      {
        pion1PidEffBranch->Fill();
        pion1PidErrBranch->Fill();
      }
			if (nPi >= 3) 
      {
        pion2PidEffBranch->Fill();
        pion2PidErrBranch->Fill();
			}
			if (nPi >= 4) 
      {
        pion3PidEffBranch->Fill();
        pion3PidErrBranch->Fill();
      }
			if (nPi >= 5) 
      {
        pion4PidEffBranch->Fill();
        pion4PidErrBranch->Fill();
      }

      // -- Proton

			if (nP >= 1)
      {
        proton0PidEffBranch->Fill();
        proton0PidErrBranch->Fill();
      }
			if (nP >= 2) 
      {
        proton1PidEffBranch->Fill();
        proton1PidErrBranch->Fill();
      }
			if (nP >= 3) 
      {
        proton2PidEffBranch->Fill();
        proton2PidErrBranch->Fill();
			}
			if (nP >= 4) 
      {
        proton3PidEffBranch->Fill();
        proton3PidErrBranch->Fill();
      }
			if (nP >= 5) 
      {
        proton4PidEffBranch->Fill();
        proton4PidErrBranch->Fill();
      }

		}

		sOutTree->Write();
		outFile->Close();
		perfFile->Close();

	}

	return 0;

}

