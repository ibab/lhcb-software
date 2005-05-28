
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromTrStoredTracks.h
 *
 *  Header file for tool : RichTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromTrStoredTracks.h,v 1.24 2005-05-28 13:10:53 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
#define RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base package
#include "RichRecBase/RichTrackCreatorBase.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichTrSegMaker.h"

// Event
#include "Event/TrStoredTrack.h"

//-------------------------------------------------------------------------------------
/** @class RichTrackCreatorFromTrStoredTracks RichTrackCreatorFromTrStoredTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Uses TrStoredTracks as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

class RichTrackCreatorFromTrStoredTracks : public RichTrackCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTrackCreatorFromTrStoredTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromTrStoredTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a TrStoredTrack.
  virtual RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input TrStoredTracks
  virtual const StatusCode newTracks() const;

  /// Returns the number of tracks in the input TrStoredTrack container.
  virtual const long nInputTracks() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

private: // methods

  /// Returns a pointer to the TrStoredTracks
  const TrStoredTracks * trStoredTracks() const;

private: // data

  /// Pointer to TrStoredTracks
  mutable TrStoredTracks * m_trTracks;

  /// Pointer to the detector ray tracing tool
  IRichRayTracing * m_rayTrace;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// Pointer to ring creator
  IRichMassHypothesisRingCreator * m_massHypoRings;

  /// Pointer to track segment maker
  IRichTrSegMaker * m_segMaker;

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Job Option "nickname" of the TrackSegment tool to use
  std::string m_trSegToolNickName;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
  bool m_buildHypoRings;

  /// Ray-tracing configuration object
  RichTraceMode m_traceMode;

};

#endif // RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
