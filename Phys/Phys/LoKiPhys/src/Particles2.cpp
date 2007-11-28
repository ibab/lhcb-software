// $Id: Particles2.cpp,v 1.7 2007-11-28 14:39:30 ibelyaev Exp $
// ============================================================================
// Include files 
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
#include "LoKi/Particles2.h"
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
//  constructor 
// ============================================================================
LoKi::Particles::TimeDistance::TimeDistance
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDistance::TimeDistance
( const LoKi::Point3D& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDistance::TimeDistance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDistance::TimeDistance
( const LoKi::Particles::VertexDistance& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDistance::TimeDistance
( const LoKi::Vertices::VertexDistance& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDistance::TimeDistance
( const LoKi::Particles::TimeDistance& vertex ) 
  : LoKi::AuxFunBase                     ( vertex ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function( vertex ) 
  , m_fun ( vertex.m_fun )
{}
// ============================================================================
//  MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TimeDistance::~TimeDistance(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TimeDistance*
LoKi::Particles::TimeDistance::clone() const
{ return new LoKi::Particles::TimeDistance(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TimeDistance::result_type 
LoKi::Particles::TimeDistance::operator() 
  ( LoKi::Particles::TimeDistance::argument p ) const
{ return time ( p ) ; }
// ============================================================================
//  the actual computation 
// ============================================================================
LoKi::Particles::TimeDistance::result_type 
LoKi::Particles::TimeDistance::time 
( LoKi::Particles::TimeDistance::argument p ) const
{ 
  if( 0 == p      ) 
  { 
    Error ( " Argument is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  // 1) evaluate the distance 
  const double distance = m_fun.distance( p ) ;
  
  // 2) recalculate distance to "time"   (c*tau) 
  const double mass     = p->momentum().M() ;
  const double mom      = p->momentum().P() ;
  if ( 0 >= mass ) 
  { 
    Error ( " Mass is invalid, return 'InvalidDistance' " ) ;  
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }  
  if ( 0 >= mom  ) 
  { 
    Error ( " |Momentum| is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }  
  const double betagamma   = mom / mass ;
  //
  return distance / betagamma ;
} ;
// ============================================================================
std::ostream&
LoKi::Particles::TimeDistance::fillStream
( std::ostream& s ) const 
{ return s << "TDIST" ; }
// ============================================================================

// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::TimeSignedDistance
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::TimeSignedDistance
( const LoKi::Point3D& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::TimeSignedDistance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::TimeSignedDistance
( const LoKi::Particles::VertexSignedDistance& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::TimeSignedDistance
( const LoKi::Vertices::VertexDistance& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::TimeSignedDistance
( const LoKi::Particles::TimeSignedDistance& vertex ) 
  : LoKi::AuxFunBase                     ( vertex ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function( vertex ) 
  , m_fun ( vertex.m_fun )
{}
// ============================================================================
//  MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TimeSignedDistance::~TimeSignedDistance(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TimeSignedDistance*
LoKi::Particles::TimeSignedDistance::clone() const
{ return new LoKi::Particles::TimeSignedDistance(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TimeSignedDistance::result_type 
LoKi::Particles::TimeSignedDistance::operator() 
  ( LoKi::Particles::TimeDistance::argument p ) const
{ return time ( p ) ; }
// ============================================================================
//  the actual computation 
// ============================================================================
LoKi::Particles::TimeSignedDistance::result_type 
LoKi::Particles::TimeSignedDistance::time 
( LoKi::Particles::TimeSignedDistance::argument p ) const
{ 
  if( 0 == p      ) 
  { 
    Error ( " Argument is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  // 1) evaluate the distance 
  const double distance = m_fun.distance( p ) ;
  
  // 2) recalculate distance to "time"   (c*tau) 
  const double mass     = p->momentum().M() ;
  const double mom      = p->momentum().P() ;
  if ( 0 >= mass ) 
  { 
    Error ( " Mass is invalid, return 'InvalidDistance' " ) ;  
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }  
  if ( 0 >= mom  ) 
  { 
    Error ( " |Momentum| is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }  
  const double betagamma   = mom / mass ;
  //
  return distance / betagamma ;
} ;
// ============================================================================
std::ostream&
LoKi::Particles::TimeSignedDistance::fillStream
( std::ostream& s ) const 
{ return s << "TDSIGN" ; }
// ============================================================================


// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDotDistance::TimeDotDistance
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDotDistance::TimeDotDistance
( const LoKi::Point3D& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDotDistance::TimeDotDistance
( const LoKi::Vertices::VertexHolder& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDotDistance::TimeDotDistance
( const LoKi::Particles::VertexDotDistance& vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( vertex )
{}
// ============================================================================
//  constructor 
// ============================================================================
LoKi::Particles::TimeDotDistance::TimeDotDistance
( const LoKi::Particles::TimeDotDistance& vertex ) 
  : LoKi::AuxFunBase                     ( vertex ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function( vertex ) 
  , m_fun ( vertex.m_fun )
{}
// ============================================================================
//  MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TimeDotDistance::~TimeDotDistance(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TimeDotDistance*
LoKi::Particles::TimeDotDistance::clone() const
{ return new LoKi::Particles::TimeDotDistance(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TimeDotDistance::result_type 
LoKi::Particles::TimeDotDistance::operator() 
  ( LoKi::Particles::TimeDotDistance::argument p ) const
{ return time ( p ) ; }
// ============================================================================
//  the actual computation 
// ============================================================================
LoKi::Particles::TimeDotDistance::result_type 
LoKi::Particles::TimeDotDistance::time 
( LoKi::Particles::TimeDotDistance::argument p ) const
{ 
  if( 0 == p      ) 
  { 
    Error ( " Argument is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  // 1) evaluate the distance 
  const double distance = m_fun.distance( p ) ;
  
  // 2) recalculate distance to "time"   (c*tau) 
  const double mass     = p->momentum().M() ;
  const double mom      = p->momentum().P() ;
  if ( 0 >= mass ) 
  { 
    Error ( " Mass is invalid, return 'InvalidDistance' " ) ;  
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }  
  if ( 0 >= mom  ) 
  { 
    Error ( " |Momentum| is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;              // RETURN 
  }  
  const double betagamma   = mom / mass ;
  //
  return distance / betagamma ;
} ;
// ============================================================================
std::ostream&
LoKi::Particles::TimeDotDistance::fillStream
( std::ostream& s ) const 
{ return s << "TDOT" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
