// $Id: RichDigitQC.h,v 1.4 2004-06-17 14:49:14 jonesc Exp $
#ifndef RICHMONITOR_RICHDIGITQC_H
#define RICHMONITOR_RICHDIGITQC_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/MCRichDigit.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichDigitQC RichDigitQC.h RichDigiQC/RichDigitQC.h
 *
 *  Monitor for RichDigits
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigitQC : public RichAlgBase {

public:

  /// Standard constructor
  RichDigitQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigitQC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Book histograms
  StatusCode bookHistograms();

private: // data

  // job options
  std::string m_histPth;         ///< Output histogram path
  std::string m_digitTDS;        ///< Location of MCRichDigits in TES

  // Histograms
  IHistogram1D* m_rich1;         ///< RICH1 digit multiplicity
  IHistogram1D* m_rich2;         ///< RICH2 digit multiplicity

  double m_hitMult[Rich::NRiches];
  int m_eventCount;

};
#endif // RICHMONITOR_RICHDIGITMONITOR_H
