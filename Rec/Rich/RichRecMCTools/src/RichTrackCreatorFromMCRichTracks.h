
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromMCRichTracks.h
 *
 *  Header file for tool : RichTrackCreatorFromMCRichTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromMCRichTracks.h,v 1.1 2006-02-09 17:59:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H
#define RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichTrackCreatorBase.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event
#include "Event/MCRichTrack.h"

//-------------------------------------------------------------------------------------
/** @class RichTrackCreatorFromMCRichTracks RichTrackCreatorFromMCRichTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Uses MCRichTracks as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

class RichTrackCreatorFromMCRichTracks : public RichTrackCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTrackCreatorFromMCRichTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromMCRichTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a reconstruction 'Track'.
  virtual LHCb::RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input Tracks
  virtual const StatusCode newTracks() const;

  /// Returns the number of tracks in the input Track container.
  virtual const long nInputTracks() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

private: // methods

  /// Returns a pointer to the MCRichTracks
  const LHCb::MCRichTracks * trTracks() const;

private: // data

  /// Pointer to MCRichTracks
  mutable LHCb::MCRichTracks * m_mcrTracks;

  /// Pointer to the detector ray tracing tool
  const IRichRayTracing * m_rayTrace;

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// Pointer to ring creator
  const IRichMassHypothesisRingCreator * m_massHypoRings;

  /// Pointer to track segment maker
  const IRichTrSegMaker * m_segMaker;

  /// Pointer to RichExpectedTrackSignal interface
  const IRichExpectedTrackSignal * m_signal;

  /// Input location of MCRichTracks in TES
  std::string m_mcrTracksLocation;

  /// Job Option "nickname" of the TrackSegment tool to use
  std::string m_trSegToolNickName;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
  bool m_buildHypoRings;

  /// Ray-tracing configuration object
  RichTraceMode m_traceMode;

};

#endif // RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H
