// $Id: RichGlobalPIDInitialize.cpp,v 1.5 2004-07-12 14:51:48 jonrob Exp $
// Include files

// local
#include "RichGlobalPIDInitialize.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDInitialize
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDInitialize>          s_factory ;
const        IAlgFactory& RichGlobalPIDInitializeFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDInitialize::RichGlobalPIDInitialize( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {}

// Destructor
RichGlobalPIDInitialize::~RichGlobalPIDInitialize() {}

//  Initialize
StatusCode RichGlobalPIDInitialize::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }
  
  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichGlobalPIDInitialize::execute() 
{
  debug() << "Execute" << endreq;

  // Set event status to OK for start of GlobalPID processing
  richStatus()->setEventOK( true );

  // Summary object
  RichGlobalPIDSummary * GPIDsummary = new RichGlobalPIDSummary();
  put( GPIDsummary, m_richGPIDSummaryLocation );

  // RichGlobalPIDTrack container
  RichGlobalPIDTracks * GPIDtracks = new RichGlobalPIDTracks();
  put( GPIDtracks, m_richGPIDTrackLocation );

  // RichGlobalPID container
  RichGlobalPIDs * globalPIDs = new RichGlobalPIDs();
  put( globalPIDs, m_richGPIDLocation );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDInitialize::finalize() 
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
