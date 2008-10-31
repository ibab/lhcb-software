// $Id: Particles4.cpp,v 1.16 2008-10-31 17:27:46 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles4.h"
#include "LoKi/PhysHelpers.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
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
 *  @date 2006-02-10 
 */
// ============================================================================
namespace 
{
  const LHCb::VertexBase * const s_VERTEX = 0 ;
} 
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LHCb::VertexBase*                vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Point3D&                   vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::VertexHolder& vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LHCb::VertexBase*                vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LoKi::Point3D&                   vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LoKi::Vertices::VertexHolder& vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::result_type 
LoKi::Particles::ImpPar::ip
( LoKi::Particles::ImpPar::argument p ) const 
{
  //
  if ( !valid() ) 
  {
    Error(" Invalid vertex! , return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  Assert ( 0 != tool() , " Invalid IDistanceCalculator*" ) ;
  //
  StatusCode sc      = StatusCode::FAILURE  ;
  double     impact = LoKi::Constants::InvalidDistance ;
  //
  switch ( type() ) 
  {
  case LoKi::Vertices::VertexHolder::_vertex :
    sc = tool() -> distance ( p , vertex()   , impact ) ;
    break ;                                                  // BREAK
  case LoKi::Vertices::VertexHolder::_point :
    sc = tool() -> distance ( p , position() , impact ) ;
    break ;                                                  // BREAK
  default :
    Error ( "Invalid switch! '" + Gaudi::Utils::toString( type() ) + "'") ;
    return LoKi::Constants::InvalidDistance ;
    break ;                                                  // BREAK    
  }
  //
  if ( sc.isFailure() ) 
  {
    Error("Error from IDistanceCalculator; return 'InvalidDistance'" , sc ) ;
    return LoKi::Constants::InvalidDistance ;                // RETURN 
  }
  //
  return impact ;
}
// ============================================================================
LoKi::Particles::ImpParChi2::result_type 
LoKi::Particles::ImpParChi2::ipchi2
( LoKi::Particles::ImpParChi2::argument p ) const 
{
  //
  if ( !valid() ) 
  {
    Error ( " Invalid vertex! , return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  Assert ( 0 != tool() , " Invalid IDistanceCalculator*") ;
  //
  StatusCode sc      = StatusCode::FAILURE  ;
  double     impact  = LoKi::Constants::InvalidDistance ;
  double     chi2    = LoKi::Constants::InvalidChi2     ;
  //
  switch ( type() ) 
  {
  case LoKi::Vertices::VertexHolder::_vertex :
    sc = tool() -> distance ( p , vertex()    , impact , chi2 ) ;
    break ;                                                  // BREAK
  case LoKi::Vertices::VertexHolder::_point :
    sc = tool() -> distance ( p , position()  , impact , chi2 ) ;
    break ;                                                  // BREAK
  default :
    Error ( "Invalid switch! '" + Gaudi::Utils::toString( type() ) + "'") ;
    return LoKi::Constants::InvalidChi2 ;
    break ;                                                  // BREAK    
  }
  //
  if ( sc.isFailure() ) 
  {
    Error("Error from IDistanceCalculator; return 'InvalidChi2'" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                // RETURN 
  }
  //
  return chi2 ;
}
// ============================================================================

// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::VertexBase::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::VertexBase::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::Vertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::Vertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const SmartRefVector<LHCb::Vertex>& vertices , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::PhysTypes::VRange& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Keeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Keeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::UniqueKeeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::RecVertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::RecVertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::RecVertices* vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ()
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Keeper<LHCb::RecVertex>& vertices ,
  const LoKi::Vertices::ImpactParamTool& tool )
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices ,
  const LoKi::Vertices::ImpactParamTool& tool )
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::Vector& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::ConstVector& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::Vector& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::ConstVector& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const SmartRefVector<LHCb::Vertex>& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::PhysTypes::VRange& vertices   ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::VertexBase>& vertices )
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::Vertex>& vertices )
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::Vertex>& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertex::Vector& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertex::ConstVector& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertices* vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ()
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::RecVertex>& vertices )
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices ) 
  : LoKi::Particles::ImpPar ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices )
{}
// ============================================================================
LoKi::Particles::MinImpPar::result_type 
LoKi::Particles::MinImpPar::mip
( LoKi::Particles::MinImpPar::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error(" Invalid argument! , return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  if ( empty () ) 
  {
    Error ( " Empty list of vertices, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;    
  }
  //
  result_type result =  LoKi::Constants::InvalidDistance ;
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() , 
         LoKi::Helpers::PMFA<LoKi::Particles::ImpPar>
         ( this , &LoKi::Particles::ImpPar::ip ) , 
         p , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;    
  }
  //
  setVertex ( s_VERTEX ) ;
  //
  return result ; 
}
// ============================================================================
std::ostream&
LoKi::Particles::MinImpPar::fillStream
( std::ostream& stream ) const 
{ return stream <<  "MIP" ; }
// ============================================================================

// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::VertexBase::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::VertexBase::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::Vertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::Vertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const SmartRefVector<LHCb::Vertex>& vertices , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::PhysTypes::VRange& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Keeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Keeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::UniqueKeeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::RecVertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::RecVertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::RecVertices* vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper()
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Keeper<LHCb::RecVertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::Vector& vertices ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::ConstVector& vertices ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::Vector& vertices ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::ConstVector& vertices ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const SmartRefVector<LHCb::Vertex>& vertices ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::PhysTypes::VRange& vertices   ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::VertexBase>& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::Vertex>& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::Vertex>& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::RecVertex::Vector& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::RecVertex::ConstVector& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper ( vertices.begin() , vertices.end() )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertices* vertices ) 
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper()
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::RecVertex>& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices )
  : LoKi::Particles::ImpParChi2 ( LoKi::Helpers::_First( vertices ) , tool )
  , m_keeper( vertices )
{}
// ============================================================================
LoKi::Particles::MinImpParChi2::result_type 
LoKi::Particles::MinImpParChi2::mipchi2
( LoKi::Particles::MinImpParChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error(" Invalid argument! , return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( empty () ) 
  {
    Error ( " Empty list of vertices, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;    
  }
  //
  result_type result =  LoKi::Constants::InvalidChi2 ;
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() , 
         LoKi::Helpers::PMFA<LoKi::Particles::ImpParChi2>
         ( this , &LoKi::Particles::ImpParChi2::chi2 ) , 
         p , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;    
  }
  //
  return result ;
} 
// ============================================================================
std::ostream&
LoKi::Particles::MinImpParChi2::fillStream
( std::ostream& stream ) const 
{ return stream <<  "MIPCHI2" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
