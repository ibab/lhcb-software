/*
 * MintGenerator.cpp
 *
 *  Created on: May 11, 2011
 *      Author: mcoombes
 */
//Local
#include "D_hhhh.h"

//C++
#include <iostream>

//ROOT
#include "TRandom3.h"

//MINT
#include "SignalGenerator.h"
#include "ParticlePropertiesList.h"

using namespace std;
using namespace MINT;



MintGen::MintGen()
{
	// TODO Auto-generated constructor stub
}

MintGen::~MintGen()
{
	// TODO Auto-generated destructor stub
}


void MintGen::SetInputTextFile(std::string inputFile)
{
	m_inputFileName = inputFile;
}


const TLorentzVector& MintGen::getDaugtherMom(unsigned int i)
{
	 const TLorentzVector& prt_mom = m_dE->p(1+i); //p0 is a D which we are not interested in here

	 return prt_mom;
}

void MintGen::SetDalitzEvent(IDalitzEvent* dE)
{
	m_dE = dE;
}



void MintGen::Initalize(std::vector<int> patternVec)
{
	//ParticlePropertiesList pl();
	//pl.fillDirList('/afs/cern.ch/user/m/mcoombes/cmtuser/Gauss_v40r0/Mint/');
	std::cout <<"m_inputFileName " << m_inputFileName << std::endl;
	NamedParameterBase::setDefaultInputFile(m_inputFileName);
	NamedParameter<int> RandomSeed("RandomSeed", 0);
	int RandSeed = RandomSeed;

	NamedParameter<int>  Nevents("Nevents", 1000);
	NamedParameter<double> integPrecision("IntegPrecision", 1.e-2);

	NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, -211, 211); // takes Pdg_id (421 is D0)
	std::vector<int> TextEvtpattern = EventPattern.getVector();
	DalitzEventPattern pdg_text(TextEvtpattern);

	//std::vector<int>
	DalitzEventPattern pdg(patternVec);
	//this->EvtPattern(patternVec);

//	NamedParameter<int> doNormCheck("doNormCheck", 0);

	std::cout << " got event pattern: " << pdg << std::endl;
	std::cout << " Text input event pattern: " << pdg_text << std::endl;

	m_sg = new SignalGenerator(pdg);
}

std::vector<TLorentzVector> MintGen::DecayEventRFVec()
{
	MINT::counted_ptr<IDalitzEvent> newEvt = m_sg->newEvent();

	IDalitzEvent* dE = newEvt.get();

	this->SetDalitzEvent(dE);

	std::vector<TLorentzVector> daughters;

	for (int i = 1; i < 5; i++)
	{
		daughters.push_back(m_dE->p(i));
	}
	return daughters;
}



