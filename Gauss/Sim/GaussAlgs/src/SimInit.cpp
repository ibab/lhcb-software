// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/Time.h"
#include "GaudiAlg/IGenericTool.h"

// from GenEvent
#include "Event/GenHeader.h"

// from MCEvent
#include "Event/MCHeader.h"

// local
#include "SimInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimInit
//
// 2006-01-16 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SimInit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimInit::SimInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator ),
    m_memoryTool( 0 ),
    m_detDataSvc( 0 )
{
    declareProperty( "GenHeader" , m_genHeader = LHCb::GenHeaderLocation::Default ) ;
    declareProperty( "MCHeader"  , m_mcHeader  = LHCb::MCHeaderLocation::Default  ) ;
}

//=============================================================================
// Destructor
//=============================================================================
SimInit::~SimInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimInit::initialize() {
  StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  std::string toolName = name()+"Memory";
  m_memoryTool = tool<IGenericTool>( "MemoryTool", toolName, this, true );

  // Get IDetDataSvc interface of DetDataSvc
  try {
    m_detDataSvc = svc<IDetDataSvc>("DetDataSvc/DetectorDataSvc", true );
  } catch (GaudiException) {
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the GenHeader if it exist (i.e.
  // Generator phase already run or events read from file)
  LHCb::GenHeader* evt = 
    get<LHCb::GenHeader>( m_genHeader );

  // Initialize the random number
  std::vector<long int> seeds = getSeeds( evt->runNumber(), evt->evtNumber() );
  sc = this->initRndm( seeds );
  if ( sc.isFailure() ) return sc;  // error printed already by initRndm  
  this->printEventRun( evt->evtNumber(), evt->runNumber(), &seeds );

  // Create MCHeader and partially fill it - updated in Simulation phase
  LHCb::MCHeader* header = new LHCb::MCHeader();
  header->setApplicationName( this->appName() ); // update it later?
  header->setApplicationVersion( this->appVersion() );
  header->setRunNumber( evt->runNumber() );
  header->setEvtNumber( evt->evtNumber() );
  header->setEvtTime( m_detDataSvc->eventTime().ns() );
  header->setCondDBTags( this->condDBTags() );
  put( header, m_mcHeader );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SimInit::finalize() {

  debug() << "==> Finalize" << endmsg;

  return LbAppInit::finalize();  // must be called after all other actions
}

//=============================================================================
