// $Id: STClusterKiller.h,v 1.3 2009-02-10 09:44:15 mneedham Exp $
#ifndef STCLUSTERKILLER_H
#define STCLUSTERKILLER_H 1

#include "Kernel/STAlgBase.h"
#include "Event/STCluster.h"

#include <string>

/** @class STClusterKiller STClusterKiller.h
 *
 *  Class for killing clusters and random
 *
 *  @author M.Needham
 *  @date   06/10/2007
 */

class ISTClusterSelector;

namespace LHCb{
  class STChannelID;
}

class STClusterKiller :public ST::AlgBase {

public:
  
  // Constructors and destructor
  STClusterKiller( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STClusterKiller();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  
  void removedClusters(const LHCb::STClusters* clusterCont,
                       std::vector<LHCb::STChannelID>& deadClusters) const;

   // smart interface to generator

  std::string m_selectorType;
  ISTClusterSelector* m_clusterSelector;

  std::string m_inputLocation;

};

#endif // STCLUSTERKILLER_H
