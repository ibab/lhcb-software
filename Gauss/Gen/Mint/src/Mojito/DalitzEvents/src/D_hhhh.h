/*
 * MintGen.h
 *
 *  Created on: May 11, 2011
 *      Author: mcoombes
 *
 *      Purpose: Interface between Gauss model and MINT
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

//C++
#include <string>
#include <iostream>
#include <vector>

//MINT
#include "SignalGenerator.h"
#include "IDalitzEvent.h"
#include "Mint/IMintGen.h"

//ROOT
#include "TVector3.h"

namespace MINT {
	class MintGen : virtual public IMintGen{
	public:
		MintGen();
		virtual ~MintGen();
		void SetInputTextFile(std::string inputFile);

		// Decay Event in parent Rest Frame
		std::vector<std::vector<double> > DecayEventRFVec();

		const TLorentzVector& getDaugtherMom(unsigned int i);

		//PGDID
		void SetDaughters_pgdID(std::vector<int> pgd_id);
		std::vector<int> GetDaughters_pgdID();
		void SetMothers_pgdID(std::vector<int>);
		int GetMothers_pgdID();

		//DalitzEvent

		void SetDalitzEvent(IDalitzEvent*);

		void Initalize(std::vector<int>);

	
	private:
		std::string m_inputFileName;
		IDalitzEvent* m_dE;
		SignalGenerator* m_sg;
	};
}

#endif /* MintGen_H_ */
