
/** @file RichGlobalPIDAlgBase.h
 *
 *  Header file for RICH global PID algorithm base class : RichGlobalPIDAlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.h,v 1.4 2004-07-27 10:56:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */

#ifndef RICHRECALGS_RICHGLOBALPIDALGBASE_H
#define RICHRECALGS_RICHGLOBALPIDALGBASE_H 1

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichGlobalPIDTrack.h"
#include "Event/RichGlobalPIDSummary.h"
#include "Event/RichGlobalPID.h"

// Definitions
#include "RichGlobalPID/RichGlobalPIDTkQuality.h"
#include "RichRecBase/RichTrackID.h"

/** @class RichGlobalPIDAlgBase RichGlobalPIDAlgBase.h
 *
 *  Abstract base class for GlobalPID algorithms
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */

class RichGlobalPIDAlgBase : public RichRecAlgBase {

public:

  ///< Standard constructor
  RichGlobalPIDAlgBase( const std::string& name,
                        ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDAlgBase() = 0;  ///< Destructor

  // Protected methods
protected:

  StatusCode gpidTracks();  ///< Update pointer to RichGlobalPIDTracks
  StatusCode gpidPIDs();    ///< Update pointer to RichGlobalPIDPIDs
  StatusCode gpidSummary(); ///< Update pointer to RichGlobalPIDSummary

  // Protected data
protected:

  /// Pointer to parent RichGlobalPIDTrack container
  RichGlobalPIDTracks * m_GPIDtracks;

  /// Pointer to parent RichGlobalPIDSummary object
  RichGlobalPIDSummary * m_GPIDSummary;

  /// Pointer to parent RichGlobalPID container
  RichGlobalPIDs * m_GPIDs;

  /// Location of working RICH Global PID tracks in TES
  std::string m_richGPIDTrackLocation;

  /// Location of Global PID Summary object in TES
  std::string m_richGPIDSummaryLocation;
  
  /// Location of Global PID results in TES
  std::string m_richGPIDLocation;

  /// Global algorithm name. Common to all sub-algorithms
  std::string m_richGPIDName;

};

inline StatusCode RichGlobalPIDAlgBase::gpidTracks()
{
  m_GPIDtracks = get<RichGlobalPIDTracks>( m_richGPIDTrackLocation );
  return StatusCode::SUCCESS;
}

inline StatusCode RichGlobalPIDAlgBase::gpidPIDs()
{
  m_GPIDs = get<RichGlobalPIDs>( m_richGPIDLocation );
  return StatusCode::SUCCESS;
}

inline StatusCode RichGlobalPIDAlgBase::gpidSummary()
{
  m_GPIDSummary = get<RichGlobalPIDSummary>( m_richGPIDSummaryLocation );
  return StatusCode::SUCCESS;
}

#endif // RICHRECALGS_RICHGLOBALPIDALGBASE_H
