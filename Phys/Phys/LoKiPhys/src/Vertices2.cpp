// $Id: Vertices2.cpp,v 1.3 2007-07-23 17:35:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles4.h"
#include "LoKi/Vertices2.h"
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpPar::ImpPar
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpactParamTool& tool     ) 
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( (const LHCb::VertexBase*)0 , tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpPar::ImpPar
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpParBase&      tool     ) 
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpPar::ImpPar
( const LoKi::Vertices::ImpactParamTool& tool     ,
  const LHCb::Particle*                  particle ) 
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( (const LHCb::VertexBase*)0 , tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpPar::ImpPar
( const LoKi::Vertices::ImpParBase&      tool     ,
  const LHCb::Particle*                  particle )
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Vertices::ImpPar::ImpPar
( const LoKi::Vertices::ImpPar& right )
  : LoKi::AuxFunBase( right )
  , LoKi::Function<const LHCb::VertexBase*> ( right )
  , m_evaluator ( right.m_evaluator  )
  , m_particle  ( right.m_particle ) 
{} 
// ============================================================================
//  mandatory clone ("virtual constructor")
// ============================================================================
LoKi::Vertices::ImpPar*
LoKi::Vertices::ImpPar::clone() const 
{ return new LoKi::Vertices::ImpPar(*this) ; }
// ============================================================================
//  destructor
// ============================================================================
LoKi::Vertices::ImpPar::~ImpPar(){}
// ============================================================================
//  the only one essential method 
// ============================================================================
LoKi::Vertices::ImpPar::result_type 
LoKi::Vertices::ImpPar::operator() 
  ( LoKi::Vertices::ImpPar::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* argument is invalid! return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  m_evaluator.setVertex ( v ) ;
  if ( 0 == m_particle ) 
  { Warning( "LHCb::Particle* parameter is invalid!") ; }
  // use the actual evaluator 
  return m_evaluator ( m_particle ) ;
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream&
LoKi::Vertices::ImpPar::fillStream( std::ostream& s ) const
{ return s << "VIP" ; }
// ============================================================================


// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpParChi2::ImpParChi2
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpactParamTool& tool     ) 
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( (const LHCb::VertexBase*)0 , tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpParChi2::ImpParChi2
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpParBase&      tool     ) 
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool     ,
  const LHCb::Particle*                  particle ) 
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( (const LHCb::VertexBase*)0 , tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
//  constructor from the particle and tool
// ============================================================================
LoKi::Vertices::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpParBase&      tool     ,
  const LHCb::Particle*                  particle )
  : LoKi::Function<const LHCb::VertexBase*> ()
  , m_evaluator ( tool )
  , m_particle  ( particle ) 
{} 
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Vertices::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpParChi2& right )
  : LoKi::AuxFunBase( right )
  , LoKi::Function<const LHCb::VertexBase*> ( right )
  , m_evaluator ( right.m_evaluator  )
  , m_particle  ( right.m_particle ) 
{} 
// ============================================================================
//  mandatory clone ("virtual constructor")
// ============================================================================
LoKi::Vertices::ImpParChi2*
LoKi::Vertices::ImpParChi2::clone() const 
{ return new LoKi::Vertices::ImpParChi2(*this) ; }
// ============================================================================
//  destructor
// ============================================================================
LoKi::Vertices::ImpParChi2::~ImpParChi2(){}
// ============================================================================
//  the only one essential method 
// ============================================================================
LoKi::Vertices::ImpParChi2::result_type 
LoKi::Vertices::ImpParChi2::operator() 
  ( LoKi::Vertices::ImpParChi2::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* argument is invalid! return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  m_evaluator.setVertex ( v ) ;
  if ( 0 == m_particle ) 
  { Warning( "LHCb::Particle* parameter is invalid!") ; }
  // use the actual evaluator 
  return m_evaluator ( m_particle ) ;
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream&
LoKi::Vertices::ImpParChi2::fillStream( std::ostream& s ) const
{ return s << "VIPCHI2" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
