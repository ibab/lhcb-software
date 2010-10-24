// $Id: LoKiMCDict.h,v 1.15 2010-05-31 20:33:54 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKIMCDICT_H 
#define LOKI_LOKIMCDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MoreFunctions.h"
#include "LoKi/LoKiMC.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/RangeList.h"
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/MCAlgsDicts.h"
#include "LoKi/MCMoniDicts.h"
#include "LoKi/MCFinderDicts.h"
#include "LoKi/MCExtractDicts.h"
#include "LoKi/Trees.h"
#include "LoKi/TreeOps.h"
#include "LoKi/MCDecays.h"
#include "LoKi/MCChildSelector.h"
// ============================================================================
#include "LoKi/IMCDecay.h"
#include "LoKi/FinderDict.h"
// ============================================================================
#include "LoKi/IMCHybridTool.h"
#include "LoKi/MCHybridEngine.h"
// ============================================================================
/** @file
 *  The dictionaries for the package Phys/LoKiMC
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-01
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Functors 
  {
    // ========================================================================
    // the specific printout
    // ========================================================================
    template <>
    std::ostream& Empty<const LHCb::MCParticle*>::fillStream
    ( std::ostream& s ) const { return s << "MCEMPTY" ; }
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& 
    Size<const LHCb::MCParticle*>::fillStream
    ( std::ostream& s ) const { return s << "MCSIZE" ; }
    // ========================================================================
    // the specific printout
    // ========================================================================
    template <>
    std::ostream& 
    Empty<const LHCb::MCVertex*>::fillStream
    ( std::ostream& s ) const { return s << "MCVEMPTY" ; }
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& 
    Size<const LHCb::MCVertex*>::fillStream
    ( std::ostream& s ) const { return s << "MCVSIZE" ; }
    // ========================================================================
  } // end of namespace LoKi::Functors 
  // ==========================================================================
} // end of namespace LoKi
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
    LoKi::Dicts::Funcs<const LHCb::MCParticle*>      m_f1 ;
    LoKi::Dicts::Funcs<const LHCb::MCVertex*>        m_f2 ;
    LoKi::Dicts::VFuncs<const LHCb::MCParticle*>     m_vf1 ;
    LoKi::Dicts::VFuncs<const LHCb::MCVertex*>       m_vf2 ;
    // operators 
    LoKi::Dicts::FuncOps<const LHCb::MCParticle*>    m_o1 ;
    LoKi::Dicts::FuncOps<const LHCb::MCVertex*>      m_o2 ;
    LoKi::Dicts::CutsOps<const LHCb::MCParticle*>    m_o3 ;
    LoKi::Dicts::CutsOps<const LHCb::MCVertex*>      m_o4 ;
    // 
    LoKi::Dicts::MapsOps<const LHCb::MCParticle*>    m_fo1  ;
    LoKi::Dicts::MapsOps<const LHCb::MCVertex*>      m_fo2  ;
    LoKi::Dicts::PipeOps<const LHCb::MCParticle*>    m_fo3  ;
    LoKi::Dicts::PipeOps<const LHCb::MCVertex*>      m_fo4  ;
    LoKi::Dicts::FunValOps<const LHCb::MCParticle*>  m_fo5  ;
    LoKi::Dicts::FunValOps<const LHCb::MCVertex*>    m_fo6  ;
    LoKi::Dicts::CutValOps<const LHCb::MCParticle*>  m_fo51 ;
    LoKi::Dicts::CutValOps<const LHCb::MCVertex*>    m_fo61 ;
    LoKi::Dicts::ElementOps<const LHCb::MCParticle*> m_fo7  ;
    LoKi::Dicts::ElementOps<const LHCb::MCVertex*>   m_fo8  ;
    LoKi::Dicts::SourceOps<const LHCb::MCParticle*>  m_fo9  ;
    LoKi::Dicts::SourceOps<const LHCb::MCVertex*>    m_fo10 ;
    // calls 
    LoKi::Dicts::FunCalls<LHCb::MCParticle>       m_c1 ;
    LoKi::Dicts::CutCalls<LHCb::MCParticle>       m_c2 ;
    LoKi::Dicts::FunCalls<LHCb::MCVertex>         m_c3 ;
    LoKi::Dicts::CutCalls<LHCb::MCVertex>         m_c4 ;
    /// the special operators for identifiers
    LoKi::Dicts::PIDOps<LoKi::MCParticles::Identifier>    m_i1 ;
    LoKi::Dicts::PIDOps<LoKi::MCParticles::AbsIdentifier> m_i2 ;
    // ========================================================================
    LoKi::Functors::Empty<const LHCb::MCParticle*>  m_e1 ;
    LoKi::Functors::Empty<const LHCb::MCVertex*>    m_e2 ;    
    LoKi::Functors::Size<const LHCb::MCParticle*>   m_s1 ;
    LoKi::Functors::Size<const LHCb::MCVertex*>     m_s2 ;    
    /// decay related stuff 
    Decays::Tree_<const LHCb::MCParticle*>           m_tree1 ;
    Decays::Trees::Any_<const LHCb::MCParticle*>     m_tree2 ;
    Decays::Trees::None_<const LHCb::MCParticle*>    m_tree3 ;
    Decays::Trees::Stable_<const LHCb::MCParticle*>  m_tree4 ;
    LoKi::Dicts::TreeOps<const LHCb::MCParticle*>    m_trops ;   
    // ========================================================================
    // Decay Finder 
    // ========================================================================
    Decays::IMCDecay::Finder m_finder ;
    Decays::IMCDecay::Tree   m_tree   ;
    LoKi::Dicts::FinderDicts_<const LHCb::MCParticle*> m_finderDicts ;
    // ========================================================================
   } ;  
  //===========================================================================
} // end of anonymout namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKIMCDICT_H
// ============================================================================
