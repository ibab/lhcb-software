
//-----------------------------------------------------------------------------
/** @file RichPixelMonitor.h
 *
 *  Header file for algorithm class : RichPixelMonitor
 *
 *  CVS Log :-
 *  $Id: RichPixelMonitor.h,v 1.2 2005-06-23 15:14:55 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHPIXELMONITOR_H
#define RICHRECMONITOR_RICHPIXELMONITOR_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
/** @class RichPixelMonitor RichPixelMonitor.h
 *
 *  Monitor class for RichRecPixels.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichPixelMonitor : public RichRecAlgBase {

public:

  /// Standard constructor
  RichPixelMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator );

  virtual ~RichPixelMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private:

  /// Book histograms
  StatusCode bookHistograms();

  /// Book MC histograms
  StatusCode bookMCHistograms();

  /// Book final histograms
  StatusCode bookFinalHistograms();

private:

  const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface

  bool m_truth;            ///< Flag to turn on MC truth histograms

  // job options
  std::string m_histPth;   ///< Output histogram path
  std::string m_mcHistPth; ///< Output MC truth histogram path

  IHistogram1D* m_obsHitsXGlobal[Rich::NRiches];     ///< Observed hits x global
  IHistogram1D* m_obsHitsYGlobal[Rich::NRiches];     ///< Observed hits y global
  IHistogram1D* m_obsHitsZGlobal[Rich::NRiches];     ///< Observed hits z global
  IHistogram2D* m_obsHitsXYGlobal[Rich::NRiches];    ///< Observed hits xVy global
  IHistogram2D* m_obsHitsXZGlobal[Rich::NRiches];    ///< Observed hits xVz global
  IHistogram2D* m_obsHitsYZGlobal[Rich::NRiches];    ///< Observed hits yVz global

  IHistogram1D* m_obsBkgHitsXGlobal[Rich::NRiches];     ///< Observed background hits x global
  IHistogram1D* m_obsBkgHitsYGlobal[Rich::NRiches];     ///< Observed background hits y global
  IHistogram1D* m_obsBkgHitsZGlobal[Rich::NRiches];     ///< Observed background hits z global
  IHistogram2D* m_obsBkgHitsXYGlobal[Rich::NRiches];    ///< Observed background hits xVy global
  IHistogram2D* m_obsBkgHitsXZGlobal[Rich::NRiches];    ///< Observed background hits xVz global
  IHistogram2D* m_obsBkgHitsYZGlobal[Rich::NRiches];    ///< Observed background hits yVz global

  IHistogram1D* m_obsHitsXLocal[Rich::NRiches];      ///< Observed hits x local
  IHistogram1D* m_obsHitsYLocal[Rich::NRiches];      ///< Observed hits y local
  IHistogram1D* m_obsHitsZLocal[Rich::NRiches];      ///< Observed hits z local
  IHistogram2D* m_obsHitsXYLocal[Rich::NRiches];     ///< Observed hits xVy local

  IHistogram1D* m_obsBkgHitsXLocal[Rich::NRiches];      ///< Observed hits x local
  IHistogram1D* m_obsBkgHitsYLocal[Rich::NRiches];      ///< Observed hits y local
  IHistogram1D* m_obsBkgHitsZLocal[Rich::NRiches];      ///< Observed hits z local
  IHistogram2D* m_obsBkgHitsXYLocal[Rich::NRiches];     ///< Observed hits xVy local

  IHistogram2D* m_pdCloseUpXZ[Rich::NRiches]; ///< Close up on a signal PD xVz
  IHistogram2D* m_pdCloseUpYZ[Rich::NRiches]; ///< Close up on a signal PD yVz

  IHistogram2D* m_pdColNumXY[Rich::NRiches][2]; ///< Column numbering scheme for PDs. yVx
  IHistogram2D* m_pdColNumXZ[Rich::NRiches][2]; ///< Column numbering scheme for PDs. zVx
  IHistogram2D* m_pdColNumYZ[Rich::NRiches][2]; ///< Column numbering scheme for PDs. yVz
  IHistogram2D* m_pdRowNumXY[Rich::NRiches][2]; ///< Row numbering scheme for PDs. yVx
  IHistogram2D* m_pdRowNumXZ[Rich::NRiches][2]; ///< Row numbering scheme for PDs. zVx
  IHistogram2D* m_pdRowNumYZ[Rich::NRiches][2]; ///< Row numbering scheme for PDs. yVz


  IHistogram1D* m_pixBkg[Rich::NRiches];     ///< Pixel log(background) distribution

  IHistogram2D* m_sigHitsLoc[Rich::NRadiatorTypes];

  // variables for average PD positions
  RichMap<RichSmartID::KeyType, double> m_xHits;
  RichMap<RichSmartID::KeyType, double> m_yHits;
  RichMap<RichSmartID::KeyType, double> m_zHits;
  RichMap<RichSmartID::KeyType, int> m_hitCount;

};

#endif // RICHRECMONITOR_RICHPIXELMONITOR_H
