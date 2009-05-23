
//--------------------------------------------------------------------------
/** @file RichTracklessRingFilterAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingFilterAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingFilterAlg.cpp,v 1.6 2009-05-23 20:17:13 jonrob Exp $
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
                   m_inputRings  = LHCb::RichRecRingLocation::MarkovRings+"All"  );
  declareProperty( "OutputRings", 
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Best" );
  declareProperty( "MinNumPixels",        m_minNumHits = 5   );
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
  debug() << "Found " << inrings->size() << " input rings at " << m_inputRings << endmsg;

  // Create a container for the output rings
  LHCb::RichRecRings * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Count Rings per radiator
  typedef Rich::Map<Rich::RadiatorType,unsigned int> RadCount;
  RadCount rCount;
  //rCount[Rich::Aerogel]  = 0;
  rCount[Rich::Rich1Gas] = 0;
  rCount[Rich::Rich2Gas] = 0;

  // Loop over the input rings and select the 'best' ones
  for ( LHCb::RichRecRings::const_iterator iRing = inrings->begin();
        iRing != inrings->end(); ++iRing )
  {
    // average ring hit prob
    const double avProb = (*iRing)->averagePixelProb();
    // Number of pixels on the ring
    const unsigned int nPixels = (*iRing)->richRecPixels().size();
    
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Ring " << (*iRing)->key() 
                << " av prob = " << avProb
                << " nPixels = " << nPixels << endmsg;
 
    if ( avProb < m_minAvProb   ||
         nPixels < m_minNumHits ) continue;

    // If get here, ring is selected so clone and save
    ++rCount[(*iRing)->radiator()];
    outrings->insert( new LHCb::RichRecRing(**iRing), (*iRing)->key() );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Selected " << outrings->size() << " rings at " << m_outputRings << endmsg;

  for ( RadCount::const_iterator rad = rCount.begin(); rad != rCount.end(); ++rad )
  {
    counter("Selected good "+Rich::text(rad->first)+" rings") += rad->second;
  }

  return StatusCode::SUCCESS;
}
