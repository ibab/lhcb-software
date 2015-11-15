// $Id: Trajectory2Particle.cpp,v 1.12 2007-10-02 13:14:51 gcorti Exp $

// Include files

// from STD
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/MsgStream.h"

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Event/MCParticle.h"
#include "Event/HepMCEvent.h"

// from GiGa
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaException.h"
// from G4
#include "G4ParticleDefinition.hh"

// local
#include "Trajectory2Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Trajectory2Particle
//
// 2001-07-22 : Ivan Belyeav
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor
//=============================================================================
GiGaCnvFunctors::Trajectory2Particle::Trajectory2Particle
( LHCb::IParticlePropertySvc* Svc )
  : m_ppSvc ( Svc   )
{ if( 0 != ppSvc() ) { ppSvc()->addRef() ; } }


//=============================================================================
// Destructor
//=============================================================================
GiGaCnvFunctors::Trajectory2Particle::~Trajectory2Particle()
{ if( 0 != ppSvc() ) { ppSvc()->release(); m_ppSvc = 0 ;} }


//=============================================================================
// copy constructor
//=============================================================================
GiGaCnvFunctors::Trajectory2Particle::
Trajectory2Particle( const Trajectory2Particle& right )
  : m_ppSvc( right.ppSvc() )
{ if( 0 != ppSvc() ) { ppSvc()->addRef() ; } }


//=============================================================================
// perform the conversion of G4VTrajectory object to
//=============================================================================
static const std::string
ErrMsg1("GiGaCnv::Trajectory2Particle::IParticlePropertySvc* points to NULL!");
static const std::string
ErrMsg2("GiGaCnv::Trajectory2Particle::GiGaTrajectory* points to NULL!");
static const std::string
ErrMsg3("GiGaCnv::Trajectory2Particle::G4ParticleDefinition* points to NULL!");
static const std::string
ErrMsg4("GiGaCnv::Trajectory2Particle::ParticleProperty* points to NULL for ");

//=============================================================================
LHCb::MCParticle*
GiGaCnvFunctors::Trajectory2Particle::operator()
  ( const GiGaTrajectory* trajectory ) const
{
  if( 0 == ppSvc ()   ) { throw GiGaException( ErrMsg1 ) ; }
  if( 0 == trajectory ) { throw GiGaException( ErrMsg2 ) ; }

  const G4ParticleDefinition* pDef = trajectory->partDef() ;
  if( 0 == pDef       ) { throw GiGaException( ErrMsg3 );}

  // create and fill new MCparticle object
  LHCb::MCParticle* particle = new LHCb::MCParticle();
  // if it is a resonance, take generator properties instead of
  // G4Trajectories information (for broad resonances)
  if ( ( pDef -> IsShortLived() ) && ( 0 != trajectory -> pHepMCEvent() ) ) {
    HepMC::GenEvent    * gEvt = trajectory -> pHepMCEvent() -> pGenEvt() ;
    HepMC::GenParticle * gPart =
      gEvt -> barcode_to_particle( trajectory -> signalBarcode() ) ;
    CLHEP::HepLorentzVector theFMom( gPart -> momentum().px() ,
                                     gPart -> momentum().py() ,
                                     gPart -> momentum().pz() ,
                                     gPart -> momentum().e () ) ;
    Gaudi::LorentzVector momentum( theFMom );
    particle -> setMomentum( momentum ) ;
  }
  else {
    Gaudi::LorentzVector momentum( trajectory->fourMomentum() );
    particle->setMomentum( momentum ) ;
  }

  // the oscillation flag is no longer in the particle but in the
  // vertex set it there!
  // particle->setHasOscillated( trajectory->hasOscillated()          ) ;
  // ions have zero as pdg encoding
  int pdgID = pDef->GetPDGEncoding();
  if( 0 == pdgID ) {
    const LHCb::ParticleProperty* pProp = ppSvc()->find( pDef->GetParticleName() );
    if( NULL != pProp ) {
      pdgID = (pProp->pid()).pid();
    }
  }

  particle->setParticleID( LHCb::ParticleID( pdgID ) );
  return particle;
  ///
}

//=============================================================================
