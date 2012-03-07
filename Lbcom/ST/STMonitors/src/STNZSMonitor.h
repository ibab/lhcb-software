// $Id: STNZSMonitor.h,v 1.9 2009-12-03 18:46:37 mtobin Exp $
#ifndef STNZSMonitor_H
#define STNZSMonitor_H 1

// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STNZSMonitor STNZSMonitor.h
 *
 *  Class for monitoring the noise of the Tell1's. For each Tell1 the noise
 *  versus the strip number is stored in a histogram. The histograms can be
 *  stored using sourceID or tell1 name with the option \b UseSourceID. The
 *  noise is calculated using a ISTNoiseCalculationTool.  The algorithm can be
 *  configured using the following options:
 *  - \b UpdateRate: Rate at which the histograms are updated (in number of
 *    events). Useful in online mode. Set to -1 to update only at the end.
 *  - \b NoiseToolType: Sets the type of the noise calculation tool. (default is ST::STNoiseCalculationTool)
 *  - \b NoiseToolName: Sets the name of the noise calculation tool. (default is TTNoiseCalculationTool)
 *
 *  @author J. van Tilburg, N. Chiapolini
 *  @date   10/02/2009
 */
namespace ST {
  class ISTNoiseCalculationTool;
}

class TH2D;
class TProfile;

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
  void updateNoiseHistogram(unsigned int tell1ID, bool updateTitle=false);

  //const std::string   m_basenameNoiseHisto; 
  int                 m_evtNumber;

  unsigned int m_nTELL1s; ///< Number of TELL1 boards expected.

  // jobOptions:

  /// Location in the Event Data Store with the ADC values
  std::string m_dataLocation;

  /// When set to true: use the sourceID in the histogram name. Otherwise use
  /// the tell1 name.
  bool m_useSourceID;

  /// Rate at which the histograms are updated (in number of events).
  /// Set to -1 to update only at the end.
  int m_updateRate;
  
  /// Dumps noise calculation variables to histograms for debugging
  bool m_checkCalculation;
  /// Plot noise calculation variables (mean, mean squared and rms)
  void dumpNoiseCalculation(unsigned int sourceID);
  
  std::vector< unsigned int > m_limitToTell;/// List of TELL1s to look at
  bool   m_selectedTells;///< Use only selected TELL1s
  
  bool m_useODINTime;///< ODIN time of first event is added to the histogram title in the finalize method
  const LHCb::ODIN* m_ODIN;///< Time of the first event in the run
  std::string m_odinEvent;///< String of the time of the first run

  bool m_rawNoise;///< Fill histograms with the RAW noise

  /// Map of noise histograms booked in initialize
  std::map<int, TProfile*> m_noiseHistos;

  /// Map of pedestal histograms booked in initialize
  std::map<int, TProfile*> m_pedestalHistos;

  TH2D* m_2d_NoisePerLinkVsTell1;///< 2d map of noise vs link
  TProfile* m_1dp_AvNoisePerTell1;///< Average noise in each tell1

  TH2D* m_2d_PedestalPerLinkVsTell1;///< 2d map of pedestal vs link
  TProfile* m_1dp_AvPedestalPerTell1;///< Average pedestal in each tell1

  /// 2d map used for normalisation of noise, pedestal plots as 2d profile histograms are not supported by online monitoring
  TH2D* m_2d_NormalisationPerLinkVsTell1;

  ST::ISTNoiseCalculationTool* m_noiseTool; ///< Tool to calculate noise
  std::string m_noiseToolType; ///< Tool type (default is STNoiseCalculationTool)
  std::string m_noiseToolName; ///< Tool name (default is TTNoiseCalculationTool)

  /// Period of the an exponential moving average.(read from the noise tool)
  int m_followingPeriod;

  /// Rate at which the counters for histograms are reset (read from the noise tool)
  int m_resetRate;
  
  /// Number of events to be skipped (read from noise tool)
  int m_skipEvents;


};

#endif // STNZSMonitor_H
