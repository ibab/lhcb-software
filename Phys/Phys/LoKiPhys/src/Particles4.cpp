// $Id: Particles4.cpp,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/SystemOfUnits.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/PrimVertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles4.h"
#include "LoKi/Helpers.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-10 
 */
// ============================================================================


// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LHCb::Vertex*                    vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Point3D&                   vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LHCb::Vertex*                    vertex )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LoKi::Point3D&                   vertex )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( tool ) 
{}
// ============================================================================
LoKi::Particles::ImpPar::ImpPar
( const LoKi::Particles::ImpPar& right  ) 
  : LoKi::Function<const LHCb::Particle*> ( right )
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
    Error ( "Invalid switch! '" + LoKi::Print::print( long(type()) ) + "'") ;
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



// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LHCb::Vertex*                    vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Point3D&                   vertex , 
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LHCb::Vertex*                    vertex )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpactParamTool& tool   ,
  const LoKi::Point3D&                   vertex )
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( vertex , tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::ImpParBase( tool ) 
{}
// ============================================================================
LoKi::Particles::ImpParChi2::ImpParChi2
( const LoKi::Particles::ImpParChi2& right  ) 
  : LoKi::Function<const LHCb::Particle*> ( right )
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
    Error ( "Invalid switch! '" + LoKi::Print::print( long(type()) ) + "'") ;
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
            + LoKi::Print::print ( error ) ) ;
    return LoKi::Constants::InvalidChi2 ;                // RETURN 
  }
  
  return impact * impact / error / error ;
}
// ============================================================================


// ============================================================================
/// constructor from vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::Vertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::Vertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const SmartRefVector<LHCb::Vertex>& vertices , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::PhysTypes::VRange& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::PrimVertex::Vector& vertices         , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::PrimVertex::ConstVector& vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const SmartRefVector<LHCb::PrimVertex>& vertices , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LHCb::PrimVertices* vertices   , 
  const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices->begin() , vertices->end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::Vector& vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LHCb::Vertex::ConstVector& vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const SmartRefVector<LHCb::Vertex>& vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const LoKi::PhysTypes::VRange& vertices   ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::PrimVertex::Vector& vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::PrimVertex::ConstVector& vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool ,
  const SmartRefVector<LHCb::PrimVertex>& vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices.begin() , vertices.end() )
{ removeVertex() ; }
// ============================================================================
/// constructor from the vertices and the tool
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Vertices::ImpactParamTool& tool , 
  const LHCb::PrimVertices* vertices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_fun      ( LoKi::Helpers::_First( vertices ) , tool )
  , m_vertices ( vertices->begin() , vertices->end() )
{ removeVertex() ; }
// ============================================================================
// Copy constructor 
// ============================================================================
LoKi::Particles::MinImpPar::MinImpPar
( const LoKi::Particles::MinImpPar& right ) 
  : LoKi::Function<const LHCb::Particle*> ( right )
  , m_fun      ( right.m_fun      )
  , m_vertices ( right.m_vertices )
{}
// ============================================================================
size_t LoKi::Particles::MinImpPar::removeVertex 
( const LHCb::Vertex* vertex )  
{
  VERTICES::iterator iv =  
    std::remove( m_vertices.begin() , m_vertices.end() , vertex ) ;
  const size_t removed = m_vertices.end() - iv ;
  m_vertices.erase  ( iv , m_vertices.end() ) ;
  return removed ;
};
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
  if ( m_vertices.end() == LoKi::Helpers::_Min_vertex 
       ( m_vertices.begin() , m_vertices.end() , m_fun  , p , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;    
  }
  
  return result ;
  
} ;
// ============================================================================





// ============================================================================
// The END 
// ============================================================================
