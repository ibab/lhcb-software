// $Id: Calo2Track.h,v 1.1 2006-06-14 16:40:19 odescham Exp $
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
    const std::string ClusterMatch  = "Rec/Calo/ClusterMatch";
    const std::string PhotonMatch   = "Rec/Calo/PhotonMatch";
    const std::string BremMatch     = "Rec/Calo/BremMatch";
    const std::string ElectronMatch = "Rec/Calo/ElectronMatch";
    const std::string PrsE          = "Rec/Calo/PrsE";
    const std::string EcalE         = "Rec/Calo/EcalE";
    const std::string HcalE         = "Rec/Calo/HcalE";
    const std::string EcalChi2      = "Rec/Calo/EcalChi2";
    const std::string BremChi2      = "Rec/Calo/BremChi2";
  };
  namespace Calo2Track {
    typedef IRelationWeighted< LHCb::CaloCluster , LHCb::Track , float >             IClusTrTable;
    typedef IRelationWeighted< LHCb::Track , LHCb::CaloCluster , float >             ITrClusTable;
    typedef IRelationWeighted< LHCb::CaloHypo , LHCb::Track , float >                IHypoTrTable;
    typedef IRelationWeighted< LHCb::Track , LHCb::CaloHypo , float >                ITrHypoTable;
    typedef IRelation< LHCb::Track , float >                                         ITrEvalTable;
  };   
};

#endif // CALO2TRACK_H 
