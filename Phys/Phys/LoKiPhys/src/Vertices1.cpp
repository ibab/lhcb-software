// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Geometry.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices1.h"
#include "LoKi/PhysHelpers.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Vertices
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
 *  @date 2006-02-16 
 */
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function()  
  , LoKi::Vertices::VertexHolder ( vertex )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Point3D&  point ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function()  
  , LoKi::Vertices::VertexHolder ( point )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexHolder& right ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function()  
  , LoKi::Vertices::VertexHolder ( right  )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::result_type 
LoKi::Vertices::VertexDistance::distance  
( LoKi::Vertices::VertexDistance::argument v ) const 
{
  if ( !valid() ) { Warning("VertexHiolder base is invalid!") ; }
  if ( 0 != v ) {  return ( v->position() - this->position()).R() ; } //RETURN 
  Error ( " Invalid Vertex, return 'InvalidDistance'") ;
  return LoKi::Constants::InvalidDistance ;      // RETURN 
}
// ============================================================================
LoKi::Vertices::VertexDeltaZ::result_type 
LoKi::Vertices::VertexDeltaZ::deltaZ
( LoKi::Vertices::VertexDeltaZ::argument v ) const
{
  if ( 0 == v ) 
  {
    Error ( " Invalid Vertex, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;      // RETURN 
  }
  if ( !valid() ) 
  { 
    Warning("VertexHolder base is invalid, return 'InvalidDistance'") ; 
    return LoKi::Constants::InvalidDistance ;      // RETURN 
  }
  return v->position().Z() - this->position().Z() ;
}
// ============================================================================
LoKi::Vertices::VertexAbsDeltaZ::result_type 
LoKi::Vertices::VertexAbsDeltaZ::absDeltaZ
( LoKi::Vertices::VertexAbsDeltaZ::argument v ) const
{
  if ( 0 == v ) 
  {
    Error ( " Invalid Vertex, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;      // RETURN 
  }
  if ( !valid() ) 
  { 
    Warning("VertexHolder base is invalid, return 'InvalidDistance'") ; 
    return LoKi::Constants::InvalidDistance ;      // RETURN 
  }
  return std::fabs ( v->position().Z() - this->position().Z() ) ;
}
// ============================================================================
LoKi::Vertices::VertexSignedDistance::result_type 
LoKi::Vertices::VertexSignedDistance::signedDistance  
( LoKi::Vertices::VertexSignedDistance::argument v ) const 
{
  if ( 0 != v ) 
  {
    const double dist = VertexDistance::distance ( v ) ;
    return 
      v->position().Z() < position().Z() ? ( -1 * dist ) : dist ;
  }
  Error ( " Invalid Vertex, return 'InvalidDistance'") ;
  return LoKi::Constants::InvalidDistance ;      // RETURN 
}
// ============================================================================
LoKi::Vertices::VertexDistance::result_type 
LoKi::Vertices::VertexDistance::chi2  
( LoKi::Vertices::VertexDistance::argument v ) const 
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;         // RETURN 
  }
  else if ( !valid() ) 
  {
    Error ( " Invalid Status, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;         // RETURN 
  }
  
  double _chi2 = LoKi::Constants::InvalidChi2 ;
  
  StatusCode sc = StatusCode::SUCCESS ;
  switch ( type() ) 
  {
  case _vertex  : 
    sc = LoKi::Geometry::chi2 
      ( v        -> position  () ,
        v        -> covMatrix () , 
        vertex() -> position  () , 
        vertex() -> covMatrix () , _chi2 ) ; break ;
  case _point   : 
    sc = LoKi::Geometry::chi2 
      ( point()                  ,
        v        -> position  () ,
        v        -> covMatrix () , _chi2 ) ; break ;
  default:
    sc = StatusCode::FAILURE ; break ;
  } ;
  //
  if ( sc.isFailure() ) 
  { Error ( "Evaluation is invalid, return " + 
            Gaudi::Utils::toString ( _chi2 ) , sc, 0 ) ; }
  //
  return _chi2 ;
}
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::VertexBase::ConstVector& vs ) 
  : LoKi::Vertices::VertexDistance ( LoKi::Point3D () ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>( vs.begin() , vs.end() )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::Vertex::ConstVector& vs ) 
  : LoKi::Vertices::VertexDistance ( LoKi::Point3D () ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>( vs.begin() , vs.end() )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::RecVertex::ConstVector& vs ) 
  : LoKi::Vertices::VertexDistance ( LoKi::Point3D () ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>( vs.begin() , vs.end() )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::RecVertex::Container* vs ) 
  : LoKi::Vertices::VertexDistance ( LoKi::Point3D () ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>()
{
  if ( 0 != vs ) { addObjects( vs->begin() , vs->end() ) ; }
} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::PhysTypes::VRange& vs ) 
  : LoKi::Vertices::VertexDistance ( LoKi::Point3D () ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>( vs.begin() , vs.end() )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::result_type 
LoKi::Vertices::MinVertexDistance::minvd
( LoKi::Vertices::MinVertexDistance::argument v ) const
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance;                     // RETURN 
  }
  
  result_type result =  LoKi::Constants::PositiveInfinity ;
  
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() ,
         LoKi::Helpers::PMFA<LoKi::Vertices::VertexDistance>
         ( this , &LoKi::Vertices::VertexDistance::distance) , 
         v , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;               // RETURN 
  }
  //
  return result ;                                           // RETURN 
}
// ============================================================================
LoKi::Vertices::MinVertexDistance::result_type 
LoKi::Vertices::MinVertexDistance::minvdchi2 
( LoKi::Vertices::MinVertexDistance::argument v ) const
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;                     // RETURN 
  }
  
  result_type result =  LoKi::Constants::PositiveInfinity ;
  
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() ,
         LoKi::Helpers::PMFA<LoKi::Vertices::VertexDistance>
         ( this , &LoKi::Vertices::VertexDistance::chi2) ,
         v , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;               // RETURN 
  }
  //
  return result ;                                           // RETURN 
}
// ============================================================================

// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::VertexBase::ConstVector& vs ) 
  : LoKi::Vertices::MinVertexDistance ( vs ) 
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::Vertex::ConstVector& vs ) 
  : LoKi::Vertices::MinVertexDistance ( vs ) 
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::RecVertex::ConstVector& vs ) 
  : LoKi::Vertices::MinVertexDistance ( vs ) 
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::RecVertex::Container* vs )  
  : LoKi::Vertices::MinVertexDistance ( vs ) 
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::PhysTypes::VRange& vs ) 
  : LoKi::Vertices::MinVertexDistance ( vs ) 
{} 

// ============================================================================
// The END 
// ============================================================================

