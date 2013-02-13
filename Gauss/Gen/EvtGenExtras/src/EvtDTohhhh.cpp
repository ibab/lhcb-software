//--------------------------------------------------------------------------
//
//  D0 -> h h h h  with the MINT generator
//	Matt Coombes
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenModels/EvtDTohhhh.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtPDL.hh"

// Gaudi
#include "GaudiKernel/System.h"

// C++
#include <string>
#include <fstream>
#include <sstream>

// MINT
#include "Mint/IMintGen.h"
#include "Mint/D_hhhh.h"



using namespace std;
using namespace MINT;

EvtDTohhhh::~EvtDTohhhh() {}


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


	m_MintGen = new MintGen();

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
	for (int i=0; i<4; i++)
	{
		EvtId dugh_id = getDaug(i);
		if (dugh_id == KM)
		{
			dugh_pdg_id = -321;
		}
		else if (dugh_id == KP)
		{
			dugh_pdg_id = 321;
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

	// Input model file
	std::string InputTextFile;

	// Get Model number
	int modelNumber = getArg(0);
	stringstream ss;//create a stringstream
	ss << modelNumber;//add number to the stream
	string modelNumberStr = ss.str();//return a string with the contents of the stream

	// Get number of Pions in decay
	stringstream pions;//create a stringstream
	pions << num_pions;//add number to the stream
	string num_pionsStr = pions.str();//return a string with the contents of the stream

	// Set Directories to look for model file
	std::vector<std::string> Directories;
	Directories.push_back(""); // First look locally
	if (System::getEnv( "MINTROOT" ) != "UNKNOWN")
	{
		Directories.push_back(System::getEnv( "MINTROOT" ) +"/Models/");
	}
	if (System::getEnv( "DECFILESROOT" ) != "UNKNOWN")
	{
		Directories.push_back(System::getEnv( "DECFILESROOT" ) +"/MintFiles/");
	}

	// Iterated over directories to look in
	std::vector<std::string>::iterator it;
	for (it = Directories.begin(); it < Directories.end(); it++)
	{
		ifstream inp;
		ofstream out;

		if (num_pions == 4)
		{
			InputTextFile = *it + "D_"+num_pionsStr+"pi_v"+modelNumberStr+".txt";
		}
		else if (num_pions == 3)
		{
			InputTextFile = *it + "D_K"+num_pionsStr+"pi_v"+modelNumberStr+".txt";
		}
		else if (num_pions == 2)
		{
			InputTextFile = *it + "D_2K"+num_pionsStr+"pi_v"+modelNumberStr+".txt";
		}

		// Check if file exists by attempting to open it
		inp.open(InputTextFile.c_str(), ifstream::in);
		inp.close();
		// If it does not exist look in next directory
		if(inp.fail())
		{
			inp.clear(ios::failbit);
		}
		else
		{
			break;
		}
	}

	std::cout << "Number of Pions " << num_pions << std::endl;
	std::cout << "InputTextFile " << InputTextFile << std::endl;
	m_MintGen->SetInputTextFile(InputTextFile);

	m_MintGen->Initalize(Evt_pattern_id);
}


void EvtDTohhhh::decay( EvtParticle *p){

  p ->getId ();                              
  
  // Create Event in MINT
  std::vector<std::vector<double> > daughters;
  
  // create daughters map with pdgID and 4 momentum of particles
  daughters = m_MintGen->DecayEventRFVec();

  // Create daughters in EvtGen
  p->initializePhaseSpace(getNDaug(),getDaugs());

  std::vector<int> daughsID;
  
  for (int u = 0; u < 4; u++)
  {

	  EvtVector4R daugmom(daughters[u][0],daughters[u][1],daughters[u][2],daughters[u][3]);
//	  std::cout << " T: " << daughters[u][0] << " X: "  << daughters[u][1] <<
//	            	" Y: " << daughters[u][2] << " Z: " << daughters[u][3] << std::endl;

	  // get daughter
	  EvtParticle* daugh = (p->getDaug(u));

	  // Set daughter 4 momentum from MINT
	  daugh->setP4(daugmom);
  }

  return ;
}

