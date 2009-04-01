// $Id: FixedLuminosityForSpillOver.cpp,v 1.1 2009-04-01 09:48:00 robbep Exp $
// Include files 

// local
#include "FixedLuminosityForSpillOver.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// From Generators
#include "Generators/GenCounters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedLuminosityForSpillOver
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FixedLuminosityForSpillOver );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedLuminosityForSpillOver::FixedLuminosityForSpillOver( const std::string& type,
							  const std::string& name,
							  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_numberOfZeroInteraction( 0 ) ,
    m_nEvents( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty ( "Luminosity"    , m_luminosity    = 2.e32 /Gaudi::Units::cm2/Gaudi::Units::s      ) ;
    declareProperty ( "CrossingRate"  , m_crossingRate  = 30.0 * Gaudi::Units::megahertz  ) ;
    declareProperty ( "TotalXSection" , m_totalXSection = 102.4 * Gaudi::Units::millibarn ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedLuminosityForSpillOver::~FixedLuminosityForSpillOver( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedLuminosityForSpillOver::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  m_randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;  

  info() << "Poisson distribution with fixed luminosity. " << endmsg ;
  info() << "Luminosity (10^32 / cm^2 s): " 
         << m_luminosity / 1.e32 * Gaudi::Units::cm2 * Gaudi::Units::s << endmsg ;
  info() << "Bunch crossing rate (MHz): " << m_crossingRate / Gaudi::Units::megahertz 
         << endmsg ;
  info() << "Total cross section (mbarn): " << m_totalXSection / Gaudi::Units::millibarn 
         << endreq ;

  m_mean = m_luminosity * m_totalXSection / m_crossingRate ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedLuminosityForSpillOver::numberOfPileUp( double & currentLuminosity ) {
  currentLuminosity = m_luminosity ;
  unsigned int result = 0 ;
  m_nEvents++ ;
  Rndm::Numbers poissonGenerator( m_randSvc , Rndm::Poisson( m_mean ) ) ;
  result = (unsigned int) poissonGenerator() ;
  if ( 0 == result ) m_numberOfZeroInteraction++ ;
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedLuminosityForSpillOver::printPileUpCounters( ) {
  using namespace GenCounters ;
  info() << "***********   Luminosity counters   **************" << std::endl ;
  printCounter( info() , "all events (including empty events)", m_nEvents ) ;
  printCounter( info() , "events with 0 interaction" , 
                m_numberOfZeroInteraction ) ;
  info() << endmsg ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode FixedLuminosityForSpillOver::finalize( ) {
  release( m_randSvc ) ;
  return GaudiTool::finalize( ) ;
}
