
//-----------------------------------------------------------------------------
/** @file RichNULLMCTruthTool.h
 *
 *  Header file for tool : Rich::MC::NULLMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichNULLMCTruthTool.h,v 1.1 2009-07-23 16:12:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCTOOLS_RichNULLMCTruthTool_H
#define RICHMCTOOLS_RichNULLMCTruthTool_H 1

// base class
#include "RichKernel/RichToolBase.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "Kernel/RichParticleIDType.h"
#include "RichKernel/RichPixelCluster.h"

// Event model
#include "Kernel/RichParticleIDType.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichDigitSummary.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCRichHit.h"

// Interfaces
#include "MCInterfaces/IRichMCTruthTool.h"

namespace Rich
{
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @class NULLMCTruthTool RichNULLMCTruthTool.h
     *
     *  NULL MC truth tool. Always returns no associations.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class NULLMCTruthTool : public Rich::ToolBase,
                            virtual public IMCTruthTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      NULLMCTruthTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

      /// Destructor
      virtual ~NULLMCTruthTool();

    public: // methods (and doxygen comments) inherited from interface

      // get MCRichHits for MCParticle
      const SmartRefVector<LHCb::MCRichHit> & mcRichHits( const LHCb::MCParticle * mcp ) const;

      // Get the MCRichHits associated to a given RichSmartID
      const SmartRefVector<LHCb::MCRichHit> & mcRichHits( const LHCb::RichSmartID smartID ) const;

      // Get a vector of MCParticles associated to given RichSmartID
      bool mcParticles( const LHCb::RichSmartID id,
                        std::vector<const LHCb::MCParticle*> & mcParts ) const;

      // Determines the particle mass hypothesis for a given MCParticle
      Rich::ParticleIDType mcParticleType( const LHCb::MCParticle * mcPart ) const;

      // Finds the MCRichDigit association for a RichSmartID channel identifier
      const LHCb::MCRichDigit * mcRichDigit( const LHCb::RichSmartID id ) const;

      // Finds the MCRichTrack associated to a given MCParticle
      const LHCb::MCRichTrack * mcRichTrack( const LHCb::MCParticle * mcPart ) const;

      // Finds the MCRichOpticalPhoton associated to a given MCRichHit
      const LHCb::MCRichOpticalPhoton * mcOpticalPhoton( const LHCb::MCRichHit * mcHit ) const;

      // Access the bit-pack history object for the given RichSmartID
      bool getMcHistories( const LHCb::RichSmartID id,
                           std::vector<const LHCb::MCRichDigitSummary*> & histories ) const;

      // Checks if the given RichSmartID is the result of a background hit
      bool isBackground ( const LHCb::RichSmartID id ) const;

      // Checks if the given RichSmartID is the result of a photon which
      // underwent reflections inside the HPD
      bool isHPDReflection ( const LHCb::RichSmartID id ) const;

      // Checks if the given RichSmartID is the result of backscattering
      // of the HPD silicon sensor
      bool isSiBackScatter ( const LHCb::RichSmartID id ) const;

      // Checks if the given RichSmartID is the result of scintillation
      // of the radiator
      bool isRadScintillation ( const LHCb::RichSmartID id ) const;

      // Checks if the given RichSmartID is the result of true Cherenkov
      bool isCherenkovRadiation( const LHCb::RichSmartID id,
                                 const Rich::RadiatorType rad ) const;

      // Returns true if MC information for the RICH hits are available
      bool richMCHistoryAvailable() const;

      // Checks if RICH extended MC information (MCRichOpticalPhoton, MCRichSegment etc.)
      bool extendedMCAvailable() const;

      // new ones

      // Get the MCRichHits associated to a cluster of RichSmartIDs
      void
      mcRichHits( const Rich::HPDPixelCluster& cluster,
                  SmartRefVector<LHCb::MCRichHit> & hits ) const;

      // Access the bit-pack history objects for the given cluster of RichSmartIDs
      bool
      getMcHistories( const Rich::HPDPixelCluster& cluster,
                      std::vector<const LHCb::MCRichDigitSummary*> & histories ) const;

      // Checks if the given cluster of RichSmartIDs is the result of a background
      bool isBackground ( const Rich::HPDPixelCluster& cluster ) const;

      // Checks if the given cluster of RichSmartIDs is the result of a photon which
      // underwent reflections inside the HPD
      bool isHPDReflection ( const Rich::HPDPixelCluster& cluster ) const;

      // Checks if the given cluster of RichSmartIDs is the result of backscattering
      // of the HPD silicon sensor
      bool isSiBackScatter ( const Rich::HPDPixelCluster& cluster ) const;

      // Checks if the given cluster of RichSmartIDs is the result of scintillation
      // of the radiator
      bool isRadScintillation ( const Rich::HPDPixelCluster& cluster ) const;

      // Checks if the given RichSmartID is the result of true Cherenkov
      bool isCherenkovRadiation( const Rich::HPDPixelCluster& cluster,
                                 const Rich::RadiatorType rad ) const;

      // Get a vector of MCParticles associated to given RichSmartID cluster
      bool mcParticles( const Rich::HPDPixelCluster& cluster,
                        std::vector<const LHCb::MCParticle*> & mcParts ) const;

    };

  }
}

#endif // RICHMCTOOLS_RichNULLMCTruthTool_H
