// $Id: RichSegmentCreator.h,v 1.6 2004-03-16 13:45:06 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_RICHRECSEGMENTTOOL_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichSegmentCreator.h"

/** @class RichSegmentCreator RichSegmentCreator.h
 *
 *  Tool for the creation and manipulation of RichRecSegment objects
 *  This is a utility tool and should only used by the RichTrackCreator tools.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichSegmentCreator : public RichRecToolBase,
                           virtual public IRichSegmentCreator,
                           virtual public IIncidentListener {

public:

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

  /// Save a new RichRecSegment in the container
  void saveSegment( RichRecSegment * segment ) const;

  /// Create a new RichRecSegment
  RichRecSegment * newSegment( RichTrackSegment& segment, 
                               RichRecTrack* pTrk ) const;

  /// Return a pointer to RichRecSegments
  RichRecSegments * richSegments() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

private:  // Private data

  /// Pointer to RichRecTracks
  mutable RichRecSegments * m_segments;

  /// Location of RichRecSegments in TES
  std::string m_richRecSegmentLocation;

  // parameters
  std::vector<int> m_binsEn;
  std::vector<double> m_maxPhotEn;
  std::vector<double> m_minPhotEn;

};

inline void RichSegmentCreator::InitNewEvent()
{
  m_segments = 0;
}

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
