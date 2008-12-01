// $Id: STLiteClusterMonitor.h,v 1.1 2008-12-01 16:29:28 mneedham Exp $
#ifndef STLiteClusterMonitor_H
#define STLiteClusterMonitor_H 1

#include "Kernel/STHistoAlgBase.h"

#include "Event/STLiteCluster.h"

/** @class STLiteClusterMonitor STLiteClusterMonitor.h
 *
 *  Class for monitoring STLiteClusters
 *
 *  @author M.Needham
 *  @date   02/12/2008
 */

class STLiteClusterMonitor : public ST::HistoAlgBase {

public:
 

  /// constructer
  STLiteClusterMonitor( const std::string& name, 
                    ISvcLocator *svcloc );

  /// destructer
  virtual ~STLiteClusterMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;

  void fillHistograms(const LHCb::STLiteCluster& aCluster);

  std::string m_clusterLocation;
  
};

#endif // MCSTLiteClusterMonitor_H
