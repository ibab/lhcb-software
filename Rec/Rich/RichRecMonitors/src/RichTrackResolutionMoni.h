
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.h
 *
 *  Header file for algorithm class : RichTrackResolutionMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackResolutionMoni.h,v 1.2 2005-06-23 15:14:56 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"
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

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTrackInfoTool.h"

//---------------------------------------------------------------------------
/** @class RichTrackResolutionMoni RichTrackResolutionMoni.h
 *
 *  Monitors the resolution of the tracking information, in terms important
 *  for the RICH reconstruction.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichTrackResolutionMoni : public RichRecAlgBase {

public:

  /// Standard constructor
  RichTrackResolutionMoni( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichTrackResolutionMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Book histograms
  const StatusCode bookHistograms();

  /// Book Monte Carlo histograms
  const StatusCode bookMCHistograms();

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  const IRichMCTrackInfoTool * m_mcTkInfo; ///< MC track information

  // job options
  std::string m_histPth;   ///< Output histogram path
  std::string m_mcHistPth; ///< Output MC truth histogram path

  /// Track selector
  RichTrackSelector m_trSelector;

  // charge selection (a better way of handling this is needed)
  int m_chargeSel;

  IHistogram1D* m_dTrEntrX[Rich::NRadiatorTypes]; ///< Rec-MC entry point x
  IHistogram1D* m_dTrEntrY[Rich::NRadiatorTypes]; ///< Rec-MC entry point y
  IHistogram1D* m_dTrEntrZ[Rich::NRadiatorTypes]; ///< Rec-MC entry point z
  IHistogram1D* m_dTrExitX[Rich::NRadiatorTypes]; ///< Rec-MC exit point x
  IHistogram1D* m_dTrExitY[Rich::NRadiatorTypes]; ///< Rec-MC exit point y
  IHistogram1D* m_dTrExitZ[Rich::NRadiatorTypes]; ///< Rec-MC exit point z
  IHistogram1D* m_dTrPathL[Rich::NRadiatorTypes]; ///< Rec-MC path length

  IHistogram1D* m_dTrPanelX[Rich::NRadiatorTypes]; ///< Rec-MC panel point x
  IHistogram1D* m_dTrPanelY[Rich::NRadiatorTypes]; ///< Rec-MC panel point y
  IHistogram1D* m_dTrPanelZ[Rich::NRadiatorTypes]; ///< Rec-MC panel point z

  IHistogram1D* m_dTrAngleEntry[Rich::NRadiatorTypes]; ///< Angle between reco and MC entry point vectors
  IHistogram1D* m_dTrAngleMid[Rich::NRadiatorTypes]; ///< Angle between reco and MC middle point vectors
  IHistogram1D* m_dTrAngleExit[Rich::NRadiatorTypes]; ///< Angle between reco and MC exit point vectors
  IHistogram1D* m_txResEntry[Rich::NRadiatorTypes]; ///< Resolution on the x slope at entry point
  IHistogram1D* m_tyResEntry[Rich::NRadiatorTypes]; ///< Resolution on the y slope at entry point
  IHistogram1D* m_txResExit[Rich::NRadiatorTypes]; ///< Resolution on the x slope at entry point
  IHistogram1D* m_tyResExit[Rich::NRadiatorTypes]; ///< Resolution on the y slope at entry point

  IHistogram1D* m_xPullEntry[Rich::NRadiatorTypes]; ///< Pull distribution for x at entry
  IHistogram1D* m_yPullEntry[Rich::NRadiatorTypes]; ///< Pull distribution for y at entry
  IHistogram1D* m_txPullEntry[Rich::NRadiatorTypes]; ///< Pull distribution for tx at entry
  IHistogram1D* m_tyPullEntry[Rich::NRadiatorTypes]; ///< Pull distribution for ty at entry
  IHistogram1D* m_pPullEntry[Rich::NRadiatorTypes]; ///< Pull distribution for P at entry

  IHistogram1D* m_xPullExit[Rich::NRadiatorTypes]; ///< Pull distribution for x at exit
  IHistogram1D* m_yPullExit[Rich::NRadiatorTypes]; ///< Pull distribution for y at exit
  IHistogram1D* m_txPullExit[Rich::NRadiatorTypes]; ///< Pull distribution for tx at exit
  IHistogram1D* m_tyPullExit[Rich::NRadiatorTypes]; ///< Pull distribution for ty at exit
  IHistogram1D* m_pPullExit[Rich::NRadiatorTypes]; ///< Pull distribution for P at exit

  IHistogram1D* m_recAngInOut[Rich::NRadiatorTypes]; /// Reconstructed angle between entry and exit vectors
  IHistogram1D* m_mcAngInOut[Rich::NRadiatorTypes]; /// MC angle between entry and exit vectors
  IHistogram1D* m_angInOutRes[Rich::NRadiatorTypes]; /// Resolution on difference between entry and exit directions

  IHistogram1D* m_segWithMc[Rich::NRadiatorTypes]; ///< Fraction of tracks per radiator with MC information

  IHistogram1D* m_nSegsRad[Rich::NRadiatorTypes];  ///< The number of segments per event for each radiator

  IHistogram2D* m_nTks; ///< Number of RICH tracks versus total number of tracks

  IHistogram1D* m_nMCTrajPoints[Rich::NRadiatorTypes]; ///< Number of Monte Carlo radiator trajectory points

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
