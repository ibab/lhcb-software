// $Id: RichDigiAlgMoni.h,v 1.1 2003-09-20 15:12:53 jonesc Exp $
#ifndef RICHMONITOR_RICHDIGIALGMONI_H
#define RICHMONITOR_RICHDIGIALGMONI_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichHit.h"

// RichKernel
#include "RichKernel/RichDetectorType.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Detector tools
#include "RiSicbGeom/PixelFinder.h"
#include "RichDetTools/IMaPMTDetTool.h"
#include "RichDetTools/IRichDetInterface.h"

// temporary histogramming numbers
#include "RichDetParams.h"

/** @class RichDigiAlgMoni RichDigiAlgMoni.h RichDigiQC/RichDigiAlgMoni.h
 *
 *  Monitor for Rich Digitisation algorithm performance
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiAlgMoni : public Algorithm {

public:

  /// Standard constructor
  RichDigiAlgMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiAlgMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Book histograms
  StatusCode bookHistograms();

  bool getPosition( const RichSmartID & id, HepPoint3D & position );

private: // data

  IMaPMTDetTool *     m_mapmtDet;
  IPixelFinder *      m_sicbDet;
  IRichDetInterface * m_detInt;

  // job options
  std::string m_histPth;        ///< Output histogram path
  std::string m_digitTES;       ///< Location of RichDigits in TES
  std::string m_mcdigitTES;     ///< Location of MCRichDigits in TES
  std::string m_mcdepTES;       ///< Location of MCRichDeposits in TES
  std::string m_mchitTES;       ///< Location of MCRichHits in TES

  // temporary whilst we have different detector tools
  std::string m_detMode;

  // Histograms

  IHistogram1D* m_digitMult[Rich::NRiches]; ///< MCRichDigit event multiplicity
  IHistogram1D* m_hitMult[Rich::NRiches]; ///< MCRichHit event multiplicity

  IHistogram1D* m_tofDep[Rich::NRiches];   ///< TOF Information for MCRichDeposit
  IHistogram1D* m_depEnDep[Rich::NRiches]; ///< Deposit Energy for MCRichDeposit

  IHistogram1D* m_tofHit[Rich::NRiches];   ///< TOF Information for MCRichDeposit
  IHistogram1D* m_depEnHit[Rich::NRiches]; ///< Deposit Energy for MCRichDeposit

  IHistogram1D* m_pdDigsXGlobal[Rich::NRiches];     ///< Observed PD digits x global
  IHistogram1D* m_pdDigsYGlobal[Rich::NRiches];     ///< Observed PD digits y global
  IHistogram1D* m_pdDigsZGlobal[Rich::NRiches];     ///< Observed PD digits z global
  IHistogram2D* m_pdDigsXYGlobal[Rich::NRiches];    ///< Observed PD digits xVy global
  IHistogram2D* m_pdDigsXZGlobal[Rich::NRiches];    ///< Observed PD digits xVz global
  IHistogram2D* m_pdDigsYZGlobal[Rich::NRiches];    ///< Observed PD digits yVz global
  IHistogram2D* m_pdCloseUpXZ[Rich::NRiches];
  IHistogram2D* m_pdCloseUpYZ[Rich::NRiches]; 

  IHistogram1D* m_mcHitsXGlobal[Rich::NRiches];     ///< Observed hits x global
  IHistogram1D* m_mcHitsYGlobal[Rich::NRiches];     ///< Observed hits y global
  IHistogram1D* m_mcHitsZGlobal[Rich::NRiches];     ///< Observed hits z global
  IHistogram2D* m_mcHitsXYGlobal[Rich::NRiches];    ///< Observed hits xVy global
  IHistogram2D* m_mcHitsXZGlobal[Rich::NRiches];    ///< Observed hits xVz global
  IHistogram2D* m_mcHitsYZGlobal[Rich::NRiches];    ///< Observed hits yVz global
  IHistogram2D* m_mcCloseUpXZ[Rich::NRiches];
  IHistogram2D* m_mcCloseUpYZ[Rich::NRiches];

  IHistogram1D* m_digiErrX[Rich::NRiches]; ///< Distance in x between hit and digit positions
  IHistogram1D* m_digiErrY[Rich::NRiches]; ///< Distance in y between hit and digit positions
  IHistogram1D* m_digiErrZ[Rich::NRiches]; ///< Distance in z between hit and digit positions
  IHistogram1D* m_digiErrR[Rich::NRiches]; ///< Absolute distance between hit and digit positions

};
#endif // RICHMONITOR_RICHDIGIALGMONI_H
