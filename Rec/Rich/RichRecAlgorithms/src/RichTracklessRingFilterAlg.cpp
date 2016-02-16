
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
: Rich::Rec::AlgBase ( name, pSvcLocator ),
  m_minNumHits       ( 3, 5   ),
  m_minAvProb        ( 3, 0.7 )
{
  declareProperty( "InputRings",  
                   m_inputRings  = LHCb::RichRecRingLocation::MarkovRings+"All"  );
  declareProperty( "OutputRings", 
                   m_outputRings = LHCb::RichRecRingLocation::MarkovRings+"Best" );
  declareProperty( "MinNumPixels",        m_minNumHits );
  declareProperty( "MinAveragePixelProb", m_minAvProb  );
}

// Destructor
TracklessRingFilterAlg::~TracklessRingFilterAlg() {}

StatusCode TracklessRingFilterAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  _ri_debug << "Input Rings                 : " << m_inputRings  << endmsg;
  _ri_debug << "Output Rings                : " << m_outputRings << endmsg;
  _ri_debug << "Min. # pixels per ring      : " << m_minNumHits  << endmsg;
  _ri_debug << "Min. average prob.          : " << m_minAvProb   << endmsg;

  return sc;
}

StatusCode TracklessRingFilterAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Load the input rings
  const auto * inrings = get<LHCb::RichRecRings>(m_inputRings);
  _ri_debug << "Found " << inrings->size() << " input rings at " << m_inputRings << endmsg;

  // Create a container for the output rings
  auto * outrings = new LHCb::RichRecRings();
  put ( outrings, m_outputRings );

  // Count Rings per radiator
  typedef Rich::Map<Rich::RadiatorType,unsigned int> RadCount;
  RadCount rCount;
  rCount[Rich::Aerogel]  = 0;
  rCount[Rich::Rich1Gas] = 0;
  rCount[Rich::Rich2Gas] = 0;

  // Loop over the input rings and select the 'best' ones
  for ( const auto ring : *inrings )
  {
    // average ring hit prob
    const auto avProb = ring->averagePixelProb();
    // Number of pixels on the ring
    const auto nPixels = ring->richRecPixels().size();

    // radiator
    const auto rad = ring->radiator();
    
    _ri_verbo << " -> Ring " << ring->key() 
              << " av prob = " << avProb
              << " nPixels = " << nPixels << endmsg;
 
    if ( avProb < m_minAvProb[rad]   ||
         nPixels < m_minNumHits[rad] ) continue;

    // If get here, ring is selected so clone and save
    ++rCount[rad];
    outrings->insert( new LHCb::RichRecRing(*ring), ring->key() );
  }
  
  _ri_debug << "Selected " << outrings->size() << " rings at " << m_outputRings << endmsg;

  for ( const auto & rad : rCount )
  {
    counter("Selected good "+Rich::text(rad.first)+" rings") += rad.second;
  }

  return StatusCode::SUCCESS;
}
