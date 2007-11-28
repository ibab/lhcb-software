// $Id: Particles4.cpp,v 1.12 2007-11-28 14:39:30 ibelyaev Exp $
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
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Particles::ImpPar& right  ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Vertices::ImpParBase            ( right ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::result_type 
LoKi::Particles::ImpPar::ip
( LoKi::Particles::ImpPar::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error(" Invalid argument! , return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  if ( !valid() ) 
  {
    Error(" Invalid vertex! , return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  if ( 0 == tool()  ) 
  {
    Error(" Invalid tool! , return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  StatusCode sc      = StatusCode::FAILURE  ;
  double     impact = 0 ;
  double     error  = 0 ;
  
  switch ( type() ) 
  {
  case LoKi::Vertices::VertexHolder::_vertex :
    sc = tool() -> calcImpactPar( *p , *vertex()   , impact , error ) ;
    break ;                                                  // BREAK
  case LoKi::Vertices::VertexHolder::_point :
    sc = tool() -> calcImpactPar( *p , position()  , impact , error ) ;
    break ;                                                  // BREAK
  default :
    Error ( "Invalid switch! '" + Gaudi::Utils::toString( type() ) + "'") ;
    return LoKi::Constants::InvalidDistance ;
    break ;                                                  // BREAK    
  }
  
  if ( sc.isFailure() ) 
  {
    Error("Error from IGeomDispCalculator; return 'InvalidDistance'" , sc ) ;
    return LoKi::Constants::InvalidDistance ;                // RETURN 
  }
  
  return impact ;
}
// ============================================================================
std::ostream&
LoKi::Particles::ImpPar::fillStream
( std::ostream& stream ) const 
{ return stream <<  "IP" ; }
// ============================================================================


// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LHCb::VertexBase*                vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Point3D&                   vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::VertexHolder&  vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LHCb::VertexBase*                vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LoKi::Point3D&                   vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LoKi::Vertices::VertexHolder& vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::ImpParBase( tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Particles::ImpParChi2& right  ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Vertices::ImpParBase            ( right ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::result_type 
LoKi::Particles::ImpParChi2::chi2
( LoKi::Particles::ImpParChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Invalid argument! , return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  if ( !valid() ) 
  {
    Error ( " Invalid vertex! , return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  if ( 0 == tool()  ) 
  {
    Error ( " Invalid tool! , return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  StatusCode sc      = StatusCode::FAILURE  ;
  double     impact = 0 ;
  double     error  = 0 ;
  
  switch ( type() ) 
  {
  case LoKi::Vertices::VertexHolder::_vertex :
    sc = tool() -> calcImpactPar( *p , *vertex()   , impact , error ) ;
    break ;                                                  // BREAK
  case LoKi::Vertices::VertexHolder::_point :
    sc = tool() -> calcImpactPar( *p , position()  , impact , error ) ;
    break ;                                                  // BREAK
  default :
    Error ( "Invalid switch! '" + Gaudi::Utils::toString( type() ) + "'") ;
    return LoKi::Constants::InvalidChi2 ;
    break ;                                                  // BREAK    
  }
  
  if ( sc.isFailure() ) 
  {
    Error("Error from IGeomDispCalculator; return 'InvalidChi2'" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                // RETURN 
  }
  if ( 0 >= error ) 
  {
    Error ( "Invalid 'Sigma'; return 'InvalidChi2'" 
            + Gaudi::Utils::toString ( error ) ) ;
    return LoKi::Constants::InvalidChi2 ;                // RETURN 
  }
  
  return impact * impact / error / error ;
}
// ============================================================================
std::ostream&
LoKi::Particles::ImpParChi2::fillStream
( std::ostream& stream ) const 
{ return stream <<  "IPCHI2" ; }
// ============================================================================


// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::VertexBase::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::VertexBase::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::Vertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::Vertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const SmartRefVector<LHCb::Vertex>& vertices , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::PhysTypes::VRange& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Keeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Keeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::UniqueKeeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::RecVertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::RecVertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::RecVertices* vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Keeper<LHCb::RecVertex>& vertices ,
  const LoKi::Vertices::ImpactParamTool& tool )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices ,
  const LoKi::Vertices::ImpactParamTool& tool )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::Vector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::ConstVector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::Vector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::ConstVector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const SmartRefVector<LHCb::Vertex>& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::PhysTypes::VRange& vertices   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::VertexBase>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::Vertex>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::Vertex>& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertex::Vector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertex::ConstVector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertices* vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::RecVertex>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
// Copy constructor 
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Particles::MinImpPar& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Keeper<LHCb::VertexBase>        ( right )
  , m_fun                                 ( right.m_fun )
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
  
  result_type result =  LoKi::Constants::InvalidDistance ;
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() , m_fun  , p , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;    
  }
  
  return result ;
  
} ;
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
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::VertexBase::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::Vertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::Vertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const SmartRefVector<LHCb::Vertex>& vertices , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::PhysTypes::VRange& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Keeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Keeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::UniqueKeeper<LHCb::Vertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::RecVertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::RecVertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LHCb::RecVertices* vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Keeper<LHCb::RecVertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::Vector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::VertexBase::ConstVector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::Vector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::ConstVector& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const SmartRefVector<LHCb::Vertex>& vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::PhysTypes::VRange& vertices   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::VertexBase>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::VertexBase>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::Vertex>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::Vertex>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::RecVertex::Vector& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::RecVertex::ConstVector& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices.begin() , vertices.end() )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::RecVertices* vertices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{
  if ( 0 != vertices ) 
  { addObjects ( vertices->begin() , vertices->end() ) ; }
};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::Keeper<LHCb::RecVertex>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
//  constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Keeper<LHCb::VertexBase>( vertices )
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{};
// ============================================================================
// Copy constructor 
// ============================================================================
LoKi::Particles::MinImpParChi2::MinImpParChi2
( const LoKi::Particles::MinImpParChi2& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Keeper<LHCb::VertexBase>        ( right )
  , m_fun                                 ( right.m_fun )
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
  
  result_type result =  LoKi::Constants::InvalidChi2 ;
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() , m_fun  , p , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;    
  }
  
  return result ;
} ;
// ============================================================================
std::ostream&
LoKi::Particles::MinImpParChi2::fillStream
( std::ostream& stream ) const 
{ return stream <<  "MIPCHI2" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
