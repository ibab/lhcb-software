// $Id: MCKinematics.cpp,v 1.5 2007-11-28 14:14:00 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Report.h"
#include "LoKi/Constants.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCKinematics.h"
// ============================================================================
// MCEvent
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
/** @file
 *
 *  Implementation file for the funtions from file  LoKi/MCKinematics.h
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
 *  @date 2001-01-23 
 */
// ============================================================================
/*  trivial function to evaluate the mass MCParticle
 *  @param  p particle 
 *  @return invariant mass 
 *  @see LHCb::MCParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::MCKinematics::mass 
( const LHCb::MCParticle* p ) 
{
  if ( 0 == p ) 
  {
    LoKi::Report::Error
      ("mass(LHCb::MCParticle*),invaild argument,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  return LoKi::Kinematics::mass
    ( p->momentum() ) ;
} 
// ============================================================================
/*  trivial function to evaluate the mass MCParticles
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @return invariant mass 
 *  @see LHCb::MCParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::MCKinematics::mass 
( const LHCb::MCParticle* p1 , 
  const LHCb::MCParticle* p2 ) 
{
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(2*LHCb::MCParticle*),invaild argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(2*LHCb::MCParticle*),invaild argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  return LoKi::Kinematics::mass 
    ( p1 -> momentum() , 
      p2 -> momentum() ) ;
} 
// ============================================================================
/*  trivial function to evaluate the mass MCParticles
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @param  p3 the third particle 
 *  @return invariant mass 
 *  @see LHCb::MCParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::MCKinematics::mass 
( const LHCb::MCParticle* p1 , 
  const LHCb::MCParticle* p2 ,
  const LHCb::MCParticle* p3 ) 
{
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(3*LHCb::MCParticle*),invaild argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(3*LHCb::MCParticle*),invaild argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p3 ) 
  {
    LoKi::Report::Error
      ("mass(3*LHCb::MCParticle*),invaild argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  return LoKi::Kinematics::mass 
    ( p1 -> momentum() , 
      p2 -> momentum() ,
      p3 -> momentum() ) ;
}
// ============================================================================
/*  trivial function to evaluate the mass MCParticle
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @param  p3 the third particle 
 *  @param  p4 the fourth particle 
 *  @return invariant mass 
 *  @see LHCb::MCParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::MCKinematics::mass 
( const LHCb::MCParticle* p1 , 
  const LHCb::MCParticle* p2 ,
  const LHCb::MCParticle* p3 ,
  const LHCb::MCParticle* p4 ) 
{
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(3*LHCb::MCParticle*),invaild argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(3*LHCb::MCParticle*),invaild argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p3 ) 
  {
    LoKi::Report::Error
      ("mass(3*LHCb::MCParticle*),invaild argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p4 ) 
  {
    LoKi::Report::Error
      ("mass(4*LHCb::MCParticle*),invaild argument 4,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  return LoKi::Kinematics::mass 
    ( p1 -> momentum() , 
      p2 -> momentum() ,
      p3 -> momentum() ,
      p4 -> momentum() ) ;
}
// ============================================================================
// The END 
// ============================================================================

