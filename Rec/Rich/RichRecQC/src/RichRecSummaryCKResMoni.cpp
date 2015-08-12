
//-------------------------------------------------------------------------------
/** @file RichRecSummaryCKResMoni.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::SummaryCKResMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//-------------------------------------------------------------------------------

// local
#include "RichRecSummaryCKResMoni.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( SummaryCKResMoni )

// Standard constructor, initializes variables
SummaryCKResMoni::SummaryCKResMoni( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : HistoAlgBase        ( name, pSvcLocator ),
    m_richPartProp      ( 0 ),
    m_richRecMCTruth    ( 0 ),
    m_minBeta           ( Rich::NRadiatorTypes, 0.999 )
{
  // Declare job options
  declareProperty( "SummaryLocation",   
                   m_summaryLoc = contextSpecificTES(LHCb::RichSummaryTrackLocation::Default) );
  declareProperty( "MinBeta",           m_minBeta    );
  // histo ranges
  std::vector<double> tmp = boost::assign::list_of(0.01)(0.005)(0.0025);
  declareProperty( "CKResHistoRange", m_ckResRange = tmp);
}

// Destructor
SummaryCKResMoni::~SummaryCKResMoni() {}

// Initialisation
StatusCode SummaryCKResMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "TrackSelector", m_trSelector, this );

  return sc;
}

// Main execution
StatusCode SummaryCKResMoni::execute()
{
  // Try and load the Summary data
  if ( !exist<LHCb::RichSummaryTracks>(m_summaryLoc) )
  {
    return Warning( "No RichSummaryTracks at '"+m_summaryLoc+"'", StatusCode::SUCCESS );
  }
  const LHCb::RichSummaryTracks * sumTracks = get<LHCb::RichSummaryTracks>(m_summaryLoc);

  // Is MC available
  const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
  const bool mcRICHOK  = richRecMCTool()->pixelMCHistoryAvailable();

  // Rich Histo ID
  const Rich::HistoID hid;

  // loop over the summary tracks
  for ( LHCb::RichSummaryTracks::const_iterator iTrack = sumTracks->begin();
        iTrack != sumTracks->end(); ++iTrack )
  {
    // apply track selection
    if ( !m_trSelector->trackSelected((*iTrack)->track()) ) continue;

    // get MCParticle for this track
    const LHCb::MCParticle * mcP = m_richRecMCTruth->mcParticle( (*iTrack)->track() );

    Rich::ParticleIDType mcType = Rich::Pion; // If MC not available, assume pion
    if ( mcTrackOK )
    {
      // True particle type
      mcType = richRecMCTool()->mcParticleType( (*iTrack)->track() );
      if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..
    }

    // momentum
    const double pTot = (*iTrack)->track()->p();

    // beta for pion
    const double beta = m_richPartProp->beta( pTot, mcType );

    // loop over segments
    const LHCb::RichSummaryRadSegment::Vector & segs = (*iTrack)->radSegments();
    for ( LHCb::RichSummaryRadSegment::Vector::const_iterator iSeg = segs.begin();
          iSeg != segs.end(); ++iSeg )
    {

      // Radiator info
      const Rich::RadiatorType rad = (*iSeg).radiator();

      // only do MC counts for good tracks
      if ( beta < m_minBeta[rad] ) continue;

      // expected CK theta
      const double thetaExpTrue = (*iSeg).expectedCkTheta( mcType );

      // photons for this segment
      const LHCb::RichSummaryPhoton::Vector & photons = (*iSeg).photons();
      for ( LHCb::RichSummaryPhoton::Vector::const_iterator iPhot = photons.begin();
            iPhot != photons.end(); ++iPhot )
      {

        // reconstructed theta
        const double thetaRec = (*iPhot).cherenkovTheta();
        // reconstructed phi
        //const double phiRec   = (*iPhot).cherenkovPhi();

        plot1D( thetaRec-thetaExpTrue,
                hid(rad,"ckResAll"), "Rec-Exp Cktheta : All photons",
                -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );

        if ( mcRICHOK )
        {
          // is this a true Cherekov photon ?
          const LHCb::MCParticle * photonParent
            = m_richRecMCTruth->trueCherenkovPhoton( mcP, (*iPhot).smartID(), rad );
          if ( photonParent )
          {
            plot1D( thetaRec-thetaExpTrue,
                    hid(rad,"ckResTrue"), "Rec-Exp Cktheta : MC true photons",
                    -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );
          }
        }

      } // loop over photons

    } // loop over summary rad segments

  } // loop over summary tracks

  return StatusCode::SUCCESS;
}
