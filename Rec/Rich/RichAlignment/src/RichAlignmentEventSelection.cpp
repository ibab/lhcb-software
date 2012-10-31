
// local
#include "RichAlignmentEventSelection.h"

// namespaces
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( EventSelection )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventSelection::EventSelection( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : AlgBase             ( name , pSvcLocator ),
    m_rich              ( Rich::InvalidDetector ),
    m_pType             ( Rich::Unknown ),
    m_pTypes            ( 7, 0 ),
    m_trSelector        ( NULL ),
    m_ckAngle           ( NULL )
{
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks",    m_maxUsedTracks = 200 );
  declareProperty( "DeltaThetaRange",     m_deltaThetaRange = 0.004 );
  declareProperty( "ParticleType",        m_particleType = 2 ); // pion
  declareProperty( "RichDetector",        m_richTemp = 1 ); // default is Rich2
}

//=============================================================================
// Destructor
//=============================================================================
EventSelection::~EventSelection() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode EventSelection::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "RichCherenkovAngle",   m_ckAngle  );
  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );

  m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
  debug() << "Fixed particle type:" << m_pType << endmsg;
  m_rich = static_cast<Rich::DetectorType>(m_richTemp);
  debug() << "Detector:" << m_rich << endmsg;

  debug() << "Finished Initialization" << endmsg;
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventSelection::execute()
{

  debug() << "Execute" << endreq;

  // clear count maps for this event
  m_combiMap.clear();

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If any containers are empty, form them
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  }

  if ( (int)richTracks()->size() > m_maxUsedTracks )
  {
    return StatusCode::SUCCESS;
  }

  if ( richPixels()->empty() )
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  }

  if ( richPhotons()->empty() )
  {
    photonCreator()->reconstructPhotons();
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment* segment = *iSeg;

    const Rich::DetectorType rich = segment->trackSegment().rich();

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    if (rad == Rich::Aerogel || rich != m_rich) continue;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    double thetaExpected(0.0);

    thetaExpected =  m_ckAngle->avgCherenkovTheta( segment, m_pType);

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Found " << segment->richRecPhotons().size()
              << " in this segment" << endreq;

    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton* photon = *iPhot;

      // get the geometrical photon
      const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

      // Cherenkov angles
      const double thetaRec = gPhoton.CherenkovTheta();
      if ( !gPhoton.primaryMirror() || !gPhoton.secondaryMirror() )
      {
        Error( "Mirror information not set in photon !" );
        continue;
      }
      const MirrorNumber& sphMirNum  = gPhoton.primaryMirror()->mirrorNumber();
      const MirrorNumber& flatMirNum = gPhoton.secondaryMirror()->mirrorNumber();

      const double delTheta = thetaRec - thetaExpected;
      if ( fabs(delTheta) > m_deltaThetaRange ) continue;

      const bool unAmbiguousPhoton = photon->geomPhoton().unambiguousPhoton();

      if ( !unAmbiguousPhoton ) { continue; }

      // mirror combination number
      const unsigned long long key = (100000*(unsigned long long)sphMirNum) + flatMirNum;

      // count the photons in each of the mirror combinations
      ++m_combiMap[key];

    }// end of photons loop
  }// end of loop over segments

  //event selection
  bool ok = false;

  for ( IDToCount::const_iterator iC = m_combiMap.begin();
        iC != m_combiMap.end(); ++iC )
  {
    if ( iC->second >= 10 && m_totalMap[iC->first] <100 )
    {
      ok = true;
      break;
    }
  }

  if ( ok )
  {
    //need to add the photons in this event to the total photon arrays
    //only if the event passed!!!!
    for ( IDToCount::const_iterator iC = m_combiMap.begin();
          iC != m_combiMap.end(); ++iC )
    {
      m_totalMap[iC->first] += iC->second;
    }
  }

  setFilterPassed(ok);

  if ( msgLevel(MSG::DEBUG) )
  {
    if(ok)
    {
      debug() << "event passed" << endmsg;
    }
    else
    {
      debug() << "event didn't pass" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
