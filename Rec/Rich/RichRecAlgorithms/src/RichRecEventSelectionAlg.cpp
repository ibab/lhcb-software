
//--------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.cpp,v 1.3 2008-08-26 19:39:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecEventSelectionAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventSelectionAlg );

// Standard constructor, initializes variables
EventSelectionAlg::EventSelectionAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  declareProperty( "MinPixels", m_minPixels = 10 );
  declareProperty( "MinRings",  m_minRings  = 0  );
  declareProperty( "MinTracks", m_minTracks = 0  );
  declareProperty( "RingLocation", m_ringLoc = "Rec/Rich/Markov/RingsIsolated" );
}

// Destructor
EventSelectionAlg::~EventSelectionAlg() {}

StatusCode EventSelectionAlg::execute()
{
  // Event Status
  bool OK = richStatus()->eventOK();
  
  // Pixels
  if ( OK )
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
    // enough hits ?
    OK = ( richPixels()->size() >= m_minPixels );
  }

  // rings
  if ( OK && m_minRings > 0 )
  {
    // get the rings
    const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>(m_ringLoc);
    // enough rings ?
    OK = ( rings->size() >= m_minRings );
  }

  // tracks
  if ( OK && m_minTracks > 0 )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    // enough hits ?
    OK = ( richTracks()->size() >= m_minTracks );
  }

  // set if this events is selected
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}
