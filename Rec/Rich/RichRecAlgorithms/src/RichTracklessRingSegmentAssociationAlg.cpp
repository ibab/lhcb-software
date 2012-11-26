
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

  info() << "Input Rings                 : " << m_inputRings  << endmsg;
  info() << "Max. seperation             : " << m_maxDist << endmsg;

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
  for ( LHCb::RichRecRings::const_iterator iR = rings->begin();
        iR != rings->end(); ++iR )
  {
    // Detector info
    const Rich::RadiatorType rad = (*iR)->radiator();
    const Rich::DetectorType det = (*iR)->rich();

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Ring " << (*iR)->key() << " " << det << " " << rad << endmsg;

    // Ring centre point
    const Gaudi::XYZPoint & ringPtnLocal = (*iR)->centrePointLocal();

    // reset all segment associations to NULL
    (*iR)->setRichRecSegment(NULL);

    // Loop over segments for this ring + radiator
    for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
          iSeg != richSegments()->end(); ++iSeg )
    {
      // Only uses segments in correct RICH and for the same radiator
      if ( det != (*iSeg)->trackSegment().rich() ||
           rad != (*iSeg)->trackSegment().radiator()
           ) continue;

      // Track hit point (if it were a photon) on detector plane, in local coords
      // (i.e. centre of Ch ring)
      const Gaudi::XYZPoint & tkPtnLocal = (*iSeg)->pdPanelHitPointLocal();

      // compute track / ring centre seperation
      const double sep = std::sqrt( std::pow ( ringPtnLocal.x()-tkPtnLocal.x(), 2 ) +
                                    std::pow ( ringPtnLocal.y()-tkPtnLocal.y(), 2 ) );
      if ( sep < m_maxDist[rad] )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Segment " << (*iSeg)->key() << " within matching distance" << endmsg;

        // Get current best for this segment
        if ( mmap.find(*iSeg) == mmap.end() ) mmap[*iSeg] = SegRingPair(*iR,9e20);
        SegRingPair & best = mmap[*iSeg];

        // if a better match for this segment save it
        if ( sep < best.second ) best = SegRingPair(*iR,sep);

      }

    } // loop over segments

  } // loop over rings

  // Make final association, checking if a ring has been matched to more than one segment
  Rich::Map<LHCb::RichRecRing*, double> tmap;
  for ( MatchMap::const_iterator iM = mmap.begin(); iM != mmap.end(); ++iM )
  {
    const double         dist = (*iM).second.second; 
    LHCb::RichRecSegment* seg = (*iM).first;
    LHCb::RichRecRing*   ring = (*iM).second.first;
    if ( tmap.find(ring) == tmap.end() )
    {
      // This ring is not yet associated, so do so
      ring->setRichRecSegment(seg);
      tmap[ring] = dist;
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Setting  segment " << seg->key() 
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
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Updating segment " << seg->key() 
                  << " match to ring " << ring->key() << endmsg;
      }
    }

  }

  return StatusCode::SUCCESS;
}
