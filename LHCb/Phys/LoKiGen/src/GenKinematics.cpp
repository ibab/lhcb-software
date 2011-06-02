// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
// ============================================================================
// LoKiGen 
// ============================================================================
#include "LoKi/GenKinematics.h"
// ============================================================================
/** @file
 *
 * Implementation file for class : GenKinematics
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
/*  trivial function to evaluate the mass HepMC::GenParticle
 *  @param  p particle 
 *  @return invariant mass 
 *  @see HepMC::GenParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::GenKinematics::mass 
( const HepMC::GenParticle* p ) 
{
  if ( 0 == p ) 
  {
    LoKi::Report::Error
      ("mass(HepMC::GenParticle*),invaild argument,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  return LoKi::Kinematics::mass 
    ( LoKi::LorentzVector ( p->momentum() ) ) ;
}
// ============================================================================
/*  trivial function to evaluate the mass HepMC::GenParticle
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @return invariant mass 
 *  @see HepMC::GenParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::GenKinematics::mass 
( const HepMC::GenParticle* p1 , 
  const HepMC::GenParticle* p2 ) 
{
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(2*HepMC::GenParticle*),invaild argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(2*HepMC::GenParticle*),invaild argument 2,return'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return LoKi::Kinematics::mass
    ( LoKi::LorentzVector ( p1 -> momentum() ) , 
      LoKi::LorentzVector ( p2 -> momentum() ) ) ;  
}
// ============================================================================
/*  trivial function to evaluate the mass HepMC::GenParticle
 *  @param  p1 the first particle 
 *  @param  p2 the third particle 
 *  @param  p3 the second particle 
 *  @return invariant mass 
 *  @see HepMC::GenParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::GenKinematics::mass 
( const HepMC::GenParticle* p1 , 
  const HepMC::GenParticle* p2 ,
  const HepMC::GenParticle* p3 ) 
{
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(3*HepMC::GenParticle*),invaild argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(3*HepMC::GenParticle*),invaild argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p3 ) 
  {
    LoKi::Report::Error
      ("mass(3*HepMC::GenParticle*),invaild argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return LoKi::Kinematics::mass 
    ( LoKi::LorentzVector ( p1 -> momentum() ) , 
      LoKi::LorentzVector ( p2 -> momentum() ) , 
      LoKi::LorentzVector ( p3 -> momentum() ) ) ;  
}
// ============================================================================
/*  trivial function to evaluate the mass HepMC::GenParticle
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @param  p3 the third  particle 
 *  @param  p3 the fourth  particle 
 *  @return invariant mass 
 *  @see HepMC::GenParticle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::GenKinematics::mass 
( const HepMC::GenParticle* p1 , 
  const HepMC::GenParticle* p2 ,
  const HepMC::GenParticle* p3 ,
  const HepMC::GenParticle* p4 ) 
{
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(4*HepMC::GenParticle*),invaild argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(4*HepMC::GenParticle*),invaild argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p3 ) 
  {
    LoKi::Report::Error
      ("mass(4*HepMC::GenParticle*),invaild argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == p4 ) 
  {
    LoKi::Report::Error
      ("mass(4*HepMC::GenParticle*),invaild argument 4,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return LoKi::Kinematics::mass
    ( LoKi::LorentzVector ( p1 -> momentum() ) , 
      LoKi::LorentzVector ( p2 -> momentum() ) , 
      LoKi::LorentzVector ( p3 -> momentum() ) ,
      LoKi::LorentzVector ( p4 -> momentum() ) ) ;  
}
// ============================================================================
// The END 
// ============================================================================

