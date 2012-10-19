#ifndef STCosmicFilter_H
#define STCosmicFilter_H 1

#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class STCluster;
}

/** @class STCosmicFilter STCosmicFilter.h
 *
 *  Class for monitoring STClusters
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STCosmicFilter : public ST::HistoAlgBase {

public:
 
  /// constructer
  STCosmicFilter( const std::string& name, 
                    ISvcLocator *svcloc );

  /// destructer
  virtual ~STCosmicFilter();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  bool reselect(const std::vector<LHCb::STCluster*>& clusters) const;
  std::string m_summaryLocation;
  std::string m_clusterLocation;
  double m_chargeThreshold;
  double m_highChargeThreshold;
  unsigned int m_maxClusters;
  unsigned int m_nClus1;
  unsigned int m_nClus2;
  double m_xWindow;

};

#endif // MCSTCosmicFilter_H
