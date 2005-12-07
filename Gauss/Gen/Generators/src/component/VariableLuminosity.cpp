// $Id: VariableLuminosity.cpp,v 1.3 2005-12-07 22:51:01 robbep Exp $
// Include files 

// local
#include "VariableLuminosity.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"

// From CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// From Generators
#include "Generators/GenCounters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VariableLuminosity
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<VariableLuminosity>          s_factory ;
const        IToolFactory& VariableLuminosityFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VariableLuminosity::VariableLuminosity( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_peakLuminosity( 0. ) ,
    m_numberOfZeroInteraction( 0 ) ,
    m_nEvents( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty ( "Luminosity"    , m_luminosity    = 2.e32 /cm2/s      ) ;
    declareProperty ( "FillDuration"  , m_fillDuration  = 7.0 * 3600 * s    ) ;
    declareProperty ( "BeamDecayTime" , m_beamDecayTime = 10.0 * 3600 * s   ) ;
    declareProperty ( "CrossingRate"  , m_crossingRate  = 30.0 * megahertz  ) ;
    declareProperty ( "TotalXSection" , m_totalXSection = 102.4 * millibarn ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
VariableLuminosity::~VariableLuminosity( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode VariableLuminosity::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  m_randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  
  sc = m_flatGenerator.initialize( m_randSvc , Rndm::Flat( 0 , 1 ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random generator" ) ;

  info() << "Poisson distribution with 'LHCb mean'. " << endmsg ;
  info() << "Luminosity (10^32 / cm^2 s): " << m_luminosity / 1.e32 * cm2 * s
         << endmsg ;
  info() << "Fill duration (hours): " << m_fillDuration / 3600 / s << endmsg ;
  info() << "Beam decay time (hours): " << m_beamDecayTime / 3600 / s 
         << endmsg ;
  info() << "Bunch crossing rate (MHz): " << m_crossingRate / megahertz 
         << endmsg ;
  info() << "Total cross section (mbarn): " << m_totalXSection / millibarn 
         << endreq ;
  

  // m_luminosity is the average luminosity over the fill duration.
  // m_peakLuminosity is the luminosity at t=0
  m_peakLuminosity = m_luminosity * m_fillDuration / m_beamDecayTime /
    ( 1.0 - exp( -m_fillDuration / m_beamDecayTime ) ) ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int VariableLuminosity::numberOfPileUp( double & currentLuminosity ) {
  unsigned int result = 0 ;
  double theTime , mean ;
  while ( 0 == result ) {
    m_nEvents++ ;
    theTime = m_flatGenerator() * m_fillDuration ;
    currentLuminosity = m_peakLuminosity * exp( -theTime/m_beamDecayTime ) ;
    mean = currentLuminosity * m_totalXSection / m_crossingRate ;
    Rndm::Numbers poissonGenerator( m_randSvc , Rndm::Poisson( mean ) ) ;
    result = (unsigned int) poissonGenerator() ;
    if ( 0 == result ) m_numberOfZeroInteraction++ ;
  }
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void VariableLuminosity::printPileUpCounters( ) {
  using namespace GenCounters ;
  info() << "***********   Luminosity counters   **************" << std::endl ;
  printCounter( info() , "all events (including empty events)" , m_nEvents ) ;
  printCounter( info() , "with 0 interaction" , m_numberOfZeroInteraction ) ;
  info() << endmsg ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode VariableLuminosity::finalize( ) {
  release( m_randSvc ) ;
  return GaudiTool::finalize( ) ;
}
