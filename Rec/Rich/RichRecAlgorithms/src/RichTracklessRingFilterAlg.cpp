
//--------------------------------------------------------------------------
/** @file RichTracklessRingFilterAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingFilterAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingFilterAlg.cpp,v 1.2 2008-06-13 13:28:49 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichTracklessRingFilterAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TracklessRingFilterAlg );

// Standard constructor, initializes variables
TracklessRingFilterAlg::TracklessRingFilterAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  declareProperty( "InputRings",  
                   m_inputRings  = LHCb::RichRecRingLocation::MarkovRings        );
  declareProperty( "OutputRings", 
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Best" );
  declareProperty( "MinMinNumPixels",     m_minNumHits = 5   );
  declareProperty( "MinAveragePixelProb", m_minAvProb  = 0.7 );
}

// Destructor
TracklessRingFilterAlg::~TracklessRingFilterAlg() {}

StatusCode TracklessRingFilterAlg::initialize()
{
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Input Rings            : " << m_inputRings  << endreq;
  info() << "Output Rings           : " << m_outputRings << endreq;
  info() << "Min. # pixels per ring : " << m_minNumHits  << endreq;
  info() << "Min. average prob.     : " << m_minAvProb   << endreq;

  return sc;
}

StatusCode TracklessRingFilterAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Load the input rings
  const LHCb::RichRecRings * inrings = get<LHCb::RichRecRings>(m_inputRings);

  // Create a container for the output rings
  LHCb::RichRecRings * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Loop over the input rings and select the 'best' ones
  for ( LHCb::RichRecRings::const_iterator iRing = inrings->begin();
        iRing != inrings->end(); ++iRing )
  {
    // average ring hit prob
    //const double avProb = (*iRing)->averagePixelProb();
    const double avProb = avRingHitProb(*iRing);
    if ( avProb < m_minAvProb ) continue;

    // Number of pixels on the ring
    const unsigned int nPixels = (*iRing)->richRecPixels().size();
    if ( nPixels < m_minNumHits ) continue;

    // If get here, ring is selected so clone and save
    outrings->insert( new LHCb::RichRecRing(**iRing), (*iRing)->key() );
  }

  return StatusCode::SUCCESS;
}

// (TEMPORARY UNTIL PART OF RICHRECRING)
double
TracklessRingFilterAlg::avRingHitProb( LHCb::RichRecRing * ring ) const
{
  double prob(0);
  for ( LHCb::RichRecPixelOnRing::Vector::iterator iP = ring->richRecPixels().begin();
        iP != ring->richRecPixels().end(); ++iP )
  {
    prob += (*iP).associationProb();
  }
  return ( ring->richRecPixels().empty() ? 0 : prob/(double)ring->richRecPixels().size() );
}
