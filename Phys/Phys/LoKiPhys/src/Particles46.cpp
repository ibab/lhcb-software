// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/MissingParticle.h"
#include "LoKi/Particles46.h"
#include "LoKi/Child.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// ============================================================================
/** @file 
 *  Implementation file for functioonf from the file LoKi/Particles46.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
 *  @date 2015-02-06 
 *
 *                     $Revision$
 *  Last modification: $Date$
 *                 by: $Author$
 */
// ============================================================================
/*  constructor with all parameters 
 *  @param mother   nominal mass of mother  particle 
 *  @param missing  nominal mass of missing particle 
 *  @param onlygood count only good solutions 
 */
// ============================================================================
LoKi::Particles::MissingParticle::MissingParticle
( const double mother    , 
  const double missing   , 
  const bool   only_good ) 
  : LoKi::AuxFunBase      ( std::tie ( mother , missing , only_good ) ) 
  , LoKi::AuxDesktopBase  ()
  , m_mother    ()
  , m_missing   ()
  , m_m_mother  ( mother    )
  , m_m_missing ( missing   )
  , m_only_good ( only_good ) 
{
  Assert ( 0 <  m_m_mother  , 
           "Non-positive mass of mother  particle is specified!" ) ;
  Assert ( 0 <= m_m_missing , 
           "Negative     mass of missing particle is specified!" ) ;
  Assert ( m_m_missing < m_m_mother , 
           "Invalid hierarchy of mother/missing masses"          ) ;
}
// ============================================================================
/*  constructor with all parameters 
 *  @param mother   nominal mass of mother  particle 
 *  @param missing  nominal mass of missing particle 
 *  @param onlygood count only good solutions 
 */
// ============================================================================
LoKi::Particles::MissingParticle::MissingParticle
( const std::string& mother    , 
  const std::string& missing   , 
  const bool         only_good ) 
  : LoKi::AuxFunBase      ( std::tie ( mother , missing , only_good ) ) 
  , LoKi::AuxDesktopBase  ()
  , m_mother    ( mother    )
  , m_missing   ( missing   )
  , m_m_mother  ( -1 )
  , m_m_missing ( -1 )
  , m_only_good ( only_good ) 
{
  if ( gaudi() ) 
  {
    m_m_mother  = LoKi::Particles::massFromName ( m_mother  ) ;
    m_m_missing = LoKi::Particles::massFromName ( m_missing ) ;
    Assert ( m_m_missing < m_m_mother , 
             "Invalid hierarchy of mother/missing masses" ) ;
  }
}
// ============================================================================}
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MissingParticle::~MissingParticle(){}
// ============================================================================
// get masses 
// ============================================================================
void LoKi::Particles::MissingParticle::getMasses () const 
{ 
  //
  if ( !m_mother .empty() ) 
  { m_m_mother   = LoKi::Particles::massFromName ( m_mother   ) ; }
  if ( !m_missing.empty() ) 
  { m_m_missing  = LoKi::Particles::massFromName ( m_missing  ) ; }
  //
}
// ============================================================================

// ============================================================================
/* constructor with all parameters 
 *  @param mother   nominal mass of mother  particle 
 *  @param missing  nominal mass of missing particle 
 *  @param onlygood count only good solutions 
 */
// ============================================================================
LoKi::Particles::MP_nSolutions::MP_nSolutions
( const double mother    , 
  const double missing   , 
  const bool   only_good ) 
  : LoKi::AuxFunBase ( std::tie ( mother , missing , only_good ) ) 
  , LoKi::Particles::MissingParticle ( mother , missing , only_good ) 
{}
// ============================================================================
/* constructor with all parameters 
 *  @param mother   nominal mass of mother  particle 
 *  @param missing  nominal mass of missing particle 
 *  @param onlygood count only good solutions 
 */
// ============================================================================
LoKi::Particles::MP_nSolutions::MP_nSolutions
( const std::string& mother      , 
  const std::string& missing     , 
  const bool   only_good ) 
  : LoKi::AuxFunBase ( std::tie ( mother , missing , only_good ) ) 
  , LoKi::Particles::MissingParticle ( mother , missing , only_good ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MP_nSolutions::~MP_nSolutions(){}
// ============================================================================
// MANDARORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MP_nSolutions*
LoKi::Particles::MP_nSolutions::clone() const 
{ return new LoKi::Particles::MP_nSolutions ( *this ) ; }
// ============================================================================
// MANDARORY: the only interesitng method 
// ============================================================================
LoKi::Particles::MP_nSolutions::result_type 
LoKi::Particles::MP_nSolutions::operator() 
  ( LoKi::Particles::MP_nSolutions::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "Invalid particle, return -10" ) ;
    return -10 ;                                            // RETURN 
  }
  // own vertex 
  const LHCb::VertexBase* ev = p->endVertex() ;
  if ( 0 == ev ) 
  {
    Error ( "Invalid end-vertex, return -9" ) ;
    return -9 ;                                              // RETURN
  }  
  //
  if ( !goodMasses() ) { getMasses() ; }
  if ( !goodMasses() ) 
  {
    Error("Unable to get good masses, return -8") ;
    return -8 ;                                               // RETURN 
  }
  //
  if ( !desktop() ) { loadDesktop() ; }
  Assert ( 0 != desktop() , "Unable to load IPhysDesktop!" ) ;
  //
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Warning ( "No primary vertex is available, reutrn -7") ;
    return -7 ;                                               // RETURN 
  }
  //
  // get the flight direction 
  const LoKi::ThreeVector dir = ev->position() - pv->position() ;
  // construct the missing particle 
  const LoKi::Kinematics::MissingParticle mp ( p->momentum() , 
                                               dir           , 
                                               mother  ()    , 
                                               missing ()    ) ;
  //
  // number of solutions 
  const short n = mp.nSolutions() ;
  //
  if ( !only_good() || 0 == n ) { return n ; }  // RETURN
  //
  if ( 1 == n ) 
  {
    const LoKi::LorentzVector tot = p->momentum () + mp.v1() ;
    const LoKi::ThreeVector   p3 ( tot.Px() , tot.Py() , tot.Pz() ) ;
    return 0 < p3.Dot( dir ) ? 1 : 0 ;
  }
  //
  const LoKi::LorentzVector t1 = p->momentum () + mp.v1 () ;
  const LoKi::LorentzVector t2 = p->momentum () + mp.v2 () ;
  //
  const LoKi::ThreeVector   p3_1 ( t1.Px() , t1.Py() , t1.Pz() ) ;
  const LoKi::ThreeVector   p3_2 ( t2.Px() , t2.Py() , t2.Pz() ) ;
  //
  const bool ok1 = 0 < p3_1.Dot( dir ) ;
  const bool ok2 = 0 < p3_2.Dot( dir ) ;
  //
  return 
    ok1 && ok2 ? 2 : 
    ok1        ? 1 :
    ok2        ? 1 : 0 ;
}
LoKi::Particles::MissingDNeutrinoDOCA::MissingDNeutrinoDOCA(
    const double mother,
    const double missing,
    const bool   only_good
  )
  : LoKi::AuxFunBase( std::tie(mother,missing, only_good) )
  , LoKi::Particles::MissingParticle(mother,missing, only_good)
{}

LoKi::Particles::MissingDNeutrinoDOCA::MissingDNeutrinoDOCA( 
    const std::string& mother,
    const std::string& missing,
    const bool   only_good
  )
  : LoKi::AuxFunBase( std::tie(mother,missing, only_good) )
  , LoKi::Particles::MissingParticle(mother,missing, only_good)
{}

LoKi::Particles::MissingDNeutrinoDOCA::~MissingDNeutrinoDOCA(){}
// ============================================================================
// MANDARORY: the only interesitng method 
// ============================================================================
LoKi::Particles::MissingDNeutrinoDOCA::result_type 
LoKi::Particles::MissingDNeutrinoDOCA::operator() 
  ( LoKi::Particles::MissingDNeutrinoDOCA::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "Invalid particle, return -10" ) ;
    return -10 ;                                            // RETURN 
  }
  // get D and X
  const LHCb::Particle* rec_d = LoKi::Child::child( p , 1);
  
  if ( 0 == rec_d ) 
  {
    Error ("Invalid D0 particle, return -10") ;
    return -10 ;
  }

  const LHCb::Particle* rec_x = LoKi::Child::child( p , 2);

  if ( 0 == rec_x )
  {
    Error ("Invalid X particle, return -10") ;
    return -10 ;
  }

  if(rec_d->particleID().abspid() != 421 )
  {
    Error ("D particle not in the first of decay, return -10") ;
    return -10 ;                                            
  }
  const LHCb::VertexBase* dv = rec_d->endVertex();
  if ( 0 == dv )
  {
    Error ( "Invalid end-vertex, return -9" ) ;
    return -9 ;  
  }  

  Gaudi::XYZPoint xp;
  if(rec_x->isBasicParticle()) xp = rec_x->referencePoint();
  else{
    const LHCb::VertexBase* xv = rec_x->endVertex();
    if ( 0 == dv )
    { 
      Error ( "Invalid end-vertex, return -9" ) ;
      return -9 ;
    } 
    xp = xv->position();
  }

  //
  if ( !desktop() ) { loadDesktop() ; }
  Assert ( 0 != desktop() , "Unable to load IPhysDesktop!" ) ;
  //
  const LHCb::VertexBase* pv = bestVertex( p ) ;
  if ( 0 == pv ) 
  {
    Warning ( "No primary vertex is available, reutrn -9") ;
    return -9 ;                                               // RETURN 
  }

  typedef Gaudi::Math::Line<Gaudi::XYZPoint, Gaudi::XYZVector> LINE;

  LINE line1(xp, Gaudi::XYZVector(rec_x->momentum().x(),rec_x->momentum().y(),rec_x->momentum().z()));
  LINE line2(pv->position(),dv->position()-pv->position());
  
  const double d = Gaudi::Math::distance(line1,line2);
  return d;
}
// ============================================================================
// The END 
// ============================================================================
