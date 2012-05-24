// $Id$
// ============================================================================
#ifndef LOKI_PHYSTYPES_H 
#define LOKI_PHYSTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/CoreTypes.h"
#include "LoKi/Functions.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/RecVertex.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::PhysTypes LoKi/PhysTypes.h
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  By usage of this code one clearly states the disagreement 
   *  with the campain of Dr.O.Callot et al.: 
   *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu 
   *  @date   2006-02-11
   */
  namespace  PhysTypes 
  {
    // ========================================================================
    /// type of 'cuts' for LHCb::Particle       (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Predicate Cuts  ;
    /// type of 'functions' for LHCb::Particle  (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Function  Func  ;
    /// type of 'cuts' for LHCb::Particle       (assignable)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate  Cut ; 
    /// type of 'functions' for LHCb::Particle  (assignable)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::FunctionFromFunction    Fun ;
    /// type of 'cuts' for LHCb::VertexBase     (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate VCuts ;
    /// type of 'functions' for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Function  VFunc ;
    /// type of 'cuts' for LHCb::Vertex         (assignable)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::PredicateFromPredicate VCut ; 
    /// type of 'functions' for LHCb::Vertex    (assignable)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::FunctionFromFunction   VFun ;
    //
    // ========================================================================    
    // functional types for LHCb::Particle: 
    // ========================================================================    
    //
    // type of "map"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Map       Maps     ;
    // type of "map"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Maps>::Type                          Map      ;
    
    // type of "pipe"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Pipe      Pipes    ;
    // type of "pipe"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Pipes>::Type                         Pipe     ; 
    
    // type of "fun-val"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::FunVal    FunVals  ;
    // type of "fun-val"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<FunVals>::Type                       FunVal   ; 

    // type of "fun-val"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::CutVal    CutVals  ;
    // type of "fun-val"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<CutVals>::Type                       CutVal   ; 
    
    // type of "source"  for LHCb::Particle (interface)
    typedef LoKi::BasicFunctors<const LHCb::Particle*>::Source    Sources  ;
    // type of "source"  for LHCb::Particle (assignable)
    typedef LoKi::Assignable<Sources>::Type                       Source   ;
    //
    // ========================================================================
    // functional types for LHCb::VertexBase
    // ========================================================================
    //
    // type of "map"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Map     VMaps    ;
    // type of "map"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VMaps>::Type                         VMap     ;
    
    // type of "pipe"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Pipe    VPipes   ;
    // type of "pipe"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VPipes>::Type                        VPipe    ; 
    
    // type of "fun-val"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::FunVal  VFunVals ;
    // type of "fun-val"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VFunVals>::Type                      VFunVal  ; 

    // type of "cut-val"  for LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::CutVal  VCutVals  ;
    // type of "cut-val"  for LHCb::VertexBase (assignable)
    typedef LoKi::Assignable<VCutVals>::Type                      VCutVal   ;

    // type of "source"  for  LHCb::VertexBase (interface)
    typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  VSources  ;
    // type of "source"  for Particles (assignable)
    typedef LoKi::Assignable<VSources>::Type                      VSource   ;
    // ========================================================================
  } //                                         end of namespace LoKi::PhysTypes
  // ==========================================================================
  namespace Types 
  {    
    // ========================================================================
    /// type of 'cuts' for LHCb::Particle        (interface)
    typedef LoKi::PhysTypes::Cuts                               Cuts  ;
    /// type of 'functions' for LHCb::Particle   (interface)
    typedef LoKi::PhysTypes::Func                               Func  ;
    /// type of 'cuts' for LHCb::Particle        (assignable)
    typedef LoKi::PhysTypes::Cut                                Cut   ; 
    /// type of 'functions' for LHCb::Particle   (assignable)
    typedef LoKi::PhysTypes::Fun                                Fun   ;
    /// type of 'cuts' for LHCb::VertexBase      (interface)
    typedef LoKi::PhysTypes::VCuts                              VCuts ;
    /// type of 'functions' for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VFunc                              VFunc ;
    /// type of 'cuts' for LHCb::VertexBase      (assignable)
    typedef LoKi::PhysTypes::VCut                               VCut  ; 
    /// type of 'functions' for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VFun                               VFun  ;  
    //
    // ========================================================================
    // functional  types for LHCb::Particle 
    // ========================================================================
    //
    // type for "map" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Maps                               Maps      ;
    // type for "map" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Map                                Map       ;
    // type for "pipe" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Pipes                              Pipes     ;
    // type for "pipe" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Pipe                               Pipe      ;
    // type for "fun-val" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::FunVals                            FunVals   ;
    // type for "fun-val" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::FunVal                             FunVal    ;
    // type for "cut-val" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::CutVals                            CutVals   ;
    // type for "cut-val" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::CutVal                             CutVal    ;
    // type for "source" for LHCb::Particle (interface)
    typedef LoKi::PhysTypes::Sources                            Sources   ;
    // type for "source" for LHCb::Particle (assignable)
    typedef LoKi::PhysTypes::Source                             Source    ;
    //
    // ========================================================================
    // functional  types for LHcb::VertexBase 
    // ========================================================================
    //
    // type for "map" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VMaps                              VMaps     ;
    // type for "map" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VMap                               VMap      ;
    // type for "pipe" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VPipes                             VPipes    ;
    // type for "pipe" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VPipe                              VPipe     ;
    // type for "fun-val" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VFunVals                           VFunVals  ;
    // type for "fun-val" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VFunVal                            VFunVal   ;
    // type for "cut-val" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VCutVals                           VCutVals  ;
    // type for "cut-val" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VCutVal                            VCutVal   ;
    // type for "source" for LHCb::VertexBase (interface)
    typedef LoKi::PhysTypes::VSources                           VSources  ;
    // type for "source" for LHCb::VertexBase (assignable)
    typedef LoKi::PhysTypes::VSource                            VSource   ;
    // ========================================================================
  }  //                                            end of namespace LoKi::Types 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  inline std::ostream& 
  Constant<const LHCb::Particle*,bool>::fillStream   ( std::ostream& s ) const 
  { return s << ( this->m_value ? "ALL" : "NONE" ) ; }
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================  
  template <>
  inline std::ostream& 
  Constant<const LHCb::VertexBase*,bool>::fillStream ( std::ostream& s ) const 
  { return s << ( this->m_value ? "VALL" : "VNONE" ) ; }
  // ==========================================================================  
} //                                                      end of namespace LoKi
// ============================================================================
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
// a little bit of streamers 
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::ConstVector&    input , 
  const LoKi::Types::Cuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Container*      input , 
  const LoKi::Types::Cuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Selection*      input , 
  const LoKi::Types::Cuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const SmartRefVector<LHCb::Particle>& input , 
  const LoKi::Types::Cuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Range&          input , 
  const LoKi::Types::Cuts&              cuts  ) ;
// ============================================================================
// VERTICES 
// ============================================================================
GAUDI_API 
LHCb::VertexBase::ConstVector
operator>> 
( const LHCb::VertexBase::ConstVector&  input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::VertexBase::ConstVector
operator>> 
( const LHCb::Vertex::ConstVector&      input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::RecVertex::ConstVector
operator>> 
( const LHCb::RecVertex::ConstVector&   input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::VertexBase::ConstVector
operator>> 
( const SmartRefVector<LHCb::Vertex>&   input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::RecVertex::ConstVector
operator>> 
( const SmartRefVector<LHCb::RecVertex>& input , 
  const LoKi::Types::VCuts&              cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::VertexBase::ConstVector
operator>> 
( const LHCb::Vertex::Container*        input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::RecVertex::ConstVector
operator>> 
( const LHCb::RecVertex::Container*     input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::VertexBase::ConstVector
operator>> 
( const LoKi::Types::VRange&            input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
// Functions 
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Particle::ConstVector&    input , 
  const LoKi::Types::Func&              cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Particle::Range&          input , 
  const LoKi::Types::Func&              cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Particle::Container*      input , 
  const LoKi::Types::Func&              cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Particle::Selection*      input , 
  const LoKi::Types::Func&              cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::Particle>& input , 
  const LoKi::Types::Func&              cuts  ) ;
// ============================================================================
// vertex functions 
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::VertexBase::ConstVector&  input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Vertex::ConstVector&      input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::RecVertex::ConstVector&   input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Vertex::Container*        input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::RecVertex::Container*     input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::Vertex>&   input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::RecVertex>&input , 
  const LoKi::Types::VFunc&             cuts  ) ;
// ============================================================================
// Some weird stuff 
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::ConstVector&    input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Container*      input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Selection*      input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const SmartRefVector<LHCb::Particle>& input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Particle::ConstVector
operator>> 
( const LHCb::Particle::Range&          input , 
  const LoKi::Types::VCuts&             cuts  ) ;
// ============================================================================



// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PHYSTYPES_H
// ============================================================================
