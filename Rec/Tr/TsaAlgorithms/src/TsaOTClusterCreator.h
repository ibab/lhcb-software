// $Id: TsaOTClusterCreator.h,v 1.2 2006-08-17 08:36:08 mneedham Exp $
#ifndef _TSAOTCLUSTERCREATOR_H_
#define _TSAOTCLUSTERCREATOR_H_

/** @class TsaOTClusterCreator TsaOTClusterCreator.h 
 *
 *  Makes the pattern rec clusters
 *
 *  @author M.Needham
 *  @date   07/03/2006
 */

#include "TsaBaseAlg.h"
#include "Event/OTTime.h"
#include "TsaKernel/OTCluster.h"

#include "GaudiKernel/VectorMap.h"

namespace LHCb{
  class OTChannelID;
}

class DeOTDetector;
class DeOTModule;
class IUsedLHCbID;

#include <string>

class TsaOTClusterCreator :public TsaBaseAlg {

public:
  
  // Constructors and destructor
  TsaOTClusterCreator(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaOTClusterCreator();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();  

private:


  typedef GaudiUtils::VectorMap<unsigned int,DeOTModule*> ModuleMap;
  ModuleMap m_modMap;
  
  StatusCode convert(LHCb::OTTimes* clusCont, 
                        Tsa::OTClusters* pattClusCont);
 

  bool processModule(LHCb::OTTimes::iterator start, 
                     LHCb::OTTimes::iterator stop,
		     LHCb::OTTimes::iterator& iterCluster);

  void makeString(LHCb::OTTimes::iterator start, 
                  LHCb::OTTimes::iterator& stop,
		  LHCb::OTTimes::iterator end);

  void createHits(LHCb::OTTimes::iterator start, 
                  LHCb::OTTimes::iterator stop,
		  Tsa::OTClusters*  pattClusCont );


  double driftRadius(const LHCb::OTTime* aCluster, const double wireLength) const;

  DeOTDetector* m_tracker;

  DeOTModule* m_cachedModule; 
  unsigned int m_hotModule;
  double m_maxOcc;
  double m_distFudgeFactor;
  unsigned int m_clusterSize; 

  bool m_filterClusters;
  std::string m_clusterFilterName;
  IUsedLHCbID* m_usedClusterTool;
  
  double m_sqrt12;

};

#endif //_TSAOTCLUSTERSCREATOR_H_










