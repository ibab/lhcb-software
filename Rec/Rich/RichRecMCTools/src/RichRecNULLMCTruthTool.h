
//-----------------------------------------------------------------------------
/** @file RichRecNULLMCTruthTool.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::NULLMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichRecNULLMCTruthTool.h,v 1.1 2009-07-23 16:16:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichRecNULLMCTruthTool_H
#define RICHRECMCTOOLS_RichRecNULLMCTruthTool_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecTrack.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class NULLMCTruthTool RichRecNULLMCTruthTool.h
       *
       *  NULL MC truth tool. Always returns no associations.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/03/2002
       */
      //-----------------------------------------------------------------------------

      class NULLMCTruthTool : public Rich::Rec::ToolBase,
                              virtual public Rich::Rec::MC::IMCTruthTool
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        NULLMCTruthTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

        /// Destructor
        virtual ~NULLMCTruthTool();

      public: // Public interface methods

        // Find best MCParticle association for a given reconstructed Track
        const LHCb::MCParticle * mcParticle( const LHCb::Track * track,
                                             const double minWeight = DefMinWeightTrToMCP ) const;

        // Find best MCParticle association for a given RichRecTrack
        const LHCb::MCParticle * mcParticle( const LHCb::RichRecTrack * richTrack,
                                             const double minWeight = DefMinWeightTrToMCP ) const;

        // Find best MCParticle association for a given RichRecSegment
        const LHCb::MCParticle * mcParticle( const LHCb::RichRecSegment * richSegment,
                                             const double minWeight = DefMinWeightTrToMCP ) const;

        // Determines the particle mass hypothesis for a given reconstructed Track
        Rich::ParticleIDType mcParticleType( const LHCb::Track * track,
                                             const double minWeight = DefMinWeightTrToMCP ) const;

        // Truth particle type for given RichRecTrack
        Rich::ParticleIDType mcParticleType( const LHCb::RichRecTrack * richTrack,
                                             const double minWeight = DefMinWeightTrToMCP ) const;

        // Truth particle type for given RichRecSegment
        Rich::ParticleIDType mcParticleType( const LHCb::RichRecSegment * richSegment,
                                             const double minWeight = DefMinWeightTrToMCP ) const;

        // Find parent MCParticles associated to a given RichRecPixel
        bool mcParticle( const LHCb::RichRecPixel * richPixel,
                         std::vector<const LHCb::MCParticle*> & mcParts ) const;

        // Find parent MCRichHits for a given RichRecPixel
        void mcRichHits( const LHCb::RichRecPixel * richPixel,
                         SmartRefVector<LHCb::MCRichHit> & hits ) const;

        // Find parent MCRichOpticalPhotons associated to a given RichRecPixel
        bool mcRichOpticalPhoton( const LHCb::RichRecPixel * richPixel,
                                  SmartRefVector<LHCb::MCRichOpticalPhoton> & phots ) const;

        // Is this a true photon candidate ?
        // Do the associated segment and pixel have the same MC parent
        const LHCb::MCParticle * trueRecPhoton( const LHCb::RichRecPhoton * photon ) const;

        // Is this a true photon candidate ?
        // Do the segment and pixel have the same MC parent
        const LHCb::MCParticle * trueRecPhoton( const LHCb::RichRecSegment * segment,
                                                const LHCb::RichRecPixel * pixel ) const;

        // Is this a true photon candidate ?
        // Does the RichSmartID result from a hit from the given MCParticle
        const LHCb::MCParticle * trueRecPhoton( const LHCb::MCParticle * mcPart,
                                                const LHCb::RichSmartID id ) const;

        // Is this a true photon candidate ?
        const LHCb::MCParticle * trueRecPhoton( const LHCb::MCParticle * mcPart,
                                                const Rich::HPDPixelCluster& cluster ) const;

        // Returns the associated MCRichHit if given RichRecPhoton is true (null otherwise)
        const LHCb::MCRichHit * trueCherenkovHit( const LHCb::RichRecPhoton * photon ) const;

        // Returns the associated MCRichOpticalPhoton if given RichRecPhoton is true (null otherwise)
        const LHCb::MCRichOpticalPhoton * trueOpticalPhoton( const LHCb::RichRecPhoton * photon ) const;

        // Returns the associated MCRichOpticalPhoton if given available for given segment and pixel
        const LHCb::MCRichOpticalPhoton * trueOpticalPhoton( const LHCb::RichRecSegment * segment,
                                                             const LHCb::RichRecPixel * pixel ) const;

        // Is this a true Cherenkov photon candidate ?
        // Do the associated segment and pixel have the same MC parent AND was the pixel
        // the result of Cherenkov radiation from the relevant radiator
        const LHCb::MCParticle * trueCherenkovPhoton( const LHCb::RichRecPhoton * photon ) const;

        // Is this a true Cherenkov photon candidate ?
        // Do the segment and pixel have the same MC parent AND was the pixel
        // the result of Cherenkov radiation from the relevant radiator
        const LHCb::MCParticle * trueCherenkovPhoton( const LHCb::RichRecSegment * segment,
                                                      const LHCb::RichRecPixel * pixel ) const;

        // Is this a true Cherenkov photon candidate ?
        //  Does the RichSmartID result from a hit from the given MCParticle AND was the hit
        //  the result of Cherenkov radiation from the relevant radiator
        const LHCb::MCParticle * trueCherenkovPhoton( const LHCb::MCParticle * mcPart,
                                                      const LHCb::RichSmartID id,
                                                      const Rich::RadiatorType rad ) const;

        // Is the hit due to Cherenkov radiation for given radiator medium ?
        const LHCb::MCParticle * trueCherenkovRadiation( const LHCb::RichRecPixel * pixel,
                                                         const Rich::RadiatorType rad ) const;

        // Is the RichSmartID due to true MC Cherenkov radiation from given radiator medium ?
        const LHCb::MCParticle * trueCherenkovRadiation( const LHCb::RichSmartID id,
                                                         const Rich::RadiatorType rad ) const;

        // Is any of the RichSmartIDs in nthe cluster due to true MC Cherenkov radiation from given radiator medium
        const LHCb::MCParticle * trueCherenkovRadiation( const Rich::HPDPixelCluster& cluster,
                                                         const Rich::RadiatorType rad ) const;

        // Returns the MCRichSegment associated to a given RichRecSegment
        const LHCb::MCRichSegment * mcRichSegment( const LHCb::RichRecSegment * segment ) const;

        // Returns the MCRichTrack associated to a given RichRecTrack
        const LHCb::MCRichTrack * mcRichTrack( const LHCb::RichRecTrack * track ) const;

        // Returns the MCRichTrack associated to a given RichRecSegment
        const LHCb::MCRichTrack * mcRichTrack( const LHCb::RichRecSegment * segment ) const;

        // Returns pointer to vector of MCRichSegments associated to a given RichRecTrack
        const SmartRefVector<LHCb::MCRichSegment> * mcRichSegments( const LHCb::RichRecTrack * track ) const;

        // Access the RichRecRing representing the MC CK ring for te given RichRecSegment
        const LHCb::RichRecRing * mcCKRing( const LHCb::RichRecSegment * segment ) const;

        // Access the MCParticle associated to a given RichRecRing
        MCPartAssocInfo mcParticle( const LHCb::RichRecRing * ring,
                                    const double assocFrac = 0.75 ) const;

        // Is this RichRecPixel background ?
        bool isBackground( const LHCb::RichRecPixel * pixel ) const;

        // Checks if Track -> MCparticle association is available
        bool trackToMCPAvailable() const;

        // Checks if RichRecPixel MC history is available
        bool pixelMCHistoryAvailable() const;

        // Checks if MC association information for RichRecPhotons is available
        bool photonMCAssocAvailable() const;

        // Checks if RICH extended MC information (MCRichOpticalPhoton, MCRichSegment etc.)
        bool extendedMCAvailable() const;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichRecNULLMCTruthTool_H
