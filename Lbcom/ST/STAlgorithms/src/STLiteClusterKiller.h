// $Id: STLiteClusterKiller.h,v 1.2 2009-07-03 13:43:54 mneedham Exp $
#ifndef STCLUSTERKILLER_H
#define STCLUSTERKILLER_H 1

#include "Kernel/STAlgBase.h"
#include "Event/STLiteCluster.h"

#include <string>

/** @class STLiteClusterKiller STLiteClusterKiller.h
 *
 *  Class for killing clusters and random
 *
 *  @author M.Needham
 *  @date   06/10/2007
 */

class ISTChannelIDSelector;

namespace LHCb{
  class STChannelID;
}

class STLiteClusterKiller :public ST::AlgBase {

public:
  
  // Constructors and destructor
  STLiteClusterKiller( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STLiteClusterKiller();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  
  void livingClusters(const LHCb::STLiteCluster::STLiteClusters* clusterCont,
                      std::vector<LHCb::STLiteCluster>& selectedClusters) const;

   // smart interface to generator

  std::string m_selectorName;
  std::string m_selectorType;
  ISTChannelIDSelector* m_clusterSelector;

  std::string m_inputLocation;

};

#endif // STCLUSTERKILLER_H
