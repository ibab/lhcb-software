#ifndef SUBCLUSTERSELECTORTOOL_H 
#define SUBCLUSTERSELECTORTOOL_H 1


// Include files
// from Gaudi
#include  "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h"
#include  "CaloDet/DeCalorimeter.h"
#include  "Event/CaloCluster.h"
#include  "CaloCorrectionBase.h"
#include  "CaloInterfaces/ICaloSubClusterTag.h"


namespace CaloClusterMask {
  enum  Mask{area3x3        = 0 ,  // MUST BE 0 FOR BACKWARD COMPATIBILITY
             area2x2        = 1 ,
             SwissCross     = 2 ,
             Last
  };
  static const int nMask = Last+1;
  static const std::string maskName[nMask] = { "3x3", "2x2","SwissCross","Unknown" };
}



static const InterfaceID IID_SubClusterSelectorTool ( "SubClusterSelectorTool", 1, 0 );

/** @class SubClusterSelectorTool SubClusterSelectorTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2014-06-20
 */
class SubClusterSelectorTool : public GaudiTool, virtual public IIncidentListener {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_SubClusterSelectorTool; }

  /// Standard constructor
  SubClusterSelectorTool( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual ~SubClusterSelectorTool( ); ///< Destructor

  virtual void handle(const Incident&  ) { 
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "IIncident Svc reset" << endmsg; 
    updateParamsFromDB();
  }

  StatusCode getParamsFromOptions();
  void updateParamsFromDB();

  StatusCode tag(LHCb::CaloCluster* cluster);
  StatusCode tagEnergy(LHCb::CaloCluster* cluster);
  StatusCode tagPosition(LHCb::CaloCluster* cluster);
  

protected:

private:

  std::string m_condition ; 
  std::vector<std::string>          m_taggerE  ;   
  std::vector<std::string>          m_taggerP  ;  
  std::vector<ICaloSubClusterTag*>  m_tagE      ;  
  std::vector<ICaloSubClusterTag*>  m_tagP      ;  
  std::string                       m_det      ;
  std::map<std::string,std::string> m_clusterTaggers;

  DeCalorimeter*                    m_detector;
  CaloCorrectionBase*               m_dbAccessor;
  std::vector<std::string>          m_DBtaggerE  ;   
  std::vector<std::string>          m_DBtaggerP  ;  
  LHCb::CaloDigitStatus::Status     m_energyStatus;
  LHCb::CaloDigitStatus::Status     m_positionStatus;
  std::string m_sourceE;
  std::string m_sourceP;
};
#endif // SUBCLUSTERSELECTORTOOL_H
