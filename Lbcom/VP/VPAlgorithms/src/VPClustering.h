#ifndef VPCLUSTERING_H 
#define VPCLUSTERING_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// LHCb
// Event/DigiEvent
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
// Det/VPDet
#include "VPDet/DeVP.h"

/** @class VPClustering VPClustering.h
 *  
 *  @author Daniel Hynds
 *  @date   2013-08-13
 */

class VPClustering : public GaudiAlgorithm {
  public: 
  /// Standard constructor
  VPClustering(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor    
  virtual ~VPClustering();
    
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

  private:

  std::string m_digitLocation;
  std::string m_liteClusterLocation;
  std::string m_clusterLocation;

  DeVP* m_vpDet;

  bool m_debug;

  StatusCode clusterDigits();
  int sensorNumber(LHCb::VPDigit* digit);
  bool isEdge(LHCb::VPDigit* digit);
  void addToCluster(std::vector<LHCb::VPDigit*>& cluster, std::vector< std::pair<LHCb::VPChannelID,int> >& totVec,
                    std::vector<LHCb::VPDigit*>& digitCont, unsigned int hit);

  struct less_than_channelID {
    inline bool operator() (const LHCb::VPDigit* obj1, const LHCb::VPDigit* obj2) {
      return (obj1->channelID() < obj2->channelID());
    }
  };

};

#endif // VPCLUSTERING_H

