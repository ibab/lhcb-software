
//--------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.cpp,v 1.6 2008-10-16 09:42:35 cattanem Exp $
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
  declareProperty( "MinPixels", m_minPixels = 10,
                   "The minimum number of total RICH hits" );
  declareProperty( "MinRings",  m_minRings  = 0,
                   "The minimum number of rings at the given TES location" );
  declareProperty( "MinTracks", m_minTracks = 0,
                   "The minimum number of tracks"  );
  declareProperty( "MinHPDsWithHits",  m_minHPDsWithHits = 0,
                   "The minimum number of HPDs to have more that 'MinHPDHits' in them" );
  declareProperty( "MinHPDHits", m_minHPDHits = 0,
                   "The minimum number of hits to have in at least 'MinHPDsWithHits' HPDs");
  declareProperty( "RingLocation", m_ringLoc = "Rec/Rich/Markov/RingsIsolated",
                   "The TES location in which to count the number of ring objects" );
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
    // enough hits overall ?
    OK = ( (int)richPixels()->size() >= m_minPixels );
    if ( OK && m_minHPDsWithHits > 0 )
    {
      Rich::Map<LHCb::RichSmartID,unsigned int> hpdCount;
      std::set<LHCb::RichSmartID> selectedHPDs;
      // loop over pixels
      for ( LHCb::RichRecPixels::const_iterator iP = richPixels()->begin();
            iP != richPixels()->end(); ++iP )
      {
        // count hits in each HPD
        if ( ++hpdCount[(*iP)->hpd()] > m_minHPDHits ) selectedHPDs.insert((*iP)->hpd());
      }
      OK = ( selectedHPDs.size() >= m_minHPDsWithHits );
    }
  }

  // rings
  if ( OK && m_minRings > 0 )
  {
    if ( !exist<LHCb::RichRecRings>(m_ringLoc) )
    {
      Warning( "No Rings at '"+m_ringLoc+"'");
      OK = false;
    }
    else
    {
      // get the rings
      const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>(m_ringLoc);
      // enough rings ?
      OK = ( (int)rings->size() >= m_minRings );
    }
  }

  // tracks
  if ( OK && m_minTracks > 0 )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    // enough tracks ?
    OK = ( (int)richTracks()->size() >= m_minTracks );
  }

  // set if this events is selected
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}
