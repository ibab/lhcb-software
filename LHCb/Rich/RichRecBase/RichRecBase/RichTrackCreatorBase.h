
//---------------------------------------------------------------------------------
/** @file RichTrackCreatorBase.h
 *
 *  Header file for tool base class : RichTrackCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorBase.h,v 1.7 2006-11-06 23:34:46 jonrob Exp $
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
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRayTracing.h"

// Kernel
#include "RichKernel/RichException.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/RichMap.h"

// Event
#include "Event/RichRecTrack.h"

//---------------------------------------------------------------------------------------
/** @class RichTrackCreatorBase RichTrackCreatorBase.h RichRecBase/RichTrackCreatorBase.h
 *
 *  Base class for all RichRecTrack creator tools.
 *
 *  Implements common functionality needed by all concrete implementations.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------------

class RichTrackCreatorBase : public RichRecToolBase,
                             virtual public IRichTrackCreator,
                             virtual public IIncidentListener
{

public: // methods for Gaudi

  /// Standard constructor
  RichTrackCreatorBase( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorBase( ) {};

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
    TrackCount() : triedTracks(0), selectedTracks(0), aeroSegs(0), rich1GasSegs(0), rich2GasSegs(0) {}
    unsigned int triedTracks;    ///< Number of tracks passing track selection
    unsigned int selectedTracks; ///< Number of tracks selected for RICH analysis
    unsigned int aeroSegs;       ///< Number of aerogel segments created
    unsigned int rich1GasSegs;      ///< Number of Rich1Gas segments created
    unsigned int rich2GasSegs;        ///< Number of Rich2Gas segments created
    /** Increment segment count for given radiator
     *  @param rad Radiator type to increment count for */
    void countRadiator( const Rich::RadiatorType rad )
    {
      if      ( Rich::Rich1Gas   == rad ) { ++rich1GasSegs; }
      else if ( Rich::Rich2Gas     == rad ) { ++rich2GasSegs;   }
      else if ( Rich::Aerogel == rad ) { ++aeroSegs;  }
    }
  };

  /// Tag for tracks. A combination for type and uniqueness
  typedef std::pair<Rich::Track::Type,bool> TrackTag;

  /// Map of track statistics objects for different track types
  class TrackTypeCount : public Rich::Map< TrackTag, TrackCount >
  {
  public:
    TrackTypeCount() {}
    /// Access track stats object for given track type
    inline TrackCount & trackStats( const Rich::Track::Type trType, ///< Track type enumeration
                                    const bool unique               ///< Track unique flag
                                    )
    {
      const TrackTag tkFlag(trType,unique);
      return (*this)[tkFlag];
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
  const Rich::IRichTrackSelector & trackSelector() const;

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
  const IRichSmartIDTool * smartIDTool() const
  {
    return m_smartIDTool;
  }

  /// Returns a pointer to the ray tracing tool
  const IRichRayTracing * rayTraceTool() const
  {
    return m_rayTrace;
  }

  /// Stores the HPD panel ray traced impact points for the given segment
  StatusCode rayTraceHPDPanelPoints( const LHCb::RichTrackSegment & trSeg,
                                     LHCb::RichRecSegment * newSegment ) const;

protected: // data

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

  /// Working object to keep track of formed objects
  mutable Rich::HashMap<unsigned long, bool> m_trackDone;

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// Pointer to the detector ray tracing tool
  const IRichRayTracing * m_rayTrace;

  /// Pointer to RichRecTracks
  mutable LHCb::RichRecTracks * m_tracks;

  /// Output location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  /// Number of events processed tally
  unsigned int m_Nevts;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Track Selector
  const Rich::IRichTrackSelector * m_trSelector;

  /// Track statistics, summed for all events
  mutable TrackTypeCount m_nTracksAll;

  /// Track statistics for last event. used for debug printout
  mutable TrackTypeCount m_nTracksLast;

  /// Ray-tracing configuration object
  LHCb::RichTraceMode m_traceMode;

};

inline bool RichTrackCreatorBase::bookKeep() const
{
  return m_bookKeep;
}

inline const Rich::IRichTrackSelector & RichTrackCreatorBase::trackSelector() const
{
  return *m_trSelector;
}

inline RichTrackCreatorBase::TrackTypeCount & RichTrackCreatorBase::trackStats() const
{
  return m_nTracksAll;
}

inline void RichTrackCreatorBase::setDetInfo( LHCb::RichRecTrack * track,
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

#endif // RICHRECBASE_RICHPTRACKCREATORBASE_H
