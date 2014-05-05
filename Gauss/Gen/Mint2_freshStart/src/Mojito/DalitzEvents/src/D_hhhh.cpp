/*
 * MintGenerator.cpp
 *
 *  Created on: May 11, 2011
 *      Author: mcoombes
 */
//Local
#include "Mint/D_hhhh.h"

//C++
#include <iostream>

//ROOT
//#include "TRandom3.h"
#include "TRandom.h"
#include "TVector3.h"

//MINT
#include "Mint/SignalGenerator.h"
#include "Mint/ParticlePropertiesList.h"

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


std::vector<double> MintGen::getDaughterMom(IDalitzEvent* dE, int daughter)
{
	std::vector<double> DaughterMom;
	DaughterMom.push_back(dE->p(daughter).T()/1000);
	DaughterMom.push_back(dE->p(daughter).X()/1000);
	DaughterMom.push_back(dE->p(daughter).Y()/1000);
	DaughterMom.push_back(dE->p(daughter).Z()/1000);

	return DaughterMom;
}


void MintGen::Initalize(const std::vector<int> &patternVec, TRandom* rnd)
{
	m_swap = false;
	std::cout <<"m_inputFileName " << m_inputFileName << std::endl;
	NamedParameterBase::setDefaultInputFile(m_inputFileName);


	NamedParameter<int> RandomSeed("RandomSeed", 0);
	std::cout << " Random Seed " << RandomSeed << std::endl;


	NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
	NamedParameter<std::string> integMethod("IntegMethod"
						  , (std::string)"efficient");


	NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, -211, 211); // takes Pdg_id (421 is D0)
	std::vector<int> TextEvtpattern = EventPattern.getVector();
//	DalitzEventPattern pdg_text(TextEvtpattern);
	DalitzEventPattern pdg(patternVec);
//	DalitzEventPattern pdg(EventPattern.getVector());

	// some initialisation
//	TRandom3 ranLux;
//	ranLux.SetSeed((int)RandomSeed);
	std::cout << " got event pattern: " << pdg << std::endl;

	if (patternVec[0] < 0)
	{
		m_swap = true;
		cout << "Mother Particle " <<patternVec[0] << std::endl;
		pdg = pdg.makeCPConjugate();
	}

	std::cout << " got event pattern: " << pdg << std::endl;


//	std::cout << " Text input event pattern: " << pdg_text << std::endl;

	m_sg = new SignalGenerator(pdg, rnd);
}

std::vector<std::vector<double> > MintGen::DecayEventRFVec()
{
	MINT::counted_ptr<IDalitzEvent> newEvt = m_sg->newEvent();


	IDalitzEvent* dE = newEvt.get();
  
	std::vector<std::vector<double> > daughters;

	// Loop over 4 daughters
	// Start at 1 as 0 is the mother particles
	for (int i = 1; i < 5; i++)
	{
		std::vector<double> DaughterMom = this->getDaughterMom(dE,i);
		daughters.push_back(DaughterMom);
	}
	return daughters;
}
