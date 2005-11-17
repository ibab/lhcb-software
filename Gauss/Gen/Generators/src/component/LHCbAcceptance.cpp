// $Id: LHCbAcceptance.cpp,v 1.2 2005-11-17 15:56:46 robbep Exp $
// Include files 

// local
#include "LHCbAcceptance.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from HepMC
#include "HepMC/GenParticle.h"

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<LHCbAcceptance>          s_factory ;
const        IToolFactory& LHCbAcceptanceFactory = s_factory ; 


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
                               const HepMC::GenEvent * theEvent ,
                               const HardInfo * theHardInfo ) const {
  ParticleVector::iterator it ;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) 
    if ( fabs( sin ( (*it) -> momentum().theta() ) ) > 
         fabs( sin( m_thetaMax ) ) ) 
      it = theParticleVector.erase( it ) ;
    else ++it ;
  
  return ( ! theParticleVector.empty() ) ;
}

