/*
 * DecayD.C
 *
 *  Created on: May 11, 2011
 *      Author: mcoombes
 */
//Local
#include "DecayD.h"

//C++
#include <iostream>
#include <map>

//#include "HepLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/SimpleVector.h"

//ROOT
#include "TRandom3.h"

//MINT
#include "SignalGenerator.h"
#include "ParticlePropertiesList.h"

using namespace std;
using namespace CLHEP;
using namespace HepMC;
using namespace MINT;

/// Convert from HepMC::ThreeVector to CLHEP::Hep3Vector
inline CLHEP::Hep3Vector convertTo( const HepMC::ThreeVector& v )
     { return CLHEP::Hep3Vector( v.x(), v.y(), v.z() ); }

/// Convert from HepMC::FourVector to CLHEP::HepLorentzVector
inline CLHEP::HepLorentzVector convertTo( const HepMC::FourVector& v )
     { return CLHEP::HepLorentzVector( v.x(), v.y(), v.z(), v.t() );
     }


DecayD::DecayD()
{
	// TODO Auto-generated constructor stub

}

DecayD::DecayD(HepMC::GenEvent* evt)
	:m_evt(evt)
{
	// TODO Auto-generated constructor stub
}

DecayD::~DecayD() {
	// TODO Auto-generated destructor stub
}


HepMC::GenEvent* DecayD::OutputHepMC()
{
	return m_evt;
}

void DecayD::SetInputTextFile(std::string inputFile)
{
	m_inputFileName = inputFile;
}

void DecayD::SetMotherParticle(HepMC::GenParticle* prt)
{
	m_prt = prt;
}

void DecayD::setMothers3Momentum()
{
	HepLorentzVector Mum_mom;
	Mum_mom = convertTo( m_prt->momentum() );
	double px = Mum_mom.px();
	double py = Mum_mom.py();
	double pz = Mum_mom.pz();
	TVector3 mum3mom(px,py,pz);
	m_dE->setMothers3Momentum(mum3mom);
}
const TLorentzVector& DecayD::getDaugtherMom(unsigned int i)
{
	 const TLorentzVector& prt_mom = m_dE->p(1+i); //p0 is a D which we are not interested in here

	 return prt_mom;
}



void DecayD::setdecayOutput()
{
	//Add new output Vertex to the Event
	GenVertex* v1 = new GenVertex();
	m_evt->add_vertex( v1 );

	std::cout << "Set decay Input particle to Vertex"<< std::endl;
	m_prt->set_status(1);
	m_prt->print();
	//Add D as input particle to the decay Vertex

	v1->add_particle_in(m_prt);
	v1->print();
	// Loop over output particles and add them to the event
	for (int outPrt = 0; outPrt<4; outPrt++)
	{
		//Get Particle momentum
		const TLorentzVector& prt_mom = getDaugtherMom(outPrt);

		// Add particle to Vertex
		v1->add_particle_out( new GenParticle( HepLorentzVector(prt_mom[0],prt_mom[1],prt_mom[2],prt_mom[3]), m_pdgID[outPrt], 209 ) );
		// Remember if getting pdg_id default pdg_id[0] is D not wanted here.
	}
}

void DecayD::SetDaughters_pgdID(std::vector<int> pgd_id)
{
	pgd_id.erase (pgd_id.begin()); //erase first element (D pdg_ID)
	m_pdgID = pgd_id;
}

std::vector<int> DecayD::GetDaughters_pgdID()
{
	return m_pdgID;
}

void DecayD::SetMothers_pgdID(std::vector<int> pgd_id)
{
	m_mumPdgID = pgd_id[0];
}
int DecayD::GetMothers_pgdID()
{
	return m_mumPdgID;
}

void DecayD::SetDalitzEvent(IDalitzEvent* dE)
{
	m_dE = dE;
}


void DecayD::Initalize()
{
	//ParticlePropertiesList pl();
	//pl.fillDirList('/afs/cern.ch/user/m/mcoombes/cmtuser/Gauss_v40r0/Mint/');
	std::cout <<"m_inputFileName " << m_inputFileName << std::endl;
	MINT::NamedParameterBase::setDefaultInputFile(m_inputFileName);
	NamedParameter<int> RandomSeed("RandomSeed", 0);
	int RandSeed = RandomSeed;

	NamedParameter<std::string> InputFileName("InputFileName", (std::string) "");
	bool generateNew = (std::string) InputFileName == "";



	NamedParameter<std::string> IntegratorEventFile("IntegratorEventFile"
											, (std::string) "SignalIntegrationEvents.root"
											, (char*) 0);
	NamedParameter<std::string> IntegratorInputFile("IntegratorInputFile"
											, (std::string) "sgIntegrator"
											, (char*) 0);
	NamedParameter<int>  Nevents("Nevents", 2000);
	NamedParameter<int>  doScan("doScan", 0);
	NamedParameter<double> integPrecision("IntegPrecision", 1.e-2);

	NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211); // takes Pdg_id (421 is D0)

	DalitzEventPattern pdg(EventPattern.getVector());

	NamedParameter<int> doNormCheck("doNormCheck", 0);

	std::cout << " got event pattern: " << pdg << std::endl;

	std::vector<int> prt_pdgID = EventPattern.getVector();

	SetDaughters_pgdID(prt_pdgID);
	SetMothers_pgdID(prt_pdgID);

	//SignalGenerator sg(pdg);
	m_sg = new SignalGenerator(pdg);
}

void DecayD::DecayWithBR(bool decayWithBR)
{
	m_decayWithBR = decayWithBR;
}

void DecayD::DecayEvent()
{

	for ( GenEvent::particle_const_iterator p = m_evt->particles_begin();
			 p != m_evt->particles_end(); ++p )
	{
		if( (*p)->status() == 1 )
		{
			//std::cout << "pdg_id: " << (*p)->pdg_id() << std::endl;
			int pdg_id = (*p)->pdg_id();
		//	(*p)->print();
			if (m_mumPdgID == pdg_id) // If particle is a D
			{
				std::cout << "Found D" << std::endl;
				std::cout << "Decay Particle" << std::endl;

				// If only Decay correct Branching Ratio of events
				if (m_decayWithBR==true)
				{
					TRandom3* RandBR = new TRandom3(12);
					double RandBRNum = RandBR->Rndm(0);
					NamedParameter<double> BR("BR", 1.0);

					// If the random number is larger than the Branching ratio
					if (RandBRNum > BR)
					{
						std::cout<<"Do nothing as less than Branching Ratio"<<std::endl;
						continue;
					}

				}
				this->SetMotherParticle(*p);

			//	m_evt->print();
				MINT::counted_ptr<IDalitzEvent> newEvt = m_sg->newEvent();
				IDalitzEvent* dE = newEvt.get();
				this->SetDalitzEvent(dE);

				this->setMothers3Momentum();

				std::cout << "Mothers 3 mom Set "<< std::endl;

				this->setdecayOutput();
				std::cout << "Decay output Set "<< std::endl;
			}
		}
	}
}

void DecayD::DecayOnlyDtoK3PiEvent()
{
//	std::cout << "Loop over particles" << std::endl;

	for ( GenEvent::particle_const_iterator p = m_evt->particles_begin();
			 p != m_evt->particles_end(); ++p )
	{
		// Get particles pdg_id
		int pdg_id = (*p)->pdg_id();

	//	std::cout << "If particle is a D" << std::endl;

		if (m_mumPdgID == pdg_id) // If particle is a D
		{
			std::cout << "Found D get vertex" << std::endl;

			// Get Decay Vertex
			GenVertex* v1 = (*p)->end_vertex();

			(*p)->print();

			if (v1==0)
			{
				m_evt->print();

				std::cout << " No decay Vertex found" << std::endl;
				continue;
			}

			std::cout << "Print Vertex" << std::endl;

			v1->print();

			std::cout << "Found vertex" << std::endl;

			//std::cout << "Particles in " << v1->particles_in_size() << std::endl;

			// Check 1 input particle and 4 output particles
			if (v1->particles_in_size() == 1
				&& v1->particles_out_size() == 4)
			{
				std::cout << "D vertex has 1 in particle and 4 out" << std::endl;

				std::vector<int> k3PI_pdg = m_pdgID;
				std::vector<int>::iterator k3PI_pdg_it;

				// iterate over output particles
				for ( GenVertex::particles_out_const_iterator vout = v1->particles_in_const_begin() ; vout != v1->particles_in_const_end(); ++vout)
				{
					std::cout << "Output paticles" << std::endl;

					// Iterate over K3PI output particles
					for ( k3PI_pdg_it=k3PI_pdg.begin(); k3PI_pdg_it != k3PI_pdg.end(); ++k3PI_pdg_it)
					{
						//Match an output particle to one of the desired output particles
						if ((*vout)->pdg_id() == (*k3PI_pdg_it))
						{
							k3PI_pdg.erase(k3PI_pdg_it); // Remove the found particle from list
							std::cout << "Found a K or pi output paticle" << std::endl;

							break;
						}
					}
				}


				if (k3PI_pdg.size() == 0) // If all final state particles are found
				{
					std::cout << "Vector Size is Zero" << std::endl;

					this->SetMotherParticle(*p);

				//	m_evt->print();
					MINT::counted_ptr<IDalitzEvent> newEvt = m_sg->newEvent();
					IDalitzEvent* dE = newEvt.get();
					this->SetDalitzEvent(dE);

					this->setMothers3Momentum();

					// Override decay vertex
					// iterate over output particles
					for ( GenVertex::particles_out_const_iterator vout = v1->particles_in_const_begin() ; vout != v1->particles_in_const_end(); ++vout)
					{
						int counter;
						// Iterate over K3PI output particles
						for ( k3PI_pdg_it=m_pdgID.begin(); k3PI_pdg_it != m_pdgID.end(); ++k3PI_pdg_it)
						{
							//Match an output particle to one of the desired output particles
							if ((*vout)->pdg_id() == (*k3PI_pdg_it))
							{
								// Set momentum of out put pions and kaons
								const TLorentzVector& prt_mom = getDaugtherMom(counter);
								FourVector prt_out(prt_mom[0],prt_mom[1],prt_mom[2],prt_mom[3]);
								(*vout)->set_momentum(prt_out);
							}
							counter++;
						}
					}

				}
			}
		}
	}
}


//void DecayD::DecayEventRF()
map<int,TLorentzVector> DecayD::DecayEventRF()
{
			//	m_evt->print();
			MINT::counted_ptr<IDalitzEvent> newEvt = m_sg->newEvent();
			IDalitzEvent* dE = newEvt.get();
			this->SetDalitzEvent(dE);
//
//			const TLorentzVector prt_mom = dE->p(1);
//			std::cout << "Daughters Mom "<< prt_mom << std::endl;
//
//			const TLorentzVector prt_mom1 = m_dE->p(1);
//			std::cout << "Daughters Mom "<< prt_mom1 << std::endl;
//			std::cout << "Decay output Set "<< std::endl;

			std::vector<int> dught_PID = this->GetDaughters_pgdID();


			map<int,TLorentzVector> daughters;

			int counter = 0;
			for (int i = 0; i < 4; i++)
			{
				if (dught_PID[i] == 211)
				{
					if (counter == 1)
					{
						dught_PID[i] = 21100;
					}
					counter++;
				}
				daughters[dught_PID[i]] = m_dE->p(i+1);
			}

			return daughters;
}






