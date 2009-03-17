// $Id: STSummaryAnalyser.h,v 1.3 2009-03-17 10:47:58 nchiapol Exp $
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
  std::string         m_pcnHisto[2];
  std::string         m_errorHisto[2];
  std::string	      m_dataSizeHisto[2];

  static const int    c_id             = 0;
  static const int    c_title          = 1;

  static const int    c_nErrorBins     = 4;
  const double c_binIDaSynch;
  const double c_binIDmissing; 
  const double c_binIDcorrupted;
  const double c_binIDrecovered;

  /// Tell1 mapping from tell1 name to source ID.
  const std::map<unsigned int, unsigned int>* m_TELL1Mapping;
  //const STBoardMapping::Map *m_TELL1Mapping;
  
  int                 m_evtNumber;
  
  // jobOptions

  /// Detector type. Can be set to IT or TT. Changes m_dataLocation accordingly
  std::string m_detType;

  /// Location in the Event Data Store with the ADC values
  //std::string m_dataLocation;

  /// Locatuib ub tge Event Data Store with the Summary Info
  std::string m_summaryLocation;

  /// When set to true: use the sourceID in the histogram name. Otherwise use
  /// the tell1 name.
  bool m_useSourceID;

  /// Number of events to be skipped. Useful when running over
  /// common-mode-subtracted data where the pedestals have not been calculated. 
  int m_skipEvents;

  /// When set to true: do not fill the bin for missing banks 
  bool m_suppressMissing;
  
  /// When set to true: do not fill the bin for recovered banks 
  bool m_suppressRecovered;

};

#endif // STSummaryAnalyser_H
