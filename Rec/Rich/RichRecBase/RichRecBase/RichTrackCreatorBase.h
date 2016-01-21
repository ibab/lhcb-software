
//---------------------------------------------------------------------------------
/** @file RichTrackCreatorBase.h
 *
 *  Header file for tool base class : RichTrackCreatorBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPTRACKCREATORBASE_H
#define RICHRECBASE_RICHPTRACKCREATORBASE_H 1

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichRecGeomTool.h"

// Kernel
#include "RichKernel/RichException.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/RichMap.h"

// Event
#include "Event/RichRecTrack.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------------
    /** @class TrackCreatorBase RichTrackCreatorBase.h RichRecBase/RichTrackCreatorBase.h
     *
     *  Base class for all RichRecTrack creator tools.
     *
     *  Implements common functionality needed by all concrete implementations.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   20/05/2005
     */
    //---------------------------------------------------------------------------------------

    class TrackCreatorBase : public Rich::Rec::ToolBase,
                             virtual public ITrackCreator,
                             virtual public IIncidentListener
    {

    public: // methods for Gaudi

      /// Standard constructor
      TrackCreatorBase( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

      /// Destructor
      virtual ~TrackCreatorBase( ) {}

      // Initialize method
      virtual StatusCode initialize();

      // Finalize method
      virtual StatusCode finalize();

      // Implement the handle method for the Incident service.
      virtual void handle( const Incident& incident );

    public: // methods from interface

      // Return a pointer to the container of RichRecTracks
      LHCb::RichRecTracks * richTracks() const;

    private: // methods

      /// Printout the track creation statistics
      void printStats() const;

    protected: // helper classes

      /// Track Statistics class for Rich Reconstruction
      class TrackCount
      {
      public:
        /// Default constructor
        TrackCount() {}
        unsigned int triedTracks    = 0; ///< Number of tracks passing track selection
        unsigned int selectedTracks = 0; ///< Number of tracks selected for RICH analysis
        unsigned int aeroSegs       = 0; ///< Number of aerogel segments created
        unsigned int rich1GasSegs   = 0; ///< Number of Rich1Gas segments created
        unsigned int rich2GasSegs   = 0; ///< Number of Rich2Gas segments created
        /** Increment segment count for given radiator
         *  @param rad Radiator type to increment count for */
        inline void countRadiator( const Rich::RadiatorType rad )
        {
          if      ( Rich::Rich1Gas  == rad ) { ++rich1GasSegs; }
          else if ( Rich::Rich2Gas  == rad ) { ++rich2GasSegs; }
          else if ( Rich::Aerogel   == rad ) { ++aeroSegs;     }
        }
        /** Decrement segment count for given radiator
         *  @param rad Radiator type to decrement count for */
        inline void uncountRadiator( const Rich::RadiatorType rad )
        {
          if      ( Rich::Rich1Gas  == rad ) { --rich1GasSegs; }
          else if ( Rich::Rich2Gas  == rad ) { --rich2GasSegs; }
          else if ( Rich::Aerogel   == rad ) { --aeroSegs;     }
        }
      };

      /// Tag for tracks. A combination for type and uniqueness
      typedef std::pair<Rich::Rec::Track::Type,bool> TrackTag;

      /// Map of track statistics objects for different track types
      class TrackTypeCount : public Rich::Map< TrackTag, TrackCount >
      {
      public:
        TrackTypeCount() {}
        /// Access track stats object for given track type
        inline TrackCount & trackStats( const Rich::Rec::Track::Type trType, ///< Track type enumeration
                                        const bool unique               ///< Track unique flag
                                        )
        {
          return (*this)[ TrackTag(trType,unique) ];
        }
      };

    protected: // methods

      /// Initialise for a new event
      virtual void InitNewEvent();

      /// Finalise current event
      virtual void FinishEvent();

      /** Should book-keeping be performed ?
       *
       *  @return boolean indicating if track book-keeping should be performed
       *  @retval true  Book-keeping is to be done
       *  @retval false Book-keeping is to be skipped for increased speed
       */
      bool bookKeep() const;

      /// Read access to track selector
      const Rich::Rec::ITrackSelector & trackSelector() const;

      /// Access to track statistics object
      TrackTypeCount & trackStats() const;

      /** Set detector and radiator information into RichRecTrack
       *
       *  @param track Track to set data for
       *  @param rad   Radiator to set active
       */
      void setDetInfo( LHCb::RichRecTrack * track,
                       const Rich::RadiatorType rad ) const;

      /// Returns a pointer to the SmartID tool
      const Rich::ISmartIDTool * smartIDTool() const
      {
        return m_smartIDTool;
      }

      /// Returns a pointer to the ray tracing tool
      const Rich::IRayTracing * rayTraceTool() const
      {
        if ( !m_rayTrace ) { acquireTool( "RichRayTracing", m_rayTrace, nullptr, true ); }
        return m_rayTrace;
      }

      /// Stores the HPD panel ray traced impact points for the given segment
      bool rayTraceHPDPanelPoints( const LHCb::RichTrackSegment & trSeg,
                                   LHCb::RichRecSegment * newSegment ) const;

    protected: // data

      /// Flag to indicate if the tool has been used in a given event
      mutable bool m_hasBeenCalled = false;

      /// Working object to keep track of formed objects
      mutable Rich::HashMap<int, bool> m_trackDone;

    private: // data

      /// Pointer to RichSmartID tool
      const Rich::ISmartIDTool * m_smartIDTool = nullptr;

      /// Pointer to the detector ray tracing tool
      mutable const Rich::IRayTracing * m_rayTrace = nullptr;

      /// Geometry tool
      const IGeomTool * m_geomTool = nullptr; 

      /// Pointer to RichRecTracks
      mutable LHCb::RichRecTracks * m_tracks = nullptr;

      /// Output location for RichRecTracks in TES
      std::string m_richRecTrackLocation;

      /// Number of events processed tally
      unsigned long long m_Nevts = 0;

      /// Flag to turn on or off the book keeping features to save cpu time.
      bool m_bookKeep = false;

      /// Track Selector
      const Rich::Rec::ITrackSelector * m_trSelector = nullptr;

      /// Track statistics, summed for all events
      mutable TrackTypeCount m_nTracksAll;

      /// Track statistics for last event. used for debug printout
      mutable TrackTypeCount m_nTracksLast;

      /// Cached trace modes for each radiator
      std::vector<LHCb::RichTraceMode> m_traceModeRad;

    protected: // data

      /// Maximum number of raw input tracks per event to consider for processing
      unsigned int m_maxInputTracks = 0;

      /// Maximum number of selected tracks per event to consider for processing
      unsigned int m_maxSelTracks = 0;

    };

    inline bool TrackCreatorBase::bookKeep() const
    {
      return m_bookKeep;
    }

    inline const Rich::Rec::ITrackSelector & TrackCreatorBase::trackSelector() const
    {
      return *m_trSelector;
    }

    inline TrackCreatorBase::TrackTypeCount & TrackCreatorBase::trackStats() const
    {
      return m_nTracksAll;
    }

    inline void TrackCreatorBase::setDetInfo( LHCb::RichRecTrack * track,
                                              const Rich::RadiatorType rad ) const
    {
      if ( Rich::Aerogel == rad )
      {
        track->setInRICH1(true);
        track->setInAerogel(true);
      }
      else if ( Rich::Rich1Gas == rad )
      {
        track->setInRICH1(true);
        track->setInGas1(true);
      }
      else if ( Rich::Rich2Gas == rad )
      {
        track->setInRICH2(true);
        track->setInGas2(true);
      }
      else
      {
        throw RichException( "Unknown radiator type" + Rich::text(rad) );
      }
    }

  }
} // RICH

/** Backwards compatibility typedef
 * @todo Remove eventually
 */
typedef Rich::Rec::TrackCreatorBase RichTrackCreatorBase;

#endif // RICHRECBASE_RICHPTRACKCREATORBASE_H
