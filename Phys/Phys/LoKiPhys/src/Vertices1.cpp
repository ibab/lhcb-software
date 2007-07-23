// $Id: Vertices1.cpp,v 1.9 2007-07-23 17:35:57 ibelyaev Exp $
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
  : LoKi::Function<const LHCb::VertexBase*>()  
  , LoKi::Vertices::VertexHolder ( vertex )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Point3D&  point ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , LoKi::Vertices::VertexHolder ( point )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexHolder& right ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , LoKi::Vertices::VertexHolder ( right  )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexDistance& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::Function<const LHCb::VertexBase*>( right )  
  , LoKi::Vertices::VertexHolder ( right  )
{} 
// ============================================================================
LoKi::Vertices::VertexDistance::~VertexDistance(){}
// ============================================================================
LoKi::Vertices::VertexDistance*
LoKi::Vertices::VertexDistance::clone() const 
{ return new LoKi::Vertices::VertexDistance(*this) ; }
// ============================================================================
LoKi::Vertices::VertexDistance::result_type 
LoKi::Vertices::VertexDistance::operator() 
  ( LoKi::Vertices::VertexDistance::argument v ) const 
{ return distance( v ) ; }
// ============================================================================
LoKi::Vertices::VertexDistance::result_type 
LoKi::Vertices::VertexDistance::distance  
( LoKi::Vertices::VertexDistance::argument v ) const 
{
  if ( !valid() ) { Warning("VertexHiolder bas eis invalid!") ; }
  if ( 0 != v ) {  return ( v->position() - this->position()).R() ; } //RETURN 
  Error ( " Invalid Vertex, return 'InvalidDistance'") ;
  return LoKi::Constants::InvalidDistance ;      // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexDistance::fillStream
( std::ostream& s ) const 
{ return s << "VVDIST" ; }
// ============================================================================
LoKi::Vertices::VertexSignedDistance::VertexSignedDistance
( const LHCb::VertexBase* vertex ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , m_fun ( vertex )
{} 
// ============================================================================
LoKi::Vertices::VertexSignedDistance::VertexSignedDistance
( const LoKi::Point3D&  point ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , m_fun ( point )
{} 
// ============================================================================
LoKi::Vertices::VertexSignedDistance::VertexSignedDistance
( const LoKi::Vertices::VertexHolder& right ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , m_fun ( right )
{} 
// ============================================================================
LoKi::Vertices::VertexSignedDistance::VertexSignedDistance
( const LoKi::Vertices::VertexDistance& right ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , m_fun ( right ) 
{} 
// ============================================================================
LoKi::Vertices::VertexSignedDistance::VertexSignedDistance
( const LoKi::Vertices::VertexSignedDistance& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::Function<const LHCb::VertexBase*>( right )  
  , m_fun ( right.m_fun ) 
{} 
// ============================================================================
LoKi::Vertices::VertexSignedDistance::~VertexSignedDistance(){}
// ============================================================================
LoKi::Vertices::VertexSignedDistance*
LoKi::Vertices::VertexSignedDistance::clone() const 
{ return new LoKi::Vertices::VertexSignedDistance(*this) ; }
// ============================================================================
LoKi::Vertices::VertexSignedDistance::result_type 
LoKi::Vertices::VertexSignedDistance::operator() 
  ( LoKi::Vertices::VertexSignedDistance::argument v ) const 
{ return distance( v ) ; }
// ============================================================================
LoKi::Vertices::VertexSignedDistance::result_type 
LoKi::Vertices::VertexSignedDistance::distance  
( LoKi::Vertices::VertexSignedDistance::argument v ) const 
{
  if ( 0 != v ) 
  {
    const double dist = m_fun.distance( v ) ;
    return 
      v->position().Z() < m_fun.position().Z() ? ( -1 * dist ) : dist ;
  }
  Error ( " Invalid Vertex, return 'InvalidDistance'") ;
  return LoKi::Constants::InvalidDistance ;      // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexSignedDistance::fillStream
( std::ostream& s ) const 
{ return s << "VVDSIGN" ; }
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LHCb::VertexBase* vertex ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , LoKi::Vertices::VertexHolder ( vertex )
{} 
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LoKi::Point3D&  point ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , LoKi::Vertices::VertexHolder ( point )
{} 
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LoKi::Vertices::VertexHolder& right ) 
  : LoKi::Function<const LHCb::VertexBase*>()  
  , LoKi::Vertices::VertexHolder ( right  )
{} 
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LoKi::Vertices::VertexChi2Distance& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::Function<const LHCb::VertexBase*>( right )  
  , LoKi::Vertices::VertexHolder ( right  )
{} 
// ============================================================================
LoKi::Vertices::VertexChi2Distance::~VertexChi2Distance(){}
// ============================================================================
LoKi::Vertices::VertexChi2Distance*
LoKi::Vertices::VertexChi2Distance::clone() const 
{ return new LoKi::Vertices::VertexChi2Distance(*this) ; }
// ============================================================================
LoKi::Vertices::VertexChi2Distance::result_type 
LoKi::Vertices::VertexChi2Distance::operator() 
  ( LoKi::Vertices::VertexChi2Distance::argument v ) const 
{ return chi2 ( v ) ; }
// ============================================================================
LoKi::Vertices::VertexChi2Distance::result_type 
LoKi::Vertices::VertexChi2Distance::chi2  
( LoKi::Vertices::VertexChi2Distance::argument v ) const 
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
  if ( sc.isFailure() ) 
  { Error ( "Evaluation is invalid, return " + 
            Gaudi::Utils::toString ( _chi2 ) , sc ) ; }
  //
  return _chi2 ;
}
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexChi2Distance::fillStream
( std::ostream& s ) const 
{ return s << "VVDCHI2" ; }
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::VertexBase::Vector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::VertexBase::ConstVector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::Vertex::Vector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::Vertex::ConstVector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const SmartRefVector<LHCb::Vertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::RecVertex::Vector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::RecVertex::ConstVector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LHCb::RecVertices* vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ()
  , m_fun                            ( LoKi::Point3D()       )
{
  if ( 0 != vs ) { addObjects( vs->begin() , vs->end() ) ; }
} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::PhysTypes::VRange& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::Keeper<LHCb::VertexBase>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::UniqueKeeper<LHCb::VertexBase>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::Keeper<LHCb::Vertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::UniqueKeeper<LHCb::Vertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::Keeper<LHCb::RecVertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::MinVertexDistance 
( const LoKi::Vertices::MinVertexDistance& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::Function<const LHCb::VertexBase*> ( right        ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>    ( right        )
  , m_fun                               ( right.m_fun  ) 
{} 
// ============================================================================
LoKi::Vertices::MinVertexDistance::result_type 
LoKi::Vertices::MinVertexDistance::operator() 
  ( LoKi::Vertices::MinVertexChi2Distance::argument v ) const
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance;                     // RETURN 
  }
  
  result_type result =  LoKi::Constants::PositiveInfinity ;
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() , m_fun  , v , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;               // RETURN 
  }
  //
  return result ;                                           // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Vertices::MinVertexDistance::fillStream
( std::ostream& s ) const 
{ return s << "MINVVD" ; }
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::VertexBase::Vector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::VertexBase::ConstVector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::Vertex::Vector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::Vertex::ConstVector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const SmartRefVector<LHCb::Vertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} 
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::RecVertex::Vector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::RecVertex::ConstVector& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::RecVertices* vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ()
  , m_fun                            ( LoKi::Point3D()       )
{
  if ( 0 != vs ) { addObjects( vs->begin() , vs->end() ) ; }
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::PhysTypes::VRange& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs.begin() , vs.end() )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::Keeper<LHCb::VertexBase>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::UniqueKeeper<LHCb::VertexBase>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::Keeper<LHCb::Vertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::UniqueKeeper<LHCb::Vertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::Keeper<LHCb::RecVertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vs ) 
  : LoKi::Function<const LHCb::VertexBase*>()
  , LoKi::UniqueKeeper<LHCb::VertexBase> ( vs )
  , m_fun                            ( LoKi::Point3D()       )
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::Vertices::MinVertexChi2Distance& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::Function<const LHCb::VertexBase*> ( right        ) 
  , LoKi::UniqueKeeper<LHCb::VertexBase>    ( right        )
  , m_fun                               ( right.m_fun  ) 
{} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::result_type 
LoKi::Vertices::MinVertexChi2Distance::operator() 
  ( LoKi::Vertices::MinVertexChi2Distance::argument v ) const
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;                     // RETURN 
  }
  
  result_type result =  LoKi::Constants::PositiveInfinity ;
  if ( end() == LoKi::Helpers::_Min_vertex 
       ( begin() , end() , m_fun  , v , result ) ) 
  {
    Error ( "Invalid evaluation; return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;               // RETURN 
  }
  //
  return result ;                                           // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::MinVertexChi2Distance::fillStream
( std::ostream& s ) const 
{ return s << "MINVVDCHI2" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

