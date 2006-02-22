// $Id: Particles7.cpp,v 1.1 2006-02-22 20:53:48 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles7.h"
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
 *  @date 2006-02-21 
 */
// ============================================================================


// ============================================================================
LoKi::Particles::VFunAsPFun::VFunAsPFun
( const LoKi::Types::VFunc& vfun , 
  const double              bad  ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_vfun ( vfun ) 
  , m_bad  ( bad  ) 
{}
// ===========================================================================
LoKi::Particles::VFunAsPFun::VFunAsPFun
( const LoKi::Particles::VFunAsPFun& right ) 
  : LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_vfun ( right.m_vfun ) 
  , m_bad  ( right.m_bad  ) 
{}
// ===========================================================================
LoKi::Particles::VFunAsPFun::~VFunAsPFun(){}
// ===========================================================================
LoKi::Particles::VFunAsPFun*
LoKi::Particles::VFunAsPFun::clone() const
{ return new VFunAsPFun(*this) ; }
// ===========================================================================
LoKi::Particles::VFunAsPFun::result_type
LoKi::Particles::VFunAsPFun::operator() 
  ( const LoKi::Particles::VFunAsPFun::argument p ) const 
{
  if ( 0 != p ) { return m_vfun ( p->endVertex() ) ; }      // RETURN 
  Error ( " Invalid Particle, return " + LoKi::Print::print( m_bad ) ) ;
  return m_bad;                                              // RETURN 
};
// ===========================================================================
std::ostream& 
LoKi::Particles::VFunAsPFun::fillStream
( std::ostream& s ) const 
{ return s << "VFASPF[" << m_vfun << "," << m_bad << "]" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

