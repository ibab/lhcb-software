#ifndef STPulseMonitor_H
#define STPulseMonitor_H 1

// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STPulseMonitor STPulseMonitor.h
 *
 *  Class for monitoring the pulse shape as a function of the spill. It uses
 *  TAE NZS data to get the full shape of the pulse. 
 *  - First it loops over all input clusters. This means that you provide the
 *    decoded clusters for a given spill. If you want to keep this part
 *    flexible, you can also decode the clusters in all spills and then merge
 *    them using the STEventMerge algorithm.
 *  - Then, for each cluster it loops over the spills and determines the charge
 *    in the strips of this cluster.
 *
 *  The following histograms are produced per service box for each spill:
 *  - Histogram with the total charge.
 *  - Histogram with the highest charge in the cluster.
 *  - Four histograms with the charge of 1-,2-,3- and 4-strip clusters.
 *  The same histograms are also produced with signal over noise (S/N).
 * 
 *  Main job options:
 *  - <b>DetType</b>: Specify either "TT" or "IT".
 *  - <b>ClusterLocation</b>: Location of the input clusters. The central spill
 *    is used per default. When merging the events use: "Raw/TT/MergedSpills".
 *  - <b>InputData</b>: Location of the NZS data. By default it takes the
 *    common-mode-subtracted data (from the emulation). Alternatively, you can
 *    also take the pedestal subtracted ADCs: "Raw/TT/PedSubADCs". 
 *  - <b>Spills</b>: List to the spills to loop over. By default this list is
 *    set to: {"Prev2","Prev1","Central","Next1","Next2"}.
 *  - <b>ChargeCut</b>: Minimum cut on the charge of the cluster. By default
 *    it is set to -1.0 (i.e. not used).
 *  - <b>BunchID</b>: Possibility to select the bunch IDs. This can remove
 *    (cosmic) background events. For October09 TED it should be set to 2686.
 *    By default this list is empty, which means it is not used.
 *  - <b>SkipShortThick</b>: Flag to skip ladders that are short and thick.
 *    Useful for the IT analysis. By default set to true. Has no effect for TT.
 *  - <b>DirNames</b>: List of strings that determines whether the histograms in
 *    the subdivided within a service box. For instance, when specifying "type",
 *    it will subdivide the histograms per sector type (within a service box).
 *    When specifying "sector", it will subdivide the histograms per sector
 *    (within a service box). By default this list is set to: {"all"}, which
 *    means that there is no subdivision.
 *  - <b>UseNZSdata</b>: Flag to use NZS data. Default is set to true. In that
 *    case the InputData has to be in the NZS format. If this flag is false the
 *    InputData location has to contain STClusters.
 *  
 *  A presentation on this algorithm and the results was giving on 20.10.2009 in
 *  the ST TED run analysis meeting by Helge: http://indico.cern.ch/event/71185 
 *
 *  @author J. van Tilburg
 *  @date   22/10/2009
 */

class STPulseMonitor : public ST::HistoAlgBase {

public:
 
  /// Standard constructer
  STPulseMonitor( const std::string& name, ISvcLocator *svcloc );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  // internal event counter
  int                 m_evtNumber;

  // jobOptions:

  /// Location in the Event Data Store with the ADC values
  std::string m_dataLocation;

  /// Location in the Event Data Store with the STClusters
  std::string m_clusterLocation;

  /// List to the spills to loop over. E.g. "Next1", "Central", "Prev2", etc.
  std::vector<std::string> m_spills;

  /// Cut on the bunch ID (distinguish TED from cosmics)
  std::vector<unsigned int> m_bunchID;

  bool m_useNZSdata;     ///< Flag to use either NZS or ZS data
  double m_chargeCut;    ///< Cut on the total charge of the STClusters
  bool m_skipShortThick; ///< Skip short and thick ladders (for IT only)
  /// List of directory names to subidvide the sectors in the service box.
  /// Possible names are "all", "sector", "type".
  std::vector<std::string> m_dirNames;

};

#endif // STPulseMonitor_H
