
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonYieldAlg.cpp
 *
 * Header file for monitor algorithm RichMCCKPhotonYieldAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCCKPhotonYieldAlg.h"

// units namespace
//using namespace Gaudi::Units;
using namespace Rich::MC;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCCKPhotonYieldAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCCKPhotonYieldAlg::MCCKPhotonYieldAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::HistoAlgBase ( name , pSvcLocator      ),
    m_mcTruth        ( NULL                    ),
    m_maxP           ( Rich::NRadiatorTypes, 9999*Gaudi::Units::GeV ),
    m_minEntryR      ( Rich::NRadiatorTypes, 0 ),
    m_minExitR       ( Rich::NRadiatorTypes, 0 ),
    m_minEntryX      ( Rich::NRadiatorTypes, 0 ),
    m_minExitX       ( Rich::NRadiatorTypes, 0 ),
    m_minEntryY      ( Rich::NRadiatorTypes, 0 ),
    m_minExitY       ( Rich::NRadiatorTypes, 0 ),
    m_maxEntryR      ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_maxExitR       ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_maxEntryX      ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_maxExitX       ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_maxEntryY      ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_maxExitY       ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_minPathLength  ( Rich::NRadiatorTypes, 0 ),
    m_maxPathLength  ( Rich::NRadiatorTypes, 999999*Gaudi::Units::cm ),
    m_minMRAD        ( Rich::NRadiatorTypes, 0 ),
    m_maxMRAD        ( Rich::NRadiatorTypes, 999999 )
{
  // job options
  declareProperty( "MCRichHitsLocation",
                   m_mcRichHitsLoc = LHCb::MCRichHitLocation::Default );

  // Min MCParticle momentum
  declareProperty( "MinParticleMomentum", m_minP = std::vector<double>(3, 80*Gaudi::Units::GeV));

  // Max MCParticle momentum
  declareProperty( "MaxParticleMomentum", m_maxP );

  // Entry fudicial cuts
  declareProperty( "MinEntryR", m_minEntryR );
  declareProperty( "MinEntryX", m_minEntryX );
  declareProperty( "MinEntryY", m_minEntryY );
  declareProperty( "MaxEntryR", m_maxEntryR );
  declareProperty( "MaxEntryX", m_maxEntryX );
  declareProperty( "MaxEntryY", m_maxEntryY );

  // slope cuts
  declareProperty( "MinMRAD",   m_minMRAD   );
  declareProperty( "MaxMRAD",   m_maxMRAD   );

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
MCCKPhotonYieldAlg::~MCCKPhotonYieldAlg() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCCKPhotonYieldAlg::execute()
{

  // Load the MCRichHits that are to be analysed
  if ( !exist<LHCb::MCRichHits>(m_mcRichHitsLoc) )
  {
    return Warning( "No MCRichHits at '"+m_mcRichHitsLoc+"'", StatusCode::SUCCESS );
  }
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
  const Rich::HistoID hid;

  // ranges for histograms
  const double maxPlength[]  = { 60*Gaudi::Units::mm, 1200*Gaudi::Units::mm, 2500*Gaudi::Units::mm };
  const double maxSlope[]    = { 0.1, 0.1, 0.1 };

  // Is extended RICH info available ?
  const bool haveExtendedInfo = mcTruthTool()->extendedMCAvailable();
  if (haveExtendedInfo) debug() << "Found extended RICH MC" << endmsg;

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
    double pathLen(-1), entrySlope(-1), exitSlope(-1);
    if ( haveExtendedInfo )
    {
      // Get the MCRichTrack for this MCParticle
      const LHCb::MCRichTrack * mcTk    = mcTruthTool()->mcRichTrack(mcpart);
      // Get the MCRichSegment for this MCParticle and radiator
      const LHCb::MCRichSegment * mcSeg = ( mcTk ? mcTk->segmentInRad(rad) : NULL );
      // did this succeed ? If not reject this track
      if ( !mcSeg )
      {
        debug() << "Failed to locate associated MCRichSegment -> reject" << endmsg;
        continue;
      }
      debug() << "Found associated MCRichSegment" << endmsg;

      // shortcuts to entry and exit points
      const Gaudi::XYZPoint & entP = mcSeg->entryPoint();
      const Gaudi::XYZPoint & extP = mcSeg->exitPoint();

      // Apply some spatial cuts
      if ( fabs(entP.x()) < m_minEntryX[rad] || fabs(entP.x()) > m_maxEntryX[rad] ) continue;
      if ( fabs(extP.x()) < m_minExitX[rad]  || fabs(extP.x()) > m_maxExitX[rad]  ) continue;
      if ( fabs(entP.y()) < m_minEntryY[rad] || fabs(entP.y()) > m_maxEntryY[rad] ) continue;
      if ( fabs(extP.y()) < m_minExitY[rad]  || fabs(extP.y()) > m_maxExitY[rad]  ) continue;
      const double entryR = sqrt( gsl_pow_2(entP.x()) + gsl_pow_2(entP.y()) );
      if ( entryR < m_minEntryR[rad] || entryR > m_maxEntryR[rad] ) continue;
      const double exitR  = sqrt( gsl_pow_2(extP.x()) + gsl_pow_2(extP.y()) );
      if ( exitR < m_minExitR[rad]   || exitR > m_maxExitR[rad]   ) continue;

      // path length cuts
      pathLen = mcSeg->pathLength();
      if ( pathLen < m_minPathLength[rad] || pathLen > m_maxPathLength[rad] ) continue;

      // entry and exit slopes
      entrySlope = atan2( entryR, entP.z() );
      exitSlope  = atan2( exitR,  extP.z() );

      // slope cuts
      if ( entrySlope < m_minMRAD[rad] || entrySlope > m_maxMRAD[rad] ||
           exitSlope  < m_minMRAD[rad] || exitSlope  > m_maxMRAD[rad] ) continue;

    }
    debug() << " -> Segment selected" << endmsg;

    // ------------------------------------------------------------------------------

    // loop over the MCRichHits for this MCParticle/radiator pair
    const MCRichHitVector & mchits = (*iP2R).second;
    unsigned int nSignalHits(0);
    debug() << " -> Found " << mchits.size() << " MCRichHits for MCParticle "
            << mcpart->key() << " in " << rad << endmsg;
    for ( MCRichHitVector::const_iterator iH = mchits.begin();
          iH != mchits.end(); ++iH )
    {
      // count only signal hits
      if ( !(*iH)->isBackground() ) { ++nSignalHits; }
    }
    debug() << "  -> Found " << nSignalHits << " " << rad << " signal hits" << endmsg;

    // only consider rads and MCParticles with at least one 'signal' hit
    if ( nSignalHits>0 )
    {
      // histogram the signal hits
      plot1D( nSignalHits, hid(rad,"SigPhots"),
              "Signal Photons / MCParticle", -0.5, 100.5, 101 );

      // Slope at vertex
      const double mcSlope = atan2( fabs(mcpart->pt()), fabs(mcpart->p()) );

      // hits versus MC slope angle
      plot1D( mcSlope,hid(rad,"mcpAng"), "MCParticle atan(pt/p)",0, 0.2, 100 );
      profile1D( mcSlope, nSignalHits, hid(rad,"SigPhotsVmcpAngProf"),
                 "Signal Photons / MCParticle Versus MCP atan(pt/p)",
                 0, maxSlope[rad], 100 );
      plot2D( mcSlope, nSignalHits, hid(rad,"SigPhotsVmcpAng"),
              "Signal Photons / MCParticle Versus MCP atan(pt/p)",
              0, maxSlope[rad], -0.5, 40.5, 100, 41 );

      // Plots needing extended info
      if ( haveExtendedInfo )
      {
        profile1D( entrySlope, nSignalHits, hid(rad,"SigPhotsVentrySlopeProf"),
                   "Signal Photons / MCParticle Versus entry slope atan(R/z)",
                   0, maxSlope[rad], 100 );
        plot2D( entrySlope, nSignalHits, hid(rad,"SigPhotsVentrySlope"),
                "Signal Photons / MCParticle Versus entry slope atan(R/z)",
                0, maxSlope[rad], -0.5, 40.5, 100, 41 );
        profile1D( exitSlope, nSignalHits, hid(rad,"SigPhotsVexitSlopeProf"),
                   "Signal Photons / MCParticle Versus exit slope atan(R/z)",
                   0, maxSlope[rad], 100 );
        plot2D( exitSlope, nSignalHits, hid(rad,"SigPhotsVexitSlope"),
                "Signal Photons / MCParticle Versus exit slope atan(R/z)",
                0, maxSlope[rad],  -0.5, 40.5, 100, 41 );
        profile1D( pathLen, nSignalHits, hid(rad,"SigPhotsVpLengthProf"),
                   "Signal Photons / MCParticle Versus Rad. path length",
                   0, maxPlength[rad], 100 );
        plot2D( pathLen, nSignalHits, hid(rad,"SigPhotsVpLength"),
                "Signal Photons / MCParticle Versus Rad. path length",
                0, maxPlength[rad], -0.5, 40.5, 100, 41 );
      }

      // count the tracks
      ++(m_signalRadHits[rad]).nTracks;
      // count the signal photons
      (m_signalRadHits[rad]).nPhotons += nSignalHits;

    } // at least one signal hit

  } // loop over MCParticles

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCCKPhotonYieldAlg::finalize()
{
  const std::string & lines 
    = "===================================================================================";

  // Statistical tools
  const StatDivFunctor eff("%8.2f +-%5.2f");

  info() << lines << endmsg;

  // track selection
  info() << " Track Selection : Min. Ptot (aero/R1Gas/R2Gas) = "
         << m_minP << " MeV/c" << endmsg;
  info() << "                 : Max. Ptot (aero/R1Gas/R2Gas) = "
         << m_maxP << " MeV/c" << endmsg;
  info() << "                 : Min. Entry R (aero/R1Gas/R2Gas) = " << m_minEntryR << endmsg;
  info() << "                 : Max. Entry R (aero/R1Gas/R2Gas) = " << m_maxEntryR << endmsg;
  info() << "                 : Min. Exit  R (aero/R1Gas/R2Gas) = " << m_minExitR << endmsg;
  info() << "                 : Max. Exit  R (aero/R1Gas/R2Gas) = " << m_maxExitR << endmsg;
  info() << "                 : Min. MRAD    (aero/R1Gas/R2Gas) = " << m_minMRAD << endmsg;
  info() << "                 : Max. MRAD    (aero/R1Gas/R2Gas) = " << m_maxMRAD << endmsg;

  // Summarise the photon tallies for each radiator type
  for ( RichRadCount::const_iterator iC = m_signalRadHits.begin();
        iC != m_signalRadHits.end(); ++iC )
  {
    if ( (*iC).second.nPhotons > 0 )
    {
      std::string rad(Rich::text((*iC).first));
      rad.resize(15,' ');
      info() << " " << rad << " Av. # CK photons = "
             << eff((*iC).second.nPhotons,(*iC).second.nTracks) 
             << " photons/MCParticle" << endmsg;
    }
  }

  info() << lines << endmsg;

  // must be called after all other actions
  return HistoAlgBase::finalize();
}

//=============================================================================
