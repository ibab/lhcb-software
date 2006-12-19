//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDInitialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.cpp,v 1.12 2006-12-19 09:06:20 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//------------------------------------------------------------------------------------

// local
#include "RichGlobalPIDInitialize.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespaces
using namespace LHCb;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RichGlobalPIDInitialize );

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
