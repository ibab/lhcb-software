// $Id: Calo2Track.h,v 1.2 2006-06-15 17:45:24 odescham Exp $
#ifndef CALO2TRACK_H 
#define CALO2TRACK_H 1

//Forward dec'
template <class FROM,class TO,class WEIGHT> class IRelationWeighted; 
template <class FROM,class TO> class IRelation; 

namespace LHCb{ 
  class CaloHypo;
  class CaloCluster;
  class Track;
};

/** @class Calo2Track Calo2Track.h Event/Calo2Track.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-02-21
 */
namespace LHCb { 
  namespace CaloIdLocation {
    // Calo-Track matching tables   : IRelationWeighted< CaloCluster/Hypo , Track , float > 
    const std::string ClusterMatch  = "Rec/Calo/ClusterMatch";
    const std::string PhotonMatch   = "Rec/Calo/PhotonMatch";
    const std::string BremMatch     = "Rec/Calo/BremMatch";
    const std::string ElectronMatch = "Rec/Calo/ElectronMatch";
    // Acceptance tables       : IRelation< Track , bool  >
    const std::string InSpd         =  "Rec/Calo/InAccSpd";
    const std::string InPrs         =  "Rec/Calo/InAccPrs";
    const std::string InBrem        =  "Rec/Calo/InAccBrem";
    const std::string InEcal        =  "Rec/Calo/InAccEcal";
    const std::string InHcal        =  "Rec/Calo/InAccHcal";
        // Intermediate estimators : IRelation< Track , float >   
    const std::string PrsE          = "Rec/Calo/PrsE";
    const std::string EcalE         = "Rec/Calo/EcalE";
    const std::string HcalE         = "Rec/Calo/HcalE";
    const std::string EcalChi2      = "Rec/Calo/EcalChi2";
    const std::string BremChi2      = "Rec/Calo/BremChi2";
    // CaloPID DLLs            : IRelation< Track , float >   
    const std::string PrsPIDe       = "Rec/Calo/PrsPIDe";
    const std::string EcalPIDe      = "Rec/Calo/EcalPIDe";
    const std::string BremPIDe      = "Rec/Calo/BremPIDe";
    const std::string HcalPIDe      = "Rec/Calo/HcalPIDe";
    const std::string EcalPIDmu     = "Rec/Calo/EcalPIDmu";
    const std::string HcalPIDmu     = "Rec/Calo/HcalPIDmu";
  };
  namespace Calo2Track {
    typedef IRelationWeighted< LHCb::CaloCluster , LHCb::Track , float >             IClusTrTable;
    typedef IRelationWeighted< LHCb::Track , LHCb::CaloCluster , float >             ITrClusTable;
    typedef IRelationWeighted< LHCb::CaloHypo , LHCb::Track , float >                IHypoTrTable;
    typedef IRelationWeighted< LHCb::Track , LHCb::CaloHypo , float >                ITrHypoTable;
    typedef IRelation< LHCb::Track , float >                                         ITrEvalTable;
    typedef IRelation< LHCb::Track , bool  >                                         ITrAccTable;
  };   
};

#endif // CALO2TRACK_H 
