// $Id: STClusterKiller.h,v 1.2 2008-10-14 08:49:04 mneedham Exp $
#ifndef STCLUSTERKILLER_H
#define STCLUSTERKILLER_H 1

#include "Kernel/STAlgBase.h"
#include "Event/STCluster.h"


/** @class STClusterKiller STClusterKiller.h
 *
 *  Class for killing clusters and random
 *
 *  @author M.Needham
 *  @date   06/10/2007
 */

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
  SmartIF<IRndmGen> m_uniformDist; 
  std::string m_inputLocation;
  double m_eff;
};

#endif // STCLUSTERKILLER_H
