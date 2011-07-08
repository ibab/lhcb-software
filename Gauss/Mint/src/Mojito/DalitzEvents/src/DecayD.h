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
#include <map>

//Event
//#include "Event/HepMCEvent.h"

//MINT
#include "SignalGenerator.h"
#include "IDalitzEvent.h"
//#include "NamedParameter.h"

//ROOT
#include "TVector3.h"



//#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"


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
	// Decay HepMC event in lab frame
	void DecayEvent();
	// Decay Event in parent Rest Frame
	//void DecayEventRF();
	std::map<int,TLorentzVector> DecayEventRF();


	void setMothers3Momentum();
	void SetMotherParticle(HepMC::GenParticle*);
	//const TLorentzVector getDaugtherMom(unsigned int i);
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

	void DecayWithBR(bool decayWithBR);

	double getWeight () const;

	double getGeneratorPdfRelativeToPhaseSpace () const;

private:
	HepMC::GenEvent* m_evt;
	std::string m_inputFileName;
	IDalitzEvent* m_dE;
	std::vector<int> m_pdgID; // Daughters PGD_ID
	int m_mumPdgID;
	bool m_decayWithBR;

	SignalGenerator* m_sg;

	HepMC::GenParticle* m_prt;

};

#endif /* DECAYD_H_ */
