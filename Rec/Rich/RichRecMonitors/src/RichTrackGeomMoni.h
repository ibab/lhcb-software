
//---------------------------------------------------------------------------
/** @file RichTrackGeomMoni.h
 *
 *  Header file for algorithm class : RichTrackGeomMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackGeomMoni.h,v 1.1.1.1 2005-06-18 11:44:46 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

// base class
#include "RichRecBase/RichRecMoniAlgBase.h"

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

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

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

class RichTrackGeomMoni : public RichRecMoniAlgBase {

public:

  /// Standard constructor
  RichTrackGeomMoni( const std::string& name,
                     ISvcLocator* pSvcLocator );

  virtual ~RichTrackGeomMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Book histograms
  const StatusCode bookHistograms();

  /// Book histograms
  const StatusCode bookMCHistograms();

  /// Book final histograms
  const StatusCode bookFinalHistograms();

private: // data

  IRichRayTracing * m_rayTrace;        ///< Pointer RichDetector ray tracing tool
  IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  IRichRecGeomTool * m_geomTool;     ///< Pointer to the Geometry tool
  IRichGeomEff * m_geomEffic; ///< Pointer to RichGeomEff tool
  IRichMCTrackInfoTool * m_mcTkInfo; ///< MC track information

  // job options
  std::string m_histPth;   ///< Output histogram path
  std::string m_mcHistPth; ///< Output MC truth histogram path

  /// Track selector
  RichTrackSelector m_trSelector;

  /// Ray-tracing configuration object
  RichTraceMode m_traceMode;

  IHistogram1D* m_projPDHitsXGlo[Rich::NRiches];    ///< Projected PD hits x global
  IHistogram1D* m_projPDHitsYGlo[Rich::NRiches];    ///< Projected PD hits y global
  IHistogram1D* m_projPDHitsZGlo[Rich::NRiches];    ///< Projected PD hits z global
  IHistogram2D* m_projPDHitsXYGlo[Rich::NRiches];   ///< Projected PD hits xVy global
  IHistogram2D* m_projPDHitsXZGlo[Rich::NRiches];   ///< Projected PD hits xVz global
  IHistogram2D* m_projPDHitsYZGlo[Rich::NRiches];   ///< Projected PD hits yVz global

  IHistogram1D* m_projPDHitsXLoc[Rich::NRiches];    ///< Projected PD hits x local
  IHistogram1D* m_projPDHitsYLoc[Rich::NRiches];    ///< Projected PD hits y local
  IHistogram1D* m_projPDHitsZLoc[Rich::NRiches];    ///< Projected PD hits z local
  IHistogram2D* m_projPDHitsXYLoc[Rich::NRiches];   ///< Projected PD hits xVy local

  IHistogram1D* m_projPDPanelHitsXGlo[Rich::NRiches];  ///< Projected PDPanel hits x global
  IHistogram1D* m_projPDPanelHitsYGlo[Rich::NRiches];  ///< Projected PDPanel hits y global
  IHistogram1D* m_projPDPanelHitsZGlo[Rich::NRiches];  ///< Projected PDPanel hits z global
  IHistogram2D* m_projPDPanelHitsXYGlo[Rich::NRiches]; ///< Projected PDPanel hits xVy global
  IHistogram2D* m_projPDPanelHitsXZGlo[Rich::NRiches]; ///< Projected PDPanel hits xVz global
  IHistogram2D* m_projPDPanelHitsYZGlo[Rich::NRiches]; ///< Projected PDPanel hits yVz global

  IHistogram1D* m_projPDPanelHitsXLoc[Rich::NRiches];  ///< Projected PDPanel hits x local
  IHistogram1D* m_projPDPanelHitsYLoc[Rich::NRiches];  ///< Projected PDPanel hits y local
  IHistogram1D* m_projPDPanelHitsZLoc[Rich::NRiches];  ///< Projected PDPanel hits z local
  IHistogram2D* m_projPDPanelHitsXYLoc[Rich::NRiches]; ///< Projected PDPanel hits xVy local

  IHistogram1D* m_projSpheHitsXGlo[Rich::NRiches];    ///< Projected Sphe hits x global
  IHistogram1D* m_projSpheHitsYGlo[Rich::NRiches];    ///< Projected Sphe hits y global
  IHistogram1D* m_projSpheHitsZGlo[Rich::NRiches];    ///< Projected Sphe hits z global
  IHistogram2D* m_projSpheHitsXYGlo[Rich::NRiches];   ///< Projected Sphe hits xVy global
  IHistogram2D* m_projSpheHitsXZGlo[Rich::NRiches];   ///< Projected Sphe hits xVz global
  IHistogram2D* m_projSpheHitsYZGlo[Rich::NRiches];   ///< Projected Sphe hits yVz global

  IHistogram1D* m_projFlatHitsXGlo[Rich::NRiches];    ///< Projected Flat hits x global
  IHistogram1D* m_projFlatHitsYGlo[Rich::NRiches];    ///< Projected Flat hits y global
  IHistogram1D* m_projFlatHitsZGlo[Rich::NRiches];    ///< Projected Flat hits z global
  IHistogram2D* m_projFlatHitsXYGlo[Rich::NRiches];   ///< Projected Flat hits xVy global
  IHistogram2D* m_projFlatHitsXZGlo[Rich::NRiches];   ///< Projected Flat hits xVz global
  IHistogram2D* m_projFlatHitsYZGlo[Rich::NRiches];   ///< Projected Flat hits yVz global

  IHistogram1D* m_trEntrZ[Rich::NRadiatorTypes];      ///< z of track entrance point
  IHistogram1D* m_trExitZ[Rich::NRadiatorTypes];      ///< z of track exit point
  IHistogram2D* m_trEntrXY[Rich::NRadiatorTypes];     ///< yVx of track entrance point
  IHistogram2D* m_trExitXY[Rich::NRadiatorTypes];     ///< yVx of track exit point
  IHistogram1D* m_trLength[Rich::NRadiatorTypes];     ///< track length in radiator

  IHistogram1D* m_mctrEntrZ[Rich::NRadiatorTypes];      ///< z of MC track entrance point
  IHistogram1D* m_mctrExitZ[Rich::NRadiatorTypes];      ///< z of MC track exit point
  IHistogram2D* m_mctrEntrXY[Rich::NRadiatorTypes];     ///< yVx of MC track entrance point
  IHistogram2D* m_mctrExitXY[Rich::NRadiatorTypes];     ///< yVx of MC track exit point
  IHistogram1D* m_mctrLength[Rich::NRadiatorTypes];     ///< MC track length in radiator

  IHistogram2D* m_pdColNumXY[Rich::NRiches][2];  ///< Column numbering scheme for PDs. yVx
  IHistogram2D* m_pdColNumXZ[Rich::NRiches][2];  ///< Column numbering scheme for PDs. zVx
  IHistogram2D* m_pdColNumYZ[Rich::NRiches][2];  ///< Column numbering scheme for PDs. yVz
  IHistogram2D* m_pdRowNumXY[Rich::NRiches][2];  ///< Row numbering scheme for PDs. yVx
  IHistogram2D* m_pdRowNumXZ[Rich::NRiches][2];  ///< Row numbering scheme for PDs. zVx
  IHistogram2D* m_pdRowNumYZ[Rich::NRiches][2];  ///< Row numbering scheme for PDs. yVz

  IHistogram2D* m_hpdPanAccVx[Rich::NRadiatorTypes]; ///< HPD panel acceptance versus local x
  IHistogram2D* m_hpdPanAccVy[Rich::NRadiatorTypes]; ///< HPD panel acceptance versus local y

  IHistogram2D* m_geomEffVx[Rich::NRadiatorTypes]; ///< HPD acceptance versus local x
  IHistogram2D* m_geomEffVy[Rich::NRadiatorTypes]; ///< HPD acceptance versus local y

  // working variables for average PD positions
  RichMap<RichSmartID, double> m_xHits;
  RichMap<RichSmartID, double> m_yHits;
  RichMap<RichSmartID, double> m_zHits;
  RichMap<RichSmartID, int> m_hitCount;

};

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
