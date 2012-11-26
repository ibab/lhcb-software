
//--------------------------------------------------------------------------
/** @file RichTracklessRingFilterAlg.cpp
 *
 *  Implementation file for algorithm class : RichTracklessRingFilterAlg
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
DECLARE_ALGORITHM_FACTORY( TracklessRingFilterAlg )

// Standard constructor, initializes variables
TracklessRingFilterAlg::TracklessRingFilterAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  using namespace boost::assign;
  declareProperty( "InputRings",  
                   m_inputRings  = LHCb::RichRecRingLocation::MarkovRings+"All"  );
  declareProperty( "OutputRings", 
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Best" );
  declareProperty( "MinNumPixels",        m_minNumHits = std::vector<unsigned int>(3, 5  ) );
  declareProperty( "MinAveragePixelProb", m_minAvProb  = std::vector<double>      (3, 0.7) );
}

// Destructor
TracklessRingFilterAlg::~TracklessRingFilterAlg() {}

StatusCode TracklessRingFilterAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Input Rings                 : " << m_inputRings  << endmsg;
  info() << "Output Rings                : " << m_outputRings << endmsg;
  info() << "Min. # pixels per ring      : " << m_minNumHits  << endmsg;
  info() << "Min. average prob.          : " << m_minAvProb   << endmsg;

  return sc;
}

StatusCode TracklessRingFilterAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Load the input rings
  const LHCb::RichRecRings * inrings = get<LHCb::RichRecRings>(m_inputRings);
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << inrings->size() << " input rings at " << m_inputRings << endmsg;

  // Create a container for the output rings
  LHCb::RichRecRings * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Count Rings per radiator
  typedef Rich::Map<Rich::RadiatorType,unsigned int> RadCount;
  RadCount rCount;
  rCount[Rich::Aerogel]  = 0;
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

    // radiator
    const Rich::RadiatorType rad = (*iRing)->radiator();
    
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Ring " << (*iRing)->key() 
                << " av prob = " << avProb
                << " nPixels = " << nPixels << endmsg;
 
    if ( avProb < m_minAvProb[rad]   ||
         nPixels < m_minNumHits[rad] ) continue;

    // If get here, ring is selected so clone and save
    ++rCount[rad];
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
