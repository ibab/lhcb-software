
//---------------------------------------------------------------------------
/** @file RichTrackGeomMoni.h
 *
 *  Header file for algorithm class : RichTrackGeomMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackGeomMoni.h,v 1.3 2005-10-31 13:30:58 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

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
#include "Event/MCRichSegment.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichGeomEff.h"

//---------------------------------------------------------------------------
/** @class RichTrackGeomMoni RichTrackGeomMoni.h
 *
 *  Monitors the general geometrical details of the RichRecTrack
 *  and RichRecSegment objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichTrackGeomMoni : public RichRecHistoAlgBase 
{

public:

  /// Standard constructor
  RichTrackGeomMoni( const std::string& name,
                     ISvcLocator* pSvcLocator );

  virtual ~RichTrackGeomMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRayTracing * m_rayTrace;        ///< Pointer RichDetector ray tracing tool
  const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  const IRichRecGeomTool * m_geomTool;     ///< Pointer to the Geometry tool
  const IRichGeomEff * m_geomEffic; ///< Pointer to RichGeomEff tool
  const IRichMCTrackInfoTool * m_mcTkInfo; ///< MC track information

  /// Track selector
  RichTrackSelector m_trSelector;

  /// Ray-tracing configuration object
  RichTraceMode m_traceMode;

  // working variables for average PD positions
  RichMap<RichSmartID, double> m_xHits;
  RichMap<RichSmartID, double> m_yHits;
  RichMap<RichSmartID, double> m_zHits;
  RichMap<RichSmartID, int> m_hitCount;

};

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
