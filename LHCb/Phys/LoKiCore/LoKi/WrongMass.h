// $Id: WrongMass.h,v 1.7 2007-07-26 13:27:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verision $Revision: 1.7 $
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
  namespace Kinematics 
  {
    // ========================================================================
    /** Use a wrong mass-hypothesis for the given 4-momemtum
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
    } 
    // ========================================================================    
    /** Use a wrong mass-hypothesis for the given 4-momentum 
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
    // ========================================================================    
    /** Use a wrong mass-hypothesis for the given 4-momentum 
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
    // ========================================================================
    /** Use a wrong mass-hypothesis for the given 4-momenta 
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
    // ========================================================================
    /** Use a wrong mass-hypothesis for the given 4-momenta 
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
    // ========================================================================
    /** Use a wrong mass-hypothesis for the given 4-momenta 
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
    // ========================================================================    
    /** get the four momentum fo the combination of particles 
     *  using the mass hypothesis (masses, pids of names) form some 
     *  vector 
     *  
     *  evaluate the 4 momentum using wrong hypos for particles:
     *  @code 
     *  
     *  const LHCb::Particle::ConstVector& parts = ... ;
     *  // it works also for LHCb::MCParticle 
     *  // const LHCb::Particle::ConstVector& parts = ... ;
     *
     *  // vector of wrong masses:
     *  const std::vector<double>&         masses = ... ;
     * 
     *  LoKi::LorentzVector r1 = 
     *     wrongMass ( parts.begin()  , parts.end() , 
     *                 masses.begin() , _VALID_     ) ;
     *
     *  // vector of wrong pids
     *  const std::vector<LHCb::ParticleID>&    pids = ... ;
     * 
     *  LoKi::LorentzVector r2 = 
     *     wrongMass ( parts.begin()  , parts.end() , 
     *                 masses.begin() , _VALID_     ) ;
     *
     *  // vector of wrong mass-hypos:
     *  const std::vector<std::string>&  hypos = ... ;
     * 
     *  LoKi::LorentzVector r3 = 
     *     wrongMass ( parts.begin()  , parts.end() , 
     *                 hypos.begin() , _VALID_     ) ;
     *
     *  @endcode 
     */
    template <class PARTICLE,class WMASSVAL,class PREDICATE>
    inline 
    LoKi::LorentzVector 
    wrongMass 
    ( PARTICLE            first  , 
      PARTICLE            last   , 
      WMASSVAL            begin  ,
      PREDICATE           cut    , 
      LoKi::LorentzVector result = LoKi::LorentzVector() ) 
    {
      for ( ; first != last ; ++first , ++begin )
      {
        if ( cut (*first) ) 
        { result += wrongMass ( (*first)->momentum() , *begin ) ; }  
      }
      return result ;
    }
    // ========================================================================
  } // end of namespace LoKi::Kinematics 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_WRONGMASS_H
// ============================================================================
