#ifndef CALO2MC_H 
#define CALO2MC_H 1

namespace LHCb {
  class MCParticle;
  class CaloCluster;
  class CaloDigit;
}

template <class FROM,class TO,class WEIGHT> class IRelationWeighted; 
template <class FROM,class TO> class LinkerWithKey;
template <class FROM,class TO> class LinkedTo;
namespace LHCb{ 
  template <class FROM,class TO,class WEIGHT> class RelationWeighted1D;
}

/** @class Calo2MC Calo2MC.h Event/Calo2MC.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-02-21
 */
namespace LHCb { 
  namespace Calo2MC {
    typedef IRelationWeighted< LHCb::CaloCluster , LHCb::MCParticle , float >  IClusterTable ; 
    typedef LHCb::RelationWeighted1D<LHCb::CaloCluster,LHCb::MCParticle,float> ClusterTable ;
    
    typedef IRelationWeighted<LHCb::CaloDigit,LHCb::MCParticle,float>          IDigitTable; 
    typedef LHCb::RelationWeighted1D<LHCb::CaloDigit,LHCb::MCParticle,float>   DigitTable ; 

    typedef LinkerWithKey<LHCb::MCParticle,LHCb::CaloDigit>                   DigitLink ;
    typedef LinkedTo<LHCb::MCParticle,LHCb::CaloDigit>                        DigitLinkTo;

    typedef LinkerWithKey<LHCb::MCParticle,LHCb::CaloHypo>                   HypoLink ;
    typedef LinkedTo<LHCb::MCParticle,LHCb::CaloHypo>                        HypoLinkTo;

    //    typedef RelationWeighted1D<TrgCaloCluster,MCParticle,float> TrgClusterTable ;
    
  }
  
}

#endif // CALO2MC_H 
