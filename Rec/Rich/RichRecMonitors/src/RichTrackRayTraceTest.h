
//---------------------------------------------------------------------------
/** @file RichTrackRayTraceTest.h
 *
 *  Header file for algorithm class : RichTrackRayTraceTest
 *
 *  CVS Log :-
 *  $Id: RichTrackRayTraceTest.h,v 1.1 2006-08-09 11:08:50 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// rec helpers
#include "RichRecBase/RichTrackSelector.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"

// Interfaces
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

//---------------------------------------------------------------------------
/** @class RichTrackRayTraceTest RichTrackRayTraceTest.h
 *
 *  Monitors the general geometrical details of the RichRecTrack
 *  and RichRecSegment objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichTrackRayTraceTest : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichTrackRayTraceTest( const std::string& name,
                         ISvcLocator* pSvcLocator );

  virtual ~RichTrackRayTraceTest( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  // test ray tracing
  void testRayTrace( const LHCb::RichTraceMode traceMode,
                     LHCb::RichRecSegment * segment ) const;

private: // data

  /// Pointer RichDetector ray tracing tool
  const IRichRayTracing * m_rayTrace;    

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_idTool;

  /// Track selector
  RichTrackSelector m_trSelector;

};

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
