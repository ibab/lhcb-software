// $Id: RichGlobalPIDAlgBase.h,v 1.1.1.1 2003-06-30 16:10:54 jonesc Exp $
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
#include "RichRecBase/RichTrackType.h"

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

#endif // RICHRECALGS_RICHGLOBALPIDALGBASE_H
