
//--------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.cpp,v 1.7 2009-09-04 10:36:24 jonrob Exp $
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

namespace
{
  static const unsigned int s_large_number = 999999;
}

// Standard constructor, initializes variables
EventSelectionAlg::EventSelectionAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{

  declareProperty( "MinPixels", m_minPixels = 0,
                   "The minimum number of total RICH hits" );
  declareProperty( "MinRings",  m_minRings  = 0,
                   "The minimum number of rings at the given TES location" );
  declareProperty( "MinTracks", m_minTracks = 0,
                   "The minimum number of tracks"  );
  declareProperty( "MinHPDsWithHits",  m_minHPDsWithHits = 0,
                   "The minimum number of HPDs to have more that 'MinHPDHits' in them" );
  declareProperty( "MinHPDHits", m_minHPDHits = 0,
                   "The minimum number of hits to have in at least 'MinHPDsWithHits' HPDs");

  declareProperty( "MaxPixels", m_maxPixels = s_large_number,
                   "The maximum number of total RICH hits" );
  declareProperty( "MaxRings",  m_maxRings  = s_large_number,
                   "The maximum number of rings at the given TES location" );
  declareProperty( "MaxTracks", m_maxTracks = s_large_number,
                   "The maximum number of tracks"  );
  declareProperty( "MaxHPDsWithHits",  m_maxHPDsWithHits = s_large_number,
                   "The maximum number of HPDs to have more that 'MinHPDHits' in them" );
  declareProperty( "MaxHPDHits", m_maxHPDHits = s_large_number,
                   "The maximum number of hits to have in at least 'MinHPDsWithHits' HPDs");

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
    const unsigned int nPixTotal = richPixels()->size();
    OK = ( nPixTotal >= m_minPixels && nPixTotal <= m_maxPixels );
    // hits per HPD
    if ( OK && ( m_minHPDsWithHits > 0 || m_minHPDsWithHits < s_large_number ) )
    {
      Rich::Map<LHCb::RichSmartID,unsigned int> hpdCount;
      std::set<LHCb::RichSmartID> selectedHPDs;
      // loop over pixels
      for ( LHCb::RichRecPixels::const_iterator iP = richPixels()->begin();
            iP != richPixels()->end(); ++iP )
      {
        // count hits in each HPD
        unsigned int & count = ++hpdCount[(*iP)->hpd()];
        if ( count > m_minHPDHits && 
             count < m_maxHPDHits  ) selectedHPDs.insert((*iP)->hpd());
      }
      OK = ( selectedHPDs.size() >= m_minHPDsWithHits && 
             selectedHPDs.size() <= m_maxHPDsWithHits );
    }
  }

  // rings
  if ( OK && ( m_minRings > 0 || m_maxRings < s_large_number ) )
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
      OK = ( rings->size() >= m_minRings && rings->size() <= m_maxRings );
    }
  }

  // tracks
  if ( OK && ( m_minTracks > 0 || m_maxTracks < s_large_number ) )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    // enough tracks ?
    OK = ( richTracks()->size() >= m_minTracks && richTracks()->size() <= m_maxTracks );
  }

  // set if this events is selected or not
  setFilterPassed(OK);

  return StatusCode::SUCCESS;
}
