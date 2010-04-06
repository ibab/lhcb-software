// $Id: GenPIDOperators.cpp,v 1.4 2010-04-06 20:11:52 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// ST D& STL
// ============================================================================
#include <set>
// ============================================================================
// Local 
// ============================================================================
#include "LoKi/GenParticles.h"
#include "LoKi/GenPIDOperators.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from the file LoKi/GenPIDOperators.h
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
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  )
{
  typedef LoKi::GenTypes::GFunc Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
}
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  )
{
  typedef LoKi::GenTypes::GFunc Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
}
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   )
{ return id != pid ; } 
// ============================================================================

// ============================================================================
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator==( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================

// ============================================================================
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::GenTypes::GFunc Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
}
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::GenTypes::GFunc Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
}
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id != pid ; } 
// ============================================================================

// ============================================================================
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator==( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================

namespace 
{
  typedef LoKi::EqualToList<const HepMC::GenParticle*>       InList ;
  typedef LoKi::NotEqualToList<const HepMC::GenParticle*> NotInList ;
}

// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) 
{
  return InList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) 
{
  return InList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) 
{
  std::set<int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.insert ( id.pid() ) ;
  }
  //
  return InList ( id , _pids.begin() , _pids.end() ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) 
{
  std::set<int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.insert( ipid->pid() ) ; }
  //      
  return InList ( id , _pids.begin() , _pids.end() ) ;
}  
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) 
{
  //
  std::set<unsigned int> _set ;
  for ( std::vector<int>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {
    if ( 0 <= *ipid ) { _set.insert (  (*ipid) ) ; }
    else              { _set.insert ( -(*ipid) ) ; }
  }
  //
  return InList ( id , _set.begin() , _set.end() ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) 
{
  return InList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) 
{
  //
  std::set<unsigned int> _set ;
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _set.insert ( id.abspid() ) ;
  }
  //
  return InList ( id , _set.begin() , _set.end() ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) 
{
  //
  std::set<unsigned int> _set ;
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _set.insert ( ipid->abspid() ) ; }
  //      
  return InList ( id , _set.begin() , _set.end() ) ;
}  
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) 
{
  return NotInList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) 
{
  return NotInList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) 
{
  std::set<int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.insert( id.pid() ) ;
  }
  //
  return NotInList ( id , _pids.begin() , _pids.end() ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const std::vector<LHCb::ParticleID>&  pids ) 
{
  std::set<int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.insert ( ipid->pid() ) ; }
  //      
  return NotInList ( id , _pids.begin() , _pids.end() ) ;
}  
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                    pids ) 
{
  std::set<unsigned int> _ipids ;
  //
  for ( std::vector<int>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {
    if ( 0 <= *ipid ) { _ipids.insert (  (*ipid) ) ; }
    else              { _ipids.insert ( -(*ipid) ) ; }
  }
  //
  return NotInList ( id , _ipids.begin() , _ipids.end() ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) 
{
  return NotInList ( id , pids ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) 
{
  std::set<unsigned int> _pids ;
  //
  for ( std::vector<std::string>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  {    
    LHCb::ParticleID id = LoKi::Particles::pidFromName ( *ipid ) ;
    _pids.insert ( id.abspid() ) ;
  }
  //
  return NotInList ( id , _pids.begin() , _pids.end() ) ;
}
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) 
{
  std::set<unsigned int> _pids ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = pids.begin() ; 
        pids.end() != ipid ; ++ipid ) 
  { _pids.insert ( ipid->abspid() ) ; }
  //      
  return NotInList ( id , _pids.begin() , _pids.end() ) ;
}  
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
