// $Id: WrongMass.cpp,v 1.5 2007-07-23 17:07:44 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/WrongMass.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
/** @file
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
 *  @date 2006-03-08
 */
// ============================================================================
/*  Use a wrong mass-hypothesis for the given 4-momentum 
 *  @param v    vector of 4-momentum 
 *  @param name particle name  
 *  @return the 4-momneum with "wrong" mass hypothesis 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-08
 */
// ============================================================================
LoKi::LorentzVector 
LoKi::Kinematics::wrongMass
( const LoKi::LorentzVector& v    , 
  const std::string&         name ) 
{ return wrongMass ( v , LoKi::Particles::massFromName ( name ) ) ; } 
// ============================================================================
/*  Use a wrong mass-hypothesis for the given 4-momentum 
 *  @param v    vector of 4-momentum 
 *  @param name particle name  
 *  @return the 4-momentum with "wrong" mass hypothesis 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-08
 */
// ============================================================================
LoKi::LorentzVector 
LoKi::Kinematics::wrongMass
( const LoKi::LorentzVector& v    , 
  const LHCb::ParticleID&    pid  ) 
{
  return wrongMass ( v , LoKi::Particles::massFromPID( pid ) ) ;
} 
// ============================================================================
/*  Use a wrong mass-hypothesis for the given 4-momenta 
 *  to evaluate the effective invarinat mass 
 *
 *  @param v1    the first 4-momentum 
 *  @param name1 the name of the first particle
 *  @param v2    the second 4-momentum 
 *  @param name2 the name of the second particle 
 *  @return the invariant mass evaluated using "wrong" masses 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-08
 */
// ============================================================================
double 
LoKi::Kinematics::wrongMass
( const LoKi::LorentzVector& v1    ,
  const std::string&         name1 ,
  const LoKi::LorentzVector& v2    , 
  const std::string&         name2 ) 
{
  return wrongMass 
    ( v1 ,  LoKi::Particles::massFromName ( name1 ) , 
      v2 ,  LoKi::Particles::massFromName ( name2 ) ) ;
} 
// ============================================================================
/*  Use a wrong mass-hypothesis for the given 4-momenta 
 *  to evaluate the effective invarinat mass 
 *
 *  @param v1    the first 4-momentum 
 *  @param name1 the name of the first particle
 *  @param v2    the second 4-momentum 
 *  @param name2 the name of the second particle 
 *  @return the invariant mass evaluated using "wrong" masses 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-08
 */
// ============================================================================
double 
LoKi::Kinematics::wrongMass
( const LoKi::LorentzVector& v1    ,
  const LHCb::ParticleID&    pid1  ,
  const LoKi::LorentzVector& v2    , 
  const LHCb::ParticleID&    pid2  ) 
{
  return wrongMass 
    ( v1 ,  LoKi::Particles::massFromPID ( pid1 ) , 
      v2 ,  LoKi::Particles::massFromPID ( pid2 ) ) ;
} 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
