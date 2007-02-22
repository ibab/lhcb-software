// $Id: LHCbAcceptance.cpp,v 1.7 2007-02-22 13:30:24 robbep Exp $
// Include files 

// local
#include "LHCbAcceptance.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from HepMC
#include "HepMC/GenParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( LHCbAcceptance );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LHCbAcceptance::LHCbAcceptance( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ThetaMax" , m_thetaMax = 400 * mrad ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
LHCbAcceptance::~LHCbAcceptance( ) { ; }

//=============================================================================
// Acceptance function
//=============================================================================
bool LHCbAcceptance::applyCut( ParticleVector & theParticleVector ,
                               const HepMC::GenEvent * /* theGenEvent */ ,
                               const LHCb::GenCollision * /* col */ ) const {
  ParticleVector::iterator it ;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) 
    if ( fabs( sin ( (*it) -> momentum().theta() ) ) > 
         fabs( sin( m_thetaMax ) ) ) 
      it = theParticleVector.erase( it ) ;
    else ++it ;

  return ( ! theParticleVector.empty() ) ;
}

