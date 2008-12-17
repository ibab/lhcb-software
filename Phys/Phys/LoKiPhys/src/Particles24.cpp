// $Id: Particles24.cpp,v 1.1 2008-12-17 16:53:20 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
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
( const Decays::iNode& node ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate()
  , m_node ( node ) 
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
( const LoKi::Particles::DecTree::iTree& tree ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate()
  , m_tree ( tree ) 
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
    Error ( "LHCb::Particle* point to NULL, return false") ;
    return false ;
  }
  if ( !valid() ) 
  {
    Error ( "LoKi::Particles::DecTree::Tree  is invalid, return false") ;
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
