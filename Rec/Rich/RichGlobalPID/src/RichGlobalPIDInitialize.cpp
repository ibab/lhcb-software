//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDInitialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.cpp,v 1.7 2005-04-06 21:34:35 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//------------------------------------------------------------------------------------

// local
#include "RichGlobalPIDInitialize.h"

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

  // trick to force pre-loading of various tools. avoids loading
  // during first processed event and thus biased timing numbers
  trackCreator();
  pixelCreator();

  return sc;
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
