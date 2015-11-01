// $Id: $
// Include files

// STL
#include <math.h>

// from Gaudi
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/IAlgContextSvc.h"

// Loki
#include "LoKi/IDecay.h"
#include "LoKi/Decays.h"
#include "LoKi/ParticleProperties.h"

#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/DecayTree.h"

// local
#include "SubstitutePIDTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SubstitutePIDTool
//
// 2011-12-07 : Patrick Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SubstitutePIDTool::SubstitutePIDTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_map  ()
  , m_subs ()
  , m_to_be_updated1( false )
  , m_initialized( false )
{
  declareInterface<ISubstitutePID>(this);
  declareProperty
    ( "Substitutions" ,
      m_map           ,
      "PID-substitutions :  { ' decay-component' : 'new-pid' }" )
    -> declareUpdateHandler ( &SubstitutePIDTool::updateHandler , this ) ;
}

//=============================================================================
// Destructor
//=============================================================================
SubstitutePIDTool::~SubstitutePIDTool() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode SubstitutePIDTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "Initialize" << endmsg;
  if ( "ToolSvc.SubstitutePIDTool" == name() )
  {
    return Error("Running this tool as a public tool without defining an instance name is dangerous. Quitting.") ;
  }
  sc = decodeCode(m_map) ; /// try if it's non empty
  m_initialized = true ;
  return sc;
}

// ============================================================================
// decode the code
// ============================================================================
StatusCode SubstitutePIDTool::decodeCode( const SubstitutionMap& newMap )
{
  //
  // decode "substitutions"
  //
  if (msgLevel(MSG::DEBUG))
    debug() << "decodeCode newMap " << newMap << endmsg ;
  if ( newMap.empty() )
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "passed an empty map. decodeCode will do nothing" << endmsg ;
    if ( m_map.empty() && m_initialized )
    {
      return Error ( "Empty 'Substitute' map passed to decodeCode" );
    }
    else 
    {
      return StatusCode::SUCCESS ;
    }
  }
  else 
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "passed a non empty map. decodeCode will update." << endmsg ;
    m_map = newMap ;
  }
  if ( m_map.empty() )
  {
    // could happen if tool is configured directly
    return Error ( "Empty 'Substitute' map" ) ;
  }
  //
  if (msgLevel(MSG::DEBUG)) debug() << "decodeCode map " << m_map << endmsg ;
  // get the factory
  Decays::IDecay* factory = tool<Decays::IDecay>( "LoKi::Decay" );
  //
  m_subs.clear() ;
  LHCb::IParticlePropertySvc* ppSvc =
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);
  for ( SubstitutionMap::const_iterator item = m_map.begin() ;
        m_map.end() != item ; ++item )
  {
    /// construct the tree
    if (msgLevel(MSG::DEBUG))
      debug() << "* -> Map: " << item->first << " : " << item->second << endmsg ;
    Decays::IDecay::Tree tree = factory->tree ( item->first ) ;
    if ( !tree  )
    {
      StatusCode sc = tree.validate ( ppSvc ) ;
      if ( sc.isFailure() )
      {
        return Error ( "Unable to validate the tree '" +
                       tree.toString() + "' built from the descriptor '"
                       + item->first   + "'" , sc ) ;
      }
    }
    // get ParticleID
    const LHCb::ParticleProperty* pp = ppSvc->find ( item->second ) ;
    if ( !pp )
    { return Error ( "Unable to find ParticleID for '" + item->second + "'" ) ; }
    //
    if (msgLevel(MSG::DEBUG))
      debug() << "* -> Inserting " << tree
              << " in map for PID " <<  pp->particleID() << endmsg ;
    //
    m_subs.push_back ( Substitution( tree , pp->particleID() ) ) ;
  }
  //
  if ( m_subs.size() != m_map.size() )
  { return Error("Mismatch in decoded substitution container") ; }
  if (msgLevel(MSG::DEBUG))
  {
    debug() << "ISub Size: " << m_subs.size() << " " << m_map.size() << endmsg ;
    for ( Substitutions::iterator isub = m_subs.begin() ;
          m_subs.end() != isub ; ++isub )
    {
      debug() << "* -> ISub: " << isub -> m_pid
              << " Size: " << m_subs.size() << endmsg ;
    }
  }
  //
  m_to_be_updated1 = false ;
  // release service
  release(ppSvc);
  return StatusCode::SUCCESS ;
}

// ============================================================================
// loop over particles
// ============================================================================
StatusCode
SubstitutePIDTool::substitute( const LHCb::Particle::ConstVector& input,
                               LHCb::Particle::ConstVector& output )
{
  //
  // substitute
  //
  if (m_to_be_updated1) decodeCode(m_map);
  StatEntity& cnt = counter("#substituted") ;
  for ( LHCb::Particle::ConstVector::const_iterator ip =
          input.begin() ; input.end() != ip ; ++ip )
  {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { continue ; }
    //
    // clone the whole decay tree
    LHCb::DecayTree tree ( *p ) ;
    //
    cnt += substitute ( tree.head() ) ;
    output.push_back ( tree.release() ) ;
  }

  return StatusCode::SUCCESS ;

}

// ============================================================================
// perform the actual substitution
// ============================================================================
unsigned int SubstitutePIDTool::substitute ( LHCb::Particle* p )
{
  if ( 0 == p ) { return 0 ; }
  //
  unsigned int substituted = 0 ;
  //
  for ( Substitutions::iterator isub = m_subs.begin() ;
        m_subs.end() != isub ; ++isub )
  {
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "ISub: " << isub -> m_pid << " Size: " << m_subs.size() 
              << endmsg ;
    LHCb::Particle** _p = &p ;
    //
    LHCb::Particle::ConstVector found ;
    if ( 0 == isub->m_finder.findDecay ( _p , _p + 1 , found ) ) { continue ; }
    //
    for ( LHCb::Particle::ConstVector::const_iterator ip = found.begin() ;
          found.end() != ip ; ++ip )
    {
      const LHCb::Particle* pf = *ip ;
      int pid = pf->particleID().pid() ;
      if ( 0 == pf  ) { continue ; }
      LHCb::Particle* pf_ = const_cast<LHCb::Particle*>( pf ) ;
      if ( 0 == pf_ ) { continue ; }
      pf_ ->setParticleID ( isub -> m_pid ) ;
      //
      if (msgLevel(MSG::DEBUG)) debug() << "Substituted a " << pid << " by a "
                                        << pf_->particleID().pid() << endmsg ;
      ++substituted    ;
      ++(isub->m_used) ;
    }
  }
  //
  if ( 0 < substituted ) { correctP4( p ) ; }
  //
  return substituted ;
  //
}

// ============================================================================
// perform the recursive 4-momentum correction
// ============================================================================
unsigned int SubstitutePIDTool::correctP4 ( LHCb::Particle* p )
{
  if ( 0 == p ) { return 0 ; } // RETURN
  //
  if ( p->isBasicParticle () )
  {
    const Gaudi::LorentzVector& oldMom = p->momentum() ;
    //
    const double newMass   = LoKi::Particles::massFromPID ( p->particleID() ) ;
    const double newEnergy = std::sqrt ( oldMom.P2() + newMass*newMass ) ;
    //
    p -> setMomentum( Gaudi::LorentzVector( oldMom.Px() ,
                                            oldMom.Py() ,
                                            oldMom.Pz() , 
                                            newEnergy  ) );
    p -> setMeasuredMass( newMass );
    return 1 ;
  }
  //
  typedef SmartRefVector<LHCb::Particle> DAUGHTERS ;
  const DAUGHTERS& daughters = p->daughters() ;
  //
  unsigned int num = 0 ;
  //
  Gaudi::LorentzVector sum ;
  for ( DAUGHTERS::const_iterator idau = daughters.begin() ;
        daughters.end() != idau ; ++idau )
  {
    const LHCb::Particle* dau = *idau ;
    if ( 0 == dau ) { continue ; }                // CONTINUE
    //
    num += correctP4 ( const_cast<LHCb::Particle*> ( dau ) ) ;
    sum += dau->momentum() ;
  }
  //
  if ( 0 != num )
  {
    p -> setMomentum     ( sum     ) ;
    p -> setMeasuredMass ( sum.M() ) ;
  }
  //
  return num ;
}

//=============================================================================
void SubstitutePIDTool::updateHandler ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  m_to_be_updated1 = true ;
  /// mark as "to-be-updated"
  Warning ( "The structural property '" + p.name() +
            "' is updated. It will take effect at the next call" ,
            StatusCode( StatusCode::SUCCESS, true ) ) ;
  if ( msgLevel(MSG::DEBUG) )
    debug () << "The updated property is: " << p << endmsg ;
}

//=============================================================================
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SubstitutePIDTool )
//=============================================================================
