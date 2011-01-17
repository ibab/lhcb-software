// $Id: RelationsDict.h,v 1.9 2006-06-11 15:23:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.9 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONSDICT_H 
#define RELATIONS_RELATIONSDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relations.h"
// ============================================================================

/** @file
 *
 *  Helper structures to simplify nesessary instantiations for 
 *  buinding of Reflex dictionaries
 * 
 *  @code
 *
 *  namespace SomeNamespace 
 *   {
 *    struct __Instantiations 
 *     {    
 *     GaudiDict::Relation1DDict<
 *       LHCb::Track,float>                                    _1 ;
 *     GaudiDict::Relation2DDict<
 *       LHCb::Particle,LHCb::MCParticle>                      _2 ;
 *     GaudiDict::Relation2DDict<
 *       LHCb::Particle,LHCb::FlavourTag>                      _3 ;
 *     GaudiDict::Relation2DDict<
 *       LHCb::Particle,LHCb::Vertex>                          _4 ;
 *     GaudiDict::RelationWeighted2DDict<
 *       LHCb::CaloHypo,LHCb::Track,float>                     _5 ;
 *     GaudiDict::RelationWeighted2DDict<
 *       LHCb::CaloCluster,LHCb::MCParticle,float>             _6 ;
 *     GaudiDict::RelationWeighted2DDict<
 *       LHCb::ProtoParticle,LHCb::MCParticle, double>         _7 ;
 *     GaudiDict::RelationWeighted2DDict<
 *       LHCb::Particle,LHCb::MCParticle, double>              _8 ;
 *     GaudiDict::RelationWeighted2DDict<
 *       LHCb::Particle,LHCb::Vertex,float>                    _9 ;
 *     } ;
 *  };
 *
 *  @endcode 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-02
 */

namespace GaudiDict
{
  
  template <class FROM,class TO> 
  struct _Rel1D
  {
    //
    typedef typename LHCb::Relation1D<FROM,TO>                    T1 ;
    typedef typename LHCb::Relation1D<FROM,TO>::Range             T2 ;
    typedef typename LHCb::Relation1D<FROM,TO>::Base              T3 ;
    typedef typename T3::Base                                     T4 ;
    typedef typename T4::Entry                                    T5 ;
    typedef typename T5::First                                    T6 ;
    typedef typename T5::Second                                   T7 ;
    typedef typename T4::TypeTraits                               T8 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;
    T3 m_t3 ;
    T4 m_t4 ;
    T5 m_t5 ;
    T6 m_t6 ;
    T7 m_t7 ;
    T8 m_t8 ;
  };
  
  template <class FROM,class TO> 
  struct _Rel2D
  {
    //
    typedef typename LHCb::Relation2D<TO,FROM>                          T1 ;
    typedef typename LHCb::Relation2D<TO,FROM>::Range                   T2 ;
    typedef typename LHCb::Relation2D<TO,FROM>::Base                    T3 ;
    typedef typename T3::Base                                           T4 ;
    typedef typename T4::Base                                           T5 ;
    typedef typename T5::Entry                                          T6 ;
    typedef typename T6::First                                          T7 ;
    typedef typename T6::Second                                         T8 ;
    typedef typename T5::TypeTraits                                     T9 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;
    T3 m_t3 ;
    T4 m_t4 ;
    T5 m_t5 ;
    T6 m_t6 ;  
    T7 m_t7 ;  
    T8 m_t8 ;  
    T9 m_t9 ;  
  };

  template <class FROM,class TO,class WEIGHT> 
  struct _RelW1D
  {
    //
    typedef typename LHCb::RelationWeighted1D<FROM,TO,WEIGHT>           T1 ;
    typedef typename LHCb::RelationWeighted1D<FROM,TO,WEIGHT>::Range    T2 ;
    typedef typename LHCb::RelationWeighted1D<FROM,TO,WEIGHT>::Base     T3 ;
    typedef typename T3::Base                                           T4 ;
    typedef typename T4::Entry                                          T5 ;
    typedef typename T5::First                                          T6 ;
    typedef typename T5::Second                                         T7 ;
    typedef typename T5::Third                                          T8 ;
    typedef typename T4::TypeTraits                                     T9 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;
    T3 m_t3 ;
    T4 m_t4 ;
    T5 m_t5 ;
    T6 m_t6 ;
    T7 m_t7 ;
    T8 m_t8 ;
    T9 m_t9 ;
  };
  
  template <class FROM,class TO,class WEIGHT> 
  struct _RelW2D
  {
    //
    typedef typename LHCb::RelationWeighted2D<TO,FROM,WEIGHT>           T1 ;
    typedef typename LHCb::RelationWeighted2D<TO,FROM,WEIGHT>::Range    T2 ;
    typedef typename LHCb::RelationWeighted2D<TO,FROM,WEIGHT>::Base     T3 ;
    typedef typename T3::Base                                           T4 ;
    typedef typename T4::Base                                           T5 ;
    typedef typename T5::Entry                                          T6 ;
    typedef typename T6::First                                          T7 ;
    typedef typename T6::Second                                         T8 ;
    typedef typename T6::Third                                          T9 ;
    typedef typename T5::TypeTraits                                     T10 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;
    T3 m_t3 ;
    T4 m_t4 ;
    T5 m_t5 ;
    T6 m_t6 ;  
    T7 m_t7 ;  
    T8 m_t8 ;  
    T9 m_t9 ;  
    T10 m_t10 ;  
  };

  template <class FROM,class TO> 
  struct Relation1DDict
  {
    //
    typedef typename GaudiDict::_Rel1D<FROM,TO>  T1 ;
    typedef typename GaudiDict::_Rel1D<TO,FROM>  T2 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;      
  };
  
  
  template <class FROM,class TO> 
  struct Relation2DDict
  {
    //
    typedef typename GaudiDict:: Relation1DDict<FROM,TO>  T1 ;
    typedef typename GaudiDict:: Relation1DDict<TO,FROM>  T2 ;
    typedef typename GaudiDict::_Rel2D<FROM,TO>  T3 ;
    typedef typename GaudiDict::_Rel2D<TO,FROM>  T4 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;      
    T3 m_t3 ;
    T4 m_t4 ;      
  };
  
  template <class FROM,class TO,class WEIGHT> 
  struct RelationWeighted1DDict
  {
    //
    typedef typename GaudiDict::_RelW1D<FROM,TO,WEIGHT>  T1 ;
    typedef typename GaudiDict::_RelW1D<TO,FROM,WEIGHT>  T2 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;      
  };
  
  
  template <class FROM,class TO,class WEIGHT> 
  struct RelationWeighted2DDict
  {
    //
    typedef typename GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>  T1 ;
    typedef typename GaudiDict::RelationWeighted1DDict<TO,FROM,WEIGHT>  T2 ;
    typedef typename GaudiDict::_RelW2D<FROM,TO,WEIGHT>  T3 ;
    typedef typename GaudiDict::_RelW2D<TO,FROM,WEIGHT>  T4 ;
    //
    T1 m_t1 ;
    T2 m_t2 ;      
    T3 m_t3 ;
    T4 m_t4 ;      
  };
  

} //end of namespace RelDict

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_RELATIONSDICT_H
// ============================================================================
