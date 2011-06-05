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
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) //  variable ... was declared but never referenced
#pragma warning(disable:191) // type qualifier is meaningless on cast type
#endif
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
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
} // 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DICT_LOKIPROTOPARTICLESDICT_H
// ============================================================================
