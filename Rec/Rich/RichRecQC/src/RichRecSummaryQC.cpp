
//-------------------------------------------------------------------------------
/** @file RichRecSummaryQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::SummaryQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//-------------------------------------------------------------------------------

// local
#include "RichRecSummaryQC.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( SummaryQC )

// Standard constructor, initializes variables
SummaryQC::SummaryQC( const std::string& name,
                      ISvcLocator* pSvcLocator )
  : HistoAlgBase        ( name, pSvcLocator ),
    m_richPartProp      ( 0 ),
    m_richRecMCTruth    ( 0 ),
    m_nEvts             ( 0 ),
    m_nTracks           ( 0 ),
    m_nSegments         ( Rich::NRadiatorTypes, 0 ),
    m_nSegmentsMC       ( Rich::NRadiatorTypes, 0 ),
    m_nPhotons          ( Rich::NRadiatorTypes, 0 ),
    m_nTruePhotons      ( Rich::NRadiatorTypes, 0 )
{
  // Declare job options
  declareProperty( "SummaryLocation",   
                   m_summaryLoc = contextSpecificTES(LHCb::RichSummaryTrackLocation::Default) );
  declareProperty( "MinBeta",           m_minBeta   = 0.999 );
}

// Destructor
SummaryQC::~SummaryQC() {}

// Initialisation
StatusCode SummaryQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "TrackSelector", m_trSelector, this );

  return sc;
}

// Main execution
StatusCode SummaryQC::execute()
{
  debug() << "Execute" << endmsg;

  // Try and load the Summary data
  if ( !exist<LHCb::RichSummaryTracks>(m_summaryLoc) )
  {
    return Warning( "No RichSummaryTracks at '"+m_summaryLoc+"'", StatusCode::SUCCESS );
  }
  const LHCb::RichSummaryTracks * sumTracks = get<LHCb::RichSummaryTracks>(m_summaryLoc);
  
  // temporary tallies
  unsigned int nTracks(0);
  std::vector<unsigned> nSegments(Rich::NRadiatorTypes,0), nPhotons(Rich::NRadiatorTypes,0);
  std::vector<unsigned> nTruePhotons(Rich::NRadiatorTypes,0), nSegmentsMC(Rich::NRadiatorTypes,0);

  // loop over the summary tracks
  for ( LHCb::RichSummaryTracks::const_iterator iTrack = sumTracks->begin();
        iTrack != sumTracks->end(); ++iTrack )
  {
    // apply track selection
    if ( !m_trSelector->trackSelected((*iTrack)->track()) ) continue;

    // get MCParticle for this track
    const LHCb::MCParticle * mcP = m_richRecMCTruth->mcParticle( (*iTrack)->track() );

    // True particle type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( (*iTrack)->track() );
    if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
    if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..

    // momentum
    const double pTot = (*iTrack)->track()->p();

    // beta for pion
    const double beta = m_richPartProp->beta( pTot, Rich::Pion );

    // count tracks
    ++nTracks;

    // loop over segments
    const LHCb::RichSummaryRadSegment::Vector & segs = (*iTrack)->radSegments();
    for ( LHCb::RichSummaryRadSegment::Vector::const_iterator iSeg = segs.begin();
          iSeg != segs.end(); ++iSeg )
    {

      // Radiator info
      const Rich::RadiatorType rad = (*iSeg).radiator();

      // photons for this segment
      unsigned int truePhotons = 0;
      const LHCb::RichSummaryPhoton::Vector & photons = (*iSeg).photons();
      for ( LHCb::RichSummaryPhoton::Vector::const_iterator iPhot = photons.begin();
            iPhot != photons.end(); ++iPhot )
      {

        // count all photons
        ++nPhotons[rad];

        // only do MC counts for good tracks
        if ( beta > m_minBeta )
        {
          // is this a true Cherekov photon ?
          const LHCb::MCParticle * photonParent
            = m_richRecMCTruth->trueCherenkovPhoton( mcP, (*iPhot).smartID(), rad );
          if ( photonParent )
          {
            // count true photons
            ++truePhotons;
          }
        }

      } // loop over photons

        // tallies
      ++nSegments[rad];
      if ( truePhotons>0 )
      {
        nTruePhotons[rad] += truePhotons;
        ++nSegmentsMC[rad];
      }

    } // loop over summary rad segments

  } // loop over summary tracks

    // update running tallies
  ++m_nEvts;
  m_nTracks += nTracks;
  for ( int irad = 0; irad < Rich::NRadiatorTypes; ++irad )
  {
    const Rich::RadiatorType rad = (Rich::RadiatorType)irad;
    m_nSegments[rad]    += nSegments[rad];
    m_nSegmentsMC[rad]  += nSegmentsMC[rad];
    m_nPhotons[rad]     += nPhotons[rad];
    m_nTruePhotons[rad] += nTruePhotons[rad];
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode SummaryQC::finalize()
{
  if ( m_nTracks > 0 )
  {

    // Statistical tools
    const StatDivFunctor eff("%6.2f +-%5.2f");

    const unsigned long long totSegs =
      m_nSegments[Rich::Aerogel]+m_nSegments[Rich::Rich1Gas]+m_nSegments[Rich::Rich2Gas];
    const unsigned long long totPhots =
      m_nPhotons[Rich::Aerogel]+m_nPhotons[Rich::Rich1Gas]+m_nPhotons[Rich::Rich2Gas];
    const unsigned long long totPhotsTrue =
      m_nTruePhotons[Rich::Aerogel]+m_nTruePhotons[Rich::Rich1Gas]+m_nTruePhotons[Rich::Rich2Gas];

    // print out summary info
    info() << "=======================================================================" << endmsg;
    info() << "           Rich Reconstruction Summary : " << m_nEvts << " events" << endmsg;
    info() << "-----------------------------------------------------------------------" << endmsg;

    // track selection
    info() << " Track Selection : " << m_trSelector->selectedTracks()
           << " : beta > " << m_minBeta
           << endmsg;
    info() << "-----------------------------------------------------------------------" << endmsg;

    info() << "    Created " << eff(m_nTracks,m_nEvts) << " RichSummaryTracks / event" << endmsg;

    info() << "      -> with " << eff(totSegs,m_nTracks) << " radiator segments / track" << endmsg;
    info() << "        -> " << eff(m_nSegments[Rich::Aerogel],m_nTracks)
           << " aerogel  segments / track" << endmsg;
    info() << "        -> " << eff(m_nSegments[Rich::Rich1Gas],m_nTracks)
           << " Rich1Gas segments / track" << endmsg;
    info() << "        -> " << eff(m_nSegments[Rich::Rich2Gas],m_nTracks)
           << " Rich2Gas segments / track" << endmsg;

    info() << "      -> with " << eff(totPhots,m_nTracks)  << " reco. photons / track" << endmsg;
    info() << "        -> " << eff(m_nPhotons[Rich::Aerogel],m_nSegments[Rich::Aerogel])
           << " aerogel  photons / segment" << endmsg;
    info() << "        -> " << eff(m_nPhotons[Rich::Rich1Gas],m_nSegments[Rich::Rich1Gas])
           << " Rich1Gas photons / segment" << endmsg;
    info() << "        -> " << eff(m_nPhotons[Rich::Rich2Gas],m_nSegments[Rich::Rich2Gas])
           << " Rich2Gas photons / segment" << endmsg;

    if ( totPhotsTrue>0 )
      info() << "      -> with " << eff(totPhotsTrue,m_nTracks)  << " true photons / track" << endmsg;
    if ( m_nSegmentsMC[Rich::Aerogel]>0 )
      info() << "        -> " << eff(m_nTruePhotons[Rich::Aerogel],m_nSegmentsMC[Rich::Aerogel])
             << " aerogel  photons / segment" << endmsg;
    if ( m_nSegmentsMC[Rich::Rich1Gas]>0 )
      info() << "        -> " << eff(m_nTruePhotons[Rich::Rich1Gas],m_nSegmentsMC[Rich::Rich1Gas])
             << " Rich1Gas photons / segment" << endmsg;
    if ( m_nSegmentsMC[Rich::Rich2Gas]>0 )
      info() << "        -> " << eff(m_nTruePhotons[Rich::Rich2Gas],m_nSegmentsMC[Rich::Rich2Gas])
             << " Rich2Gas photons / segment" << endmsg;

    info() << "-----------------------------------------------------------------------" << endmsg;

  }

  // Execute base class method
  return HistoAlgBase::finalize();
}
