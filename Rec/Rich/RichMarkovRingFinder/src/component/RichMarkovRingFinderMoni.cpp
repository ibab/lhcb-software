
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.cpp
 *
 *  Implementation file for algorithm class : RichMarkovRingFinderMoni
 *
 *  $Id: RichMarkovRingFinderMoni.cpp,v 1.23 2008-05-23 14:19:36 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichMarkovRingFinderMoni.h"

// namespace
using namespace LHCb;
using namespace Rich::Rec;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichMarkovRingFinderMoni );

// Standard constructor, initializes variables
RichMarkovRingFinderMoni::RichMarkovRingFinderMoni( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_rayTrace          ( NULL ),
    m_idTool            ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
RichMarkovRingFinderMoni::~RichMarkovRingFinderMoni() {};

//  Initialize
StatusCode RichMarkovRingFinderMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",       m_rayTrace          );
  acquireTool( "RichSmartIDTool",      m_idTool,   0, true );
  acquireTool( "TrackSelector",        m_trSelector, this  );

  return sc;
}

// Main execution
StatusCode RichMarkovRingFinderMoni::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

  } // end segment loop

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichMarkovRingFinderMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
