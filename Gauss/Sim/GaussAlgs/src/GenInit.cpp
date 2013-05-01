// Include files 
#include <cmath>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/SystemOfUnits.h"

// from GenEvent
#include "Event/GenHeader.h"
#include "Event/BeamParameters.h"
#include "GenEvent/BeamForInitialization.h"

// local
#include "GenInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenInit
//
// 2006-01-16 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenInit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenInit::GenInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator ),
    m_memoryTool(0)
{
  declareProperty( "FirstEventNumber", m_firstEvent = 1 );
  declareProperty( "RunNumber",        m_runNumber  = 1 );
  declareProperty( "MCHeader" ,        
                   m_mcHeader = LHCb::GenHeaderLocation::Default ) ;
  declareProperty( "BeamParameters" ,   
                   m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
  declareProperty( "BeamEnergy" , m_beamEnergy = 3.5*Gaudi::Units::TeV );
  declareProperty( "BunchLengthRMS" , m_bunchLengthRMS = 1. ) ;
  declareProperty( "NormalizedEmittance" , m_normalizedEmittance = 1.*Gaudi::Units::mm ) ;
  declareProperty( "TotalCrossSection" , m_totalCrossSection = 100. * Gaudi::Units::microbarn ) ;
  declareProperty( "HorizontalCrossingAngle" , m_horizontalCrossingAngle = 0.3 * Gaudi::Units::mrad ) ;
  declareProperty( "VerticalCrossingAngle" , m_verticalCrossingAngle = 0.3 * Gaudi::Units::mrad ) ;
  declareProperty( "HorizontalBeamlineAngle" , m_horizontalBeamlineAngle = 0.3 * Gaudi::Units::mrad ) ;
  declareProperty( "VerticalBeamlineAngle" , m_verticalBeamlineAngle = 0.3 * Gaudi::Units::mrad ) ;
  declareProperty( "BetaStar" , m_betaStar = 3.*Gaudi::Units::m ) ;
  declareProperty( "BunchSpacing" , m_bunchSpacing = 50. * Gaudi::Units::ns ) ;
  declareProperty( "XLuminousRegion" , m_xLuminousRegion = 0. ) ;
  declareProperty( "YLuminousRegion" , m_yLuminousRegion = 0. ) ;
  declareProperty( "ZLuminousRegion" , m_zLuminousRegion = 0. ) ;
  declareProperty( "Luminosity" , m_luminosity = 1.e32 /( Gaudi::Units::cm2 * Gaudi::Units::s ) ) ;

  declareProperty( "CreateBeam", m_createBeam = false );

}

//=============================================================================
// Destructor
//=============================================================================
GenInit::~GenInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GenInit::initialize() {
  StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  std::string toolName = name()+"Memory";
  m_memoryTool = tool<IGenericTool>( "MemoryTool", toolName, this, true );

  // create beam parameter object
  m_beam.setEnergy( m_beamEnergy ) ;
  m_beam.setSigmaS( m_bunchLengthRMS ) ;
  m_beam.setEpsilonN( m_normalizedEmittance ) ;
  m_beam.setTotalXSec( m_totalCrossSection ) ;
  m_beam.setHorizontalCrossingAngle( m_horizontalCrossingAngle ) ;
  m_beam.setVerticalCrossingAngle( m_verticalCrossingAngle ) ;
  m_beam.setHorizontalBeamlineAngle( m_horizontalBeamlineAngle ) ;
  m_beam.setVerticalBeamlineAngle( m_verticalBeamlineAngle ) ;
  m_beam.setBetaStar( m_betaStar ) ;
  m_beam.setBunchSpacing( m_bunchSpacing ) ;
  m_beam.setBeamSpot( Gaudi::XYZPoint( m_xLuminousRegion ,
                                       m_yLuminousRegion ,
                                       m_zLuminousRegion ) ) ;
  m_beam.setLuminosity( m_luminosity ) ;

  if (m_createBeam) {
    info() << "Create BeamForInitialization" << endmsg;
    // print the values used:
    info() << "Beam Parameters :" << endmsg ;
    info() << "Energy = " << m_beam.energy() / Gaudi::Units::TeV << " TeV" << endmsg ;
    info() << "Bunch length RMS = " << m_beam.sigmaS() / Gaudi::Units::mm << " mm" << endmsg ;
    info() << "Normalized emittance = " << m_beam.epsilonN() / Gaudi::Units::mm << " mm" << endmsg ;
    info() << "Revolution frequency = " << m_beam.revolutionFrequency() / Gaudi::Units::kilohertz << " kHz" << endmsg ;
    info() << "Total cross-section assumed = " << m_beam.totalXSec() / Gaudi::Units::millibarn << " mb" << endmsg ;
    info() << "Horizontal crossing angle = " << m_beam.horizontalCrossingAngle() / Gaudi::Units::mrad << " mrad" << endmsg ;
    info() << "Vertical crossing angle = " << m_beam.verticalCrossingAngle() / Gaudi::Units::mrad << " mrad" << endmsg ;
    info() << "Horizontal beam line angle = " << m_beam.horizontalBeamlineAngle() / Gaudi::Units::mrad << " mrad" << endmsg ;
    info() << "Vertical beam line angle = " << m_beam.verticalBeamlineAngle() / Gaudi::Units::mrad << " mrad" << endmsg ;
    info() << "beta star = " << m_beam.betaStar() / Gaudi::Units::m << " m" << endmsg ;
    info() << "Bunch spacing = " << m_beam.bunchSpacing() / Gaudi::Units::ns << " ns" << endmsg ;
    info() << "Beam spot = " << m_beam.beamSpot() << " mm" << endmsg ;
    info() << "Luminosity = " << m_beam.luminosity() * (Gaudi::Units::cm2 * Gaudi::Units::s ) << " /cm2/s" << endmsg ;
    info() << "Nu = " << m_beam.nu() << endmsg;
    info() << "emittance = " << m_beam.emittance() << endmsg;
    info() << "Luminous region = ( " << m_beam.sigmaX() << ", "
           << m_beam.sigmaY() << ", " << m_beam.sigmaZ() << " ), mm " << endmsg;

    LHCb::BeamParameters * beamParameters = new LHCb::BeamParameters( m_beam );
    BeamForInitialization::getInitialBeamParameters() = beamParameters;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  m_memoryTool->execute();

  // Initialize the random number
  longlong eventNumber = m_firstEvent - 1 + this->eventCounter();
  std::vector<long int> seeds = getSeeds( m_runNumber, eventNumber );
  sc = this->initRndm( seeds );
  if ( sc.isFailure() ) return sc;  // error printed already by initRndm
  this->printEventRun( eventNumber, m_runNumber, &seeds);
  
  // Create GenHeader and partially fill it - updated during phase execution
  LHCb::GenHeader* header = new LHCb::GenHeader();
  header->setApplicationName( this->appName() );
  header->setApplicationVersion( this->appVersion() );
  header->setRunNumber( m_runNumber );
  header->setEvtNumber( eventNumber );
  header->setEvType( 0 ) ;
  put( header, m_mcHeader );    

  // Check if BeamParameters already exists (main event only!!), in which case link from header
  if ( m_createBeam ) {
    // Create BeamParameters object and fill it.
    debug() << "Creating BeamParameters in main event" << endmsg;
    LHCb::BeamParameters* beamParameters = new LHCb::BeamParameters( m_beam ) ;
    put( beamParameters , m_beamParameters ) ;
    
    // Link the beam parameters to the header
    header -> setBeamParameters( beamParameters ) ;
  }
  else {
    debug() << "Set beam parameters link to main event" << endmsg;
    LHCb::BeamParameters* beamParameters = 
      get<LHCb::BeamParameters>( m_beamParameters );
    header -> setBeamParameters ( beamParameters );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenInit::finalize() {
  
  if (m_createBeam) {
    debug() << "Delete BeamForInitialization" << endmsg;
    delete ( BeamForInitialization::getInitialBeamParameters( ) ) ;
  }

  debug() << "==> Finalize" << endmsg;

  return LbAppInit::finalize();  // must be called after all other actions
}

//=============================================================================
