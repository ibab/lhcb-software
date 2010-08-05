// $Id$
// ============================================================================
#ifndef LOKI_LOKITRIGGERDICT_H 
#define LOKI_LOKITRIGGERDICT_H 1
// ============================================================================
// Include files
// ============================================================================
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/HltTool.h"
// ============================================================================
#include "LoKi/TriggerTypes.h"
#include "LoKi/HltCandidates.h"
#include "LoKi/HltStages.h"
#include "LoKi/Hlt1.h"
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/FuncOps.h"
// ============================================================================
#include "LoKi/ITrgFunctorFactory.h"
#include "LoKi/ITrgFunctorAntiFactory.h"
#include "LoKi/TrgEngine.h"
// ============================================================================
/** @file
 *  Reflex dictionary for LoKiTrigger package 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 */
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    // ========================================================================
    // Functors & Operations for Hlt::Candidates 
    // ========================================================================
    // operators 
    LoKi::Dicts::FuncOps    <const Hlt::Candidate*>  m_o1  ;
    LoKi::Dicts::CutsOps    <const Hlt::Candidate*>  m_o3  ;
    // functionals:
    LoKi::Dicts::MapsOps    <const Hlt::Candidate*>  m_fo1 ;
    LoKi::Dicts::PipeOps    <const Hlt::Candidate*>  m_fo2 ;
    LoKi::Dicts::FunValOps  <const Hlt::Candidate*>  m_fo3 ;
    LoKi::Dicts::CutValOps  <const Hlt::Candidate*>  m_fo4 ;
    LoKi::Dicts::ElementOps <const Hlt::Candidate*>  m_fo5 ;
    LoKi::Dicts::SourceOps  <const Hlt::Candidate*>  m_fo6 ;
    // calls
    LoKi::Dicts::FunCalls   <Hlt::Candidate>         m_c1  ;
    LoKi::Dicts::CutCalls   <Hlt::Candidate>         m_c2  ;
    // the basic functions 
    LoKi::Dicts::Funcs      <const Hlt::Candidate*>  m_f1 ;
    LoKi::Dicts::VFuncs     <const Hlt::Candidate*>  m_f3 ;
    // ========================================================================
    // Functors & Operations for Hlt::Stage 
    // ========================================================================
    // operators 
    LoKi::Dicts::FuncOps    <const Hlt::Stage*>  m_o1_  ;
    LoKi::Dicts::CutsOps    <const Hlt::Stage*>  m_o3_  ;
    // functionals: - no functionals here! 
    // calls
    LoKi::Dicts::FunCalls   <Hlt::Stage>         m_c1_  ;
    LoKi::Dicts::CutCalls   <Hlt::Stage>         m_c2_  ;
    // the basic functions 
    LoKi::Dicts::Funcs      <const Hlt::Stage*>  m_f1_  ;
    // ========================================================================    
    /// needed for various Hlt interfaces 
    LoKi::Interface<Hlt::IRegister>          m_htl1 ;
    LoKi::Interface<Hlt::IData>              m_htl2 ;
    LoKi::Interface<Hlt::IInspector>         m_htl3 ;
    Hlt::IInspector::SelList                 m_hlt4 ;
    Hlt::IInspector::AlgList                 m_hlt5 ;
    /// fictive constructor 
    _Instantiations () ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_LOKITRIGGERDICT_H
// ============================================================================
