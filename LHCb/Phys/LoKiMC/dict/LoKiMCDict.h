// $Id: LoKiMCDict.h,v 1.3 2007-06-10 19:59:04 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiMC.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/RangeList.h"
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/MCAlgsDicts.h"
#include "LoKi/MCFinderDicts.h"
// ============================================================================
namespace
{
  struct _Instantiations 
  {
    // the basic types
    LoKi::Types::MCRange                          m_r1 ;
    LoKi::Types::MCVRange                         m_r2 ;
    // range lists
    LoKi::Types::MCRangeL                         m_l1 ;
    LoKi::Types::MCVRangeL                        m_l2 ;
    // keepers:
    LoKi::Keeper<LHCb::MCParticle>                m_k1 ;
    LoKi::Keeper<LHCb::MCVertex>                  m_k2 ;
    // unique keepers
    LoKi::UniqueKeeper<LHCb::MCParticle>          m_u1 ;
    LoKi::UniqueKeeper<LHCb::MCVertex>            m_u2 ;
    // the basic functions 
    LoKi::Dicts::Funcs<const LHCb::MCParticle*>   m_f1 ;
    LoKi::Dicts::Funcs<const LHCb::MCVertex*>     m_f2 ;
    // operators 
    LoKi::Dicts::FuncOps<const LHCb::MCParticle*> m_o1 ;
    LoKi::Dicts::FuncOps<const LHCb::MCVertex*>   m_o2 ;
    // calls 
    LoKi::Dicts::FunCalls<LHCb::MCParticle>       m_c1 ;
    LoKi::Dicts::CutCalls<LHCb::MCParticle>       m_c2 ;
    LoKi::Dicts::FunCalls<LHCb::MCVertex>         m_c3 ;
    LoKi::Dicts::CutCalls<LHCb::MCVertex>         m_c4 ;
    /// the special operators for identifiers
    LoKi::Dicts::PIDOps<LoKi::MCParticles::Identifier>    m_i1 ;
    LoKi::Dicts::PIDOps<LoKi::MCParticles::AbsIdentifier> m_i2 ;
    /// fictive constructor 
    _Instantiations () ;
  } ;  
} // end of anonymout namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
