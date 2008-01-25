// $Id: Particles19.cpp,v 1.1 2008-01-25 14:42:22 ibelyaev Exp $
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
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTime*
LoKi::Particles::LifeTime::clone() const 
{ return  new LoKi::Particles::LifeTime ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTime::result_type
LoKi::Particles::LifeTime::operator() 
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
  Assert ( m_fitter.validPointer() , "ILifetimeFitter is invalid! " ) ;
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
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTime::fillStream
( std::ostream& s ) const { return s << "LIFETIME" ; }
// ============================================================================

// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2::LifeTimeChi2
( const LHCb::VertexBase* vertex , 
  const ILifetimeFitter*  tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2::LifeTimeChi2
( const ILifetimeFitter*  tool   , 
  const LHCb::VertexBase* vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2::LifeTimeChi2
( const LHCb::VertexBase*                 vertex , 
  const LoKi::Interface<ILifetimeFitter>& tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2::LifeTimeChi2
( const LoKi::Interface<ILifetimeFitter>& tool   , 
  const LHCb::VertexBase*                 vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeChi2*
LoKi::Particles::LifeTimeChi2::clone() const 
{ return  new LoKi::Particles::LifeTimeChi2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeChi2::result_type
LoKi::Particles::LifeTimeChi2::operator() 
  ( LoKi::Particles::LifeTimeChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex() , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( m_fitter.validPointer() , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    m_fitter -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IlifetimeFitter::fit, return -InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // evaluate chi2 
  return Gaudi::Math::pow ( i_time / i_error , 2 ) ; // RETURN 
}

// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeChi2::fillStream
( std::ostream& s ) const { return s << "LTCHI2" ; }
// ============================================================================



// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2::LifeTimeSignedChi2
( const LHCb::VertexBase* vertex , 
  const ILifetimeFitter*  tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2::LifeTimeSignedChi2
( const ILifetimeFitter*  tool   , 
  const LHCb::VertexBase* vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2::LifeTimeSignedChi2
( const LHCb::VertexBase*                 vertex , 
  const LoKi::Interface<ILifetimeFitter>& tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2::LifeTimeSignedChi2
( const LoKi::Interface<ILifetimeFitter>& tool   , 
  const LHCb::VertexBase*                 vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2*
LoKi::Particles::LifeTimeSignedChi2::clone() const 
{ return  new LoKi::Particles::LifeTimeSignedChi2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2::result_type
LoKi::Particles::LifeTimeSignedChi2::operator() 
  ( LoKi::Particles::LifeTimeSignedChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex() , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( m_fitter.validPointer() , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    m_fitter -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
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
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeSignedChi2::fillStream
( std::ostream& s ) const { return s << "LTSIGNCHI2" ; }
// ============================================================================




// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2::LifeTimeFitChi2
( const LHCb::VertexBase* vertex , 
  const ILifetimeFitter*  tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2::LifeTimeFitChi2
( const ILifetimeFitter*  tool   , 
  const LHCb::VertexBase* vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2::LifeTimeFitChi2
( const LHCb::VertexBase*                 vertex , 
  const LoKi::Interface<ILifetimeFitter>& tool   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2::LifeTimeFitChi2
( const LoKi::Interface<ILifetimeFitter>& tool   , 
  const LHCb::VertexBase*                 vertex )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::Vertices::VertexHolder ( vertex  ) 
  , m_fitter ( tool ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeFitChi2*
LoKi::Particles::LifeTimeFitChi2::clone() const 
{ return  new LoKi::Particles::LifeTimeFitChi2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2::result_type
LoKi::Particles::LifeTimeFitChi2::operator() 
  ( LoKi::Particles::LifeTimeFitChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  // check the vertex
  Assert ( 0 != vertex() , "Primary vertex is invalid! " ) ;
  // check the fitter 
  Assert ( m_fitter.validPointer() , "ILifetimeFitter is invalid! " ) ;
  //
  double i_time  = 0 ;
  double i_error = 0 ;
  double i_chi2  = 0 ;
  // use the fitter
  StatusCode sc = 
    m_fitter -> fit ( *vertex() , *p , i_time , i_error , i_chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IlifetimeFitter::fit, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  return i_chi2 ;                                                // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeFitChi2::fillStream
( std::ostream& s ) const { return s << "LTFITCHI2" ; }
// ============================================================================



// ============================================================================
// The END
// ============================================================================
