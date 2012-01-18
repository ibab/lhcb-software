// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <cmath>
#include <functional>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Track.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysKinematics.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from the file LoKi/PhysKinematics.h
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
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-06 
 */
// ============================================================================
/*  Trivial function to evaluate the mass LHCb::Particle
 *  @param  p particle 
 *  @return invariant mass 
 *  @see LHCb::Particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::PhysKinematics::mass 
( const LHCb::Particle* p ) 
{
  //
  if ( 0 == p ) 
  {
    LoKi::Report::Error
      ("mass(LHCb::Particle*),invalid argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return LoKi::Kinematics::mass 
    ( p->momentum() ) ;
} 
// ============================================================================
/*  Trivial function to evaluate the mass LHCb::Particle
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @return invariant mass 
 *  @see LHCb::Particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::PhysKinematics::mass 
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) 
{
  //
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(2xLHCb::Particle*),invalid argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(2xLHCb::Particle*),invalid argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return LoKi::Kinematics::mass
    ( p1->momentum() , 
      p2->momentum() ) ;
} 
// ============================================================================
/*  Trivial function to evaluate the mass LHCb::Particle
 *  @param  p1 the first particle 
 *  @param  p2 the third particle 
 *  @param  p3 the second particle 
 *  @return invariant mass 
 *  @see LHCb::Particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::PhysKinematics::mass 
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ,
  const LHCb::Particle* p3 ) 
{
  //
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(3xLHCb::Particle*),invalid argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //  
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(3xLHCb::Particle*),invalid argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  if ( 0 == p3 ) 
  {
    LoKi::Report::Error
      ("mass(3xLHCb::Particle*),invalid argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return LoKi::Kinematics::mass 
    ( p1->momentum() , 
      p2->momentum() ,
      p3->momentum() ) ;
} 
// ============================================================================
/*  Trivial function to evaluate the mass LHCb::Particle
 *  @param  p1 the first particle 
 *  @param  p2 the second particle 
 *  @param  p3 the third  particle 
 *  @param  p3 the fourth  particle 
 *  @return invariant mass 
 *  @see LHCb::Particle 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
double LoKi::PhysKinematics::mass
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ,
  const LHCb::Particle* p3 ,
  const LHCb::Particle* p4 ) 
{
  //
  if ( 0 == p1 ) 
  {
    LoKi::Report::Error
      ("mass(4xLHCb::Particle*),invalid argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  if ( 0 == p2 ) 
  {
    LoKi::Report::Error
      ("mass(4xLHCb::Particle*),invalid argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  if ( 0 == p3 ) 
  {
    LoKi::Report::Error
      ("mass(4xLHCb::Particle*),invalid argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  if ( 0 == p4 ) 
  {
    LoKi::Report::Error
      ("mass(4xLHCb::Particle*),invalid argument 4,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }  
  //
  return LoKi::Kinematics::mass
    ( p1->momentum() , 
      p2->momentum() ,
      p3->momentum() , 
      p4->momentum() ) ;
}
// ============================================================================
namespace 
{  
  // ==========================================================================
  const Gaudi::LorentzVector s_VECTOR ;
  // ==========================================================================
  struct _P4 : public std::binary_function<const LHCb::Track*,double, 
                                           Gaudi::LorentzVector>
  {
    // ========================================================================
    inline Gaudi::LorentzVector operator() 
      ( const LHCb::Track* t , 
        const double       m ) const 
    {
      //
      if ( 0 == t ) { return s_VECTOR ; }                            // RETURN 
      //  
      const Gaudi::XYZVector p3 = t->momentum () ;
      const double           E  = ::sqrt ( p3.Mag2() + m * m  ) ;
      return Gaudi::LorentzVector ( p3.X() , p3.Y() , p3.Z() , E ) ;
    }
    // ========================================================================
  } ;  
  // ==========================================================================
  template <class TRACK>
  inline double _mass
  ( TRACK                      begin , 
    TRACK                      end    ,  
    const std::vector<double>& masses ) 
  {
    //
    const std::size_t size = std::distance ( begin , end ) ;
    //
    if ( 0 == size ) { return 0 ; }
    // adjust list of masses (if needed) 
    if      ( size > masses.size() ) 
    {
      LoKi::Report::Warning ("mass(Tracks): masses to be adjusted") ;
      std::vector<double> newmasses  ( masses ) ;
      while ( size > newmasses.size() ) { newmasses.push_back ( 0 ) ; }  
      return _mass ( begin , end , newmasses ) ;
    }
    else if ( size < masses.size() ) 
    { LoKi::Report::Warning ("mass(Tracks),too many masses, ignore") ; }
    //
    _P4 p4 ;
    //
    Gaudi::LorentzVector p ;
    for ( TRACK curr = begin ; curr != end ; ++curr ) 
    { p += p4 ( *curr, masses [ curr - begin ] ) ; }
    //
    return p.M() ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
/*  trivial function to get the lorentz-vector for the track using
 *  mass-hypthesis 
 *  @param track (INPUT) the track 
 *  @param mass  (INPUT) the mass hypothesis 
 *  @return 4-vector 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-07
 */ 
// ============================================================================
Gaudi::LorentzVector
LoKi::PhysKinematics::momentum 
( const LHCb::Track* track , 
  const double       mass  ) 
{
  //
  if ( 0 == track ) 
  {
    LoKi::Report::Error
      ("momentum(LHCb::Track*),invalid argument 1,return null 4-momentum") ;
    return s_VECTOR ;
  }
  //
  _P4 p4 ;
  //
  return p4 ( track , mass ) ;
  //
}
// ===========================================================================
/*  trivial function to get the mass for the tracks using
 *  the mass-hypotheses 
 *  @param track1 (INPUT) the first  track 
 *  @param mass1  (INPUT) the first  mass hypothesis 
 *  @param track2 (INPUT) the second track 
 *  @param mass2  (INPUT) the second mass hypothesis 
 *  @return mass 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-07
 */ 
// ===========================================================================
double LoKi::PhysKinematics::mass
( const LHCb::Track* track1 , 
  const double       mass1  ,
  const LHCb::Track* track2 , 
  const double       mass2  ) 
{
  if ( 0 == track1 ) 
  {
    LoKi::Report::Error
      ("mass(2xLHCb::Track*),invalid argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == track2 ) 
  {
    LoKi::Report::Error
      ("mass(2xLHCb::Track*),invalid argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  _P4 p4 ;
  //
  Gaudi::LorentzVector p  = p4 ( track1 , mass1 ) ;
  p                      += p4 ( track2 , mass2 ) ;
  //
  return p.M() ;
}
// ============================================================================
/*  trivial function to get the mass for the tracks using
 *  the mass-hypotheses 
 *  @param track1 (INPUT) the first  track 
 *  @param mass1  (INPUT) the first  mass hypothesis 
 *  @param track2 (INPUT) the second track 
 *  @param mass2  (INPUT) the second mass hypothesis 
 *  @param track3 (INPUT) the third  track 
 *  @param mass3  (INPUT) the third  mass hypothesis 
 *  @return mass 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-07
 */ 
// ============================================================================
double LoKi::PhysKinematics::mass
( const LHCb::Track* track1 , 
  const double       mass1  ,
  const LHCb::Track* track2 , 
  const double       mass2  ,
  const LHCb::Track* track3 , 
  const double       mass3  ) 
{
  if ( 0 == track1 ) 
  {
    LoKi::Report::Error
      ("mass(3xLHCb::Track*),invalid argument 1,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == track2 ) 
  {
    LoKi::Report::Error
      ("mass(3xLHCb::Track*),invalid argument 2,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  if ( 0 == track3 ) 
  {
    LoKi::Report::Error
      ("mass(3xLHCb::Track*),invalid argument 3,return 'InvalidMass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  _P4 p4 ;
  //
  Gaudi::LorentzVector p  = p4 ( track1 , mass1 ) ;
  p                      += p4 ( track2 , mass2 ) ;
  p                      += p4 ( track3 , mass3 ) ;
  //
  return p.M() ;
}
// ============================================================================
/*  trivial function to get the mass for the tracks using
 *  the mass-hypotheses 
 *  @param tracks (INPUT) the vetcor of tracks
 *  @param masses (INPUT) the vector of masses 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-07
 */ 
// ============================================================================
double LoKi::PhysKinematics::mass
( const LHCb::Track::ConstVector& tracks , 
  const std::vector<double>&      masses ) 
{
  if ( tracks.empty() ) { return 0 ; }
  // adjust list of masses (if needed) 
  return _mass ( tracks.begin () , tracks.end() , masses ) ;
}
// ============================================================================
/*  trivial function to get the mass for the tracks using
 *  the mass-hypotheses 
 *  @param tracks (INPUT) the vetcor of tracks
 *  @param masses (INPUT) the vector of masses 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-07
 */ 
// ============================================================================
double LoKi::PhysKinematics::mass
( const SmartRefVector<LHCb::Track>& tracks , 
  const std::vector<double>&         masses ) 
{
  if ( tracks.empty() ) { return 0 ; }
  // adjust list of masses (if needed) 
  return _mass ( tracks.begin () , tracks.end() , masses ) ;
}
// ============================================================================
/*  trivial function to calculate \f$ \Delta R^2 \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return dr2 
 *  @author Vanya BELYAEV Ivan.Belyaev@ite.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::PhysKinematics::deltaR2
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 )
{
  //
  if ( 0  == p1 || 0 == p2 ) { return LoKi::Constants::InvalidDistance ; }
  if ( p1 == p2            ) { return 0 ; }
  //
  return LoKi::Kinematics::deltaR2  ( p1 -> momentum () , 
                                      p2 -> momentum () ) ;
}
// ============================================================================
/*  trivial function to calculate \f$ \Delta \eta \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return delta-eta 
 *  @author Vanya BELYAEV Ivan.Belyaev@ite.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::PhysKinematics::deltaEta
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 )
{
  //
  if ( 0  == p1 || 0 == p2 ) { return LoKi::Constants::InvalidDistance ; }
  if ( p1 == p2            ) { return 0 ; }
  //
  return LoKi::Kinematics::deltaEta ( p1 -> momentum () , 
                                      p2 -> momentum () ) ;
}
// ===========================================================================
/*  trivial function to calculate \f$ \Delta \phi \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return delta-phi 
 *  @author Vanya BELYAEV Ivan.Belyaev@ite.ru
 *  @date 2012-01-17
 */ 
// ============================================================================
double LoKi::PhysKinematics::deltaPhi
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 )
{
  //
  if ( 0  == p1 || 0 == p2 ) { return LoKi::Constants::InvalidAngle ; }
  if ( p1 == p2            ) { return 0 ; }
  //
  return LoKi::Kinematics::deltaPhi ( p1 -> momentum () , 
                                      p2 -> momentum () ) ;
}
// ========================================================================
/*  trivial function to calculate \f$ \Delta \alpha \f$ for two particles 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return delta-alpha 
 *  @author Vanya BELYAEV Ivan.Belyaev@ite.ru
 *  @date 2012-01-17
 */ 
// ========================================================================
double LoKi::PhysKinematics::deltaAlpha
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) 
{
  //
  if ( 0  == p1 || 0 == p2 ) { return LoKi::Constants::InvalidAngle ; }
  if ( p1 == p2            ) { return 0 ; }
  //
  return LoKi::Kinematics::deltaAlpha ( p1 -> momentum () , 
                                        p2 -> momentum () ) ;
}  
// ==============================================================================
/*  trivial function to calculate "mass-distance"
 *  \f$ m^2_{12} - m^2_1 - m^2_2\f$
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return delta-alpha 
 *  @author Vanya BELYAEV Ivan.Belyaev@ite.ru
 *  @date 2012-01-17
 */ 
// ==============================================================================
double LoKi::PhysKinematics::deltaM2
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) 
{
  //
  if ( 0  == p1 || 0 == p2 ) { return LoKi::Constants::InvalidDistance ; }
  if ( p1 == p2            ) { return 0 ; }
  //
  return LoKi::Kinematics::deltaM2 ( p1 -> momentum () , 
                                     p2 -> momentum () ) ;
}
// ===========================================================================
/*  trivial function to calculate Kullback-Liebler divergency 
 *  @param p1 the first  particle 
 *  @param p2 the second particle 
 *  @return kullback-leibler divergency
 *  @author Vanya BELYAEV Ivan.Belyaev@ite.ru
 *  @date 2012-01-17
 */ 
// ===========================================================================
double LoKi::PhysKinematics::kullback
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) 
{
  //
  if ( 0  == p1 || 0 == p2 ) { return LoKi::Constants::InvalidDistance ; }
  if ( p1 == p2            ) { return 0 ; }
  //
  return LoKi::Kinematics::kullback 
    ( p1 -> momentum     () , 
      p1 -> momCovMatrix () , 
      p2 -> momentum     () , 
      p2 -> momCovMatrix () ) ;
}
// ============================================================================
// The END 
// ============================================================================

