
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonYieldAlg.h
 *
 * Header file for monitor algorithm RichMCCKPhotonYieldAlg
 *
 * CVS Log :-
 * $Id: RichMCCKPhotonYieldAlg.cpp,v 1.5 2006-11-10 13:50:06 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichMCCKPhotonYieldAlg.h"

// units namespace
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Rich, RichMCCKPhotonYieldAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichMCCKPhotonYieldAlg::RichMCCKPhotonYieldAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  : RichHistoAlgBase ( name , pSvcLocator      ),
    m_mcTruth        ( NULL                    ),
    m_maxP           ( Rich::NRadiatorTypes, 9999*GeV ),
    m_minEntryR      ( Rich::NRadiatorTypes, 0 ),
    m_minExitR       ( Rich::NRadiatorTypes, 0 ),
    m_minEntryX      ( Rich::NRadiatorTypes, 0 ),
    m_minExitX       ( Rich::NRadiatorTypes, 0 ),
    m_minEntryY      ( Rich::NRadiatorTypes, 0 ),
    m_minExitY       ( Rich::NRadiatorTypes, 0 ),
    m_maxEntryR      ( Rich::NRadiatorTypes, 999999*cm ),
    m_maxExitR       ( Rich::NRadiatorTypes, 999999*cm ),
    m_maxEntryX      ( Rich::NRadiatorTypes, 999999*cm ),
    m_maxExitX       ( Rich::NRadiatorTypes, 999999*cm ),
    m_maxEntryY      ( Rich::NRadiatorTypes, 999999*cm ),
    m_maxExitY       ( Rich::NRadiatorTypes, 999999*cm ),
    m_minPathLength  ( Rich::NRadiatorTypes, 0 ),
    m_maxPathLength  ( Rich::NRadiatorTypes, 999999*cm )
{
  // job options
  declareProperty( "MCRichHitsLocation",
                   m_mcRichHitsLoc = LHCb::MCRichHitLocation::Default );

  // Min MCParticle momentum
  m_minP = boost::assign::list_of(50*GeV)(80*GeV)(80*GeV);
  declareProperty( "MinParticleMomentum", m_minP );

  // Max MCParticle momentum
  declareProperty( "MaxParticleMomentum", m_maxP );

  // Entry fudicial cuts
  declareProperty( "MinEntryR", m_minEntryR );
  declareProperty( "MinEntryX", m_minEntryX );
  declareProperty( "MinEntryY", m_minEntryY );
  declareProperty( "MaxEntryR", m_maxEntryR );
  declareProperty( "MaxEntryX", m_maxEntryX );
  declareProperty( "MaxEntryY", m_maxEntryY );

  // Exit fudicial cuts
  declareProperty( "MinExitR", m_minExitR );
  declareProperty( "MinExitX", m_minExitX );
  declareProperty( "MinExitY", m_minExitY );
  declareProperty( "MaxExitR", m_maxExitR );
  declareProperty( "MaxExitX", m_maxExitX );
  declareProperty( "MaxExitY", m_maxExitY );

  // path length cuts
  declareProperty( "MinRadPathLength", m_minPathLength );
  declareProperty( "MaxRadPathLength", m_maxPathLength );

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichMCCKPhotonYieldAlg::~RichMCCKPhotonYieldAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode Rich::RichMCCKPhotonYieldAlg::initialize()
{
  // must be done first
  const StatusCode sc = RichHistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // add custom initialisation stuff here

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Rich::RichMCCKPhotonYieldAlg::execute()
{

  // Load the MCRichHits that are to be analysed
  const LHCb::MCRichHits * hits = get<LHCb::MCRichHits> ( m_mcRichHitsLoc );

  // Loop over the hits and form a mapping from MCParticles to
  // MCRichHits in a signal radiator
  MCParticle2RichHits partsToHits;
  for ( LHCb::MCRichHits::const_iterator iH = hits->begin(); iH != hits->end(); ++iH )
  {
    if ( !(*iH)->mcParticle() ) continue; // skip MCRichHits with null MCP !!
    const MCPRadKey key((*iH)->mcParticle(),(*iH)->radiator());
    partsToHits[key].push_back( *iH );
  }

  // Rich Histo ID utility class
  const RichHistoID hid;

  // ranges for histograms
  const double maxRrange[]  = { 0.5*m, 0.5*m,   1.5*m   };
  const double maxPlength[] = { 60*mm, 1200*mm, 2500*mm };

  // Is extended RICH info available ? 
  const bool haveExtendedInfo = mcTruthTool()->extendedMCAvailable();
  if (haveExtendedInfo) debug() << "Found extended RICH MC" << endreq;

  // now, loop over the MCParticles that where found
  debug() << "Found " << partsToHits.size() << " MCParticles with MCRichHits" << endmsg;
  for ( MCParticle2RichHits::const_iterator iP2R = partsToHits.begin();
        iP2R != partsToHits.end(); ++iP2R )
  {
    // Pointer to the MCParticle
    const LHCb::MCParticle * const & mcpart = (*iP2R).first.first;
    // The radiator for this set of hits
    const Rich::RadiatorType rad  = (*iP2R).first.second;

    // some track selection cuts
    // ------------------------------------------------------------------------------

    // momentum cuts
    if ( mcpart->p() < m_minP[rad] || mcpart->p() > m_maxP[rad] ) continue;

    // do we have the info available to apply fudicial cuts on the radiator volume
    double entryR(-1), exitR(-1), pathLen(-1);
    if ( haveExtendedInfo )
    {
      // Get the MCRichTrack for this MCParticle
      const LHCb::MCRichTrack * mcTk    = mcTruthTool()->mcRichTrack(mcpart);
      // Get the MCRichSegment for this MCParticle and radiator
      const LHCb::MCRichSegment * mcSeg = ( mcTk ? mcTk->segmentInRad(rad) : NULL );
      // did this succeed ? If not reject this track
      if ( !mcSeg ) 
      { 
        debug() << "Failed to locate associated MCRichSegment -> reject" << endreq; 
        continue;
      }
      debug() << "Found associated MCRichSegment" << endreq;

      // shortcuts to entry and exit points
      const Gaudi::XYZPoint & entP = mcSeg->entryPoint();
      const Gaudi::XYZPoint & extP = mcSeg->exitPoint();

      // Apply some spatial cuts
      if ( fabs(entP.x()) < m_minEntryX[rad] || fabs(entP.x()) > m_maxEntryX[rad] ) continue;
      if ( fabs(extP.x()) < m_minExitX[rad]  || fabs(extP.x()) > m_maxExitX[rad]  ) continue;
      if ( fabs(entP.y()) < m_minEntryY[rad] || fabs(entP.y()) > m_maxEntryY[rad] ) continue;
      if ( fabs(extP.y()) < m_minExitY[rad]  || fabs(extP.y()) > m_maxExitY[rad]  ) continue;
      entryR = sqrt( gsl_pow_2(entP.x()) + gsl_pow_2(entP.y()) );
      if ( entryR < m_minEntryR[rad] || entryR > m_maxEntryR[rad] ) continue;
      exitR  = sqrt( gsl_pow_2(extP.x()) + gsl_pow_2(extP.y()) );
      if ( exitR < m_minExitR[rad]   || exitR > m_maxExitR[rad]   ) continue;

      // path length cuts
      pathLen = mcSeg->pathLength();
      if ( pathLen < m_minPathLength[rad] || pathLen > m_maxPathLength[rad] ) continue;

      debug() << " -> Segment selected" << endreq;
    }

    // ------------------------------------------------------------------------------

    // loop over the MCRichHits for this MCParticle/radiator pair
    const MCRichHitVector & mchits = (*iP2R).second;
    unsigned int nSignalHits(0);
    debug() << " -> Found " << mchits.size() << " MCRichHits for MCParticle "
            << mcpart->key() << " in " << rad << endreq;
    for ( MCRichHitVector::const_iterator iH = mchits.begin();
          iH != mchits.end(); ++iH )
    {
      // count only signal hits
      if ( !(*iH)->isBackground() ) { ++nSignalHits; }
    }
    debug() << "  -> Found " << nSignalHits << " " << rad << " signal hits" << endreq;

    // only consider rads and MCParticles with at least one 'signal' hit
    if ( nSignalHits>0 )
    {
      // histogram the signal hits
      plot1D( nSignalHits, hid(rad,"SigPhots"),
              "Signal Photons / MCParticle", -0.5, 100.5, 101 );
      // # Signal hits versus entry R (if available)
      if ( haveExtendedInfo )
      {
        profile1D( entryR, nSignalHits, hid(rad,"SigPhotsVentryR"), 
                   "Signal Photons / MCParticle Versus entry sqrt( x.x + y.y )",
                   0, maxRrange[rad], 100 );
        profile1D( exitR, nSignalHits, hid(rad,"SigPhotsVexitR"), 
                   "Signal Photons / MCParticle Versus exit sqrt( x.x + y.y )",
                   0, maxRrange[rad], 100 );
        profile2D( entryR, exitR, nSignalHits, hid(rad,"SigPhotsVentryRVexitR"),
                   "Signal Photons / MCParticle Versus entry and exit sqrt( x.x + y.y )",
                   0, maxRrange[rad], 0, maxRrange[rad] );
        profile1D( pathLen, nSignalHits, hid(rad,"SigPhotsVpLength"),
                   "Signal Photons / MCParticle Versus Rad. path length",
                   0, maxPlength[rad], 100 );
      }
      // count the tracks
      ++(m_signalRadHits[rad]).nTracks;
      // count the signal photons
      (m_signalRadHits[rad]).nPhotons += nSignalHits;
    }

  } // loop over MCParticles

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Rich::RichMCCKPhotonYieldAlg::finalize()
{

  // Statistical tools
  const RichStatDivFunctor eff("%8.2f +-%5.2f");

  info() << "=============================================================================="
         << endreq;

  // track selection
  info() << " Track Selection : Min. Ptot (aero/R1Gas/R2Gas) = " 
         << m_minP << " MeV/c" << endreq;
  info() << "                 : Max. Ptot (aero/R1Gas/R2Gas) = " 
         << m_maxP << " MeV/c" << endreq;

  // Summarise the photon tallies for each radiator type
  for ( RichRadCount::const_iterator iC = m_signalRadHits.begin();
        iC != m_signalRadHits.end(); ++iC )
  {
    if ( (*iC).second.nPhotons > 0 )
    {
      std::string rad(Rich::text((*iC).first));
      rad.resize(15,' ');
      info() << " " << rad << " Av. # CK photons = "
             << eff((*iC).second.nPhotons,(*iC).second.nTracks) << " photons/MCParticle" << endreq;
    }
  }

  info() << "=============================================================================="
         << endreq;

  // must be called after all other actions
  return RichHistoAlgBase::finalize();
}

//=============================================================================
