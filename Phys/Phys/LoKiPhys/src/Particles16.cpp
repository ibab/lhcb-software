// $Id: Particles16.cpp,v 1.7 2009-03-10 22:49:57 spradlin Exp $
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Math.h"
#include "LoKi/Particles16.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Particles::CosineDirectionAngle
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
 *  @date 2006-03-20 
 */
// ============================================================================
// constructor from the vertex  
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex ) 
{}
// ============================================================================
// constructor from the point 
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LoKi::Point3D& point ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( point ) 
{}
// ============================================================================
// constructor from the holder 
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LoKi::Vertices::VertexHolder& holder ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( holder ) 
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LoKi::Particles::CosineDirectionAngle& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Vertices::VertexHolder          ( right ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Particles::CosineDirectionAngle::~CosineDirectionAngle(){} ;
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::Particles::CosineDirectionAngle*
LoKi::Particles::CosineDirectionAngle::clone() const 
{ return new LoKi::Particles::CosineDirectionAngle(*this) ; }
// ============================================================================
LoKi::Particles::CosineDirectionAngle::result_type 
LoKi::Particles::CosineDirectionAngle::dira
( LoKi::Particles::CosineDirectionAngle::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000");
    return -1000 ;
  }
  const LHCb::VertexBase* v = p->endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return -2000");
    return -2000 ;
  }
  if ( !valid() ) 
  {
    Error ( "VertexHolder base is invalid, return -3000");
    return -3000 ;
  }
  const LoKi::ThreeVector m = p->momentum().Vect() ;
  const LoKi::ThreeVector d = v->position()-position() ;
  const double dot  = m.Dot(d) ;
  // orthogonal vectors ? 
  if ( 0 == dot ) { return 0  ;}
  const double norm = m.Mag2()*d.Mag2() ;
  if ( 0 >= norm ) 
  {
    Warning ( "Vectors of illegal length, return -4000") ;
    return -4000 ;
  }
  return  dot / ::sqrt( norm ) ;
} 
// ============================================================================
// OPTIONAL: printout 
// ============================================================================
std::ostream& 
LoKi::Particles::CosineDirectionAngle::fillStream ( std::ostream& s ) const
{ return s << "DIRA" ; }
// ============================================================================



// ============================================================================
// constructor from the vertex  
// ============================================================================
LoKi::Particles::TrgPointingScore::TrgPointingScore
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex ) 
{}
// ============================================================================
// constructor from the point 
// ============================================================================
LoKi::Particles::TrgPointingScore::TrgPointingScore
( const LoKi::Point3D& point ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( point ) 
{}
// ============================================================================
// constructor from the holder 
// ============================================================================
LoKi::Particles::TrgPointingScore::TrgPointingScore
( const LoKi::Vertices::VertexHolder& holder ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( holder ) 
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::TrgPointingScore::TrgPointingScore
( const LoKi::Particles::TrgPointingScore& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Vertices::VertexHolder          ( right ) 
{}
// ============================================================================
LoKi::Particles::TrgPointingScore::result_type 
LoKi::Particles::TrgPointingScore::pointing
( LoKi::Particles::TrgPointingScore::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000");
    return -1000 ;
  }
  const LHCb::Vertex* v = p->endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return -2000");
    return -2000 ;
  }
  if ( !valid() ) 
  {
    Error ( "VertexHolder base is invalid, return -3000");
    return -3000 ;
  }

  LHCb::Particle::ConstVector vdaugs = v->outgoingParticlesVector();
  if( vdaugs.empty() )
  {
    Warning ("Particle endVertex has no outgoing particles, return 1.0") ;
    return 1.0 ;
  }

  const LoKi::ThreeVector mom = p->momentum().Vect() ;
  const LoKi::ThreeVector disp = v->position() - position() ;
  const LoKi::ThreeVector cross = mom.Cross(disp) ;
  const double dispmag = disp.Mag2() ;
  if ( 0 >= dispmag ) 
  {
    Warning ( "Vector of illegal length, return -4000") ;
    return -4000 ;
  }

  const double pstheta = ::sqrt(cross.Mag2() / dispmag) ;

  double ptsum = 0.0;
  LHCb::Particle::ConstVector::const_iterator daug ;
  for( daug = vdaugs.begin(); daug != vdaugs.end(); ++daug )
  {
    if ( 0 == (*daug) )
    {
      // Perhaps this condition should raise an exception.  It should never
      //   occur, and if it does occur then it indicates a bug.
      Error ("Daughter LHCb::Particle* points to NULL, return -1000");
      return -1000 ;
    }

    const LoKi::ThreeVector daugmom = (*daug)->momentum().Vect() ;
    const LoKi::ThreeVector daugcross = daugmom.Cross(disp) ;
    ptsum += ::sqrt(daugcross.Mag2() / dispmag) ;
  }

  const double denom = pstheta + ptsum ;
  if ( 0 >= denom )
  {
    Warning ( "Invalid divisor, return 1") ;
    return 1 ;
  }

  return pstheta / denom ;
} 
// ============================================================================
// ============================================================================
// The END 
// ============================================================================
