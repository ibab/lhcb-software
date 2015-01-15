// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Nodes.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
#include "LoKi/MCTypes.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Services.h"
#include "LoKi/MCParticles1.h"
#include "LoKi/IMCDecay.h"
// ============================================================================
namespace 
{ 
  // ==========================================================================
  /// invalid decay
  const Decays::Trees::Types_<const LHCb::MCParticle*>::Invalid     s_TREE =
        Decays::Trees::Types_<const LHCb::MCParticle*>::Invalid();
  /// "Factory"
  const std::string  s_FACTORY = "LoKi::MCDecay" ;
  // ==========================================================================
}
// ============================================================================
// constructor from the actual node
// ============================================================================
LoKi::MCParticles::DecNode::DecNode
( const Decays::iNode& node )
  : LoKi::AuxFunBase ( std::tie ( node ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_node   ( node )
  , m_string (      ) 
{
  if ( gaudi() && !valid() ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::IParticlePropertySvc* points to NULL!") ;
    StatusCode sc = m_node.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Node" , sc ) ;
  } 
}
// ============================================================================
// constructor from the actual node
// ============================================================================
LoKi::MCParticles::DecNode::DecNode
( const std::string& node )
  : LoKi::AuxFunBase ( std::tie ( node ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_node   ( Decays::Nodes::Invalid() )
  , m_string ( node ) 
{
  if ( gaudi() ) { getNode() ; }
  if ( gaudi() && !valid() ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::IParticlePropertySvc* points to NULL!") ;
    StatusCode sc = m_node.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Node" , sc ) ;
  }
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::DecNode::result_type
LoKi::MCParticles::DecNode::operator()
  ( LoKi::MCParticles::DecNode::argument p ) const
{
  //
  if ( 0 == p )
  {
    Error ( "LHCb::MCParticle* point to NULL, return false") ;
    return false ;
  }
  //
  if ( !valid () && !m_string.empty() ) { getNode() ; }
  //
  if ( !valid () ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::IParticlePropertySvc* points to NULL!") ;
    StatusCode sc = m_node.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate Decays::Node" , sc ) ;
  }
  // use the node for evaluation
  return m_node.node ( p->particleID() ) ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::MCParticles::DecNode::fillStream( std::ostream& s ) const
{
  if ( !valid() ) { return s << " MCDECNODE(<invalid>) " ; }
  return s << " MCDECNODE( " << m_node << " ) ";
}
// ============================================================================
void LoKi::MCParticles::DecNode::getNode() const
{
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to aquire IToolSvc tool" ) ;
  //
  Decays::IDecayNode* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( s_FACTORY , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + s_FACTORY + "'" , sc ) ; 
  Assert ( 0 != tool  , "Decays::IDecayNode* points to NULL" ) ; 
  //
  m_node   = tool -> node ( m_string ) ;
  //
  Assert ( m_node.valid() , "The node is invalid : '" + m_string + "'" ) ;   
}
// ============================================================================
// constructor from the actual tree
// ============================================================================
LoKi::MCParticles::DecTree::DecTree
( const LoKi::MCParticles::DecTree::iTree& tree )
  : LoKi::AuxFunBase  ( std::tie ( tree ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_tree   ( tree )
  , m_string (      ) 
{
  if ( gaudi() && !valid () ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::IParticlePropertySvc* points to NULL!") ;
    StatusCode sc = m_tree.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate iTree" , sc ) ;
  }
}
// ============================================================================
// constructor from the actual tree
// ============================================================================
LoKi::MCParticles::DecTree::DecTree
( const std::string& tree )
  : LoKi::AuxFunBase  ( std::tie ( tree ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_tree   ( s_TREE )
  , m_string ( tree   ) 
{
  if ( gaudi() ) { getTree() ; }
  if ( gaudi() && !valid () ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::IParticlePropertySvc* points to NULL!") ;
    StatusCode sc = m_tree.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate iTree" , sc ) ;
  }  
}
// ============================================================================
void LoKi::MCParticles::DecTree::getTree() const
{
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to aquire IToolSvc tool" ) ;
  //
  Decays::IMCDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( s_FACTORY , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + s_FACTORY + "'" , sc ) ; 
  Assert ( 0 != tool  , "Decays::IDecay* points to NULL" ) ; 
  //                                            
  m_tree   = tool -> tree ( m_string ) ;
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore 
  //
  Assert ( !(!m_tree)       , "The tree is invalid : '" + m_string + "'" ) ;   
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::DecTree::result_type
LoKi::MCParticles::DecTree::operator()
  ( LoKi::MCParticles::DecTree::argument p ) const
{
  if ( 0 == p )
  {
    Error ( "LHCb::MCParticle* point to NULL, return false") ;
    return false ;
  }
  //
  if ( !valid () && !m_string.empty() ) { getTree() ; }
  if ( !valid () ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance () ;
    const LHCb::IParticlePropertySvc* ppsvc  = svcs.ppSvc() ;
    Assert ( 0 != ppsvc , "LHCb::IParticlePropertySvc* points to NULL!") ;
    StatusCode sc = m_tree.validate ( ppsvc ) ;
    Assert ( sc.isSuccess() , "Unable to validate iTree" , sc ) ;
  }
  //
  const bool marked_ = m_tree.marked() ;
  //
  // evaluate the tree 
  const bool OK = m_tree.tree ( p ) ;
  if ( OK && !marked_  ) { return true  ; }                          // RETURN
  
  // for no-marked trees, the search is completed 
  if ( !marked_        ) { return false ; }                          // RETURN  
  
  // check for "marked" elements 
  iTree::Collection marked ;
  
  // loop over all "mother" particles 
  const LHCb::MCParticle* mother = p->mother() ;
  while ( 0 != mother ) 
  {
    reset () ;
    // check if mother matches the tree 
    if  ( m_tree.tree ( mother ) ) 
    {
      // collect the marked elements 
      marked.clear() ;
      m_tree.collect ( marked ) ;
      // look for particle in the list of marked particles 
      if ( marked.end() != std::find 
           ( marked.begin() , marked.end() , p ) ) { return true ; } // RETURN
    }
    // go to the next level 
    mother = mother->mother() ;                                // NEXT LEVEL 
  }
  //
  return false ;                                                      // RETURN 
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::MCParticles::DecTree::fillStream( std::ostream& s ) const
{
  if ( !valid() ) { return s << " MCDECTREE(<invalid>) " ; }
  return s << " MCDECTREE( " << m_tree << " ) ";
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
