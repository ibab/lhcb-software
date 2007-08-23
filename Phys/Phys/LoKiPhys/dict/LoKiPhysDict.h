// $Id: LoKiPhysDict.h,v 1.6 2007-08-23 12:01:24 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKIPHYSDICT_H 
#define LOKI_LOKIPHYSDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
//#include "Event/Vertex.h"
//#include "Event/RecVertex.h"
#include "Event/Particle.h"
//#include "Event/ProtoParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiPhys.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/Monitoring.h"
#include "LoKi/ExtraInfo.h"
// ============================================================================
#include "LoKi/Child.h"
#include "LoKi/Dicts.h"
#include "LoKi/PhysAlgsDicts.h"
#include "LoKi/PhysExtractDicts.h"
#include "LoKi/PhysMoniDicts.h"
#include "LoKi/SelectVertex.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts
  {
    template <>
    class FunCalls<LHCb::VertexBase> 
    {
    private:
      typedef LHCb::VertexBase            Type ;
      typedef LoKi::Function<const Type*> Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const LHCb::VertexBase*           o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const LHCb::Vertex*               o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const LHCb::RecVertex*            o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<LHCb::Vertex>&     o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<LHCb::RecVertex>&  o ) { return fun ( o ) ; }
      static Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<LHCb::VertexBase>& o ) { return fun ( o ) ; }
    } ;
  }
}
// ============================================================================
namespace
{
  // ============================================================================
  struct _Instantiations 
  {
    // fictive constructor 
    _Instantiations () ;
    // the basic types
    LoKi::Types::Range                              m_r1 ;
    LoKi::Types::VRange                             m_r2 ;
    // rangelists:
    LoKi::Types::RangeL                             m_l1 ;
    LoKi::Types::VRangeL                            m_l2 ;
    // keepers:
    LoKi::Keeper<LHCb::Particle>                    m_k1 ;
    LoKi::Keeper<LHCb::VertexBase>                  m_k2 ;
    LoKi::Keeper<LHCb::Vertex>                      m_k3 ;
    LoKi::Keeper<LHCb::RecVertex>                   m_k4 ;
   // unique keepers
    LoKi::UniqueKeeper<LHCb::Particle>              m_u1 ;
    LoKi::UniqueKeeper<LHCb::VertexBase>            m_u2 ;
    LoKi::UniqueKeeper<LHCb::Vertex>                m_u3 ;
    LoKi::UniqueKeeper<LHCb::RecVertex>             m_u4 ;
    // the basic functions 
    LoKi::Dicts::Funcs<const LHCb::Particle*>       m_f1 ;
    LoKi::Dicts::Funcs<const LHCb::VertexBase*>     m_f2 ;
   // operators 
    LoKi::Dicts::FuncOps<const LHCb::Particle*>     m_o1 ;
    LoKi::Dicts::FuncOps<const LHCb::VertexBase*>   m_o2 ;
    // calls
    LoKi::Dicts::FunCalls<LHCb::Particle>           m_c1 ;
    LoKi::Dicts::CutCalls<LHCb::Particle>           m_c2 ;
    LoKi::Dicts::FunCalls<LHCb::VertexBase>         m_c3 ;
    LoKi::Dicts::CutCalls<LHCb::VertexBase>         m_c4 ;
    // special operators for identifiers 
    LoKi::Dicts::PIDOps<LoKi::Particles::Identifier>    m_i1 ;
    LoKi::Dicts::PIDOps<LoKi::Particles::AbsIdentifier> m_21 ;    
    // Monitoring & Extra Info    
    LoKi::ExtraInfo::GetInfo<const LHCb::Particle*>     m_53 ;
    LoKi::ExtraInfo::CheckInfo<const LHCb::Particle*>   m_54 ;
    LoKi::ExtraInfo::GetInfo<const LHCb::VertexBase*>   m_63 ;
    LoKi::ExtraInfo::CheckInfo<const LHCb::VertexBase*> m_64 ;
    // Monitoring 
    LoKi::Monitoring::Counter<const LHCb::Particle*>    m_50 ;
    LoKi::Monitoring::Stat<const LHCb::Particle*>       m_51 ;
    LoKi::Monitoring::Plot<const LHCb::Particle*>       m_52 ;
    LoKi::Monitoring::Counter<const LHCb::VertexBase*>  m_60 ;
    LoKi::Monitoring::Stat<const LHCb::VertexBase*>     m_61 ;
    LoKi::Monitoring::Plot<const LHCb::VertexBase*>     m_62 ;
    // additional 
    LoKi::TheSame<const LHCb::Particle*>                m_71 ;
    LoKi::TheSame<const LHCb::VertexBase*>              m_72 ;
  } ;  
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
