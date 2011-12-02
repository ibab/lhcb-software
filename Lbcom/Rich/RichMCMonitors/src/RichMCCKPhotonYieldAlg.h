
//-----------------------------------------------------------------------------
/** @file RichMCCKPhotonYieldAlg.h
 *
 * Header file for monitor algorithm Rich::MC::MCCKPhotonYieldAlg
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
#include "Event/MCRichTrack.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichStatDivFunctor.h"

// tool Interfaces
#include "MCInterfaces/IRichMCTruthTool.h"

// boost
#include "boost/assign/list_of.hpp"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @class MCCKPhotonYieldAlg RichMCCKPhotonYieldAlg.h
     *
     *  Monitor algorithm to study the RICH CK photon yield using
     *  only MCRichHit and MCParticle data objects
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2006-11-03
     */
    //-----------------------------------------------------------------------------

    class MCCKPhotonYieldAlg : public Rich::HistoAlgBase
    {

    public:

      /// Standard constructor
      MCCKPhotonYieldAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~MCCKPhotonYieldAlg( ); ///< Destructor

      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

    private: // utility classes

      /// Tally class to store the number of photons for a number of tracks
      class TrackPhotonTally
      {
      public:
        /// Constructor with optional number of photons and tracks
        TrackPhotonTally( const unsigned long long tracks  = 0,
                          const unsigned long long photons = 0 )
          : nTracks(tracks), nPhotons(photons) { }
        unsigned long long nTracks;   ///< tally for the number of tracks
        unsigned long long nPhotons;  ///< tally for the number of photons
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

    private: // methods

      /// Access the MC Truth Tool on-demand
      inline const Rich::MC::IMCTruthTool * mcTruthTool() const
      {
        if ( !m_mcTruth ) { acquireTool( "RichMCTruthTool", m_mcTruth, 0, true ); }
        return m_mcTruth;
      }

    private: // data

      /// Pointer to MCtruth association tool
      mutable const Rich::MC::IMCTruthTool * m_mcTruth;

      /// Location of MCRichHits to analyse
      std::string m_mcRichHitsLoc;

      // selection cuts
      std::vector<double> m_minP; ///< Min momentum per radiator
      std::vector<double> m_maxP; ///< Max momentum per radiator

      std::vector<double> m_minEntryR; ///< Minimum radiator entry R ( (x*x+y*y)^0.5 )
      std::vector<double> m_minExitR;  ///< Minimum radiator exit R ( (x*x+y*y)^0.5 )
      std::vector<double> m_minEntryX; ///< Minimum radiator entry X
      std::vector<double> m_minExitX;  ///< Minimum radiator exit X
      std::vector<double> m_minEntryY; ///< Minimum radiator entry X
      std::vector<double> m_minExitY;  ///< Minimum radiator exit X

      std::vector<double> m_maxEntryR; ///< Maximum radiator entry R ( (x*x+y*y)^0.5 )
      std::vector<double> m_maxExitR;  ///< Maximum radiator exit R ( (x*x+y*y)^0.5 )
      std::vector<double> m_maxEntryX; ///< Maximum radiator entry X
      std::vector<double> m_maxExitX;  ///< Maximum radiator exit X
      std::vector<double> m_maxEntryY; ///< Maximum radiator entry X
      std::vector<double> m_maxExitY;  ///< Maximum radiator exit X

      std::vector<double> m_minPathLength; ///< Minimum path length in the radiator volume
      std::vector<double> m_maxPathLength; ///< Maximum path length in the radiator volume

      std::vector<double> m_minMRAD;  ///< Minimum radiator slope (mrad)
      std::vector<double> m_maxMRAD;   ///< Maximum radiator slope (mrad)

      // Track and photon tally for all events
      RichRadCount m_signalRadHits;

    };

  }
} // RICH namespace

#endif // RICHMCMONITORS_RichMCCKPhotonYieldAlg_H
