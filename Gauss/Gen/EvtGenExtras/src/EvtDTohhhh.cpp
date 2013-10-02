//--------------------------------------------------------------------------
//
//  D0 -> h h h h  with the MINT generator
//	Matt Coombes
//------------------------------------------------------------------------
//
#include <cstdlib>

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenModels/EvtDTohhhh.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtRandom.hh"

// Gaudi
#include "GaudiKernel/System.h"

// C++
#include <string>
#include <fstream>
#include <sstream>

// ROOT
#include "TBuffer.h"

// MINT
#include "Mint/IMintGen.h"
#include "Mint/D_hhhh.h"

//using namespace std;
//using namespace MINT;

/*** Implementation of EvtTRandom ***/
MINT::EvtRand::EvtRand() : m_nCalls(0)
{
  //*-*-*-*-*-*-*-*-*-*-*default constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  // The seed is ignored in this class, since we want to use the EvtGen
  // seed constructed from the run number and event number
  report(DEBUG, "EvtGen") << "MINT::EvtRand::EvtRand() called" 
                          << std::endl;
}

MINT::EvtRand::EvtRand(const MINT::EvtRand& other)
  : MINT::IEvtRandom(other), m_nCalls(0)
{
  //*-*-*-*-*-*-*-*-*-*-*copy destructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  report(DEBUG, "EvtGen") << "MINT::EvtRand::EvtRand"
                          << "(MINT::EvtRand& other) called" 
                          << std::endl;
}

MINT::EvtRand::~EvtRand()
{
  //*-*-*-*-*-*-*-*-*-*-*default destructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  report(DEBUG, "EvtGen") << "EvtTRand::~EvtTRand() called"
                          << std::endl;
}

double MINT::EvtRand::Rndm(int)
{
  // Call EvtRandom::Flat()
  double rndm=EvtRandom::Flat();
  if (m_nCalls%5000==0) {
    report(DEBUG, "EvtGen") << "MINT::EvtRand::Rndm() called ("
                            << m_nCalls+1 << " calls so far). Will "
                            << "return " << rndm << std::endl;
  }
  m_nCalls++;
  return rndm;
}

void MINT::EvtRand::SetSeed(unsigned int seed)
{
  // This method doesn't do anything, since we use the EvtGen random seed
  report(DEBUG, "EvtGen") << "MINT::EvtRand::SetSeed(UInt_t seed = "
    << seed << "). This seed will be ignored" << std::endl;
}

unsigned int MINT::EvtRand::GetSeed() const
{
  report(DEBUG, "EvtGen") << "MINT::EvtRand::GetSeed() called. "
    << ". Will return 0 (output should be ignored)" << std::endl;
  return 0;
}  
/* end implementation of MINT::EvtRandom */

EvtDTohhhh::~EvtDTohhhh() 
{
  if (m_MintGen!=0) {
    delete m_MintGen;
    m_MintGen=0;
  }
  if (m_rnd!=0) {
    delete m_rnd;
    m_rnd=0;
  }
}


std::string EvtDTohhhh::getName(){

  return "D_hhhh";

}


EvtDecayBase* EvtDTohhhh::clone(){

  return new EvtDTohhhh;

}

void EvtDTohhhh::init(){

  // check that there are 3 arguments
  checkNArg(1);
  // Check that there are 4 daughters
  checkNDaug(4);

  //m_MintGen = new MintGen();
  MINT::EvtRand* evRand = new MINT::EvtRand();
  m_rnd = new MINT::EvtTRandom(evRand);

  m_MintGen = new MINT::MintGen();

  static EvtId D0=EvtPDL::getId("D0");
  static EvtId D0B=EvtPDL::getId("anti-D0");
  static EvtId KM=EvtPDL::getId("K-");
  static EvtId KP=EvtPDL::getId("K+");
  static EvtId PIM=EvtPDL::getId("pi-");
  static EvtId PIP=EvtPDL::getId("pi+");
	
  std::vector<int> Evt_pattern_id;

  // Parent
  int parent_pdg_id = 0;
  EvtId parnum=getParentId();
  if (parnum == D0)
  {
    parent_pdg_id = 421;
  }
  else if (parnum == D0B)
  {
    parent_pdg_id = -421;
  }
  Evt_pattern_id.push_back(parent_pdg_id);
	 
  //daughters
  int dugh_pdg_id = 0;
  int num_pions =0;
  bool cab_fav=false; // Is this Cabibbo favoured? (K3pi only)
  
  for (int i=0; i<4; i++)
  {
    EvtId dugh_id = getDaug(i);
    if (dugh_id == KM)
    {
      dugh_pdg_id = -321;
      if (parnum == D0) {
        cab_fav=true;
      }
      else {
        cab_fav=false;
      }
    }
    else if (dugh_id == KP)
    {
      dugh_pdg_id = 321;
      if (parnum == D0) {
        cab_fav=false;
      }
      else {
        cab_fav=true;
      }
    }
    else if (dugh_id == PIM)
    {
      dugh_pdg_id = -211;
      num_pions ++;
    }
    else if (dugh_id == PIP)
    {
      dugh_pdg_id = 211;
      num_pions++;
    }
    Evt_pattern_id.push_back(dugh_pdg_id);
  }

  // Get Model number
  int modelNumber = getArg(0);
  std::stringstream ss;//create a stringstream
  ss << modelNumber;//add number to the stream
  //return a string with the contents of the stream
  std::string modelNumberStr = ss.str();

  // Get number of Pions in decay
  std::stringstream pions;//create a stringstream
  pions << num_pions;//add number to the stream
  //return a string with the contents of the stream
  std::string num_pionsStr = pions.str();

  // Input model file
  std::string InputTextFile="";

  // Set relative model filename, i.e. exlcuding the directory
  std::string modelFile="";
  if (num_pions == 4)
  {
    modelFile = "D_"+num_pionsStr+"pi_v"+modelNumberStr+".txt";
  }
  else if (num_pions == 3)
  {
    // determine whether this is the Cabibbo favoured or DCS
    if (cab_fav) {
      modelFile = "D_K"+num_pionsStr+"pi_v"+modelNumberStr+".txt";
    }
    else {
      modelFile = "D_piK2pi_v"+modelNumberStr+".txt";
    }
  }
  else if (num_pions == 2)
  {
    modelFile = "D_2K"+num_pionsStr+"pi_v"+modelNumberStr+".txt";
  }

	// Set Directories to look for model file
	std::vector<std::string> Directories;
	Directories.push_back("");
  // First look for the model in the DecFiles package
	if (System::getEnv( "DECFILESROOT" ) != "UNKNOWN")
	{
		Directories.push_back(System::getEnv( "DECFILESROOT" ) +"/MintData/");
	}
  // if the file is not in DecFiles, check in Mint
	if (System::getEnv( "MINTROOT" ) != "UNKNOWN")
	{
		Directories.push_back(System::getEnv( "MINTROOT" ) +"/Models/");
	}
  // finally, check in the current directory
  Directories.push_back(System::getEnv("PWD"));

	// Iterate over directories to look in
	std::vector<std::string>::iterator it;
	for (it = Directories.begin(); it != Directories.end(); it++)
	{
		ifstream inp;
		ofstream out;

    InputTextFile = *it + modelFile;

		// Check if file exists by attempting to open it
		inp.open(InputTextFile.c_str(), ifstream::in);
		inp.close();

		// If it does not exist look in next directory
		if(inp.fail())
		{
      InputTextFile="";
			inp.clear(std::ios::failbit);
		}
		else
		{
			break;
		}
	}

  if (InputTextFile.empty()) {
    report(ERROR,"EvtGen") 
      << "EvtDTohhhh::init:   Model file " << modelFile 
      << " not found in $DECFILESROOT/MintData, $MINTROOT/Models "
      "or current directory." << std::endl;
    ::abort();
  }

  std::stringstream msg;
  msg <<  "EvtDTohhhh::init was initialised with the following parameters:"
      << std::endl << "  Number of Pions " << num_pions << std::endl;
  if (num_pions==3) {
    msg << "  Cabibbo favoured? " << std::boolalpha << cab_fav << std::endl;
  } 
  msg << "  InputTextFile " << InputTextFile << std::endl;

	report(NOTICE,"EvtGen") << msg.str();
    
	m_MintGen->SetInputTextFile(InputTextFile);
  
	m_MintGen->Initalize(Evt_pattern_id, m_rnd);
}


void EvtDTohhhh::decay( EvtParticle *p){

  //p ->getId ();                              
  
  // Create Event in MINT
  std::vector<std::vector<double> > daughters;

  // create nested list of daughter 4 momenta
  daughters = m_MintGen->DecayEventRFVec();

  // Create daughters in EvtGen
  p->initializePhaseSpace(getNDaug(),getDaugs());

  //std::vector<int> daughsID;
  
  std::stringstream msg;
  msg << "In EvtDTohhhh::decay(EvtParticle* p):" << std::endl;
  msg << "    Mother four-momentum = " << p->getP4Lab() << std::endl;
  for (int u = 0; u < 4; u++)
  {

	  EvtVector4R daugmom(daughters[u][0],daughters[u][1],
                        daughters[u][2],daughters[u][3]);
  
	  // get daughter
	  EvtParticle* daugh = (p->getDaug(u));

    EvtVector4R dauP4Lab_orig = daugh->getP4Lab();
    
	  // Set daughter 4 momentum from MINT
	  daugh->setP4(daugmom);

    EvtVector4R dauP4Lab_new = daugh->getP4Lab();
    msg << "    Daughter " << u << ": original four-momentum = " 
        << dauP4Lab_orig
        << ", new four-momentum = " << dauP4Lab_new << std::endl;
  }
  report(DEBUG, "EvtGen") << msg.str();
  
  return ;
}

