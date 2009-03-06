// $Id: STSummaryAnalyser.h,v 1.1 2009-03-06 16:43:21 nchiapol Exp $
#ifndef STSummaryAnalyser_H
#define STSummaryAnalyser_H 1

#include "GaudiAlg/GaudiHistoAlg.h"
#include "Kernel/STBoardMapping.h"

/** @class STSummaryAnalyser STSummaryAnalyser.h
 *
 *  @author N. Chiapolini
 *  @date   20/02/2009
 */

class STSummaryAnalyser : public GaudiHistoAlg{

public:
 
  /// constructer
  STSummaryAnalyser( const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~STSummaryAnalyser();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:

  /// Fill the noise histograms (only called every N events and at finalize)
  //void updateNoiseHistogram(int tell1ID);

  //const std::string   m_basenameNoiseHisto; 
  const std::string                m_pcnHisto;
  std::string                    m_errorHisto[2];
  
  static const int    c_id             = 0;
  static const int    c_title          = 1;

  static const int    c_nErrorBins     = 3;
  static const double c_binIDaSynch   = 0.5;
  static const double c_binIDmissing   = 1.5; 
  static const double c_binIDcorrupted = 2.5;

  /// Tell1 mapping from tell1 name to source ID.
  const std::map<unsigned int, unsigned int>* m_TELL1Mapping;
  //const STBoardMapping::Map *m_TELL1Mapping;
  
  typedef std::map<int, std::vector<double> > DataMap;  
  DataMap m_meanMap;            ///< Internal map for the pedestals
  DataMap m_meanSqMap;          ///< Internal map of the pedestal^2
  std::map<int, int> m_nEvents; ///< Internal map of number of events per tell1

  int                 m_evtNumber;
  
  // jobOptions

  /// Detector type. Can be set to IT or TT. Changes m_dataLocation accordingly
  std::string m_detType;

  /// Location in the Event Data Store with the ADC values
  std::string m_dataLocation;

  /// Locatuib ub tge Event Data Store with the Summary Info
  std::string m_summaryLocation;

  /// When set to true: use the sourceID in the histogram name. Otherwise use
  /// the tell1 name.
  bool m_useSourceID;

  /// String used as a base for the histogram's name (_$tell + number will be 
  /// added to the name)
  /// if possible this might get replaced with the name of the instance running 
  /// or removed completely
  /// std::string m_basenameHisto; 

  /// Period of the an exponential moving average.
  /// Set to -1 to have a cumulative average.
  //int m_followingPeriod;

  /// Rate at which the histograms are updated (in number of events).
  /// Set to -1 to update only at the end.
  //int m_updateRate;
  
  /// Rate at which the counters for histograms are reset (in number of events).
  /// Set to -1 to do no reset (default).
  //int m_resetRate;
  
  /// Number of events to be skipped. Useful when running over
  /// common-mode-subtracted data where the pedestals have not been calculated. 
  int m_skipEvents;

};

#endif // STSummaryAnalyser_H
