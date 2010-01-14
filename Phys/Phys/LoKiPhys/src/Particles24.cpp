// $Id: Particles24.cpp,v 1.2 2010-01-14 13:30:16 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Services.h"
#include "LoKi/Particles24.h"
// ============================================================================
/** @file 
 *  Implementation file for file LoKi/Particles24.h
 *  @date 2008-12-17 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor from the actual node 
// ============================================================================
LoKi::Particles::DecNode::DecNode 
( const Decays::iNode& node         , 
  const bool           autovalidate ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate()
  , m_node         ( node         ) 
  , m_autovalidate ( autovalidate )
{}  
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecNode::result_type 
LoKi::Particles::DecNode::operator() 
  ( LoKi::Particles::DecNode::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* point to NULL, return false") ;
    return false ;
  }
  if ( !valid () && m_autovalidate ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::ParticlePropertySvc* poinst to NULL!") ;
    StatusCode sc = validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Node" , sc ) ;
  }
  if ( !valid () )
  {
    Error ( "Decays::Node is invalid, return false") ;
    return false ; 
  }
  
  // use the node for evaluation 
  return m_node.node ( p->particleID() ) ;  
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DecNode::fillStream( std::ostream& s ) const 
{
  if ( !valid() ) { return s << "DECNODE(invalid)" ; }
  return s << "DECNODE( " << m_node << ")";
}
// ============================================================================


// ============================================================================
// constructor from the actual tree
// ============================================================================
LoKi::Particles::DecTree::DecTree
( const LoKi::Particles::DecTree::iTree& tree         , 
  const bool                             autovalidate ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate()
  , m_tree         ( tree         ) 
  , m_autovalidate ( autovalidate ) 
{}  
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecTree::result_type 
LoKi::Particles::DecTree::operator() 
  ( LoKi::Particles::DecTree::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* point to NULL, return false" ) ;
    return false ;
  }
  if ( !valid () && m_autovalidate ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::ParticlePropertySvc* poinst to NULL!") ;
    StatusCode sc = validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Tree" , sc ) ;
  }
  if ( !valid () )
  {
    Error ( "Decays::Tree is invalid, return false") ;
    return false ; 
  }
  // use the node for evaluation 
  return m_tree.tree ( p ) ;  
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DecTree::fillStream( std::ostream& s ) const 
{
  if ( !valid() ) { return s << "DECTREE(invalid)" ; }
  return s << "DECTREE( " << m_tree << ")";
}
// ============================================================================


// ============================================================================
// The END
// ============================================================================
