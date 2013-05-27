// $Id$
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
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles3.h"
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
//  constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproach::ClosestApproach
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpactParamTool& tool     )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , LoKi::Vertices::ImpactParamTool ( tool ) 
  , m_particle ( particle ) 
{}
// ============================================================================
//  constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproach::ClosestApproach
(  const LoKi::Vertices::ImpactParamTool& tool     ,
   const LHCb::Particle*                  particle )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , LoKi::Vertices::ImpactParamTool ( tool ) 
  , m_particle ( particle ) 
{}
// ============================================================================
LoKi::Particles::ClosestApproach::result_type 
LoKi::Particles::ClosestApproach::distance_ 
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) const 
{
  //
  if ( 0 == tool     ()   ) 
  { 
    Error ( " Tool      is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  //
  double dist = LoKi::Constants::InvalidDistance ;
  //
  StatusCode sc = tool() -> distance ( p1 , p2 , dist ) ;
  //
  if ( sc.isFailure ()  ) 
  { 
    Error ( " Error from IDistanceCalculator, return 'InvalidDistance' ", sc ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  //
  return dist ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::ClosestApproach::fillStream
( std::ostream& stream ) const
{ return stream << "CLAPP" ; }
// ============================================================================
LoKi::Particles::ClosestApproach::result_type 
LoKi::Particles::ClosestApproach::chi2_
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) const 
{
  //
  if ( 0 == tool     ()   ) 
  { 
    Error ( " Tool      is invalid, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  //
  double dist     = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  StatusCode sc = tool() -> distance ( p1 , p2 , dist , chi2 ) ;
  //
  if ( sc.isFailure ()  ) 
  { 
    Error ( " IDistanceCalculator::distance Failed, return 'InvalidChi2' ", sc ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  //
  return chi2 ;
} 
// ============================================================================
//  constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproachChi2::ClosestApproachChi2
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpactParamTool& tool     )
  : LoKi::Particles::ClosestApproach ( particle , tool ) 
{}
// ============================================================================
//  constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproachChi2::ClosestApproachChi2
( const LoKi::Vertices::ImpactParamTool& tool     ,
  const LHCb::Particle*                  particle )
  : LoKi::Particles::ClosestApproach ( tool , particle ) 
{}
// ============================================================================
std::ostream& 
LoKi::Particles::ClosestApproachChi2::fillStream
( std::ostream& stream ) const 
{ return stream << "CLAPPCHI2" ; }
// ============================================================================



// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LHCb::Particle::ConstVector&     particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} 
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::PhysTypes::Range&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} 
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::ConstVector&     particles ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} 
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      , 
  const LoKi::PhysTypes::Range&          particles ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} 
// ============================================================================
LoKi::Particles::MinClosestApproach::result_type 
LoKi::Particles::MinClosestApproach::distance 
( LoKi::Particles::MinClosestApproach::argument p ) const 
{ 
  if ( 0 == p             ) 
  { 
    Error ( " Argument  is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  //
  result_type result = LoKi::Constants::InvalidDistance ;
  //
  if ( end() == LoKi::Helpers::_Min_particle 
       ( begin()  , end () ,
         LoKi::Helpers::PMFA2<LoKi::Particles::ClosestApproach>
         ( &m_fun , &LoKi::Particles::ClosestApproach::distance) , p , result ) )  
  {
    Error ( " Invalid evaluation, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;    
  }
  //
  return result ;
}
// ============================================================================
LoKi::Particles::MinClosestApproach::result_type 
LoKi::Particles::MinClosestApproach::chi2
( LoKi::Particles::MinClosestApproach::argument p ) const 
{ 
  if ( 0 == p             ) 
  { 
    Error ( " Argument  is invalid, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  //
  result_type result = LoKi::Constants::InvalidDistance ;
  //
  if ( end() == LoKi::Helpers::_Min_particle 
       ( begin()  , end () ,
         LoKi::Helpers::PMFA2<LoKi::Particles::ClosestApproach>
         ( &m_fun , &LoKi::Particles::ClosestApproach::chi2) , p , result ) )
  {
    Error ( " Invalid evaluation, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2;    
  }
  //
  return result ;
}
// ============================================================================
std::ostream& 
LoKi::Particles::MinClosestApproach::fillStream
( std::ostream& stream ) const 
{ return stream << "MINCLAPP" ; }
// ============================================================================

// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LHCb::Particle::ConstVector&     particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Particles::MinClosestApproach ( particles , tool ) 
{}
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::PhysTypes::Range&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Particles::MinClosestApproach ( particles , tool ) 
{}
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::ConstVector&     particles )
  : LoKi::Particles::MinClosestApproach ( particles , tool ) 
{}
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LoKi::PhysTypes::Range&          particles )
  : LoKi::Particles::MinClosestApproach ( particles , tool ) 
{}
// ============================================================================
std::ostream& 
LoKi::Particles::MinClosestApproachChi2::fillStream
( std::ostream& stream )  const 
{ return stream << "MINCLAPPCHI2" ; } 
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
