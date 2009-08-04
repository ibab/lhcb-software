// $Id: STNZSMonitor.h,v 1.5 2009-08-04 13:52:39 mtobin Exp $
#ifndef STNZSMonitor_H
#define STNZSMonitor_H 1

// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STNZSMonitor STNZSMonitor.h
 *
 *  Class for monitoring the noise of the Tell1's. For each Tell1 the noise
 *  versus the strip number is stored in a histogram. The histograms can be
 *  stored using sourceID or tell1 name with the option \b UseSourceID. There
 *  are several options for calculating the noise:
 *  - \b FollowPeriod: This is the period of the exponential moving average. It
 *    determines the lifetime of the averages (in number of events). As long as
 *    the number of processed events is smaller than FollowPeriod the average
 *    is a cumulative average. Set this to -1 to always use a cumulative
 *    averaging.
 *  - \b UpdateRate: Rate at which the histograms are updated (in number of
 *    events). Useful in online mode. Set to -1 to update only at the end.
 *  - \b ResetRate: Rate at which the counters for histograms are reset
 *    (in number of events). Set to -1 to do no reset (default).
 *  - \b SkipEvents: Number of events to be skipped. Useful when running over
 *     common-mode-subtracted data where the pedestals have not been calculated.
 *
 *  @author J. van Tilburg, N. Chiapolini
 *  @date   10/02/2009
 */

class STNZSMonitor : public ST::HistoAlgBase {

public:
 
  /// Standard constructer
  STNZSMonitor( const std::string& name, ISvcLocator *svcloc );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Book histograms
  void bookHistograms();

  /// Fill the noise histograms (only called every N events and at finalize)
  void updateNoiseHistogram(int tell1ID);

  //const std::string   m_basenameNoiseHisto; 
  int                 m_evtNumber;

  typedef std::map<int, std::vector<double> > DataMap;  
  DataMap m_meanMap;            ///< Internal map for the pedestals
  DataMap m_meanSqMap;          ///< Internal map of the pedestal^2

  /// Internal map of number of events per tell1 and FPGA-PP
  std::map<int, std::vector<int> > m_nEvents;

  // jobOptions:

  /// Detector type. Can be set to IT or TT. Changes m_dataLocation accordingly
  std::string m_detType;

  /// Location in the Event Data Store with the ADC values
  std::string m_dataLocation;

  /// When set to true: use the sourceID in the histogram name. Otherwise use
  /// the tell1 name.
  bool m_useSourceID;

  /// Period of the an exponential moving average.
  /// Set to -1 to have a cumulative average.
  int m_followingPeriod;

  /// Rate at which the histograms are updated (in number of events).
  /// Set to -1 to update only at the end.
  int m_updateRate;
  
  /// Rate at which the counters for histograms are reset (in number of events).
  /// Set to -1 to do no reset (default).
  int m_resetRate;
  
  /// Number of events to be skipped. Useful when running over
  /// common-mode-subtracted data where the pedestals have not been calculated. 
  int m_skipEvents;

  /// Map of noise histograms booked in initialize
  std::map<int, AIDA::IHistogram1D*> m_noiseHistos;

};

#endif // STNZSMonitor_H
