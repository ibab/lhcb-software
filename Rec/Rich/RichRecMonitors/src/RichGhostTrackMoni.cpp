
//---------------------------------------------------------------------------
/** @file RichGhostTrackMoni.cpp
 *
 *  Implementation file for algorithm class : RichGhostTrackMoni
 *
 *  $Id: RichGhostTrackMoni.cpp,v 1.1 2006-12-01 14:01:40 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichGhostTrackMoni.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Rich, RichGhostTrackMoni );

// Standard constructor, initializes variables
Rich::RichGhostTrackMoni::RichGhostTrackMoni( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
Rich::RichGhostTrackMoni::~RichGhostTrackMoni() {}

// Initialize
StatusCode Rich::RichGhostTrackMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tools
  acquireTool( "TrackSelector", m_trSelector, this );

  return sc;
}

// Main execution
StatusCode Rich::RichGhostTrackMoni::execute()
{


  return StatusCode::SUCCESS;
}

// Finalize
StatusCode Rich::RichGhostTrackMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
