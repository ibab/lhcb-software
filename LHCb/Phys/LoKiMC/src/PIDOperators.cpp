// $Id: PIDOperators.cpp,v 1.7 2010-04-06 20:14:39 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/MCParticles.h"
#include "LoKi/MCPIDOperators.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from the file LoKi/MCPIDOperators.h
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
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&              pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&                    pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&                    pid  ,
            const LoKi::MCParticles::Identifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&                    pid  ,
            const LoKi::MCParticles::Identifier& id   )
{ return id != pid ; } 
// ============================================================================

// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================

// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                     pid  )
{ 
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&                       pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&                       pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id != pid ; } 
// ============================================================================

// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================



namespace 
{
  typedef LoKi::EqualToList<const LHCb::MCParticle*>       InList ;
  typedef LoKi::NotEqualToList<const LHCb::MCParticle*> NotInList ;
}

// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) 
{
  return InList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) 
{
  return InList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) 
{
  std::vector<int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.push_back ( id.pid() ) ;
  }
  //
  return InList ( id , _pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) 
{
  std::vector<int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.push_back ( ipid->pid() ) ; }
  //      
  return InList ( id , _pids ) ;
}  
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) 
{
  std::vector<unsigned int> _ipids ;
  //
  for ( std::vector<int>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {
    if ( 0 <= *ipid ) { _ipids.push_back (  (*ipid) ) ; }
    else              { _ipids.push_back ( -(*ipid) ) ; }
  }
  //
  return InList ( id , _ipids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) 
{
  return InList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) 
{
  std::vector<unsigned int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.push_back ( id.abspid() ) ;
  }
  //
  return InList ( id , _pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) 
{
  std::vector<unsigned int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.push_back ( ipid->abspid() ) ; }
  //      
  return InList ( id , _pids ) ;
}  
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) 
{
  return NotInList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) 
{
  return NotInList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) 
{
  std::vector<int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.push_back ( id.pid() ) ;
  }
  //
  return NotInList ( id , _pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) 
{
  std::vector<int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.push_back ( ipid->pid() ) ; }
  //      
  return NotInList ( id , _pids ) ;
}  
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) 
{
  std::vector<unsigned int> _ipids ;
  //
  for ( std::vector<int>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {
    if ( 0 <= *ipid ) { _ipids.push_back (  (*ipid) ) ; }
    else              { _ipids.push_back ( -(*ipid) ) ; }
  }
  //
  return NotInList ( id , _ipids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) 
{
  return NotInList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) 
{
  std::vector<unsigned int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.push_back ( id.abspid() ) ;
  }
  //
  return NotInList ( id , _pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) 
{
  std::vector<unsigned int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.push_back ( ipid->abspid() ) ; }
  //      
  return NotInList ( id , _pids ) ;
}  
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
