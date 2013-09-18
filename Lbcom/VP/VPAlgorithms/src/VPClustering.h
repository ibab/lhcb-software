// $Id: VPClustering.h, v1.0 2012-07-06 dhynds
#ifndef VPCLUSTERING_H 
#define VPCLUSTERING_H 1

// Include files

// Event
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"

// GAUDI
#include "GaudiAlg/IHistoTool.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// VPelDet
#include "VPDet/DeVP.h"


/** @class VPClustering VPClustering.h
 *  
 *
 *  @author Daniel Hynds
 *  @date   2013-08-13
 */
class VPClustering : public GaudiHistoAlg {
  public: 
  /// Standard constructor
  VPClustering( const std::string& name, ISvcLocator* pSvcLocator );
    
  virtual ~VPClustering( ); ///< Destructor
    
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  std::string m_digitLocation;
  std::string m_liteClusterLocation;
  std::string m_VPClusterLocation;
  DeVP* m_vpDet;
  bool m_clusterHits;
  bool m_debugLevel;
  StatusCode clusterDigits();
  
  bool isEdge(LHCb::VPDigit* digit);
  void addToCluster(std::vector<LHCb::VPDigit*>& cluster, std::vector< std::pair<LHCb::VPChannelID,int> >& totVec,
                    std::vector<LHCb::VPDigit*>& digitCont, unsigned int hit);

  struct less_than_channelID
  {
    inline bool operator() (const LHCb::VPDigit* obj1, const LHCb::VPDigit* obj2)
    {
      return (obj1->channelID() < obj2->channelID());
    }
  };

  protected:

  private:

  
};

#endif // VPCLUSTERING_H

