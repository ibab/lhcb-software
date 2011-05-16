/*
 * DecayD.h
 *
 *  Created on: May 11, 2011
 *      Author: mcoombes
 */

#ifndef DECAYD_H_
#define DECAYD_H_

//C++
#include <string>
#include <iostream>
#include <vector>

//Event
//#include "Event/HepMCEvent.h"

//MINT
#include "NamedParameter.h"
#include "SignalGenerator.h"
#include "IDalitzEvent.h"

//ROOT
#include "TVector3.h"



//#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/GenEvent.h"
//#include "Gen/HepMC.h"

using namespace MINT;
using namespace HepMC;

class DecayD {
public:
	DecayD();
	DecayD(HepMC::GenEvent*);
	virtual ~DecayD();
	HepMC::GenEvent* OutputHepMC();
	void SetInputTextFile(std::string inputFile);
	void DecayEvent();
	void setMothers3Momentum();
	void SetMotherParticle(HepMC::GenParticle*);
	const TLorentzVector& getDaugtherMom(unsigned int i);

	void setdecayOutput();

	//PGDID
	void SetDaughters_pgdID(std::vector<int> pgd_id);
	std::vector<int> GetDaughters_pgdID();
	void SetMothers_pgdID(std::vector<int>);
	int GetMothers_pgdID();

	//DalitzEvent

	void SetDalitzEvent(IDalitzEvent*);

	//HEPInputEvent
	void SetInputEventHep(HepMC::GenEvent* evt)
	{
		//std::cout << "Set input HeP Event" << std::endl;
		m_evt = evt;
	}

	void Initalize();

	void DecayOnlyDtoK3PiEvent();
private:
	HepMC::GenEvent* m_evt;
	std::string m_inputFileName;
	IDalitzEvent* m_dE;
	std::vector<int> m_pdgID; // Daughters PGD_ID
	int m_mumPdgID;

	SignalGenerator* m_sg;

	HepMC::GenParticle* m_prt;

};

#endif /* DECAYD_H_ */
