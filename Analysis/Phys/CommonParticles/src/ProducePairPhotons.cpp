// $Id: ProducePairPhotons.cpp,v 1.2 2004-04-16 15:14:59 beneyton Exp $
// Include files
#include <string>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/Stat.h"

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// from Event
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Vertex.h"
#include "Event/VeloClusterOnStoredTrack.h"

// local
#include "ProducePairPhotons.h"

#define IFD if ( m_IsDebugOutputLevel )

//-----------------------------------------------------------------------------
// Implementation file for class : ProducePairPhotons
//
// 2003-01-16 : Richard Beneyton
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ProducePairPhotons>          s_factory ;
const        IAlgFactory& ProducePairPhotonsFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProducePairPhotons::ProducePairPhotons( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_veloChargeName( "TrVeloCharge" )
  , m_veloCharge(0)
  , m_ChangePID(false)
  , m_UsePID(true)
  , m_MinimalProbabilityCut(1.5)
{
	// Declare properties
	declareProperty( "TrVeloChargeName", m_veloChargeName);
	declareProperty( "ChangePID", m_ChangePID);
	declareProperty( "UsePID", m_UsePID);
	declareProperty( "MinimalProbabilityCut", m_MinimalProbabilityCut);
}

//=============================================================================
// Destructor
//=============================================================================
ProducePairPhotons::~ProducePairPhotons() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ProducePairPhotons::initialize() {

	StatusCode status;
	m_IsDebugOutputLevel = false;
	if ( msgSvc()->outputLevel(name()) < 3 ) { m_IsDebugOutputLevel = true; }
	MsgStream msg(msgSvc(), name());
	IFD msg << MSG::DEBUG << "==> Initialise" << endreq;

	// Load all necessary tools via base class
	status = loadTools();
	if( status.isFailure() ) {
		msg << MSG::ERROR << "   Unable to load tools" << endreq;
		return StatusCode::FAILURE;
	}

	// Access the TrVeloCharge Tool
	IFD msg << MSG::DEBUG << " Looking for TrVeloCharge Tool called " << m_veloChargeName << "." << endreq;
	status = toolSvc()->retrieveTool(m_veloChargeName, m_veloCharge, this);
	if( status.isFailure() ) {
		msg << MSG::FATAL << " Unable to locate TrVeloCharge Tool called " << m_veloChargeName << "." << endreq;
		return status;
	}

	m_electronID = (ppSvc()->find("e-"))->jetsetID();
	msg << MSG::INFO << "ElectronID:" << m_electronID << endreq;
	m_electronParticleID = ParticleID(m_electronID);

	m_positronID = (ppSvc()->find("e+"))->jetsetID();
	msg << MSG::INFO << "PositronID:" << m_positronID << endreq;
	m_positronParticleID = ParticleID(m_positronID);

	m_electronMass = (ppSvc()->find("e-"))->mass();
	msg << MSG::INFO << "ElectronMass:" << m_electronMass << endreq;

	m_gammaID = (ppSvc()->find("gamma"))->jetsetID();
	msg << MSG::INFO << "GammaID:" << m_gammaID << endreq;
	m_gammaParticleID = ParticleID(m_gammaID);

	IFD msg << MSG::DEBUG << "    done" << endreq;

	// Initialization terminated
	return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProducePairPhotons::execute() {

	StatusCode status;
	MsgStream  msg( msgSvc(), name() );
	IFD msg << MSG::DEBUG << "==> Execute" << endreq;

	int Nreconstructed = 0;
	bool reconstructed = false;
//=============================================================================
// Get all created Particle and create gammas
//=============================================================================
	status = desktop()->getInput();
	if (!status) {
		msg << MSG::ERROR << "    not able to fill PhysDesktop " << endreq;
		return StatusCode::FAILURE;
	}
	const ParticleVector& parts = desktop()->particles();
	ParticleVector tmpparts(parts.begin(), parts.end());
	IFD msg << MSG::DEBUG << "found " << tmpparts.size() << " particles" << endreq;

	typedef std::vector<const VeloCluster*> Velo;
	typedef std::pair< Particle* , Velo > PartVelo;
	typedef std::vector< PartVelo > PartVelos;
	PartVelos partVelos;

	Particles::const_iterator icandidate;
	for ( icandidate = tmpparts.begin() ; icandidate != tmpparts.end() ;icandidate++ ) {
		Particle* candidate = *icandidate;
		if ( m_UsePID ) {
			if ( ( candidate->particleID().pid() != m_electronID ) 
				&& ( candidate->particleID().pid() != m_positronID ) ) continue;
		}
		PartVelo partVelo;
		partVelo.first = candidate;
		const ProtoParticle* proto = dynamic_cast< ProtoParticle* > (candidate->origin());
		if ( proto == 0 ) {continue;}
		const TrStoredTrack* track = proto->track();
		if ( track == 0 ) {continue;}
		partVelo.second = m_veloCharge->getVelos(track);
		partVelos.push_back(partVelo);
	}

	typedef std::pair< Particle* , bool > CandidateParticle;
	typedef std::pair< Velo , std::vector<CandidateParticle> > VeloCand;
	typedef std::vector< VeloCand > VeloCands;
	VeloCands veloCands;

	PartVelos::const_iterator ipartVelo;
	for ( ipartVelo = partVelos.begin() ; ipartVelo != partVelos.end() ; ipartVelo++ ) {
		PartVelo partVelo = *ipartVelo;
		PartVelos::const_iterator i2partVelo;
		for ( i2partVelo = ipartVelo+1 ; i2partVelo != partVelos.end() ; i2partVelo++ ) {
			PartVelo partVelo2 = *i2partVelo;
			if ( partVelo2.second == partVelo.second ) {
				msg << MSG::DEBUG << "  match " << endreq;
				bool alreadystored = false;
				VeloCands::iterator iveloCand;
				for ( iveloCand = veloCands.begin() ; iveloCand != veloCands.end() ; iveloCand++ ) {
					if ( partVelo.second == iveloCand->first ) {
						bool have1 = false;
						bool have2 = false;
						std::vector<CandidateParticle>::const_iterator icandparticle;
						for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
							Particle* particle = (*icandparticle).first;
							if ( particle == partVelo.first ) {have1 = true;}
							if ( particle == partVelo2.first ) {have2 = true;}
						}
						if ( !have1 ) {
							CandidateParticle cand;
							cand.first = partVelo.first;
							cand.second = true;
							iveloCand->second.push_back(cand);
						}
						if ( !have2 ) {
							CandidateParticle cand;
							cand.first = partVelo2.first;
							cand.second = true;
							iveloCand->second.push_back(cand);
						}
						alreadystored = true;
						msg << MSG::DEBUG << "  found... have1:" << have1 << " have2:" << have2 << endreq;
					}
				}
				if ( !alreadystored ) {
					VeloCand veloCand;
					veloCand.first = partVelo.second;
					CandidateParticle cand;
					cand.first = partVelo.first;
					cand.second = true;
					veloCand.second.push_back(cand);
					CandidateParticle cand2;
					cand2.first = partVelo2.first;
					cand2.second = true;
					veloCand.second.push_back(cand2);
					veloCands.push_back(veloCand);
					msg << MSG::DEBUG << "  added... " << endreq;
				}
			}
		}
	}

	msg << MSG::DEBUG << " found " << veloCands.size() << " velocands" << endreq;

	VeloCands::iterator iveloCand;
	for ( iveloCand = veloCands.begin() ; iveloCand != veloCands.end() ; iveloCand++ ) {
		unsigned int nPart = iveloCand->second.size();
		msg << MSG::DEBUG << " velocand... with " << nPart << " particules" << endreq;
		// count number of particle with positive or negativ charge
		// and find minimal chi2 of track...
		int nElectron = 0;
		int nPositron = 0;
		double minimalChi2Electron = 10000.;
		double minimalChi2Positron = 10000.;
		std::vector<CandidateParticle>::iterator icandparticle;
		for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
			Particle* particle = (*icandparticle).first;
			const ProtoParticle* proto = dynamic_cast< ProtoParticle* > (particle->origin());
			if ( proto == 0 ) {continue;}
			const TrStoredTrack* track = proto->track();
			if ( track == 0 ) {continue;}
			double chi2 = track->lastChiSq();
			if ( particle->charge() == -1 ) {
				nElectron++;
				if ( chi2 < minimalChi2Electron ) {minimalChi2Electron=chi2;}
			}
			if ( particle->charge() == +1 ) {
				nPositron++;
				if ( chi2 < minimalChi2Positron ) {minimalChi2Positron=chi2;}
			}
		}
		msg << MSG::DEBUG << " nElectron:" << nElectron << endreq;
		if ( nElectron < 1 ) {continue;}
		msg << MSG::DEBUG << " nPositron:" << nPositron << endreq;
		if ( nPositron < 1 ) {continue;}
		msg << MSG::DEBUG << " minimalChi2Electron:" << minimalChi2Electron << endreq;
		msg << MSG::DEBUG << " minimalChi2Positron:" << minimalChi2Positron << endreq;

		// select only track with minimal chi2
		for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
			Particle* particle = (*icandparticle).first;
			const ProtoParticle* proto = dynamic_cast< ProtoParticle* > (particle->origin());
			if ( proto == 0 ) {continue;}
			const TrStoredTrack* track = proto->track();
			if ( track == 0 ) {continue;}
			double chi2 = track->lastChiSq();
			if ( ( particle->charge() == -1 ) && ( chi2 > minimalChi2Electron ) ) {(*icandparticle).second = false;}
			if ( ( particle->charge() == +1 ) && ( chi2 > minimalChi2Positron ) ) {(*icandparticle).second = false;}
		}

		//recheck number of tracks selected...
		int nSelected = 0;
		for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
			if ( (*icandparticle).second ) {nSelected++;}
		}
		msg << MSG::DEBUG << " nSelected:" << nSelected << endreq;
		if ( nSelected != 2 ) {
			// select only particle with good pid (same track and chi2 but different pid)
			int nElectron = 0;
			int nPositron = 0;
			for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
				Particle* particle = (*icandparticle).first;
				if ( particle->charge() == -1 ) {nElectron++;}
				if ( particle->charge() == +1 ) {nPositron++;}
			}
			if ( nElectron >1 ) {
				for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
					Particle* particle = (*icandparticle).first;
					if ( particle->charge() == -1 ) {
						if ( particle->particleID().pid() != m_electronID ) { 
							(*icandparticle).second = false;
						}
					}
				}
			}
			if ( nPositron >1 ) {
				for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
					Particle* particle = (*icandparticle).first;
					if ( particle->charge() == +1 ) {
						if ( particle->particleID().pid() != m_positronID ) { 
							(*icandparticle).second = false;
						}
					}
				}
			}
			int modifiednSelected = 0;
			for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
				if ( (*icandparticle).second ) {modifiednSelected++;}
			}
			if ( modifiednSelected !=2 ) {continue;}
		}

		double charge = m_veloCharge->calculate(iveloCand->first);
		if ( charge <= m_MinimalProbabilityCut ) {
			msg << MSG::DEBUG << " velo cand not saved, cause TrVeloCharge gives:" << charge << endreq;
		}
		if ( charge > m_MinimalProbabilityCut ) {
			msg << MSG::DEBUG << " all OK...  TrVeloCharge gives:" << charge << endreq;
			// creating gamma tree

			// create "symbolink link" to electrons
			Particle* electron = 0;
			Particle* positron = 0;
			std::vector<CandidateParticle>::const_iterator icandparticle;
			for ( icandparticle = iveloCand->second.begin() ; icandparticle != iveloCand->second.end() ; icandparticle++ ) {
				if ( (*icandparticle).second ) {
					Particle* particle = (*icandparticle).first;
					if ( particle->charge() == -1) electron = particle;
					if ( particle->charge() == +1) positron = particle;
				}
			}

			// we can force ID, and update mass in momentum
			if ( m_ChangePID ) {
				const ProtoParticle* pelectron = dynamic_cast< ProtoParticle* > (electron->origin());
				if ( pelectron == 0 ) {continue;}
				const TrStateP* tselectron = pelectron->trStateP();
				if( !tselectron ) {continue;}
				const double electronmass = m_electronMass;
				HepLorentzVector electronquadriMomentum;
				double electronmomentum = tselectron->p();
				double electronslopeX   = tselectron->tx();
				double electronslopeY   = tselectron->ty();
				double electronpZ = electronmomentum/sqrt( 1.0 + electronslopeX*electronslopeX + electronslopeY*electronslopeY );
				electronquadriMomentum.setPx( electronpZ*electronslopeX );
				electronquadriMomentum.setPy( electronpZ*electronslopeY );
				electronquadriMomentum.setPz( electronpZ );
				electronquadriMomentum.setE( sqrt( electronmass*electronmass + electronmomentum*electronmomentum ) );
				// covariance is still the same
				electron->setParticleID(m_electronParticleID);
				electron->setConfLevel(1.);
				electron->setIsResonance(false);
				electron->setMass(electronmass);
				electron->setMassErr(0.);
				electron->setMomentum( electronquadriMomentum );

				const ProtoParticle* ppositron = dynamic_cast< ProtoParticle* > (positron->origin());
				if ( ppositron == 0 ) {continue;}
				const TrStateP* tspositron = ppositron->trStateP();
				if( !tspositron ) {continue;}
				const double positronmass = m_electronMass;
				HepLorentzVector positronquadriMomentum;
				double positronmomentum = tspositron->p();
				double positronslopeX   = tspositron->tx();
				double positronslopeY   = tspositron->ty();
				double positronpZ = positronmomentum/sqrt( 1.0 + positronslopeX*positronslopeX + positronslopeY*positronslopeY );
				positronquadriMomentum.setPx( positronpZ*positronslopeX );
				positronquadriMomentum.setPy( positronpZ*positronslopeY );
				positronquadriMomentum.setPz( positronpZ );
				positronquadriMomentum.setE( sqrt( positronmass*positronmass + positronmomentum*positronmomentum ) );
				// covariance is still the same
				positron->setParticleID(m_positronParticleID);
				positron->setConfLevel(1.);
				positron->setIsResonance(false);
				positron->setMass(positronmass);
				positron->setMassErr(0.);
				positron->setMomentum( positronquadriMomentum );
			}

			// now do our gamma
			Vertex gammaVtx;
			status = vertexFitter()->fitVertex(*electron,*positron,gammaVtx);
			if( !status.isSuccess() ) {continue;}
			msg << MSG::DEBUG << " VertexFitter OK, chi2:" << format("%6.2f",gammaVtx.chi2()) << endreq;

			Particle gamma;
			status = particleStuffer()->fillParticle( gammaVtx, gamma, m_gammaID );
			if( !status.isSuccess() ) {
				msg << MSG::DEBUG << " particleStuffer()->fillParticle failed" << endreq;
				continue;
			}
			// we are sure that it's a gamma (normaly...)
			gamma.setConfLevel(1.);
			gamma.setMass(0.);
			gamma.setMassErr(0.);

			// all finished, we add gamma (and so other) in desktop
			Particle* pInDesktop = desktop()->createParticle(&gamma);
			if( !pInDesktop) {
				msg << MSG::DEBUG << " desktop()->createParticle failed" << endreq;
				continue;
			}
			Nreconstructed++;
			reconstructed = true;
		}
	}

	Stat StatSeen(chronoSvc(),name()+"#N",Nreconstructed);
	if ( reconstructed ) Stat StatSeen(chronoSvc(),name()+"#evts",1);

	//status = desktop()->saveDesktop();
	status = desktop()->saveTrees(m_gammaID);
	if( status.isFailure() ) {
		msg << MSG::ERROR << "   Unable to save Desktop" << endreq;
		return StatusCode::FAILURE;
	}

	// End of execution for each event
	return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ProducePairPhotons::finalize() {

  MsgStream msg(msgSvc(), name());
  IFD msg << MSG::DEBUG << "==> Finalize" << endreq;

  // End of finalization step
  return StatusCode::SUCCESS;
}
//=============================================================================
