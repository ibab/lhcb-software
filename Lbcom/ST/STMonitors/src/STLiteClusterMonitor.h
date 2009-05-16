// $Id: STLiteClusterMonitor.h,v 1.1 2009-05-16 21:31:12 mtobin Exp $
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

namespace ST {
  class STLiteClusterMonitor : public HistoAlgBase {

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
} // End of ST namespace
#endif // STLiteClusterMonitor_H
