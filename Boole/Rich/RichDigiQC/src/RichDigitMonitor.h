// $Id: RichDigitMonitor.h,v 1.1.1.1 2003-06-30 16:56:39 jonesc Exp $
#ifndef RICHMONITOR_RICHDIGITMONITOR_H
#define RICHMONITOR_RICHDIGITMONITOR_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward Declarations
class IHistogram1D;
class IHistogram2D;

/** @class RichDigitMonitor RichDigitMonitor.h RichMonitor/RichDigitMonitor.h
 *
 *  Monitor for RichDigits
 *
 *  @author Andy Presland  (Andrew.Presland@cern.ch)
 *  @date   2002-11-20
 */

class RichDigitMonitor : public Algorithm {

public:

  /// Standard constructor
  RichDigitMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigitMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Book histograms
  StatusCode bookHistograms();

  // job options
  std::string m_histoPath;       ///< Output histogram path
  std::string m_digitTDS;        ///< Location of target RichPIDs in TDS

  // Histograms
  IHistogram1D* m_rich1;         ///< RICH1 digit multiplicity
  IHistogram1D* m_rich2;         ///< RICH2 digit multiplicity

};
#endif // RICHMONITOR_RICHDIGITMONITOR_H
