// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Power.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles19.h"
// ============================================================================
/** @file
 *
 *  Implementation file for the functors from fiel LoKi/Particles19.h
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
 *  @date 2006-02-19 
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const LHCb::VertexBase* vertex , 
  const ILifetimeFitter*  tool   ,
  const double            chi2   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter  ( tool ) 
  , m_chi2cut ( chi2 ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const ILifetimeFitter*  tool   , 
  const LHCb::VertexBase* vertex ,
  const double            chi2   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter  ( tool ) 
  , m_chi2cut ( chi2 ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const LHCb::VertexBase*                 vertex , 
  const LoKi::Interface<ILifetimeFitter>& tool   ,
  const double                            chi2   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter  ( tool ) 
  , m_chi2cut ( chi2 ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const LoKi::Interface<ILifetimeFitter>& tool   , 
  const LHCb::VertexBase*                 vertex ,
  const double                            chi2   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter  ( tool ) 
  , m_chi2cut ( chi2 ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::LifeTime::~LifeTime()
{
  if ( m_fitter && !gaudi() ) 
  {
    // Warning("ILifetimeFitter: manual reset!") ;
    m_fitter.reset() ;
  }
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::lifeTime 
( LoKi::Particles::LifeTime::argument p ) const 
{
  if ( !p ) 
  {
    Error ("LHCb::Particle* points to NULL, return -InvalidTime").ignore() ;
    return -LoKi::Constants::InvalidTime ;                         // RETURN 
  }
  // check the vertex
  Assert ( vertex() , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( tool() , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    m_fitter -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IlifetimeFitter::fit, return InvalidTime", sc, 0 ).ignore() ;
    return LoKi::Constants::InvalidTime ;  // RETURN 
  }
  /** apply embedded chi2-cut! 
   *  @attention apply embedded chi2-cut
   */
  if ( 0 < chi2cut() && chi2cut() < i_chi2 )  
  { return LoKi::Constants::InvalidTime ; }           // RETURN
  //
  return i_time ;                                                  // RETURN 
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::lifeTimeChi2
( LoKi::Particles::LifeTime::argument p ) const 
{
  if ( !p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ).ignore() ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( tool   ()  , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    tool() -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IlifetimeFitter::fit, return -InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  //
  // apply embedded chi2-cut! 
  if ( 0 < chi2cut() && chi2cut() < i_chi2 )  
  { return  LoKi::Constants::InvalidChi2 ; }                       // RETURN
  //
  // evaluate chi2 
  return Gaudi::Math::pow ( i_time / i_error , 2 ) ; // RETURN 
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::lifeTimeSignedChi2
( LoKi::Particles::LifeTime::argument p ) const 
{
  if ( !p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ).ignore() ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( fitter () , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    fitter() -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IlifetimeFitter::fit, return InvalidChi2" , sc, 0 ).ignore() ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // apply embedded chi2-cut! 
  if ( 0 < chi2cut() && chi2cut() < i_chi2 )  
  { return  LoKi::Constants::InvalidChi2 ; }                       // RETURN
  //
  // evaluate chi2:
  const double res = Gaudi::Math::pow ( i_time / i_error , 2 ) ; 
  // set the sign:
  return 0 < i_time ? res : -res ;                                 // RETURN 
}
// ============================================================================


// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::lifeTimeFitChi2
( LoKi::Particles::LifeTime::argument p ) const 
{
  if ( !p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ).ignore() ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( fitter () , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    fitter () -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IlifetimeFitter::fit, return InvalidChi2" , sc, 0 ).ignore() ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  //
  return i_chi2 ;                                                 // RETURN 
}
// ============================================================================

// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::lifeTimeError
( LoKi::Particles::LifeTime::argument p ) const 
{
  if ( !p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidTime" ) ;
    return LoKi::Constants::InvalidTime ;                         // RETURN 
  }
  // check the vertex
  Assert ( vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( fitter () , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    fitter () -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IlifetimeFitter::fit, return InvalidTime" , sc, 0 ).ignore() ;
    return LoKi::Constants::InvalidTime ;                             // RETURN 
  }
  //
  return i_error ;                                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTime::fillStream ( std::ostream& s ) const 
{
  s << "LIFETIME" ;
  if ( 0 < chi2cut() ) { s << "(" << chi2cut() << ")" ; }
  return s ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeChi2::fillStream ( std::ostream& s ) const 
{
  s << "LTCHI2" ;
  if ( 0 < chi2cut() ) { s << "(" << chi2cut() << ")" ; }
  return s ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::LifeTimeSignedChi2::fillStream ( std::ostream& s ) const 
{
  s << "LTSIGNCHI2" ;
  if ( 0 < chi2cut() ) { s << "(" << chi2cut() << ")" ; }
  return s ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::LifeTimeFitChi2::fillStream ( std::ostream& s ) const 
{ return s << "LTFITCHI2" ; }

// ============================================================================
// The END
// ============================================================================
