// $Id: Calo2MC.h,v 1.3 2008-12-10 12:29:34 cattanem Exp $
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
};

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

    //    typedef RelationWeighted1D<TrgCaloCluster,MCParticle,float> TrgClusterTable ;
    
  };
  
};

#endif // CALO2MC_H 
