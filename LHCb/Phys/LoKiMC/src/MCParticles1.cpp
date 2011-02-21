// $Id: MCParticles1.cpp,v 1.2 2010-06-05 20:15:58 ibelyaev Exp $
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
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_node ( node )
{}
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


// ============================================================================
// constructor from the actual tree
// ============================================================================
LoKi::MCParticles::DecTree::DecTree
( const LoKi::MCParticles::DecTree::iTree& tree )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_tree ( tree )
{}
// ============================================================================
// constructor from the actual tree
// ============================================================================
LoKi::MCParticles::DecTree::DecTree
( const std::string& tree )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_tree ( s_TREE )
{   
  LoKi::ILoKiSvc* ls = lokiSvc() ;
  SmartIF<IToolSvc> toolSvc ( ls ) ;
  Assert ( !(!toolSvc) , "Unable to aquire IToolSvc tool" ) ;
  
  Decays::IMCDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( s_FACTORY , tool ) ;
  Assert ( sc.isSuccess () , "Unable to retrieve '" + s_FACTORY + "'" , sc ) ; 
  Assert ( 0 != tool  , "Decays::IDecay* points to NULL" ) ; 
  //                                            
  m_tree   = tool -> tree ( tree ) ;
  toolSvc -> releaseTool ( tool ) ; // do not need the tool anymore 
  //
  Assert ( !(!m_tree)       , "The tree is invalid : '" + tree + "'" ) ;   
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
