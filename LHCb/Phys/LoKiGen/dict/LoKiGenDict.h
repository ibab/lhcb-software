// $Id: LoKiGenDict.h,v 1.4 2007-07-25 15:07:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiGen.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/Monitoring.h"
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/GenExtractDicts.h"
#include "LoKi/GenAlgsDicts.h"
#include "LoKi/GenMoniDicts.h"
// ============================================================================
#include "LoKi/IGenHybridTool.h"
#include "LoKi/GenHybridEngine.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    template <>
    class FunCalls<HepMC::GenParticle> 
    {
    private:
      typedef HepMC::GenParticle          Type ;
      typedef LoKi::Function<const Type*> Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
    } ;
    template <>
    class CutCalls<HepMC::GenParticle> 
    {
    private:
      typedef HepMC::GenParticle           Type ;
      typedef LoKi::Predicate<const Type*> Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
    } ;
    template <>
    class FunCalls<HepMC::GenVertex> 
    {
    private:
      typedef HepMC::GenVertex            Type ;
      typedef LoKi::Function<const Type*> Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
    } ;
    template <>
    class CutCalls<HepMC::GenVertex> 
    {
    private:
      typedef HepMC::GenVertex             Type ;
      typedef LoKi::Predicate<const Type*> Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
    } ;
  } 
}
// ============================================================================
namespace
{
  struct _Instantiations 
  {
    // the basic types
    LoKi::Types::GRange                                    m_r1 ;
    LoKi::Types::GVRange                                   m_r2 ;
    // rangelists:
    LoKi::Types::GRangeL                                   m_l1 ;
    LoKi::Types::GVRangeL                                  m_l2 ;
    // keepers:
    LoKi::Keeper<HepMC::GenParticle>                       m_k1 ;
    LoKi::Keeper<HepMC::GenVertex>                         m_k2 ;
    // unique keepers
    LoKi::UniqueKeeper<HepMC::GenParticle>                 m_u1 ;
    LoKi::UniqueKeeper<HepMC::GenVertex>                   m_u2 ;
    // the basic functions 
    LoKi::Dicts::Funcs<const HepMC::GenParticle*>          m_f1 ;
    LoKi::Dicts::Funcs<const HepMC::GenVertex*>            m_f2 ;
    // operators 
    LoKi::Dicts::FuncOps<const HepMC::GenParticle*>        m_o1 ;
    LoKi::Dicts::FuncOps<const HepMC::GenVertex*>          m_o2 ;
    /// mathematics:
    LoKi::Dicts::FunCalls<HepMC::GenParticle>              m_c1 ;
    LoKi::Dicts::FunCalls<HepMC::GenVertex>                m_c2 ;
    LoKi::Dicts::CutCalls<HepMC::GenParticle>              m_c3 ;
    LoKi::Dicts::CutCalls<HepMC::GenVertex>                m_c4 ;
    /// the special operators for identifiers 
    LoKi::Dicts::PIDOps<LoKi::GenParticles::Identifier>    m_i1 ;
    LoKi::Dicts::PIDOps<LoKi::GenParticles::AbsIdentifier> m_i2 ;    
    /// monitoring and historgamming
    LoKi::Monitoring::Counter<const HepMC::GenParticle*>   m_40 ;
    LoKi::Monitoring::Stat<const HepMC::GenParticle*>      m_41 ;
    LoKi::Monitoring::Plot<const HepMC::GenParticle*>      m_42 ;
    LoKi::Monitoring::Counter<const HepMC::GenVertex*>     m_50 ;
    LoKi::Monitoring::Stat<const HepMC::GenVertex*>        m_51 ;
    LoKi::Monitoring::Plot<const HepMC::GenVertex*>        m_52 ;
    /// fictive constructor 
    _Instantiations () ;
  } ;  
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
