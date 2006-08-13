
//-----------------------------------------------------------------------------
/** @file RichSegmentCreator.h
 *
 *  Header file for tool : RichSegmentCreator
 *
 *  CVS Log :-
 *  $Id: RichSegmentCreator.h,v 1.19 2006-08-13 17:15:34 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_RICHRECSEGMENTTOOL_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// Rich Kernel
#include "RichKernel/RichStatDivFunctor.h"

// interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichKernel/IRichDetParameters.h"

//-----------------------------------------------------------------------------
/** @class RichSegmentCreator RichSegmentCreator.h
 *
 *  Tool for the creation and manipulation of RichRecSegment objects
 *
 *  @attention
 *  This is a utility tool and should only used by the RichTrackCreator tools.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichSegmentCreator : public RichRecToolBase,
                           virtual public IRichSegmentCreator,
                           virtual public IIncidentListener
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichSegmentCreator( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichSegmentCreator() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used by the tool at the beginning of events to initialise
  /// a new container for the RichRecTracks
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

  // Save a new RichRecSegment in the container
  void saveSegment( LHCb::RichRecSegment * segment ) const;

  // Create a new RichRecSegment
  LHCb:: RichRecSegment * newSegment( LHCb::RichTrackSegment* segment,
                                      LHCb::RichRecTrack* pTrk ) const;

  // Return a pointer to RichRecSegments
  LHCb::RichRecSegments * richSegments() const;

private: // methods

  /// Initialise for each event
  void InitEvent();

  /// Finalise for each event
  void FinishEvent();

private:  // Private data

  /// Pointer to RichRecTracks
  mutable LHCb::RichRecSegments * m_segments;

  /// Location of RichRecSegments in TES
  std::string m_richRecSegmentLocation;

  /// Number of energy bins for each radiator
  std::vector<unsigned int> m_binsEn;

  /// Maximum photon energy for each radiator medium
  double m_maxPhotEn[Rich::NRadiatorTypes];

  /// Minimum photon energy for each radiator medium
  double m_minPhotEn[Rich::NRadiatorTypes];

  // debug segment counting
  mutable std::vector<unsigned long int> m_segCount;
  mutable std::vector<unsigned long int> m_segCountLast;

  /// Number of events processed tally
  unsigned int m_Nevts;

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

};

inline void RichSegmentCreator::InitEvent()
{
  m_segments = 0;
  if (msgLevel(MSG::DEBUG))
  {
    m_segCountLast = m_segCount;
    m_hasBeenCalled = false;
  }
}

inline void RichSegmentCreator::FinishEvent()
{
  if (msgLevel(MSG::DEBUG))
  {
    if ( m_hasBeenCalled ) ++m_Nevts;
  }
}

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
