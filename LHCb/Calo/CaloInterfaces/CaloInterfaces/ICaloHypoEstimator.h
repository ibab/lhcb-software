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

static const InterfaceID IID_ICaloHypoEstimator ( "ICaloHypoEstimator", 3, 0 );




namespace CaloDataType{

  static const double Default = -1.e+06;  // default value

  enum DataType  {  HypoE     = 0,   //0 hypo energy
                    HypoEt       ,   //1 hypo Et
                    HypoM        ,   //2 hypo Mass
                    HypoPrsE     ,   //3 Prs extra-digit E
                    HypoPrsM     ,   //4 Prs extra-digit Multiplicity
                    HypoSpdM     ,   //5 Spd extra-digit Multiplicity
                    ClusterE     ,   //6 cluster energy  ( = E9 for 3x3 cluster evaluation)
                    ToPrsE       ,   //7 Prs deposit in front (using local setting of CaloHypo2Calo)
                    ToHcalE      ,   //8 Hcal deposit in front (using local setting of CaloHypo2Calo)
                    ToPrsM       ,   //9 Prs multiplicity
                    ToSpdM       ,   //10 Spd multiplicity
                    E1           ,   // cluster seed energy
                    E9           ,   // 3x3 area cluster 
                    E19          ,   // E1/E9
                    E1Hypo       ,   // E1/HypoE
                    E4           ,   // max (2x2) energy within 3x3 
                    E49          ,   // E4/E9
                    CellID       ,   // cluster-seed id
                    Hcal2Ecal    ,   // ToHcalE/ClusterE
                    ClusterMatch ,   // chi2(2D)
                    ElectronMatch,   //20 chi2(e)
                    BremMatch    ,   // chi2(brem)
                    NeutralID    ,   // neutral pid
                    Spread       ,   // cluster spread
                    TrajectoryL  ,
                    PrsE19,
                    PrsE49,
                    PrsE4Max,
                    PrsE1,
                    PrsE2,
                    PrsE3,
                    PrsE4,//30
                    PrsE5,
                    PrsE6,
                    PrsE7,
                    PrsE8,
                    PrsE9,
                    isPhoton,
                    isPhotonEcl,
                    isPhotonFr2,
                    isPhotonFr2r4,
                    isPhotonAsym,//40
                    isPhotonKappa,
                    isPhotonEseed,
                    isPhotonE2,
                    isPhotonPrsFr2,
                    isPhotonPrsAsym,
                    isPhotonPrsM,
                    isPhotonPrsM15,
                    isPhotonPrsM30,
                    isPhotonPrsM45,
                    isPhotonPrsEmax,//50
                    isPhotonPrsE2,
                    isNotH,
                    isNotE,
                    ClusterCode,
                    ClusterFrac,
                    Last // dummy end (56)
  };                

  static const  int TypeMask[Last] ={  // 0x1 : neutral ; 0x2 : charged ; 0x3 : both
    0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,    // 20
    0x2,0x2,0x1,0x3,0x2,  // 5
    0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3, // 12
    0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,  // 8  Gamma/Pi0 input (Ecal-based)
    0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,  // 8  Gamma/Pi0 input (Prs-based)  
    0x1,0x1,0x3,0x3 //4
  };
  static const  std::string Name[Last] = {
    "HypoE"     , // 0
    "HypoEt"    ,     
    "HypoM"     ,     
    "HypoPrsE"  ,     
    "HypoPrsM"  ,     
    "HypoSpdM"  ,     
    "ClusterE"  ,  
    "ToPrsE"    ,  
    "ToHcalE"   ,  
    "ToPrsM"    ,   
    "ToSpdM"    , //10 
    "E1"        ,  
    "E9"        ,  
    "E19"       ,  
    "E1Hypo"    ,  
    "E4"        ,  
    "E49"       ,  
    "CellID"    ,  
    "Hcal2Ecal" ,  
    "ClusterMatch" , 
    "ElectronMatch", //20 
    "BremMatch"    ,  
    "NeutralID"    ,  
    "Spread"       ,  
    "TrajectoryL"  ,
    "PrsE19" ,
    "PrsE49" ,
    "PrsE4Max" ,
    "PrsE1" , 
    "PrsE2" , 
    "PrsE3" , //30
    "PrsE4" , 
    "PrsE5" , 
    "PrsE6" , 
    "PrsE7" , 
    "PrsE8" , 
    "PrsE9" ,
    "isPhoton",
    "isPhoton_Ecl",
    "isPhoton_Fr2",
    "isPhoton_Fr2r4",//40
    "isPhoton_Asym",
    "isPhoton_Kappa",
    "isPhoton_Eseed",
    "isPhoton_E2",
    "isPhoton_PrsFr2",
    "isPhoton_PrsAsym",
    "isPhoton_PrsM",
    "isPhoton_PrsM15",
    "isPhoton_PrsM30",
    "isPhoton_PrsM45",//50
    "isPhoton_PrsEmax",
    "isPhoton_PrsE2",
    "isNotH",
    "isNotE",
    "ClusterCode",
    "ClusterFrac"
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
