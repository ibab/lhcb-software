// $Id: Nodes.cpp,v 1.6 2009-05-11 15:49:29 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
// STD & STL
// ============================================================================
#include <functional>
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/Nodes.h"
// ============================================================================
/** @file 
 *  The implementation file for various decay nodes 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
// ============================================================================
namespace 
{
  // =========================================================================
  /** @struct _Pid
   *  helper structure to match pids 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   */
  struct _Pid : public std::unary_function<Decays::Nodes::_Node,bool>
  {
  public:
    // ========================================================================
    /// constructor 
    _Pid ( const LHCb::ParticleID& pid ) : m_pid ( pid ) {}
    /// the only one imporant operator 
    inline bool operator () ( const Decays::Nodes::_Node& node ) const 
    { return node  ( m_pid ) ; }  
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    _Pid ();
    // the pid itself 
    LHCb::ParticleID m_pid ; 
    // ========================================================================
  };
  // ==========================================================================
}
// Invalid 
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
Decays::Nodes::_Node::Invalid::~Invalid() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::_Node ::Invalid*
Decays::Nodes::_Node ::Invalid::clone() const 
{ return new Invalid ( *this ) ; }
// ============================================================================
// MANDATORY: check the validity
// ============================================================================
bool Decays::Nodes::_Node::Invalid::valid() const { return false ; }
// ============================================================================
// MANDATORY: the proper validation of the node
// ============================================================================
StatusCode Decays::Nodes::_Node::Invalid::validate 
( const LHCb::IParticlePropertySvc* /*svc */ )  const 
{ return StatusCode ( StatusCode::FAILURE , true ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Nodes::_Node::Invalid::operator() 
  ( const LHCb::ParticleID& /* p */ ) const { return false; }
// ============================================================================
std::ostream& 
Decays::Nodes::_Node::Invalid::fillStream ( std::ostream& s ) const
{ return s << " X0 "  ; }
// ============================================================================

// ============================================================================
// the default constructor 
// ============================================================================
Decays::Nodes::_Node::_Node ()  : m_node ( Invalid() ) {}
// ============================================================================



// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
Decays::Nodes::Or       :: ~Or       () {}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
Decays::Nodes::And      :: ~And      () {}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
Decays::Nodes::Not      :: ~Not      () {}
// ============================================================================


// ===========================================================================
// Or
// ===========================================================================
// constructor from two nodes 
// ===========================================================================
Decays::Nodes::Or::Or 
( const Decays::iNode& n1 , 
  const Decays::iNode& n2 ) 
  : Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
}
// ===========================================================================
// constructor from three nodes 
// ===========================================================================
Decays::Nodes::Or::Or 
( const Decays::iNode& n1 , 
  const Decays::iNode& n2 ,
  const Decays::iNode& n3 ) 
  : Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
  m_nodes.push_back ( n3 ) ;  
}
// ===========================================================================
// constructor from four nodes 
// ===========================================================================
Decays::Nodes::Or::Or 
( const Decays::iNode& n1 , 
  const Decays::iNode& n2 ,
  const Decays::iNode& n3 , 
  const Decays::iNode& n4 ) 
  : Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
  m_nodes.push_back ( n3 ) ;  
  m_nodes.push_back ( n4 ) ;  
}
// ===========================================================================
// constructor formn list of nodes
// ===========================================================================
Decays::Nodes::Or::Or 
( const Decays::SubNodes& nodes ) 
  : Decays::iNode () 
  , m_nodes ( nodes )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Or*
Decays::Nodes::Or::clone() const { return new Or(*this) ; }
// ===========================================================================
// MANDATORY: the only one essential method
// ===========================================================================
bool Decays::Nodes::Or::operator() ( const LHCb::ParticleID& pid ) const 
{ return m_nodes.end() != std::find_if  
    ( m_nodes.begin() , m_nodes.end() , _Pid ( pid ) ) ; }
// ===========================================================================
// MANDATORY: the specific printout
// ===========================================================================
std::ostream& Decays::Nodes::Or::fillStream ( std::ostream& s ) const 
{
  s << " (" ;
  for ( Decays::SubNodes::const_iterator node = m_nodes.begin() ;
        m_nodes.end() != node ; ++node ) 
  {
    if ( m_nodes.begin() != node ) { s << "|" ; }    
    s << *node ;
  }  
  return s << ") " ;
}
// ===========================================================================
// MANDATORY: check the validity 
// ===========================================================================
bool Decays::Nodes::Or::valid () const
{ return !m_nodes.empty() && Decays::valid ( m_nodes.begin() , m_nodes.end() ) ; }
// ===========================================================================
// MANDATORY: the proper validation of the node
// ===========================================================================
StatusCode Decays::Nodes::Or::validate 
( const LHCb::IParticlePropertySvc* svc ) const
{
  if ( m_nodes.empty() ) { return StatusCode::FAILURE ; }
  return Decays::validate ( m_nodes.begin() , m_nodes.end() , svc ) ;
}
// ===========================================================================



// ===========================================================================
// And
// ===========================================================================
// constructor from two nodes 
// ===========================================================================
Decays::Nodes::And::And
( const Decays::iNode& n1 , 
  const Decays::iNode& n2 ) 
  : Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
}
// ===========================================================================
// constructor from three nodes 
// ===========================================================================
Decays::Nodes::And::And
( const Decays::iNode& n1 , 
  const Decays::iNode& n2 ,
  const Decays::iNode& n3 ) 
  :  Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
  m_nodes.push_back ( n3 ) ;  
}
// ===========================================================================
// constructor from four nodes 
// ===========================================================================
Decays::Nodes::And::And
( const Decays::iNode& n1 , 
  const Decays::iNode& n2 ,
  const Decays::iNode& n3 , 
  const Decays::iNode& n4 ) 
  : Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
  m_nodes.push_back ( n3 ) ;  
  m_nodes.push_back ( n4 ) ;  
}
// ===========================================================================
// constructor form list of nodes
// ===========================================================================
Decays::Nodes::And::And
( const Decays::SubNodes& nodes ) 
  : Decays::iNode () 
  , m_nodes ( nodes )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::And*
Decays::Nodes::And::clone() const { return new And(*this) ; }
// ===========================================================================
// MANDATORY: the only one essential method
// ===========================================================================
bool Decays::Nodes::And::operator() ( const LHCb::ParticleID& pid ) const 
{
  if ( m_nodes.empty() ) { return false ; }
  for ( Decays::SubNodes::const_iterator node = m_nodes.begin() ;
        m_nodes.end() != node ; ++node ) 
  { if ( (*node != pid ) ) { return false; } }
  return true ;
}
// ===========================================================================
// MANDATORY: the specific printout
// ===========================================================================
std::ostream& Decays::Nodes::And::fillStream ( std::ostream& s ) const 
{
  s << " (" ;
  for ( Decays::SubNodes::const_iterator node = m_nodes.begin() ;
        m_nodes.end() != node ; ++node ) 
  {
    if ( m_nodes.begin() != node ) { s << "&" ; }    
    s << *node ;
  }  
  return s << ") " ;
}
// ===========================================================================
// MANDATORY: check the validity 
// ===========================================================================
bool Decays::Nodes::And::valid () const
{ return !m_nodes.empty() && Decays::valid ( m_nodes.begin() , m_nodes.end() ) ; }
// ===========================================================================
// MANDATORY: the proper validation of the node
// ===========================================================================
StatusCode Decays::Nodes::And::validate 
( const LHCb::IParticlePropertySvc* svc ) const
{
  if ( m_nodes.empty() ) { return StatusCode::FAILURE ; }
  return Decays::validate ( m_nodes.begin() , m_nodes.end() , svc ) ;
}
// ===========================================================================




// ===========================================================================
// constructor from the node 
// ===========================================================================
Decays::Nodes::Not::Not ( const Decays::iNode& node ) 
  : Decays::iNode ( ) 
  , m_node ( node )  
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Not*
Decays::Nodes::Not::clone() const { return new Not(*this) ; }
// ============================================================================
// valid ? 
// ============================================================================
bool Decays::Nodes::Not::valid() const { return m_node.valid() ; }
// ============================================================================
bool Decays::Nodes::Not::operator() ( const LHCb::ParticleID& pid ) const
{ return ! m_node.node ( pid ) ; }
// ============================================================================
StatusCode  Decays::Nodes::Not::validate 
( const LHCb::IParticlePropertySvc* svc ) const 
{ return m_node.validate ( svc ) ; }
// ============================================================================

// ============================================================================
std::ostream& 
Decays::Nodes::Not::fillStream ( std::ostream& s ) const
{ return s << " ~(" << m_node << ") " ; }


// ============================================================================
Decays::Nodes::Or& 
Decays::Nodes::Or::operator+= ( const Decays::iNode& node )
{ m_nodes.push_back ( node ) ; return *this ; }
// ============================================================================
Decays::Nodes::And& 
Decays::Nodes::And::operator+= ( const Decays::iNode& node )
{ m_nodes.push_back ( node ) ; return *this ; }
// ============================================================================


// ============================================================================
// The END
// ============================================================================
