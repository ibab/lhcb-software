// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles35.h"
// ============================================================================
/* @file
 *  
 *  Implementation file for class : Particles35
 *
 *  @date 2010-05-26 
 *  @author Vanya Belyaev
 */
// ============================================================================
// constructor from protoparticle-function
// ============================================================================
LoKi::Particles::ProtoPFun::ProtoPFun 
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function& fun ,
  const double                                                     bad ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun  ( fun ) 
  , m_bad   ( bad ) 
{}
// ============================================================================
// constructor from protoparticle-function
// ============================================================================
LoKi::Particles::ProtoPFun::ProtoPFun 
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function& fun )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun  ( fun ) 
  , m_bad  ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::ProtoPFun::~ProtoPFun(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ProtoPFun*
LoKi::Particles::ProtoPFun::clone() const 
{ return new LoKi::Particles::ProtoPFun ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProtoPFun::result_type 
LoKi::Particles::ProtoPFun::operator() 
  ( LoKi::Particles::ProtoPFun::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'bad'") ;
    return m_bad ;                                                   // RETURN
  }
  const LHCb::ProtoParticle* pp = p->proto() ;
  if ( 0 == pp ) 
  {
    Error ("LHCb::ProtoParticle* points to NULL, return 'bad'") ;
    return m_bad ;                                                   // RETURN
  }
  return m_fun ( pp ) ;                                              // RETURN
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::ProtoPFun::fillStream ( std::ostream& s ) const 
{
  s << " PPFUN( " << m_fun ;
  if ( m_bad != LoKi::Constants::NegativeInfinity ) { s << " ," << m_bad ; }
  return s << " ) " ;
}

// ============================================================================
// constructor from protoparticle-function
// ============================================================================
LoKi::Particles::ProtoPCut::ProtoPCut 
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Predicate& cut ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , m_cut  ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::ProtoPCut::~ProtoPCut(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ProtoPCut*
LoKi::Particles::ProtoPCut::clone() const 
{ return new LoKi::Particles::ProtoPCut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProtoPCut::result_type 
LoKi::Particles::ProtoPCut::operator() 
  ( LoKi::Particles::ProtoPCut::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'false'") ;
    return false ;                                                   // RETURN
  }
  const LHCb::ProtoParticle* pp = p->proto() ;
  if ( 0 == pp ) 
  {
    Error ("LHCb::ProtoParticle* points to NULL, return 'false'") ;
    return false ;                                                   // RETURN
  }
  return m_cut ( pp ) ;                                              // RETURN
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::ProtoPCut::fillStream ( std::ostream& s ) const 
{ return s << " PPCUT( " << m_cut << " ) " ; }



// ============================================================================
// constructor from protoparticle-function
// ============================================================================
LoKi::Particles::TrackFun::TrackFun 
( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun , 
  const double                                             bad ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun  ( fun ) 
  , m_bad  ( bad ) 
{}
// ============================================================================
// constructor from protoparticle-function
// ============================================================================
LoKi::Particles::TrackFun::TrackFun 
( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun  ( fun ) 
  , m_bad  ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TrackFun::~TrackFun(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TrackFun*
LoKi::Particles::TrackFun::clone() const 
{ return new LoKi::Particles::TrackFun ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackFun::result_type 
LoKi::Particles::TrackFun::operator() 
  ( LoKi::Particles::TrackFun::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'bad'") ;
    return m_bad ;                                                   // RETURN
  }
  //
  const LHCb::ProtoParticle* pp = p->proto() ;
  if ( 0 == pp ) 
  {
    Error ("LHCb::ProtoParticle* points to NULL, return 'bad'") ;
    return m_bad ;                                                   // RETURN
  }
  //
  const LHCb::Track* track = pp->track() ;
  if ( 0 == track ) 
  {
    Error ("LHCb::Track* points to NULL, return 'bad'") ;
    return m_bad ;                                                   // RETURN
  }
  //
  return m_fun ( track ) ;                                          // RETURN
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::TrackFun::fillStream ( std::ostream& s ) const 
{
  s << " TRFUN( " << m_fun ;
  if ( m_bad != LoKi::Constants::NegativeInfinity ) { s << " ," << m_bad ; }
  return s << " ) " ;
}



// ============================================================================
// constructor from protoparticle-function
// ============================================================================
LoKi::Particles::TrackCut::TrackCut 
( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , m_cut  ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TrackCut::~TrackCut(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TrackCut*
LoKi::Particles::TrackCut::clone() const 
{ return new LoKi::Particles::TrackCut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrackCut::result_type 
LoKi::Particles::TrackCut::operator() 
  ( LoKi::Particles::TrackCut::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'false'") ;
    return false ;                                                   // RETURN
  }
  //
  const LHCb::ProtoParticle* pp = p->proto () ;
  if ( 0 == pp ) 
  {
    Error ("LHCb::ProtoParticle* points to NULL, return 'false'") ;
    return false ;                                                   // RETURN
  }
  //
  const LHCb::Track* track = pp->track () ;
  if ( 0 == track ) 
  {
    Error ("LHCb::Track* points to NULL, return 'false'") ;
    return false ;                                                   // RETURN
  }
  //
  return m_cut ( track ) ;                                          // RETURN
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::TrackCut::fillStream ( std::ostream& s ) const 
{ return s << " TRCUT( " << m_cut << " ) " ; }
 

// ============================================================================
// The END 
// ============================================================================
