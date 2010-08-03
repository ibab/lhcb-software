// $Id: Particles24.cpp,v 1.3 2010-05-31 20:36:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Nodes.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IDecayNode.h"
#include "LoKi/IDecay.h"
#include "LoKi/Services.h"
#include "LoKi/Particles24.h"
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
// ============================================================================
/** @file 
 *  Implementation file for file LoKi/Particles24.h
 *  @date 2008-12-17 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
namespace 
{ 
  // ==========================================================================
  /// invalid Node 
  const Decays::Nodes::Invalid                                      s_NODE ;
  /// invalid decay
  const Decays::Trees::Types_<const LHCb::Particle*>::Invalid       s_TREE ;
  /// "Factory"
  const std::string  s_FACTORY = "LoKi::Decay" ;
  // ==========================================================================
}
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
// constructor from the actual node 
// ============================================================================
LoKi::Particles::DecNode::DecNode 
( const std::string& node )  
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate()
  , m_node         ( s_NODE ) 
  , m_autovalidate ( true   )
{
  
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to aquire IToolSvc tool" ) ;
  
  Decays::IDecayNode* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( s_FACTORY , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + s_FACTORY + "'" , sc ) ; 
  Assert ( 0 != tool  , "Decays::IDecayNode* points to NULL" ) ; 
  //                                            
  m_node   = tool -> node ( node ) ;
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore 
  //
  Assert ( !(!m_node) , "The node is invalid:  '" + node + "'" ) ;   
  //
}
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
// constructor from the actual node 
// ============================================================================
LoKi::Particles::DecTree::DecTree 
( const std::string& tree )  
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate()
  , m_tree         ( s_TREE ) 
  , m_autovalidate ( true   )
{
  
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to aquire IToolSvc tool" ) ;
  
  Decays::IDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( s_FACTORY , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + s_FACTORY + "'" , sc ) ; 
  Assert ( 0 != tool  , "Decays::IDecay* points to NULL" ) ; 
  //                                            
  m_tree   = tool -> tree ( tree ) ;
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore 
  //
  Assert ( !(!m_tree)       , "The tree is invalid : '" + tree + "'" ) ;   
  Assert ( !m_tree.marked() , "The tree is marked  : '" + tree + "'" ) ;
  //
}
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
