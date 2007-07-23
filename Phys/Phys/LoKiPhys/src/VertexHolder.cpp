// $Id: VertexHolder.cpp,v 1.5 2007-07-23 17:35:56 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/VertexBase.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
#include "LoKi/Exception.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/VertexHolder.h"
// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Vertices::VertexHolder 
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
// constructor from the 3Dpoint
// ============================================================================
LoKi::Vertices::VertexHolder::VertexHolder
( const LoKi::Point3D& point      ) 
  : AuxFunBase() 
  , m_type   ( _point ) 
  , m_vertex (  0     ) 
  , m_point  (  point ) 
{}
// ============================================================================
/// constructor from Vertex 
// ============================================================================
LoKi::Vertices::VertexHolder::VertexHolder
( const LHCb::VertexBase*  vertex ) 
  : AuxFunBase() 
  , m_type   ( _vertex ) 
  , m_vertex (  vertex ) 
  , m_point  () 
{}
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::Vertices::VertexHolder::VertexHolder
( const LoKi::Vertices::VertexHolder& right ) 
  : AuxFunBase ( right          ) 
  , m_type     ( right.m_type   ) 
  , m_vertex   ( right.m_vertex ) 
  , m_point    ( right.m_point  ) 
{}
// ============================================================================
LoKi::Vertices::VertexHolder::~VertexHolder(){}
// ============================================================================
const LoKi::Point3D& 
LoKi::Vertices::VertexHolder::position() const 
{
  static const LoKi::Point3D s_error = 
    LoKi::Point3D ( 0 , 0 , LoKi::Constants::InvalidDistance ) ;
  static const std::string   s_print = "Vertex Information is invalid, return " 
    + LoKi::Print::toString( s_error ) ;
  if ( !valid() ) { Error ( s_print ) ; return s_error ; }    // RETURN 
  switch ( type() ) 
  {
  case _point  : return point()                ; break ; // RETURN 
  case _vertex : return vertex()->position()   ; break ; // RETURN
  default      : Error ( s_print ) ; return s_error ; break ; // RETURN   
  }
  Error ( s_print ) ;
  return s_error ;                                            // RETURN 
} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
