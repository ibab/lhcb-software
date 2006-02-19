// $Id: Particles1.cpp,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles1.h"
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
/// constructor 
// ============================================================================
LoKi::Particles::VertexDistance::VertexDistance
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDistance::VertexDistance
( const LoKi::Point3D& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexDistance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDistance::VertexDistance
( const LoKi::Particles::VertexDistance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>( vertex ) 
  , m_fun ( vertex.m_fun )
{}
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::VertexDistance::~VertexDistance(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexDistance*
LoKi::Particles::VertexDistance::clone() const
{ return new LoKi::Particles::VertexDistance(*this) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexDistance::result_type 
LoKi::Particles::VertexDistance::operator() 
  ( LoKi::Particles::VertexDistance::argument p ) const
{ return distance ( p ) ; }
// ============================================================================
/// the actual computation 
// ============================================================================
LoKi::Particles::VertexDistance::result_type 
LoKi::Particles::VertexDistance::distance 
( LoKi::Particles::VertexDistance::argument p ) const
{ 
  if ( 0 ==p ) 
  {
    Error ( " Invalid Particle, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance;                   // RETURN 
  }
  // call for actual evalautor 
  return m_fun.distance( p->endVertex() ) ;
} ;
// ============================================================================


// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::VertexSignedDistance
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::VertexSignedDistance
( const LoKi::Point3D& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::VertexSignedDistance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::VertexSignedDistance
( const LoKi::Vertices::VertexDistance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::VertexSignedDistance
( const LoKi::Vertices::VertexSignedDistance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::VertexSignedDistance
( const LoKi::Particles::VertexSignedDistance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>( vertex ) 
  , m_fun ( vertex.m_fun )
{}
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::VertexSignedDistance::~VertexSignedDistance(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexSignedDistance*
LoKi::Particles::VertexSignedDistance::clone() const
{ return new LoKi::Particles::VertexSignedDistance(*this) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexSignedDistance::result_type 
LoKi::Particles::VertexSignedDistance::operator() 
  ( LoKi::Particles::VertexSignedDistance::argument p ) const
{ return distance ( p ) ; }
// ============================================================================
/// the actual computation 
// ============================================================================
LoKi::Particles::VertexSignedDistance::result_type 
LoKi::Particles::VertexSignedDistance::distance 
( LoKi::Particles::VertexSignedDistance::argument p ) const
{ 
  if ( 0 ==p )
  {
    Error ( " Invalid Particle, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance;                   // RETURN 
  }
  // call for actual evalautor 
  return m_fun.distance( p->endVertex() ) ;
} ;
// ============================================================================


// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDotDistance::VertexDotDistance
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , LoKi::Vertices::VertexHolder ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDotDistance::VertexDotDistance
( const LoKi::Point3D& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , LoKi::Vertices::VertexHolder ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDotDistance::VertexDotDistance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , LoKi::Vertices::VertexHolder ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexDotDistance::VertexDotDistance
( const LoKi::Particles::VertexDotDistance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>( vertex ) 
  , LoKi::Vertices::VertexHolder ( vertex )
{}
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::VertexDotDistance::~VertexDotDistance(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexDotDistance*
LoKi::Particles::VertexDotDistance::clone() const
{ return new LoKi::Particles::VertexDotDistance(*this) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexDotDistance::result_type 
LoKi::Particles::VertexDotDistance::operator() 
  ( LoKi::Particles::VertexDotDistance::argument p ) const
{ return distance ( p ) ; }
// ============================================================================
/// the actual computation 
// ============================================================================
LoKi::Particles::VertexDotDistance::result_type 
LoKi::Particles::VertexDotDistance::distance 
( LoKi::Particles::VertexDotDistance::argument p ) const
{
  if ( 0 == p      ) 
  { 
    Error("Invalid particle! return 'InvalidDistance' ");
    return  LoKi::Constants::InvalidDistance ;             // RETURN 
  }
  if ( !valid()   )
  { 
    Error("Invalid vertex! return 'InvalidDistance' ");
    return  LoKi::Constants::InvalidDistance ;             // RETURN 
  }
  const LHCb::Vertex* vx = p->endVertex() ;
  if( 0 == vx     ) 
  { 
    Error("Invalid 'endVertex'! return 'InvalidDistance' ");
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }
  // momentum direction
  const LoKi::Vector3D direction 
    ( p->momentum().Px() , 
      p->momentum().Py() , 
      p->momentum().Pz() ) ;
  // vertex distance 
  const LoKi::Vector3D dist = vx->position() - position() ;
  //
  const double dirMag = direction.R()  ;
  if ( 0 >= dirMag ) 
  { 
    Warning("Invalid momentum direction") ;
    return dist.R() ;                                       // RETURN 
  }
  //
  return dist.Dot( direction ) / dirMag ;                   // RETURN 
} ;
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexChi2Distance::VertexChi2Distance
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexChi2Distance::VertexChi2Distance
( const LoKi::Point3D& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexChi2Distance::VertexChi2Distance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexChi2Distance::VertexChi2Distance
( const LoKi::Vertices::VertexChi2Distance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_fun ( vertex )
{}
// ============================================================================
/// constructor 
// ============================================================================
LoKi::Particles::VertexChi2Distance::VertexChi2Distance
( const LoKi::Particles::VertexChi2Distance& vertex ) 
  : LoKi::Function<const LHCb::Particle*>( vertex ) 
  , m_fun ( vertex.m_fun )
{}
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::VertexChi2Distance::~VertexChi2Distance(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexChi2Distance*
LoKi::Particles::VertexChi2Distance::clone() const
{ return new LoKi::Particles::VertexChi2Distance(*this) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexChi2Distance::result_type 
LoKi::Particles::VertexChi2Distance::operator() 
  ( LoKi::Particles::VertexChi2Distance::argument p ) const
{ return chi2 ( p ) ; }
// ============================================================================
/// the actual computation 
// ============================================================================
LoKi::Particles::VertexChi2Distance::result_type 
LoKi::Particles::VertexChi2Distance::chi2
( LoKi::Particles::VertexChi2Distance::argument p ) const
{ 
  if ( 0 ==p ) 
  {
    Error ( " Invalid Particle, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance;                   // RETURN 
  }
  // call for actual evalautor 
  return m_fun.chi2( p->endVertex() ) ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

