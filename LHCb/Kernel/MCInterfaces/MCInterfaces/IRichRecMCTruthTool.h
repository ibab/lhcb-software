
//-----------------------------------------------------------------------------
/** @file IRichRecMCTruthTool.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IMCTruthTool
 *
 *  $Id: IRichRecMCTruthTool.h,v 1.2 2007-06-01 09:16:38 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Gaudi
#include "GaudiKernel/SmartRefVector.h"

// Kernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

// Event Model
namespace LHCb
{
  class MCRichOpticalPhoton;
  class MCRichSegment;
  class MCRichTrack;
  class MCRichDigit;
  class MCParticle;
  class MCRichHit;
  class RichDigit;
  class Track;
  class RichRecTrack;
  class RichRecSegment;
  class RichRecPhoton;
  class RichRecPixel;
  class RichRecRing;
}
namespace Rich
{
  class HPDPixelCluster;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRecMCTruthTool( "Rich::Rec::IMCTruthTool", 1, 0 );

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /// Default minimum weight for track to MCParticle associations
      static const double DefMinWeightTrToMCP = 0.50;

      //-----------------------------------------------------------------------------
      /** @class IMCTruthTool IRichRecMCTruthTool.h
       *
       *  Interface for tool performing MC truth associations between Rich
       *  reconstruction event objects
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/03/2002
       */
      //-----------------------------------------------------------------------------

      class IMCTruthTool : public virtual IAlgTool
      {

      public:

        /** static interface identification
         *  @return unique interface identifier
         */
        static const InterfaceID& interfaceID() { return IID_IRichRecMCTruthTool; }

        /** Find best MCParticle association for a given reconstructed Track
         *
         *  @param track Pointer to a Track
         *  @param minWeight The minimum association weight to accept for a MCParticle
         *
         *  @return Pointer to MCParticle
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Association was successful
         */
        virtual const LHCb::MCParticle *
        mcParticle ( const LHCb::Track * track,
                     const double minWeight = DefMinWeightTrToMCP ) const = 0;

        /** Find the best MCParticle association for a given RichRecTrack
         *
         *  @param richTrack Pointer to the RichRecTrack
         *  @param minWeight The minimum association weight to accept for a MCParticle
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        mcParticle( const LHCb::RichRecTrack * richTrack,
                    const double minWeight = DefMinWeightTrToMCP ) const = 0;

        /** Find the best MCParticle association for a given RichRecSegment
         *
         *  @param richSegment Pointer to the RichRecSegment
         *  @param minWeight The minimum association weight to accept for a MCParticle
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        mcParticle( const LHCb::RichRecSegment * richSegment,
                    const double minWeight = DefMinWeightTrToMCP ) const = 0;

        /** Determines the particle mass hypothesis for a given reconstructed Track
         *
         *  @param track Pointer to a Track
         *  @param minWeight The minimum association weight to accept for a MCParticle
         *
         *  @return The true particle type of the TrStoredTrack
         */
        virtual Rich::ParticleIDType
        mcParticleType ( const LHCb::Track * track,
                         const double minWeight = DefMinWeightTrToMCP ) const = 0;

        /** Truth particle type for given RichRecTrack
         *
         *  @param richTrack Pointer to the RichRecTrack
         *  @param minWeight The minimum association weight to accept for a MCParticle
         *
         *  @return True Particle type
         *  @retval Rich::Unknown if no Monte Carlo association was possible
         */
        virtual Rich::ParticleIDType
        mcParticleType( const LHCb::RichRecTrack * richTrack,
                        const double minWeight = DefMinWeightTrToMCP ) const = 0;

        /** Truth particle type for given RichRecSegment
         *
         *  @param richSegment Pointer to the RichRecSegment
         *  @param minWeight The minimum association weight to accept for a MCParticle
         *
         *  @return True Particle type
         *  @retval Rich::Unknown if no Monte Carlo association was possible
         */
        virtual Rich::ParticleIDType
        mcParticleType( const LHCb::RichRecSegment * richSegment,
                        const double minWeight = DefMinWeightTrToMCP ) const = 0;

        /** Find the parent MCParticles associated to a given RichRecPixel
         *
         *  @param richPixel Pointer to a RichRecPixel
         *  @param mcParts Vector of asscoiated MCparticle pointers to be filled
         *
         *  @return status of the Monte Carloassociation
         *  @retval true  Monte Carlo association was successful
         *  @retval false No Monte Carlo association was possible
         */
        virtual bool mcParticle( const LHCb::RichRecPixel * richPixel,
                                 std::vector<const LHCb::MCParticle*> & mcParts ) const = 0;

        /** Find the parent MCRichDigit association for a given RichRecPixel
         *
         *  @param richPixel Pointer to RichRecPixel
         *
         *  @return Pointer to associated MCRichDigit
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        //virtual const LHCb::MCRichDigit *
        //mcRichDigit( const LHCb::RichRecPixel * richPixel ) const = 0;

        /** Find the parent MCRichHits for a given RichRecPixel
         *
         *  @param richPixel Pointer to a RichRecPixel
         *  @param hits      The associated hits
         *
         *  @return Reference to the SmartRefVector of associated MCRichHits
         */
        virtual void
        mcRichHits( const LHCb::RichRecPixel * richPixel,
                    SmartRefVector<LHCb::MCRichHit> & hits ) const = 0;

        /** Find the parent MCRichOpticalPhotons associated to a given RichRecPixel
         *
         *  @param richPixel Pointer to a RichRecPixel
         *  @param phots Reference to the SmartRefVector of associated MCRichOpticalPhotons
         *
         *  @return status of the Monte Carlo association
         *  @retval true  Monte Carlo association was successful
         *  @retval false No Monte Carlo association was possible
         */
        virtual bool
        mcRichOpticalPhoton( const LHCb::RichRecPixel * richPixel,
                             SmartRefVector<LHCb::MCRichOpticalPhoton> & phots ) const = 0;

        /** Is this a true photon candidate ? Do the associated RichRecSegment and RichRecPixel
         *  used to form the given RichRecPhoton candidate have the same MCParticle parent.
         *
         *  @param photon Pointer to a RichRecPhoton
         *
         *  @return Pointer to an MCParticle
         *  @retval NULL No Monte Carlo association was possible,
         *          or the segment and pixel do not share the same parent
         *  @retval !NULL Pointer to the shared MCParticle parent
         */
        virtual const LHCb::MCParticle *
        trueRecPhoton( const LHCb::RichRecPhoton * photon ) const = 0;

        /** Access the associated MCRichHit if given RichRecPhoton is a true Cherenkov photon
         *
         *  @param photon Pointer to a RichRecPhoton
         *
         *  @return Pointer to associated MCRichHit
         *  @retval NULL No Monte Carlo association was possible,
         *          or the photon is not a true Cherenkov photon
         *  @retval !NULL Pointer to the associated MCRichHit
         */
        virtual const LHCb::MCRichHit *
        trueCherenkovHit( const LHCb::RichRecPhoton * photon ) const = 0;

        /** Access the associated MCRichOpticalPhoton if given RichRecPhoton
         *  is a true Cherenkov photon
         *
         *  @param photon Pointer to a RichRecPhoton
         *
         *  @return Pointer to associated MCRichOpticalPhoton
         *  @retval NULL No Monte Carlo association was possible,
         *          or the photon is not a true Cherenkov photon
         *  @retval !NULL Pointer to the associated MCRichOpticalPhoton
         */
        virtual const LHCb::MCRichOpticalPhoton *
        trueOpticalPhoton( const LHCb::RichRecPhoton * photon ) const = 0;

        /** Access the associated MCRichOpticalPhoton if given RichRecSegment and RichRexPixel
         *  are both associated a single Cherenkov poton
         *
         *  @param segment Pointer to a RichRecSegment
         *  @param pixel   Pointer to a RichRecPixel
         *
         *  @return Pointer to associated MCRichOpticalPhoton
         *  @retval NULL No Monte Carlo association was possible,
         *          or the given segment and pixel are not associated to a single Cherenkov photon
         *  @retval !NULL Pointer to the associated MCRichOpticalPhoton
         */
        virtual const LHCb::MCRichOpticalPhoton *
        trueOpticalPhoton( const LHCb::RichRecSegment * segment,
                           const LHCb::RichRecPixel * pixel ) const = 0;

        /** Is this a true photon candidate ?
         *  Do the RichRecSegment and RichRecPixel have the same MCParticle parent
         *
         *  @param segment Pointer to a RichRecSegment
         *  @param pixel   Pointer to a RichRecPixel
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueRecPhoton( const LHCb::RichRecSegment * segment,
                       const LHCb::RichRecPixel * pixel ) const = 0;

        /** Is this a true photon candidate ?
         *  Does the RichSmartID result from a hit from the given MCParticle
         *
         *  @param mcPart  Pointer to an MCParticle
         *  @param id      RichSmartID for the pixel
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueRecPhoton( const LHCb::MCParticle * mcPart,
                       const LHCb::RichSmartID id ) const = 0;

        /** Is this a true photon candidate ?
         *  Does the one of the hits in the RichSmartID cluster result from a hit from the given MCParticle
         *
         *  @param mcPart  Pointer to an MCParticle
         *  @param cluster RichSmartID cluster for the pixel
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueRecPhoton( const LHCb::MCParticle * mcPart,
                       const Rich::HPDPixelCluster& cluster ) const = 0;

        /** Is this a true Cherenkov photon candidate ?
         *  Do the associated segment and pixel have the same MC parent AND was the pixel
         *  the result of Cherenkov radiation from the relevant radiator
         *
         *  @param photon Pointer to a RichRecPhoton
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible or photon is not a true
         *                Cherenkov photon
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueCherenkovPhoton( const LHCb::RichRecPhoton * photon ) const = 0;

        /** Is this a true Cherenkov photon candidate ?
         *  Do the segment and pixel have the same MC parent AND was the pixel
         *  the result of Cherenkov radiation from the relevant radiator
         *
         *  @param segment Pointer to a RichRecSegment
         *  @param pixel   Pointer to a RichRecPixel
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible or photon is not a true
         *                Cherenkov photon
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueCherenkovPhoton( const LHCb::RichRecSegment * segment,
                             const LHCb::RichRecPixel * pixel ) const = 0;

        /** Is this a true Cherenkov photon candidate ?
         *  Does the RichSmartID result from a hit from the given MCParticle AND was the hit
         *  the result of Cherenkov radiation from the relevant radiator
         *
         *  @param mcPart  Pointer to an MCParticle
         *  @param id      RichSmartID for the pixel
         *  @param rad     The Cherenkov radiator type
         *
         *  @return Pointer to associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible or photon is not a true
         *                Cherenkov photon
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueCherenkovPhoton( const LHCb::MCParticle * mcPart,
                             const LHCb::RichSmartID id,
                             const Rich::RadiatorType rad ) const = 0;

        /** Is the RichRecPixel due to true MC Cherenkov radiation from given radiator medium ?
         *
         *  @param pixel   Pointer to a RichRecPixel
         *  @param rad     The radiator type
         *
         *  @return Pointer to the associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible or photon is not a true
         *                Cherenkov photon
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueCherenkovRadiation( const LHCb::RichRecPixel * pixel,
                                const Rich::RadiatorType rad ) const = 0;

        /** Is the RichSmartID due to true MC Cherenkov radiation from given radiator medium ?
         *
         *  @param id      RichSmartID
         *  @param rad     The radiator type
         *
         *  @return Pointer to the associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible or photon is not a true
         *                Cherenkov photon
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueCherenkovRadiation( const LHCb::RichSmartID id,
                                const Rich::RadiatorType rad ) const = 0;

        /** Is any of the RichSmartIDs in nthe cluster due to true MC Cherenkov radiation from given radiator medium ?
         *
         *  @param cluster  RichSmartID cluster
         *  @param rad      The radiator type
         *
         *  @return Pointer to the associated MCParticle
         *  @retval NULL  No Monte Carlo association was possible or photon is not a true
         *                Cherenkov photon
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCParticle *
        trueCherenkovRadiation( const Rich::HPDPixelCluster& cluster,
                                const Rich::RadiatorType rad ) const = 0;

        /** Access the MCRichSegment associated to a given RichRecSegment
         *
         *  @param segment Pointer to a RichRecSegment
         *
         *  @return Point to the associated MCRichSegment
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCRichSegment *
        mcRichSegment( const LHCb::RichRecSegment * segment ) const = 0;

        /** Access the MCRichTrack associated to a given RichRecTrack
         *
         *  @param track Pointer to a RichRecTrack
         *
         *  @return Pointer to the associated MCRichTrack
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCRichTrack *
        mcRichTrack( const LHCb::RichRecTrack * track ) const = 0;

        /** Access the MCRichTrack associated to a given RichRecSegment
         *
         *  @param segment Pointer to a RichRecSegment
         *
         *  @return Pointer to the associated MCRichTrack
         *  @retval NULL  No Monte Carlo association was possible
         *  @retval !NULL Monte Carlo association was successful
         */
        virtual const LHCb::MCRichTrack *
        mcRichTrack( const LHCb::RichRecSegment * segment ) const = 0;

        /** Access the MCRichSegments associated to a given RichRecTrack
         *
         *  @param track Pointer to a RichRecTrack
         *
         *  @return Pointer to the SmartRefVector of associated MCRichSegments
         */
        virtual const SmartRefVector<LHCb::MCRichSegment> *
        mcRichSegments( const LHCb::RichRecTrack * track ) const = 0;

        /** Access the RichRecRing representing the MC CK ring for te given RichRecSegment
         *
         *  @param segment Pointer to a RichRecSegment
         *
         *  @return Pointer to the RichRecRing
         */
        virtual const LHCb::RichRecRing *
        mcCKRing( const LHCb::RichRecSegment * segment ) const = 0;

        /** Is this RichRecPixel background ?
         *
         *  @param pixel   Pointer to a RichRecPixel
         *
         *  @retval true  Pixel is the result of a background hit in the detector
         *  @retval false Pixel is a Cherenkov Hit
         */
        virtual bool isBackground( const LHCb::RichRecPixel * pixel ) const = 0;

        /** Checks if Track -> MCparticle association is available
         *
         *  @retval true  Track -> MCparticle association is possible
         *  @retval false Track -> MCparticle association is NOT possible
         */
        virtual bool trackToMCPAvailable() const = 0;

        /** Checks if RichRecPixel MC history is available
         *
         *  @retval true  RichRecPixel MC history is available
         *  @retval false RichRecPixel MC history is NOT available
         */
        virtual bool pixelMCHistoryAvailable() const = 0;

        /** Checks if MC association information for RichRecPhotons is available
         *  (requires Track and RICH MC information)
         *
         *  @retval true  Photon MC information is available
         *  @retval false Photon MC information is NOT available
         */
        virtual bool photonMCAssocAvailable() const = 0;

        /** Checks if RICH extended MC information (MCRichOpticalPhoton, MCRichSegment etc.)
         *  is available.
         *
         *  @retval true  Extended MC inforamtion is available
         *  @retval false Extended MC inforamtion is NOT available
         */
        virtual bool extendedMCAvailable() const = 0;

      };

    }
  }
}

#endif // RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
