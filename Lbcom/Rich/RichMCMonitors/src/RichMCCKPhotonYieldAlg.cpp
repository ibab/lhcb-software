
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonYieldAlg.h
 *
 * Header file for monitor algorithm RichMCCKPhotonYieldAlg
 *
 * CVS Log :-
 * $Id: RichMCCKPhotonYieldAlg.cpp,v 1.1.1.1 2006-11-07 11:56:23 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichMCCKPhotonYieldAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Rich, RichMCCKPhotonYieldAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichMCCKPhotonYieldAlg::RichMCCKPhotonYieldAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : RichHistoAlgBase ( name , pSvcLocator      ),
    m_minP           ( Rich::NRadiatorTypes, 0 ),
    m_maxP           ( Rich::NRadiatorTypes, 0 )
{
  // job options
  declareProperty( "MCRichHitsLocation",
                   m_mcRichHitsLoc = LHCb::MCRichHitLocation::Default );

  // Min MCParticle momentum
  m_minP[Rich::Aerogel]  = 50 * Gaudi::Units::GeV;
  m_minP[Rich::Rich1Gas] = 80 * Gaudi::Units::GeV;
  m_minP[Rich::Rich2Gas] = 80 * Gaudi::Units::GeV;
  declareProperty( "MinParticleMomentum", m_minP );

  // Max MCParticle momentum
  m_maxP[Rich::Aerogel]  = 999 * Gaudi::Units::GeV;
  m_maxP[Rich::Rich1Gas] = 999 * Gaudi::Units::GeV;
  m_maxP[Rich::Rich2Gas] = 999 * Gaudi::Units::GeV;
  declareProperty( "MaxParticleMomentum", m_maxP );

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

  // now, loop over the MCParticles that where found
  debug() << "Found " << partsToHits.size() << " MCParticles with MCRichHits" << endmsg;
  for ( MCParticle2RichHits::const_iterator iP2R = partsToHits.begin();
        iP2R != partsToHits.end(); ++iP2R )
  {
    // Pointer to the MCParticle
    const LHCb::MCParticle * mcpart = (*iP2R).first.first;
    // The radiator for this set of hits
    const Rich::RadiatorType rad = (*iP2R).first.second;

    // some track selection cuts
    // ------------------------------------------------------------------------------
    // momentum cuts
    if ( mcpart->p() < m_minP[rad] || mcpart->p() > m_maxP[rad] ) continue;
    // ------------------------------------------------------------------------------

    // loop over the MCRichHits for this MCParticle
    const MCRichHitVector & mchits = (*iP2R).second;
    unsigned int nSignalHits(0);
    debug() << " -> Found " << mchits.size() << " MCRichHits for MCParticle "
            << mcpart->key() << " in " << rad << endreq;
    for ( MCRichHitVector::const_iterator iH = mchits.begin();
          iH != mchits.end(); ++iH )
    {
      // count signal only hits
      if ( !(*iH)->isBackground() ) { ++nSignalHits; }
    }
    debug() << "  -> Found " << nSignalHits << " " << rad << " signal hits" << endreq;

    // only consided rads and MCParticles with at least one 'signal' hit
    if ( nSignalHits>0 )
    {
      // histogram the signal hits
      plot1D( nSignalHits, hid(rad,"SignalPhotons"),
              "Signal Photons / MCParticle", -0.5, 100.5, 101 );
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
  info() << " Track Selection : Min. Ptot. (aero/R1Gas/R2Gas) = " 
         << m_minP << " MeV/c" << endreq;
  info() << "                 : Max. Ptot. (aero/R1Gas/R2Gas) = " 
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
