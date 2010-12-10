// $Id$
// ============================================================================
#ifndef DICT_LOKIPROTOPARTICLESDICT_H 
#define DICT_LOKIPROTOPARTICLESDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MoreFunctions.h"
#include "LoKi/Dicts.h"
#include "LoKi/Operators.h"
#include "LoKi/Filters.h"
#include "LoKi/Streamers.h"
#include "LoKi/FuncOps.h"
#include "LoKi/FuncOps.h"
#include "LoKi/PPTypes.h"
#include "LoKi/ProtoParticles.h"
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    // ========================================================================
    /// fictive constructor 
    _Instantiations () ;
    // ========================================================================
    // the basic functions 
    LoKi::Dicts::Funcs<const LHCb::ProtoParticle*>          m_f1  ;
    LoKi::Dicts::VFuncs<const LHCb::ProtoParticle*>         m_f2  ;
    // operators 
    LoKi::Dicts::FuncOps<const LHCb::ProtoParticle*>        m_o1  ;
    LoKi::Dicts::CutsOps<const LHCb::ProtoParticle*>        m_o2  ;
    // streamers 
    LoKi::Dicts::MapsOps<const LHCb::ProtoParticle*>        m_o3  ;
    LoKi::Dicts::PipeOps<const LHCb::ProtoParticle*>        m_o4  ;
    LoKi::Dicts::FunValOps<const LHCb::ProtoParticle*>      m_o5  ;
    LoKi::Dicts::CutValOps<const LHCb::ProtoParticle*>      m_o51 ;
    LoKi::Dicts::ElementOps<const LHCb::ProtoParticle*>     m_o6  ;
    LoKi::Dicts::SourceOps<const LHCb::ProtoParticle*>      m_fo7 ;
    // info
    LoKi::Dicts::InfoOps<const LHCb::ProtoParticle*>        m_io  ;
    /// mathematics:
    LoKi::Dicts::FunCalls<LHCb::ProtoParticle>              m_c1  ;
    LoKi::Dicts::CutCalls<LHCb::ProtoParticle>              m_c2  ;
    // ========================================================================
    LoKi::Functors::Empty<const LHCb::ProtoParticle*>       m_ss1 ;
    LoKi::Functors::Size<const LHCb::ProtoParticle*>        m_ss2 ;
    LoKi::Valid<const LHCb::ProtoParticle*>                 m_ss3 ;
    // ========================================================================
  } ;
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // DICT_LOKIPROTOPARTICLESDICT_H
// ============================================================================
