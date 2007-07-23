// $Id: Particles3.cpp,v 1.8 2007-07-23 17:35:54 ibelyaev Exp $
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
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
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
  : LoKi::Function<const LHCb::Particle*>() 
  , LoKi::Vertices::ImpactParamTool ( tool ) 
  , m_particle ( particle ) 
{}
// ============================================================================
//  constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproach::ClosestApproach
(  const LoKi::Vertices::ImpactParamTool& tool     ,
   const LHCb::Particle*                  particle )
  : LoKi::Function<const LHCb::Particle*>() 
  , LoKi::Vertices::ImpactParamTool ( tool ) 
  , m_particle ( particle ) 
{}
// ============================================================================
LoKi::Particles::ClosestApproach::result_type 
LoKi::Particles::ClosestApproach::distance 
( LoKi::Particles::ClosestApproach::argument p ) const 
{
  if ( 0 == p             ) 
  { 
    Error ( " Argument  is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  
  if ( 0 == tool     ()   ) 
  { 
    Error ( " Tool      is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  
  if ( 0 == particle ()   ) 
  { 
    Error ( " Particle  is invalid, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  
  double dist     = 0 ;
  double error    = 0 ;
  
  StatusCode sc = 
    tool()->calcCloseAppr( *p , *(particle()) , dist , error ) ;
  
  if ( sc.isFailure ()  ) 
  { 
    Error ( " Particle  is invalid, return 'InvalidDistance' ", sc ) ;
    return  LoKi::Constants::InvalidDistance ; 
  }
  
  return dist ;
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::ClosestApproach::fillStream
( std::ostream& stream ) const
{ return stream << "CLAPP" ; }
// ============================================================================

// ============================================================================
//  constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproachChi2::ClosestApproachChi2
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpactParamTool& tool     )
  : LoKi::Function<const LHCb::Particle*>() 
  , LoKi::Vertices::ImpactParamTool ( tool ) 
  , m_particle ( particle ) 
{}
// ============================================================================
LoKi::Particles::ClosestApproachChi2::result_type 
LoKi::Particles::ClosestApproachChi2::chi2
( LoKi::Particles::ClosestApproachChi2::argument p ) const 
{
  if ( 0 == p             ) 
  { 
    Error ( " Argument  is invalid, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  
  if ( 0 == tool     ()   ) 
  { 
    Error ( " Tool      is invalid, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  
  if ( 0 == particle ()   ) 
  { 
    Error ( " Particle  is invalid, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  
  double dist     = 0 ;
  double error    = 0 ;
  
  StatusCode sc = 
    tool()->calcCloseAppr( *p , *(particle()) , dist , error ) ;
  
  if ( sc.isFailure ()  ) 
  { 
    Error ( " Particle  is invalid, return 'InvalidChi2' ", sc ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }

  if ( 0 == error ) 
  { 
    Error ( " Error     is invalid, return 'InvalidChi2'     ", sc ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  
  return dist * dist / error / error ;
  
} ;
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
( const LHCb::Particle::Vector&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LHCb::Particle::ConstVector&     particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const SmartRefVector<LHCb::Particle>&  particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::PhysTypes::Range&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Keeper<LHCb::Particle>&    particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>  ( particles ) 
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::UniqueKeeper<LHCb::Particle>& particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>  ( particles ) 
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::Vector&          particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::ConstVector&     particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const SmartRefVector<LHCb::Particle>&  particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      , 
  const LoKi::PhysTypes::Range&          particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      , 
  const LoKi::Keeper<LHCb::Particle>& particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      , 
  const LoKi::UniqueKeeper<LHCb::Particle>& particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle>( particles )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{} ;
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Particles::MinClosestApproach& right ) 
  : LoKi::AuxFunBase                     ( right ) 
  , LoKi::Function<const LHCb::Particle*>( right )
  , LoKi::UniqueKeeper<LHCb::Particle>   ( right )
  , m_fun                                ( right.m_fun ) 
{} ;
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
  
  result_type result = LoKi::Constants::InvalidDistance ;
  if ( end() == LoKi::Helpers::_Min_particle
       ( begin() , end() , m_fun , p , result ) ) 
  {
    Error ( " Invalid evaluation, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;    
  }
  
  return result ;
};
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
( const LHCb::Particle::Vector&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LHCb::Particle::ConstVector&     particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const SmartRefVector<LHCb::Particle>&  particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::PhysTypes::Range&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Keeper<LHCb::Particle>&    particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::UniqueKeeper<LHCb::Particle>& particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::Vector&          particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::ConstVector&     particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const SmartRefVector<LHCb::Particle>&  particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LoKi::PhysTypes::Range&          particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LoKi::Keeper<LHCb::Particle>&    particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
//  constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool&    tool      ,
  const LoKi::UniqueKeeper<LHCb::Particle>& particles )
  : LoKi::Function<const LHCb::Particle*>()
  , LoKi::UniqueKeeper<LHCb::Particle> ( particles )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{};
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Particles::MinClosestApproachChi2& right ) 
  : LoKi::AuxFunBase                     ( right ) 
  , LoKi::Function<const LHCb::Particle*>( right )
  , LoKi::UniqueKeeper<LHCb::Particle>   ( right )
  , m_fun                                ( right.m_fun ) 
{} 
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::result_type 
LoKi::Particles::MinClosestApproachChi2::chi2
( LoKi::Particles::MinClosestApproachChi2::argument p ) const 
{ 
  if ( 0 == p             ) 
  { 
    Error ( " Argument  is invalid, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2 ; 
  }
  
  result_type result = LoKi::Constants::InvalidDistance ;
  if ( end() == LoKi::Helpers::_Min_particle
       ( begin() , end() , m_fun , p , result ) ) 
  {
    Error ( " Invalid evaluation, return 'InvalidChi2' " ) ;
    return  LoKi::Constants::InvalidChi2;    
  }
  
  return result ;
};
// ============================================================================
std::ostream& 
LoKi::Particles::MinClosestApproachChi2::fillStream
( std::ostream& stream )  const 
{ return stream << "MINCLAPPCHI2" ; } 
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
