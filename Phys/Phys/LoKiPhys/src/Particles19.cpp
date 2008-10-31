// $Id: Particles19.cpp,v 1.2 2008-10-31 17:27:46 ibelyaev Exp $
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
  const ILifetimeFitter*  tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const ILifetimeFitter*  tool   , 
  const LHCb::VertexBase* vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const LHCb::VertexBase*                 vertex , 
  const LoKi::Interface<ILifetimeFitter>& tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTime::LifeTime
( const LoKi::Interface<ILifetimeFitter>& tool   , 
  const LHCb::VertexBase*                 vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::lifeTime 
( LoKi::Particles::LifeTime::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return -InvalidTime") ;
    return -LoKi::Constants::InvalidTime ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex() , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( 0 != tool () , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    m_fitter -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IlifetimeFitter::fit, return -InvalidTime" , sc ) ;
    return -LoKi::Constants::InvalidTime ;                         // RETURN 
  }
  return i_time ;                                                  // RETURN 
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeChi2::result_type
LoKi::Particles::LifeTimeChi2::lifeTimeChi2
( LoKi::Particles::LifeTimeChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( 0 != tool   ()  , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    tool() -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IlifetimeFitter::fit, return -InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // evaluate chi2 
  return Gaudi::Math::pow ( i_time / i_error , 2 ) ; // RETURN 
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2::result_type
LoKi::Particles::LifeTimeSignedChi2::lifeTimeSignedChi2
( LoKi::Particles::LifeTimeSignedChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( 0 != fitter () , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    fitter() -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IlifetimeFitter::fit, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // evaluate chi2:
  const double res = Gaudi::Math::pow ( i_time / i_error , 2 ) ; 
  // set the sign:
  return 0 < i_time ? res : -res ;                                 // RETURN 
}
// ============================================================================


// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2::result_type
LoKi::Particles::LifeTimeFitChi2::lifeTimeFitChi2
( LoKi::Particles::LifeTimeFitChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex () , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( 0 != fitter () , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    fitter () -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IlifetimeFitter::fit, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  return i_chi2 ;                                                // RETURN 
}
// ============================================================================




// ============================================================================
// The END
// ============================================================================
