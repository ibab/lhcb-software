
//--------------------------------------------------------------------------
/** @file RichTracklessRingSegmentAssociationAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingSegmentAssociationAlg
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichTracklessRingSegmentAssociationAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TracklessRingSegmentAssociationAlg )

// Standard constructor, initializes variables
TracklessRingSegmentAssociationAlg::
TracklessRingSegmentAssociationAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  using namespace boost::assign;
  declareProperty( "InputRings", m_inputRings = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "MaxDistance", m_maxDist   = std::vector<double>(3, 50*Gaudi::Units::mm) );
}

// Destructor
TracklessRingSegmentAssociationAlg::~TracklessRingSegmentAssociationAlg() {}

StatusCode TracklessRingSegmentAssociationAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  _ri_debug << "Input Rings                 : " << m_inputRings  << endmsg;
  _ri_debug << "Max. seperation             : " << m_maxDist << endmsg;

  return sc;
}

StatusCode TracklessRingSegmentAssociationAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If no track segments, just return. This is OK in some cases.
  if ( richSegments()->empty()  ) return StatusCode::SUCCESS;

  // Load the input rings
  LHCb::RichRecRings * rings = get<LHCb::RichRecRings>(m_inputRings);

  typedef std::pair<LHCb::RichRecRing*, double> SegRingPair;
  typedef Rich::Map<LHCb::RichRecSegment*,SegRingPair> MatchMap;
  MatchMap mmap;

  // Loop over the rings to find associations
  for ( const auto ring : *rings )
  {
    // Detector info
    const auto rad = ring->radiator();
    const auto det = ring->rich();

    _ri_verbo << "Ring " << ring->key() << " " << det << " " << rad << endmsg;

    // Ring centre point
    const auto & ringPtnLocal = ring->centrePointLocal();

    // reset all segment associations to NULL
    ring->setRichRecSegment(nullptr);

    // Loop over segments for this ring + radiator
    for ( const auto seg : *richSegments() )
    {
      // Only uses segments in correct RICH and for the same radiator
      if ( det != seg->trackSegment().rich() ||
           rad != seg->trackSegment().radiator()
           ) continue;

      // Track hit point (if it were a photon) on detector plane, in local coords
      // (i.e. centre of Ch ring)
      const auto & tkPtnLocal = seg->pdPanelHitPointLocal();

      // compute track / ring centre seperation
      const auto sep = std::sqrt( std::pow ( ringPtnLocal.x()-tkPtnLocal.x(), 2 ) +
                                  std::pow ( ringPtnLocal.y()-tkPtnLocal.y(), 2 ) );
      if ( sep < m_maxDist[rad] )
      {
        _ri_verbo << " -> Segment " << seg->key() << " within matching distance" << endmsg;

        // Get current best for this segment
        if ( mmap.find(seg) == mmap.end() ) mmap[seg] = SegRingPair(ring,9e20);
        auto & best = mmap[seg];

        // if a better match for this segment save it
        if ( sep < best.second ) best = SegRingPair(ring,sep);

      }

    } // loop over segments

  } // loop over rings

  // Make final association, checking if a ring has been matched to more than one segment
  Rich::Map<LHCb::RichRecRing*, double> tmap;
  for ( const auto& M : mmap )
  {
    const auto dist = M.second.second; 
    auto * seg      = M.first;
    auto * ring     = M.second.first;
    if ( tmap.find(ring) == tmap.end() )
    {
      // This ring is not yet associated, so do so
      ring->setRichRecSegment(seg);
      tmap[ring] = dist;
      _ri_debug << "Setting  segment " << seg->key() 
                << " match to ring " << ring->key() << endmsg;
    }
    else
    {
      // already associated, so is this one better ?
      if ( dist < tmap[ring] )
      {
        // yes, so update association
        ring->setRichRecSegment(seg);
        tmap[ring] = dist;
        _ri_debug << "Updating segment " << seg->key() 
                  << " match to ring " << ring->key() << endmsg;
      }
    }

  }

  return StatusCode::SUCCESS;
}
