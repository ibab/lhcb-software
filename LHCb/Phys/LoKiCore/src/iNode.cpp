// $Id: iNode.cpp,v 1.1 2008-06-12 08:14:31 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iNode.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Decays::iNode 
 *  @date 2008-05-04 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================

// ===========================================================================
// iNode 
// ===========================================================================
// destructor
// ===========================================================================
LoKi::Decays::iNode::~iNode(){}
// ===========================================================================


// ===========================================================================
// LoKi::Decays::Node
// ===========================================================================
// constructor from the node
// ===========================================================================
LoKi::Decays::Node::Node
( const LoKi::Decays::iNode& node ) 
  : LoKi::Decays::iNode ()
  , m_node ( 0 ) 
{
  if ( typeid ( LoKi::Decays::Node ) == typeid ( node ) ) 
  {
    const Node& _node = dynamic_cast<const Node&>( node ) ;
    m_node = _node.m_node->clone() ;
  }
  else { m_node = node.clone() ; }
}
// ===========================================================================
// copy constructor 
// ===========================================================================
LoKi::Decays::Node::Node 
( const LoKi::Decays::Node& right ) 
  : LoKi::Decays::iNode ( right )
  , m_node ( right.m_node -> clone() ) 
{}
// ===========================================================================
// MANDATORY: virtual destructor
// ===========================================================================
LoKi::Decays::Node::~Node() { delete m_node ; m_node = 0 ; }
// ===========================================================================
// MANDATORY: the only one essential method
// ===========================================================================
bool LoKi::Decays::Node::operator() 
  ( const LHCb::ParticleID& pid ) const { return node ( pid ) ; }
// ===========================================================================
// MANDATORY: the specific printout
// ===========================================================================
std::ostream& LoKi::Decays::Node::fillStream ( std::ostream& s ) const
{ return m_node -> fillStream ( s ) ; }
// ===========================================================================
// check the validity of the node 
// ===========================================================================
bool LoKi::Decays::Node::valid() const { return m_node -> valid () ; }
// ===========================================================================
// MANDATORY: the proper validation of the node
// ===========================================================================
StatusCode LoKi::Decays::Node::validate ( IParticlePropertySvc* svc ) const 
{ return m_node -> validate ( svc ) ; }
// ===========================================================================
// assignement operator
// ===========================================================================
LoKi::Decays::Node&
LoKi::Decays::Node::operator=
( const LoKi::Decays::Node& right )
{
  if ( &right == this ) { return *this ; }
  //
  LoKi::Decays::iNode* node = right.m_node->clone() ;
  delete m_node ; 
  m_node = node ;
  //
  return *this ;
}
// ===========================================================================
// assignement operator
// ===========================================================================
LoKi::Decays::Node&
LoKi::Decays::Node::operator=
( const LoKi::Decays::iNode& right )
{
  if ( &right == this ) { return *this ; }
  //
  LoKi::Decays::iNode* node = 0 ;
  //
  if ( typeid ( LoKi::Decays::Node ) == typeid ( right ) ) 
  {
    const Node& _node = dynamic_cast<const Node&>( right ) ;
    node = _node.m_node->clone() ;
  }
  else { node = right.clone() ; }
  //
  delete m_node ; 
  m_node = node ;
  //
  return *this ;
}
// ===========================================================================
// MANDATORY: clone method ("virtual constructor")
// ===========================================================================
LoKi::Decays::Node* LoKi::Decays::Node::clone () const 
{ return new LoKi::Decays::Node(*this) ; }


// ============================================================================
// The END 
// ============================================================================

