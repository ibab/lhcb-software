// $Id: WrongMass.h,v 1.4 2006-11-25 19:12:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verision $Revision: 1.4 $
// ============================================================================
#ifndef LOKI_WRONGMASS_H 
#define LOKI_WRONGMASS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
// ============================================================================
namespace LHCb { class ParticleID ; } ;
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

namespace LoKi
{
  /** @namespace LoKi::Kinematics LoKi/WrongMass.h
   *  function for dealing with "wrong  mass hypothesis"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-08
   */
  namespace Kinematics 
  {

    /** @fn wrongMass 
     * 
     *  Use a wrong mass-hypothesis for the given 4-momemtum
     *  @param v vector of 4-momentum 
     *  @param m mass to be used 
     *  @return the 4-momneum with "wrong" mass hypothesis 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-08
     */
    inline LoKi::LorentzVector 
    wrongMass
    ( const LoKi::LorentzVector& v , 
      const double               m ) 
    { 
      LoKi::LorentzVector v0 = LoKi::LorentzVector() ;
      v0.SetXYZT ( v.Px() , v.Py() , v.Pz() , ::sqrt ( v.P2() + m * m ) ) ;
      return v0 ; 
    } ;
    
    /** @fn wrongMass 
     * 
     *  Use a wrong mass-hypothesis for the given 4-momentum 
     *  @param v    vector of 4-momentum 
     *  @param name particle name  
     *  @return the 4-momentum with "wrong" mass hypothesis 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-08
     */
    LoKi::LorentzVector 
    wrongMass
    ( const LoKi::LorentzVector& v    , 
      const std::string&         name ) ;
    
    /** @fn wrongMass 
     * 
     *  Use a wrong mass-hypothesis for the given 4-momentum 
     *  @param v    vector of 4-momentum 
     *  @param name particle name  
     *  @return the 4-momentum with "wrong" mass hypothesis 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-08
     */
    LoKi::LorentzVector 
    wrongMass
    ( const LoKi::LorentzVector& v    , 
      const LHCb::ParticleID&    pid  ) ;

    /** @fn wrongMass 
     * 
     *  Use a wrong mass-hypothesis for the given 4-momenta 
     *  to evaluate the effective invarinat mass 
     *
     *  @param v1 the first 4-momentum 
     *  @param m1 the mass to be used 
     *  @param v2 the second 4-momentum 
     *  @param m2 the mass to be used 
     *  @return the invariant mass evaluated using "wrong" masses 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-08
     */
    inline double 
    wrongMass
    ( const LoKi::LorentzVector& v1 ,
      const double               m1 ,
      const LoKi::LorentzVector& v2 , 
      const double               m2 ) 
    { return mass ( wrongMass ( v1 , m1 ) , 
                    wrongMass ( v2 , m2 ) ) ; }

    /** @fn wrongMass 
     * 
     *  Use a wrong mass-hypothesis for the given 4-momenta 
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
    double 
    wrongMass
    ( const LoKi::LorentzVector& v1    ,
      const std::string&         name1 ,
      const LoKi::LorentzVector& v2    , 
      const std::string&         name2 ) ;

    /** @fn wrongMass 
     * 
     *  Use a wrong mass-hypothesis for the given 4-momenta 
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
    double 
    wrongMass
    ( const LoKi::LorentzVector& v1    ,
      const LHCb::ParticleID&    pid1  ,
      const LoKi::LorentzVector& v2    , 
      const LHCb::ParticleID&    pid2  ) ;
    
  } // end of namespace LoKi::Kinematics 
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_WRONGMASS_H
// ============================================================================
