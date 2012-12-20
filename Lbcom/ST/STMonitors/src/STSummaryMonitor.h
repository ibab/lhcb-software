#ifndef STSummaryMonitor_H
#define STSummaryMonitor_H 1

// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STSummaryMonitor STSummaryMonitor.h
 *
 *  Algorithm to monitor the information in the STSummary class. The STSummary
 *  information is filled in the ZS decoding. The following histograms are
 *  filled:
 *  - PCN distribution. The democratically-chosen PCN is used (from STSummary),
 *  - Total data size (kB) per event,
 *  - Error information in the STSummary: each bin signals a problem in the
 *    event: bin 1, PCN sync is false; bin 2, corrupted banks, bin 3, lost banks
 *    bin 4, recovered banks.
 *  There are three main job options:
 *  - \b DetType: to switch between "IT" and "TT"
 *  - \b InputData: Input location of the STSummary data. Default is set to
 *    STSummaryLocation::TTSummary or STSummaryLocation::ITSummary depending on
 *    the setting of the detector switch.
 *  - \b SuppressMissing: Flag to suppress the filling of the 3th bin in the
 *    error information histogram (default is false).
 *  - \b SuppressRecovered: Flag to suppress the filling of the 4th bin in the
 *    error information histogram (default is false).
 *  - \b PipeLineSize: Default is set to 187.
 *
 *  @author N. Chiapolini
 *  @date   20/02/2009
 */

class STSummaryMonitor : public ST::HistoAlgBase {

public:
 
  /// constructer
  STSummaryMonitor( const std::string& name, ISvcLocator *svcloc );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  static const int c_nErrorBins = 4;
  const double c_binIDaSynch;
  const double c_binIDmissing; 
  const double c_binIDcorrupted;
  const double c_binIDrecovered;

  // jobOptions

  /// Location of the Event Data Store with the Summary Info
  std::string m_summaryLocation;

  /// When set to true: do not fill the bin for missing banks 
  bool m_suppressMissing;
  
  /// When set to true: do not fill the bin for recovered banks 
  bool m_suppressRecovered;

  /// Length of the beetle pipeline
  int m_pipeLineSize;

  // Book histograms for online monitoring
  AIDA::IHistogram1D* m_1d_pcn;
  AIDA::IHistogram1D* m_1d_errors;
  AIDA::IHistogram1D* m_1d_dataSize;

};

#endif // STSummaryMonitor_H
