// $Id: RichDigitQC.h,v 1.1 2003-09-20 15:45:18 jonrob Exp $
#ifndef RICHMONITOR_RICHDIGITQC_H
#define RICHMONITOR_RICHDIGITQC_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward Declarations
class IHistogram1D;
class IHistogram2D;

/** @class RichDigitQC RichDigitQC.h RichDigiQC/RichDigitQC.h
 *
 *  Monitor for RichDigits
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigitQC : public Algorithm {

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
  std::string m_digitTDS;        ///< Location of target RichPIDs in TDS

  // Histograms
  IHistogram1D* m_rich1;         ///< RICH1 digit multiplicity
  IHistogram1D* m_rich2;         ///< RICH2 digit multiplicity

  double m_hitMult[Rich::NRiches];
  int m_eventCount;

};
#endif // RICHMONITOR_RICHDIGITMONITOR_H
