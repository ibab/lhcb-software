// $Id: TsaOTClusterCreator.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
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

namespace LHCb{
  class OTChannelID;
}

class DeOTDetector;

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
  
  StatusCode convert(LHCb::OTTimes* clusCont, 
                        Tsa::OTClusters* pattClusCont);
 

  bool processModule(LHCb::OTTimes::iterator start, 
                     LHCb::OTTimes::iterator stop,
		     LHCb::OTTimes::iterator& iterCluster);

  void makeString(LHCb::OTTimes::iterator start, 
                  LHCb::OTTimes::iterator& stop,
		  LHCb::OTTimes::iterator end);

  void createHits(LHCb::OTTimes::iterator& start, 
                  LHCb::OTTimes::iterator& stop,
		  std::vector<Tsa::OTCluster*>& tmpCont );

  void doubletNeighbours(const LHCb::OTChannelID& aChannel, 
			 std::vector<LHCb::OTChannelID>& nChannels) const;
  
  void addNeighbours(std::vector<Tsa::OTCluster*>& tmpCont,
                     Tsa::OTClusters* pattClusCont);

  void findNeighbours(const std::vector<Tsa::OTCluster*>& clusCont,
		      const LHCb::OTChannelID aChannel,
		      std::vector<Tsa::OTCluster*>& neighbours ) const;

  void neighbourChan(const LHCb::OTChannelID& aChannel, 
        	  std::vector<LHCb::OTChannelID>& nChannels) const;

  double driftRadius(const LHCb::OTTime* aCluster);

  DeOTDetector* m_tracker;
 
  unsigned int m_hotModule;
  double m_maxOcc;
  double m_distFudgeFactor;
  unsigned int m_clusterSize; 

  
};

#endif //_TSAOTCLUSTERSCREATOR_H_










