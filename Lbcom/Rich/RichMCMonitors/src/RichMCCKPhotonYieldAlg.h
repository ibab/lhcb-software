
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonYieldAlg.h
 *
 * Header file for monitor algorithm RichMCCKPhotonYieldAlg
 *
 * CVS Log :-
 * $Id: RichMCCKPhotonYieldAlg.h,v 1.1.1.1 2006-11-07 11:56:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-03
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCMONITORS_RichMCCKPhotonYieldAlg_H
#define RICHMCMONITORS_RichMCCKPhotonYieldAlg_H 1

// base class
#include "RichKernel/RichHistoAlgBase.h"

// MCEvent
#include "Event/MCRichHit.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class RichMCCKPhotonYieldAlg RichMCCKPhotonYieldAlg.h
   *
   *  Monitor algorithm to study the RICH CK photon yield using
   *  only MCRichHit and MCParticle data objects
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2006-11-03
   */
  //-----------------------------------------------------------------------------

  class RichMCCKPhotonYieldAlg : public RichHistoAlgBase
  {

  public:

    /// Standard constructor
    RichMCCKPhotonYieldAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~RichMCCKPhotonYieldAlg( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private: // utility classes

    /// Tally class for photons per class
    class TrackPhotonTally
    {
    public:
      TrackPhotonTally( const unsigned int tracks  = 0,
                        const unsigned int photons = 0 )
        : nTracks(tracks), nPhotons(photons) { }
      long unsigned int nTracks;
      long unsigned int nPhotons;
    };

  private: // definitions

    /// A vector of pointers to MCRichHits
    typedef std::vector<const LHCb::MCRichHit *> MCRichHitVector;
    /// MCParticle/radiator key
    typedef std::pair<const LHCb::MCParticle *, Rich::RadiatorType> MCPRadKey;
    /// Mapping from MCParticles to MCRichHits
    typedef Rich::Map<MCPRadKey, MCRichHitVector> MCParticle2RichHits;
    /// Tally for number of hits in each radiator
    typedef Rich::Map<Rich::RadiatorType,TrackPhotonTally> RichRadCount;

  private: // data

    /// Location of MCRichHits to analyse
    std::string m_mcRichHitsLoc;

    // selection cuts
    std::vector<double> m_minP; ///< Min momentum per radiator
    std::vector<double> m_maxP; ///< Max momentum per radiator

    // Track and photon tally for all events
    RichRadCount m_signalRadHits;

  };

} // RICH namespace

#endif // RICHMCMONITORS_RichMCCKPhotonYieldAlg_H
