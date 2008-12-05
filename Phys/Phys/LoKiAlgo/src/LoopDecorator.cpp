// $Id: LoopDecorator.cpp,v 1.5 2008-12-05 10:42:17 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Loop.h"
#include "LoKi/LoopObj.h"
#include "LoKi/LoopDecorator.h"
#include "LoKi/Algo.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace LoKi::Dict::Loop 
 *  @date 2007-06-03 
 *  @author Vanya BELYAEV
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// invalid formula
  const std::vector<std::string>& s_formula()
  {
    static std::vector<std::string> _formula = std::vector<std::string>() ;
    if ( _formula.empty() ) 
    {
      _formula.push_back( "I" ) ;
      _formula.push_back( "n" ) ;
      _formula.push_back( "v" ) ;
      _formula.push_back( "a" ) ;
      _formula.push_back( "l" ) ;
      _formula.push_back( "i" ) ;
      _formula.push_back( "d" ) ;
      _formula.push_back( " " ) ;
      _formula.push_back( "L" ) ;
      _formula.push_back( "o" ) ;
      _formula.push_back( "o" ) ;
      _formula.push_back( "p" ) ;
    }
    return _formula ;
  }
  // ==========================================================================
  /// invalid status code 
  const StatusCode s_BAD = StatusCode::FAILURE ;
  // ==========================================================================
  /// invalid 4-momentum 
  const LoKi::LorentzVector s_vector = LoKi::LorentzVector() ;
  // ==========================================================================
  /// invalid mass
  const double  s_mass = -100 * Gaudi::Units::TeV ;
  // ==========================================================================
  /// invalid PID 
  const LHCb::ParticleID s_PID = LHCb::ParticleID() ;
  // ==========================================================================
  /// invalid PID name 
  const std::string s_pidName = "<Invalid Loop/Particle>" ;
  // ==========================================================================
}
// ============================================================================
// get the effective formula of the loop, see LoKi::LoopObj::formula
// ============================================================================
const LoKi::LoopObj::Formula& 
LoKi::Dicts::DictsLoop::formula( const LoKi::Loop& l ) 
{ return l ? l->formula() : s_formula() ; } 
// ============================================================================
// get the effective dimension of the loop (== number of components)
// ============================================================================
size_t LoKi::Dicts::DictsLoop::dim    ( const LoKi::Loop& l ) 
{ return l ? l->dim  () : 0 ; }
// ============================================================================
// Loop size (inclusing the invalid combinations!)
// ============================================================================
size_t LoKi::Dicts::DictsLoop::size   ( const LoKi::Loop& l ) 
{ return l ? l->size () : 0 ; }      
// ============================================================================
// check the validity of the current combinations
// ============================================================================
bool LoKi::Dicts::DictsLoop::valid  ( const LoKi::Loop& l ) { return l ; }
// ============================================================================
// get the current status of the loop object 
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::status ( const LoKi::Loop& l ) 
{ return l ? l->status() : s_BAD ; }
// ============================================================================
// advance the loop to the next combination
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::next 
( const LoKi::Loop& l ) { return ++l ; }
// ============================================================================
// conversion operator to LHCb::Particle and access to the children:
// ============================================================================
const LHCb::Particle* LoKi::Dicts::DictsLoop::particle 
( const LoKi::Loop& l ) 
{ return l ? l->particle (  ) : 0 ; } 
// ============================================================================
// conversion operator to LHCb::Particle and access to the children:
// ============================================================================
const LHCb::Particle* LoKi::Dicts::DictsLoop::particle 
( const LoKi::Loop& l , 
  const size_t      i ) 
{ return l ? l->particle ( i ) : 0 ; } 
// ============================================================================
// conversion to LHCb::Vertex 
// ============================================================================
const LHCb::VertexBase* LoKi::Dicts::DictsLoop::vertex 
( const LoKi::Loop& l ) { return l ; }
// ============================================================================
//  get the momentum of the particle or daughter 
// ============================================================================
LoKi::LorentzVector LoKi::Dicts::DictsLoop::momentum 
( const LoKi::Loop& l ) 
{ return l ? l->momentum (   ) : s_vector ; }
// ============================================================================
//  get the momentum of the particle or daughter 
// ============================================================================
LoKi::LorentzVector LoKi::Dicts::DictsLoop::momentum 
( const LoKi::Loop& l , 
  const size_t      i ) 
{ return l ? l->momentum ( i ) : s_vector ; }
// ============================================================================
//  get the momentum of the pair of daughters 
// ============================================================================
LoKi::LorentzVector LoKi::Dicts::DictsLoop::momentum 
( const LoKi::Loop& l     , 
  const size_t      i1    , 
  const size_t      i2    ) 
{ return l ? l->momentum ( i1 , i2 ) : s_vector ; }
// ============================================================================
//  get the momentum of the triplet of daughters 
// ============================================================================
LoKi::LorentzVector LoKi::Dicts::DictsLoop::momentum 
( const LoKi::Loop& l     , 
  const size_t      i1    , 
  const size_t      i2    ,
  const size_t      i3    ) 
{ return l ? l->momentum ( i1 , i2 , i3 ) : s_vector ; }
// ============================================================================
//  get the momentum of the 4-daughters 
// ============================================================================
LoKi::LorentzVector LoKi::Dicts::DictsLoop::momentum 
( const LoKi::Loop& l     , 
  const size_t      i1    , 
  const size_t      i2    ,
  const size_t      i3    , 
  const size_t      i4    ) 
{ return l ? l->momentum ( i1 , i2 , i3 , i4 ) : s_vector ; }
// ============================================================================
// get the invariant mass of the particel or child 
// ============================================================================
double LoKi::Dicts::DictsLoop::mass
( const LoKi::Loop& l ) 
{ return l ? l->mass (   ) :  s_mass ; }
// ============================================================================
// get the invariant mass of the particel or child 
// ============================================================================
double LoKi::Dicts::DictsLoop::mass
( const LoKi::Loop& l , 
  const size_t      i ) 
{ return l ? l->mass ( i ) :  s_mass ; }
// ============================================================================
// get the invariant mass of the pair of particles 
// ============================================================================
double LoKi::Dicts::DictsLoop::mass
( const LoKi::Loop& l     , 
  const size_t      i1    , 
  const size_t      i2    ) 
{ return l ? l->mass ( i1 , i2 ) :  s_mass ; }
// ============================================================================
// get the invariant mass of the triplet of particles 
// ============================================================================
double LoKi::Dicts::DictsLoop::mass
( const LoKi::Loop& l     , 
  const size_t      i1    , 
  const size_t      i2    ,
  const size_t      i3    ) 
{ return l ? l->mass ( i1 , i2 , i3 ) :  s_mass ; }
// ============================================================================
// get the invariant mass of the 4-particles 
// ============================================================================
double LoKi::Dicts::DictsLoop::mass
( const LoKi::Loop& l     , 
  const size_t      i1    , 
  const size_t      i2    ,
  const size_t      i3    , 
  const size_t      i4    ) 
{ return l ? l->mass ( i1 , i2 , i3 , i4 ) :  s_mass ; }
// ============================================================================
// get the effective PID 
// ============================================================================
LHCb::ParticleID LoKi::Dicts::DictsLoop::pid( const LoKi::Loop& l ) 
{ return l ? l->pid() : s_PID ; }
// ============================================================================
// get PID for the effective particle of the loop
// ============================================================================
const LHCb::ParticleProperty* LoKi::Dicts::DictsLoop::pp ( const LoKi::Loop& l ) 
{ return l ? l->pp() : 0 ; }
// ============================================================================
// get PID for the effective particle of the loop
// ============================================================================
const std::string& LoKi::Dicts::DictsLoop::pidName ( const LoKi::Loop& l ) 
{ return l ? l->pidName() : s_pidName ; }
// ============================================================================
// set the particle ID for the effective particle of the loop 
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::setPID 
( const LoKi::Loop&       l   , 
  const LHCb::ParticleID& pid ) 
{
  if ( l ) { l->setPID( pid ) ; }
  return l ;
} 
// ============================================================================
/// set the particle ID for the effectiev particle of the loop 
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::setPID 
( const LoKi::Loop&       l   , 
  const std::string&      pid ) 
{
  if ( l ) { l->setPID( pid ) ; }
  return l ;
} 
// ============================================================================
// set the particle ID for the effectiev particle of the loop 
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::setPID 
( const LoKi::Loop&             l   , 
  const LHCb::ParticleProperty* pid ) 
{
  if ( l ) { l->setPID( pid ) ; }
  return l ;
} 
// ============================================================================
// set the default IParticleCombiner tool
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::setCombiner 
( const LoKi::Loop& l      , 
  const IParticleCombiner* c ) 
{
  if ( l ) { l->setCombiner ( c ) ; }
  return l ;
} 
// ============================================================================
// set the default IParticleCombiner tool
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::setReFitter 
( const LoKi::Loop&        l , 
  const IParticleReFitter* c ) 
{
  if ( l ) { l->setReFitter ( c ) ; }
  return l ;
}
// ============================================================================
// get the associated vertex 
// ============================================================================
const LHCb::VertexBase* LoKi::Dicts::DictsLoop::pv  ( const LoKi::Loop& l ) 
{ return l ? l->pv() : 0 ; }
// ============================================================================
// set the associated vertex 
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::setPV 
( const LoKi::Loop&       l , 
  const LHCb::VertexBase* v ) 
{
  if ( l ) { l->setPV ( v ) ; }
  return l ;
} 
// ============================================================================
// make 'effective' particle using IParticleCombiner tool
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::make  
( const LoKi::Loop&        l    ) 
{ return l ? l->make (      ) : s_BAD ; }
// ============================================================================
// make 'effective' particle using IParticleCombiner tool
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::make  
( const LoKi::Loop&        l    , 
  const IParticleCombiner* comb ) 
{ return l ? l->make ( comb ) : s_BAD ; }
// ============================================================================
// make 'effective' particle using IParticleCombiner tool
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::make  
( const LoKi::Loop&  l    , 
  const std::string& nick ) 
{ return l ? l->make ( nick ) : s_BAD ; }
// ============================================================================
// refit the particle using IParticleReFitter tool
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::reFit 
( const LoKi::Loop&        l   ) 
{ return l ? l->reFit (  ) : s_BAD ; }
// ============================================================================
// refit the particle using IParticleReFitter tool
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::reFit 
( const LoKi::Loop&        l   , 
  const IParticleReFitter* fit ) 
{ return l ? l->reFit ( fit ) : s_BAD ; }
// ============================================================================
// refit the particle using IParticleReFitter tool
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::reFit 
( const LoKi::Loop&  l    , 
  const std::string& nick ) 
{ return l ? l->reFit ( nick ) : s_BAD ; }
// ============================================================================
// save the particle into LoKi storage
// ============================================================================
StatusCode LoKi::Dicts::DictsLoop::save  
( const LoKi::Loop&  l   , 
  const std::string& tag ) 
{ return l ? l->save ( tag ) : s_BAD ; }
// ============================================================================
// backup the current state of the loop 
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::backup  ( const LoKi::Loop& l ) 
{
  if ( l ) { l -> backup () ; }
  return l ; 
} 
// ============================================================================
// restore the loop from the last backup state
// ============================================================================
const LoKi::Loop& LoKi::Dicts::DictsLoop::restore ( const LoKi::Loop& l ) 
{
  if ( l ) { l -> restore () ; }
  return l ; 
} 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
