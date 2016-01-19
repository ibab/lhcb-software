
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
  : HistoAlgBase ( name, pSvcLocator ),
    m_minBeta    ( Rich::NRadiatorTypes, 0.999 )
{
  // Declare job options
  declareProperty( "SummaryLocation",   
                   m_summaryLoc = contextSpecificTES(LHCb::RichSummaryTrackLocation::Default) );
  declareProperty( "MinBeta",           m_minBeta    );
  // histo ranges
  declareProperty( "CKResHistoRange", m_ckResRange = { 0.01, 0.005, 0.0025 } );
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
  const auto * sumTracks = getIfExists<LHCb::RichSummaryTracks>(m_summaryLoc);
  if ( !sumTracks )
  {
    return Warning( "No RichSummaryTracks at '"+m_summaryLoc+"'", StatusCode::SUCCESS );
  }

  // Is MC available
  const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
  const bool mcRICHOK  = richRecMCTool()->pixelMCHistoryAvailable();

  // Rich Histo ID
  const Rich::HistoID hid;

  // loop over the summary tracks
  for ( const auto * tk : *sumTracks )
  {
    // apply track selection
    if ( !m_trSelector->trackSelected(tk->track()) ) continue;

    // get MCParticle for this track
    const auto * mcP = m_richRecMCTruth->mcParticle( tk->track() );

    Rich::ParticleIDType mcType = Rich::Pion; // If MC not available, assume pion
    if ( mcTrackOK )
    {
      // True particle type
      mcType = richRecMCTool()->mcParticleType( tk->track() );
      if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..
    }

    // momentum
    const double pTot = tk->track()->p();

    // beta for pion
    const double beta = m_richPartProp->beta( pTot, mcType );

    // loop over segments
    const auto & segs = tk->radSegments();
    for ( const auto & seg : segs )
    {

      // Radiator info
      const Rich::RadiatorType rad = seg.radiator();

      // only do MC counts for good tracks
      if ( beta < m_minBeta[rad] ) continue;

      // expected CK theta
      const double thetaExpTrue = seg.expectedCkTheta( mcType );

      // photons for this segment
      const auto & photons = seg.photons();
      for ( const auto & phot : photons )
      {

        // reconstructed theta
        const double thetaRec = phot.cherenkovTheta();
        // reconstructed phi
        //const double phiRec   = phot.cherenkovPhi();

        plot1D( thetaRec-thetaExpTrue,
                hid(rad,"ckResAll"), "Rec-Exp Cktheta : All photons",
                -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );

        if ( mcRICHOK )
        {
          // is this a true Cherekov photon ?
          const LHCb::MCParticle * photonParent
            = m_richRecMCTruth->trueCherenkovPhoton( mcP, phot.smartID(), rad );
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
