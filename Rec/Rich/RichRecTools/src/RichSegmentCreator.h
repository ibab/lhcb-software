
//-----------------------------------------------------------------------------
/** @file RichSegmentCreator.h
 *
 *  Header file for tool : RichSegmentCreator
 *
 *  CVS Log :-
 *  $Id: RichSegmentCreator.h,v 1.11 2005-02-02 10:09:40 jonrob Exp $
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
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichKernel/IRichDetParameters.h"

//-----------------------------------------------------------------------------
/** @class RichSegmentCreator RichSegmentCreator.h
 *
 *  Tool for the creation and manipulation of RichRecSegment objects
 *  This is a utility tool and should only used by the RichTrackCreator tools.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichSegmentCreator : public RichRecToolBase,
                           virtual public IRichSegmentCreator,
                           virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichSegmentCreator( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichSegmentCreator() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used by the tool at the beginning of events to initialise
  /// a new container for the RichRecTracks
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

  // Save a new RichRecSegment in the container
  void saveSegment( RichRecSegment * segment ) const;

  // Create a new RichRecSegment
  RichRecSegment * newSegment( const RichTrackSegment& segment, 
                               RichRecTrack* pTrk ) const;

  // Return a pointer to RichRecSegments
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

  /// Maximum photon energy for each radiator medium
  double m_maxPhotEn[Rich::NRadiatorTypes];

  /// Minimum photon energy for each radiator medium
  double m_minPhotEn[Rich::NRadiatorTypes];

  // debug segment counting
  mutable unsigned m_segCount[Rich::NRadiatorTypes];

};

inline void RichSegmentCreator::InitNewEvent()
{
  m_segments = 0;
  if ( msgLevel(MSG::DEBUG) ) {
    m_segCount[Rich::Aerogel] = 0;
    m_segCount[Rich::C4F10]   = 0;
    m_segCount[Rich::CF4]     = 0;
  }
}

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
