// $Id: Particles3.cpp,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <algorithm>
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/SystemOfUnits.h"
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
#include "LoKi/Particles3.h"
#include "LoKi/Helpers.h"
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
/// constructor from the particle and the tool  
// ============================================================================
LoKi::Particles::ClosestApproach::ClosestApproach
( const LHCb::Particle*                  particle , 
  const LoKi::Vertices::ImpactParamTool& tool     )
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


// ============================================================================
/// constructor from the particle and the tool  
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


// ============================================================================
/// constructor from the particle(s) and the tool
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LHCb::Particle::Vector&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LHCb::Particle::ConstVector&     particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const SmartRefVector<LHCb::Particle>&  particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::PhysTypes::Range&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::Vector&          particles )
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::ConstVector&     particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const SmartRefVector<LHCb::Particle>&  particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Vertices::ImpactParamTool& tool      , 
  const LoKi::PhysTypes::Range&          particles ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle() ; } ;
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::MinClosestApproach::MinClosestApproach 
( const LoKi::Particles::MinClosestApproach& right ) 
  : LoKi::Function<const LHCb::Particle*>( right )
  , m_particles ( right.m_particles )
  , m_fun       ( right.m_fun       ) 
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
  if ( m_particles.end() == LoKi::Helpers::_Min_particle
       ( m_particles.begin() , m_particles.end() , m_fun , p , result ) ) 
  {
    Error ( " Invalid evaluation, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;    
  }
  
  return result ;
};
// ============================================================================

// ============================================================================
/// constructor from the particle(s) and the tool
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LHCb::Particle::Vector&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LHCb::Particle::ConstVector&     particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const SmartRefVector<LHCb::Particle>&  particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::PhysTypes::Range&          particles ,
  const LoKi::Vertices::ImpactParamTool& tool      ) 
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::Vector&          particles )
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LHCb::Particle::ConstVector&     particles )
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const SmartRefVector<LHCb::Particle>&  particles )
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
  , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle(); } 
// ============================================================================
/// constructor from the particle(s) and the tool  
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Vertices::ImpactParamTool& tool      ,
  const LoKi::PhysTypes::Range&          particles )
  : LoKi::Function<const LHCb::Particle*>()
  , m_particles ( particles.begin() , particles.end() )
    , m_fun       ( LoKi::Helpers::_First ( particles ) , tool ) 
{ removeParticle(); } 
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::MinClosestApproachChi2::MinClosestApproachChi2 
( const LoKi::Particles::MinClosestApproachChi2& right ) 
  : LoKi::Function<const LHCb::Particle*>( right )
  , m_particles ( right.m_particles )
  , m_fun       ( right.m_fun       ) 
{ removeParticle(); } 
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
  if ( m_particles.end() == LoKi::Helpers::_Min_particle
       ( m_particles.begin() , m_particles.end() , m_fun , p , result ) ) 
  {
    Error ( " Invalid evaluation, return 'InvalidDistance' " ) ;
    return  LoKi::Constants::InvalidDistance ;    
  }
  
  return result ;
};
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
