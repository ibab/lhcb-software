//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDInitialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.cpp,v 1.11 2006-08-28 10:58:11 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//------------------------------------------------------------------------------------

// local
#include "RichGlobalPIDInitialize.h"

// namespaces
using namespace LHCb;

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
  const StatusCode sc = RichGlobalPIDAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biased any timing numbers
  statusCreator();   // pre-load the status creator

  return sc;
}

// Main execution
StatusCode RichGlobalPIDInitialize::execute()
{
  // Set event status to OK for start of GlobalPID processing
  richStatus()->setEventOK( true );

  // Summary object
  put( new RichGlobalPIDSummary(), m_richGPIDSummaryLocation );

  // RichGlobalPIDTrack container
  put( new RichGlobalPIDTracks(),  m_richGPIDTrackLocation   );

  // RichGlobalPID container
  put( new RichGlobalPIDs(),       m_richGPIDLocation        );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDInitialize::finalize()
{
  // Execute base class method
  return RichGlobalPIDAlgBase::finalize();
}
