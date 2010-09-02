// $Id: $
#ifndef ICALOHYPOESTIMATOR_H 
#define ICALOHYPOESTIMATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/CaloHypo.h"
#include "Event/Track.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"

static const InterfaceID IID_ICaloHypoEstimator ( "ICaloHypoEstimator", 2, 0 );




namespace CaloDataType{
  enum DataType  {  HypoE     = 0,  // hypo energy
                    HypoEt      ,     // hypo Et
                    HypoM       ,     // hypo Mass
                    HypoPrsE    ,     // Prs extra-digit E
                    HypoPrsM    ,     // Prs extra-digit Multiplicity
                    HypoSpdM    ,     // Spd extra-digit Multiplicity
                    ClusterE    ,  // cluster energy
                    ToPrsE      ,  // Prs deposit in front (using local setting of CaloHypo2Calo)
                    ToHcalE     ,  // Hcal deposit in front (using local setting of CaloHypo2Calo)
                    ToPrsM    ,  // Prs multiplicity
                    ToSpdM    ,  // Spd multiplicity
                    E1        ,  // cluster seed energy
                    E19       ,  // E1/ClusterE
                    E1Hypo    ,  // E1/HypoE
                    E4        ,  // max (2x2) energy within 3x3 
                    E49       ,  // E4/ClusterE
                    CellID    ,  // cluster-seed id
                    Hcal2Ecal ,  // ToHcalE/ClusterE
                    ClusterMatch , // chi2(2D)
                    ElectronMatch,  // chi2(e)
                    BremMatch    ,  // chi2(brem)
                    NeutralID    ,  // neutral pid
                    Spread       ,  // cluster spread
                    TrajectoryL  ,
                    Last // dummy end
  };                
  static const  std::string Name[Last] = {
    "HypoE"     ,
    "HypoEt"    ,     
    "HypoM"     ,     
    "HypoPrsE"  ,     
    "HypoPrsM"  ,     
    "HypoSpdM"  ,     
    "ClusterE"  ,  
    "ToPrsE"    ,  
    "ToHcalE"   ,  
    "ToPrsM"    ,  
    "ToSpdM"    ,  
    "E1"        ,  
    "E19"       ,  
    "E1Hypo"    ,  
    "E4"        ,  
    "E49"       ,  
    "CellID"    ,  
    "Hcal2Ecal" ,  
    "ClusterMatch" , 
    "ElectronMatch",  
    "BremMatch"    ,  
    "NeutralID"    ,  
    "Spread"       ,  
    "TrajectoryL"  
  };  

}

namespace CaloMatchType{
  enum MatchType  {  ClusterMatch     = 0,  
                    ElectronMatch      ,     
                    BremMatch   , 
                    Last};
  static const  std::string Name[Last] = { "ClusterMatch","ElectronMatch","BremMatch"};
}

namespace CaloClusterType{
  enum ClusterType  { Main     = 0,  
                      SplitOrMain,
                      Last};
  static const  std::string Name[Last] = { "Main","SplitOrMain"};
}




/** @class ICaloHypoEstimator ICaloHypoEstimator.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-08-18
 */
class ICaloHypoEstimator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloHypoEstimator; }

  typedef std::map<CaloDataType::DataType, double> caloDataType;
  typedef std::map<CaloMatchType::MatchType, const LHCb::Track*> caloMatchType;
  typedef std::map<CaloClusterType::ClusterType, const LHCb::CaloCluster*> caloClusterType;

  
  virtual StatusCode initialize()=0;
  virtual StatusCode finalize()=0;
  virtual double data(const LHCb::CaloCluster* cluster ,CaloDataType::DataType type, double def = 0.)=0;
  virtual double data(const LHCb::CaloHypo* hypo ,CaloDataType::DataType type, double def = 0)=0;
  virtual StatusCode _setProperty(const std::string&, const std::string&)=0;
  virtual ICaloHypo2Calo* hypo2Calo()=0;  
  virtual bool status()=0;  
  virtual const LHCb::Track*  toTrack(CaloMatchType::MatchType match)=0;
  virtual const LHCb::CaloCluster* toCluster(CaloClusterType::ClusterType clus=CaloClusterType::SplitOrMain)=0;

protected:
private:
  
};
#endif // ICALOHYPOESTIMATOR_H
