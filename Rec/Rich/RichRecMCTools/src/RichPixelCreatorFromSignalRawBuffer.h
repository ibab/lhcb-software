
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRawBuffer.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromSignalRawBuffer
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPixelCreatorFromSignalRawBuffer_H
#define RICHRECMCTOOLS_RichPixelCreatorFromSignalRawBuffer_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// RichKernel
#include "RichKernel/RichMap.h"

// interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "MCInterfaces/IRichMCTruthTool.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //--------------------------------------------------------------------------------------
      /** @class PixelCreatorFromSignalRawBuffer RichPixelCreatorFromSignalRawBuffer.h
       *
       *  Tool for the creation and book-keeping of RichRecPixel objects.
       *  Uses RichDigits from the digitisation but then refers to the
       *  MCRichOpticalPhoton objects select on the true Cherenkov hits.
       *  Optionally, can also select only those pixels that are associated to
       *  a RichRecxTrack, so that trackless hits can be filtered out.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/09/2003
       */
      //--------------------------------------------------------------------------------------

      class PixelCreatorFromSignalRawBuffer : public Rich::Rec::PixelCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PixelCreatorFromSignalRawBuffer( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent );

        /// Destructor
        virtual ~PixelCreatorFromSignalRawBuffer(){}

        // Initialize method
        StatusCode initialize();

      protected: // methods

        /// Build a new RichRecPixel from an Rich::HPDPixelCluster
        virtual LHCb::RichRecPixel * buildPixel ( const Rich::HPDPixelCluster& cluster ) const;

        /// Build a new RichRecPixel from a single LHCb::RichSmartID
        virtual LHCb::RichRecPixel * buildPixel( const LHCb::RichSmartID & id ) const;

        /// Initialise for a new event. Re-implmented from base class version.
        virtual void InitNewEvent();

      private: // methods

        /// List of tracked MCParticles
        typedef Rich::Map < const LHCb::MCParticle*, bool > TrackedMCPList;
        /// Get the map for tracked MCParticles for this event
        TrackedMCPList & trackedMCPs() const;

        /// Reject trackless hits
        bool rejectTrackless( const Rich::HPDPixelCluster& cluster ) const;

      private: // data

        /// General MC truth tool
        const Rich::MC::IMCTruthTool * m_mcTool = nullptr;

        /// Reconstruction MC truth tool
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool = nullptr;

        /** Flag to turn on/off the filtering of pixels that do not
            associated to any reconstructed RichRecTrack */
        bool m_trackFilter;

        /// Reject all background hits
        bool m_rejAllBackHits;

        /// Reject HPD reflection hits
        bool m_rejHPDReflHits;

        /// Reject back-scattered hits
        bool m_rejHPDBackScatter;

        /// List of tracked MCParticles
        mutable TrackedMCPList m_trackedMCPs;

        // flag to indicated tracked MCParticle list has been made for current event
        mutable bool m_trackMCPsDone = false;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichPixelCreatorFromSignalRawBuffer_H
