// $Id: FixedLuminosityForRareProcess.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// Include files 

// local
#include "FixedLuminosityForRareProcess.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// From Generators
#include "Generators/GenCounters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedLuminosityForRareProcess
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FixedLuminosityForRareProcess );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedLuminosityForRareProcess::FixedLuminosityForRareProcess( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_nEvents( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty ( "Luminosity"    , m_luminosity    = 2.e32 /Gaudi::Units::cm2/Gaudi::Units::s      ) ;
    declareProperty ( "CrossingRate"  , m_crossingRate  = 30.0 * Gaudi::Units::megahertz  ) ;
    declareProperty ( "TotalXSection" , m_totalXSection = 102.4 * Gaudi::Units::millibarn ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedLuminosityForRareProcess::~FixedLuminosityForRareProcess( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedLuminosityForRareProcess::initialize( ) {
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
unsigned int FixedLuminosityForRareProcess::numberOfPileUp( double & currentLuminosity ) {
  currentLuminosity = m_luminosity ;
  unsigned int result = 0 ;
  m_nEvents++ ;
  Rndm::Numbers poissonGenerator( m_randSvc , Rndm::Poisson( m_mean ) ) ;
  result = (unsigned int) ( poissonGenerator() + 1.0 ) ;
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedLuminosityForRareProcess::printPileUpCounters( ) {
  using namespace GenCounters ;
  info() << "***********   Luminosity counters   **************" << std::endl ;
  printCounter( info() , "all events (including empty events)", m_nEvents ) ;
  info() << endmsg ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode FixedLuminosityForRareProcess::finalize( ) {
  release( m_randSvc ) ;
  return GaudiTool::finalize( ) ;
}
