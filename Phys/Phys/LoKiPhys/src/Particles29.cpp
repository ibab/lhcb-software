// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles29.h"
#include "LoKi/Constants.h"
#include "LoKi/Child.h"
// ============================================================================
/** @file 
 *  Implementation file for Jaap's functors from file LoKi/Particles29.h
 *  
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design. 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-04-30
 */
// ============================================================================
// Default Constructor
// ============================================================================
LoKi::Particles::DHCoplanarity::DHCoplanarity()
  : LoKi::AuxDesktopBase () 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
{ }
// ============================================================================
// MANDATORY : virtual destrcutor 
// ============================================================================
LoKi::Particles::DHCoplanarity::~DHCoplanarity(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::Particles::DHCoplanarity*
LoKi::Particles::DHCoplanarity::clone() const 
{ return new DHCoplanarity ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DHCoplanarity::result_type 
LoKi::Particles::DHCoplanarity::operator() 
  ( LoKi::Particles::DHCoplanarity::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  // get the best primary vertex
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Error ("Best primary Vertex points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // 
  // get the first daughter 
  const LHCb::Particle* first = LoKi::Child::child ( p , 1 ) ;
  if ( 0 == first ) 
  {
    Error ("The first daughter points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the vertex for the first daughter  
  const LHCb::VertexBase* vx = first->endVertex() ;
  if ( 0 == vx ) 
  {
    Error ("The vertex for the first daughter points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  /// get the second daughter:
  const LHCb::Particle* second = LoKi::Child::child ( p , 2 ) ;
  if ( 0 == second ) 
  {
    Error ("The second daughter points to NULL, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the lines
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line_ ; 
  
  // the line from PV to D0-vertex 
  const Line_ line1 ( pv -> position () , 
                      vx -> position () ) ;
  
  // the line along the second daughter particle 
  const Line_ line2 ( second -> referencePoint()   , 
                      second -> momentum().Vect()  ) ;
  
  // finally evaluate the distance between the lines:
  return Gaudi::Math::distance ( line1 , line2 ) ;
}
// ============================================================================
// OPTIONAL: nice printot
// ============================================================================
std::ostream& 
LoKi::Particles::DHCoplanarity::fillStream ( std::ostream& s ) const 
{ return s << "BPVDVDOCA" ; }

// ============================================================================
// The END 
// ============================================================================
