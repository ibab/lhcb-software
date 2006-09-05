// $Id: TsaSTClusterCreator.h,v 1.2 2006-09-05 15:48:51 mneedham Exp $
#ifndef _TSASTCLUSTERCREATOR_H_
#define _TSASTCLUSTERCREATOR_H_

/** @class TsaITCluster TsaITCluster.h 
 *
 *  Makes the pattern rec clusters
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "TsaBaseAlg.h"
#include "Event/STLiteCluster.h"
#include "TsaKernel/STCluster.h"

namespace LHCb{
  class STChannelID;
}

class DeSTDetector;
class DeSTSector;
class ISTClusterPosition;

class TsaSTClusterCreator :public TsaBaseAlg {

public:
  
  // Constructors and destructor
  TsaSTClusterCreator(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaSTClusterCreator();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();  

private:

  typedef LHCb::STLiteCluster::FastContainer FastContainer;

  StatusCode convert(FastContainer* clusCont, 
                     Tsa::STClusters* pattClusCont);

 
  void findSector(const LHCb::STChannelID aChan);

  bool processBeetle(FastContainer::iterator start, 
                     FastContainer::iterator stop,
		     FastContainer::iterator& iterCluster);

  std::string m_positionToolName;
  ISTClusterPosition* m_positionTool;

  std::string m_detType;
  DeSTDetector* m_tracker;
  DeSTSector* m_cachedSector; 
  unsigned int m_cachedSectorID;

  std::string m_inputLocation;
  std::string m_outputLocation;

  unsigned int m_hotBeetle;
  int m_maxOcc; 
  
};

inline void TsaSTClusterCreator::findSector(const LHCb::STChannelID aChan) {

  // find layer - use cache for speed
  if (aChan.uniqueSector() != m_cachedSectorID){
    m_cachedSector = m_tracker->findSector(aChan);
    m_cachedSectorID = aChan.uniqueSector();
  }
}



#endif //_TSASTCLUSTERSCREATOR_H_










