// $Id: Nodes.cpp,v 1.4 2009-01-12 06:35:38 cattanem Exp $
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
// STD & STL
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
// LoKi
// ============================================================================
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
  // ==========================================================================
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
    /// the defautl constructor is disabled 
    _Pid ();
    // the pid itself 
    LHCb::ParticleID m_pid ; 
    // ========================================================================
  };
  // =========================================================================
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Any*
Decays::Nodes::Any::clone() const { return new Any(*this) ; }
// ============================================================================
// MANDATORY: check the validity
// ============================================================================
bool Decays::Nodes::Any::valid() const { return true ; }
// ============================================================================
// MANDATORY: the proper validation of the node
// ============================================================================
StatusCode Decays::Nodes::Any::validate 
( const LHCb::IParticlePropertySvc* /*svc */ )  const 
{ return StatusCode ( StatusCode::SUCCESS , true ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool Decays::Nodes::Any::operator() 
  ( const LHCb::ParticleID& /* p */ ) const { return true ; }
// ============================================================================


// ============================================================================
// constructor from the decay item
// ============================================================================
Decays::Nodes::Pid::Pid
( const Decays::Decay::Item&  item ) 
  : Decays::iNode () , m_item ( item ) {}
// ============================================================================
// constructor from Particle Property
// ============================================================================
Decays::Nodes::Pid::Pid
( const LHCb::ParticleProperty*      item ) 
  : Decays::iNode () , m_item ( item ) {}
// ============================================================================
// constructor from ParticleID
// ============================================================================
Decays::Nodes::Pid::Pid
( const LHCb::ParticleID&      item ) 
  : Decays::iNode () , m_item ( item ) {}
// ============================================================================
// constructor from Particle name
// ============================================================================
Decays::Nodes::Pid::Pid
( const std::string&           item ) 
  : Decays::iNode () , m_item ( item ) {}
// ============================================================================
// MANDATORY: check the validity
// ============================================================================
bool Decays::Nodes::Pid::valid () const { return 0 !=  m_item.pp () ; }
// ============================================================================
// MANDATORY: the proper validation of the node
// ============================================================================
StatusCode Decays::Nodes::Pid::validate 
( const LHCb::IParticlePropertySvc* svc ) 
  const { return m_item.validate ( svc ) ; }
// ============================================================================


// ============================================================================
// constructor from the decay item
// ============================================================================
Decays::Nodes::CC::CC
( const Decays::Decay::Item&  item ) 
  : Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from Particle Property
// ============================================================================
Decays::Nodes::CC::CC
( const LHCb::ParticleProperty*      item ) 
  : Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from ParticleID
// ============================================================================
Decays::Nodes::CC::CC
( const LHCb::ParticleID&      item ) 
  : Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from Particle name
// ============================================================================
Decays::Nodes::CC::CC
( const std::string&           item ) 
  : Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from Pid 
// ============================================================================
Decays::Nodes::CC::CC
( const Decays::Nodes::Pid& item ) 
  : Decays::Nodes::Pid ( item ) {}
// ============================================================================

// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Meson*
Decays::Nodes::Meson::clone() const { return new Meson(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Lepton*
Decays::Nodes::Lepton::clone() const { return new Lepton(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Nu*
Decays::Nodes::Nu::clone() const { return new Nu(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Ell*
Decays::Nodes::Ell::clone() const { return new Ell(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::EllPlus*
Decays::Nodes::EllPlus::clone() const { return new EllPlus(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::EllMinus*
Decays::Nodes::EllMinus::clone() const { return new EllMinus(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Baryon*
Decays::Nodes::Baryon::clone() const { return new Baryon(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Hadron*
Decays::Nodes::Hadron::clone() const { return new Hadron(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Charged*
Decays::Nodes::Charged::clone() const { return new Charged(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Positive*
Decays::Nodes::Positive::clone() const { return new Positive(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Negative*
Decays::Nodes::Negative::clone() const { return new Negative(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::Neutral*
Decays::Nodes::Neutral::clone() const { return new Neutral(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::HasQuark*
Decays::Nodes::HasQuark::clone() const { return new HasQuark(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
Decays::Nodes::JSpin*
Decays::Nodes::JSpin::clone() const { return new JSpin(*this) ; }

// ============================================================================
// constructor from the quark 
// ============================================================================
Decays::Nodes::HasQuark::HasQuark ( LHCb::ParticleID::Quark quark ) 
  : Decays::Nodes::Any () , m_quark ( quark ) {}
// ============================================================================
// constructor from the 2J+1
// ============================================================================
Decays::Nodes::JSpin::JSpin ( const int spin ) 
  : Decays::Nodes::Any () , m_spin ( spin ) {}
// ============================================================================


// ============================================================================
std::ostream& 
Decays::Nodes::Any::fillStream ( std::ostream& s ) const
{ return s << " X " ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Pid::fillStream ( std::ostream& s ) const
{ return s << " " << m_item.name() << " "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::CC::fillStream ( std::ostream& s ) const
{ return s << " [" << item().name() << "]cc "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Meson::fillStream ( std::ostream& s ) const
{ return s << " Meson "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Hadron::fillStream ( std::ostream& s ) const
{ return s << " Hadron "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Baryon::fillStream ( std::ostream& s ) const
{ return s << " Baryon "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Lepton::fillStream ( std::ostream& s ) const
{ return s << " Lepton "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Neutral::fillStream ( std::ostream& s ) const
{ return s << " X0 "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Charged::fillStream ( std::ostream& s ) const
{ return s << " Xq "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Positive::fillStream ( std::ostream& s ) const
{ return s << " X+ "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Negative::fillStream ( std::ostream& s ) const
{ return s << " X- "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Nu::fillStream ( std::ostream& s ) const
{ return s << " Nu "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::Ell::fillStream ( std::ostream& s ) const
{ return s << " l "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::EllPlus::fillStream ( std::ostream& s ) const
{ return s << " l+ "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::EllMinus::fillStream ( std::ostream& s ) const
{ return s << " l- "  ; }
// ============================================================================
std::ostream& 
Decays::Nodes::HasQuark::fillStream ( std::ostream& s ) const
{
  switch ( m_quark ) 
  {
  case LHCb::ParticleID::down      : return s << " Xd " ;
  case LHCb::ParticleID::up        : return s << " Xu " ;
  case LHCb::ParticleID::strange   : return s << " Xs " ;
  case LHCb::ParticleID::charm     : return s << " Xc " ;
  case LHCb::ParticleID::bottom    : return s << " Xb " ;
  case LHCb::ParticleID::top       : return s << " Xt " ;
  default: ;
  }
  return s << " HasQuark("  << (int) m_quark << ") " ; 
}
// ============================================================================
std::ostream& 
Decays::Nodes::JSpin::fillStream ( std::ostream& s ) const
{ return s << " JSpin(" << m_spin << ") " ; }


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
/*  Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
// ============================================================================
Decays::Nodes::Or operator||
( const Decays::iNode& o1 , 
  const std::string&         o2 ) 
{ return o1 || Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::Or operator||
( const Decays::iNode& o1 , 
  const LHCb::ParticleID&    o2 ) 
{ return o1 || Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::Or operator||
( const Decays::iNode& o1 , 
  const Decays::Decay::Item&   o2 ) 
{ return o1 || Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::Or operator||
( const Decays::iNode&          o1 , 
  const LHCb::ParticleProperty* o2 ) 
{ return o1 || Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::Or operator||
( const std::string&         o2 ,
  const Decays::iNode& o1 ) 
{ return Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
Decays::Nodes::Or operator||
( const LHCb::ParticleID&    o2 ,
  const Decays::iNode& o1 ) 
{ return Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
Decays::Nodes::Or operator||
( const Decays::Decay::Item&   o2 ,
  const Decays::iNode& o1 ) 
{ return Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
Decays::Nodes::Or operator||
( const LHCb::ParticleProperty* o2 ,
  const Decays::iNode&          o1 ) 
{ return Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
/*  Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
// ============================================================================
Decays::Nodes::And operator&&
( const Decays::iNode& o1 , 
  const std::string&         o2 ) 
{ return o1 && Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::And operator&&
( const Decays::iNode& o1 , 
  const LHCb::ParticleID&    o2 ) 
{ return o1 && Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::And operator&&
( const Decays::iNode& o1 , 
  const Decays::Decay::Item&   o2 ) 
{ return o1 && Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::And operator&&
( const Decays::iNode&          o1 , 
  const LHCb::ParticleProperty* o2 ) 
{ return o1 && Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
Decays::Nodes::And operator&&
( const std::string&   o2 ,
  const Decays::iNode& o1 ) 
{ return Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================
Decays::Nodes::And operator&&
( const LHCb::ParticleID&  o2 ,
  const Decays::iNode&     o1 ) 
{ return Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================
Decays::Nodes::And operator&&
( const Decays::Decay::Item&   o2 ,
  const Decays::iNode&       o1 ) 
{ return Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================
Decays::Nodes::And operator&&
( const LHCb::ParticleProperty*    o2 ,
  const Decays::iNode& o1 ) 
{ return Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================


// ===========================================================================
// The END
// ===========================================================================
