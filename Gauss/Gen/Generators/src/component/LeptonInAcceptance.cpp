// $Id: LeptonInAcceptance.cpp,v 1.1 2005-11-29 15:43:17 robbep Exp $
// Include files 
// local
#include "LeptonInAcceptance.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/HepMCEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LeptonInAcceptance
//
// 2005-11-21 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<LeptonInAcceptance>          s_factory ;
const        IToolFactory& LeptonInAcceptanceFactory = s_factory ;

// Function to test if particle is a lepton
bool isLepton( const HepMC::GenParticle * p ) {
  return ( ( 13 == abs( p -> pdg_id() ) ) || ( 11 == abs( p -> pdg_id() ) ) ) ;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LeptonInAcceptance::LeptonInAcceptance( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IFullGenEventCutTool >( this ) ;
    declareProperty( "ThetaMax" , m_thetaMax = 400 * mrad ) ;
    declareProperty( "PtMin" , m_ptMin = 4. * GeV ) ;
}
//=============================================================================
// Destructor
//=============================================================================
LeptonInAcceptance::~LeptonInAcceptance( ) { ; }
//=============================================================================
// Selection function
//=============================================================================
bool LeptonInAcceptance::studyFullEvent( EventVector & theEventVector ,
                                         HardVector & /* theHardVector */ )
  const {
  std::list< HepMC::GenParticle * > leptonList ;

  EventVector::iterator iter ;
  for ( iter = theEventVector.begin() ; theEventVector.end() != iter ; 
        ++iter ) {
    HepMC::GenEvent * theHepMCEvent = (*iter) -> pGenEvt() ;
    HepMC::copy_if( theHepMCEvent -> particles_begin() ,
                    theHepMCEvent -> particles_end() , 
                    back_inserter( leptonList )  , isLepton ) ;
  }

  if ( leptonList.empty() ) {
    debug() << "No lepton in this event" << endmsg ;
    return false ;
  }
  
  std::list< HepMC::GenParticle * >::const_iterator it ;
  double px , py , pz , pp , theta , pt ;
  for ( it = leptonList.begin() ; leptonList.end() != it ; ++it ) {
    px = (*it) -> momentum() . px() * GeV ;
    py = (*it) -> momentum() . py() * GeV ;
    pz = (*it) -> momentum() . pz() * GeV ;
    pp = sqrt( px*px + py*py + pz*pz ) ;
    pt = sqrt( px*px + py*py ) ;
    theta = acos( fabs( pz ) / pp ) ;
    if ( ( theta <= m_thetaMax ) && ( pz >= 0. ) && ( pt >= m_ptMin ) ) {
      debug() << 
        format( "Event passed [theta = %.3g] [pz = %.3g] [pt = %.3g]" , 
                theta / mrad , pz / GeV , pt / GeV ) << endmsg ;
      return true ;
    } else 
      debug() << 
        format( "Particle failed cut [theta = %.3g] [pz = %.3g] [pt = %.3g]" ,
                theta / mrad , pz / GeV , pt / GeV ) << endmsg ;
  }
  return false ;
}
