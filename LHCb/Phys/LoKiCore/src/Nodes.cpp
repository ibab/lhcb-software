// $Id: Nodes.cpp,v 1.3 2008-07-22 13:00:57 ibelyaev Exp $
// ===========================================================================
// Include files
// ===========================================================================
// Event
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Nodes.h"
// ===========================================================================
/** @file 
 *  The implementation file for various decay nodes 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
// ===========================================================================


// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Any*
LoKi::Decays::Nodes::Any::clone() const { return new Any(*this) ; }
// ============================================================================
// MANDATORY: check the validity
// ============================================================================
bool LoKi::Decays::Nodes::Any::valid() const { return true ; }
// ============================================================================
// MANDATORY: the proper validation of the node
// ============================================================================
StatusCode LoKi::Decays::Nodes::Any::validate 
( IParticlePropertySvc* /*svc */ )  const 
{ return StatusCode ( StatusCode::SUCCESS , true ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
bool LoKi::Decays::Nodes::Any::operator() 
  ( const LHCb::ParticleID& /* p */ ) const { return true ; }
// ============================================================================


// ============================================================================
// constructor from the decay item
// ============================================================================
LoKi::Decays::Nodes::Pid::Pid
( const LHCb::Decay::Item&  item ) 
  : LoKi::Decays::iNode () , m_item ( item ) {}
// ============================================================================
// constructor from Particle Property
// ============================================================================
LoKi::Decays::Nodes::Pid::Pid
( const ParticleProperty*      item ) 
  : LoKi::Decays::iNode () , m_item ( item ) {}
// ============================================================================
// constructor from ParticleID
// ============================================================================
LoKi::Decays::Nodes::Pid::Pid
( const LHCb::ParticleID&      item ) 
  : LoKi::Decays::iNode () , m_item ( item ) {}
// ============================================================================
// constructor from Particle name
// ============================================================================
LoKi::Decays::Nodes::Pid::Pid
( const std::string&           item ) 
  : LoKi::Decays::iNode () , m_item ( item ) {}
// ============================================================================
// MANDATORY: check the validity
// ============================================================================
bool LoKi::Decays::Nodes::Pid::valid () const { return 0 !=  m_item.pp () ; }
// ============================================================================
// MANDATORY: the proper validation of the node
// ============================================================================
StatusCode LoKi::Decays::Nodes::Pid::validate 
( IParticlePropertySvc* svc ) const { return m_item.validate ( svc ) ; }
// ============================================================================


// ============================================================================
// constructor from the decay item
// ============================================================================
LoKi::Decays::Nodes::CC::CC
( const LHCb::Decay::Item&  item ) 
  : LoKi::Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from Particle Property
// ============================================================================
LoKi::Decays::Nodes::CC::CC
( const ParticleProperty*      item ) 
  : LoKi::Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from ParticleID
// ============================================================================
LoKi::Decays::Nodes::CC::CC
( const LHCb::ParticleID&      item ) 
  : LoKi::Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from Particle name
// ============================================================================
LoKi::Decays::Nodes::CC::CC
( const std::string&           item ) 
  : LoKi::Decays::Nodes::Pid ( item ) {}
// ============================================================================
// constructor from Pid 
// ============================================================================
LoKi::Decays::Nodes::CC::CC
( const LoKi::Decays::Nodes::Pid& item ) 
  : LoKi::Decays::Nodes::Pid ( item ) {}
// ============================================================================





// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Meson*
LoKi::Decays::Nodes::Meson::clone() const { return new Meson(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Lepton*
LoKi::Decays::Nodes::Lepton::clone() const { return new Lepton(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Nu*
LoKi::Decays::Nodes::Nu::clone() const { return new Nu(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Ell*
LoKi::Decays::Nodes::Ell::clone() const { return new Ell(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::EllPlus*
LoKi::Decays::Nodes::EllPlus::clone() const { return new EllPlus(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::EllMinus*
LoKi::Decays::Nodes::EllMinus::clone() const { return new EllMinus(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Baryon*
LoKi::Decays::Nodes::Baryon::clone() const { return new Baryon(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Hadron*
LoKi::Decays::Nodes::Hadron::clone() const { return new Hadron(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Charged*
LoKi::Decays::Nodes::Charged::clone() const { return new Charged(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Positive*
LoKi::Decays::Nodes::Positive::clone() const { return new Positive(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Negative*
LoKi::Decays::Nodes::Negative::clone() const { return new Negative(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Neutral*
LoKi::Decays::Nodes::Neutral::clone() const { return new Neutral(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::HasQuark*
LoKi::Decays::Nodes::HasQuark::clone() const { return new HasQuark(*this) ; }
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::JSpin*
LoKi::Decays::Nodes::JSpin::clone() const { return new JSpin(*this) ; }





// ============================================================================
// constructor from the quark 
// ============================================================================
LoKi::Decays::Nodes::HasQuark::HasQuark ( LHCb::ParticleID::Quark quark ) 
  : LoKi::Decays::Nodes::Any () , m_quark ( quark ) {}
// ============================================================================
// constructor from the 2J+1
// ============================================================================
LoKi::Decays::Nodes::JSpin::JSpin ( const int spin ) 
  : LoKi::Decays::Nodes::Any () , m_spin ( spin ) {}
// ============================================================================


// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Any::fillStream ( std::ostream& s ) const
{ return s << " X " ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Pid::fillStream ( std::ostream& s ) const
{ return s << " " << m_item.name() << " "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::CC::fillStream ( std::ostream& s ) const
{ return s << " [" << item().name() << "]cc "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Meson::fillStream ( std::ostream& s ) const
{ return s << " Meson "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Hadron::fillStream ( std::ostream& s ) const
{ return s << " Hadron "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Baryon::fillStream ( std::ostream& s ) const
{ return s << " Baryon "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Lepton::fillStream ( std::ostream& s ) const
{ return s << " Lepton "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Neutral::fillStream ( std::ostream& s ) const
{ return s << " X0 "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Charged::fillStream ( std::ostream& s ) const
{ return s << " Xq "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Positive::fillStream ( std::ostream& s ) const
{ return s << " X+ "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Negative::fillStream ( std::ostream& s ) const
{ return s << " X- "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Nu::fillStream ( std::ostream& s ) const
{ return s << " Nu "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Ell::fillStream ( std::ostream& s ) const
{ return s << " l "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::EllPlus::fillStream ( std::ostream& s ) const
{ return s << " l+ "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::EllMinus::fillStream ( std::ostream& s ) const
{ return s << " l- "  ; }
// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::HasQuark::fillStream ( std::ostream& s ) const
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
LoKi::Decays::Nodes::JSpin::fillStream ( std::ostream& s ) const
{ return s << " JSpin(" << m_spin << ") " ; }


// ===========================================================================
// Or
// ===========================================================================
// constructor from two nodes 
// ===========================================================================
LoKi::Decays::Nodes::Or::Or 
( const LoKi::Decays::iNode& n1 , 
  const LoKi::Decays::iNode& n2 ) 
  : LoKi::Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
}
// ===========================================================================
// constructor from three nodes 
// ===========================================================================
LoKi::Decays::Nodes::Or::Or 
( const LoKi::Decays::iNode& n1 , 
  const LoKi::Decays::iNode& n2 ,
  const LoKi::Decays::iNode& n3 ) 
  : LoKi::Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
  m_nodes.push_back ( n3 ) ;  
}
// ===========================================================================
// constructor from four nodes 
// ===========================================================================
LoKi::Decays::Nodes::Or::Or 
( const LoKi::Decays::iNode& n1 , 
  const LoKi::Decays::iNode& n2 ,
  const LoKi::Decays::iNode& n3 , 
  const LoKi::Decays::iNode& n4 ) 
  : LoKi::Decays::iNode () 
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
LoKi::Decays::Nodes::Or::Or 
( const LoKi::Decays::SubNodes& nodes ) 
  : LoKi::Decays::iNode () 
  , m_nodes ( nodes )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Or*
LoKi::Decays::Nodes::Or::clone() const { return new Or(*this) ; }
// ===========================================================================
// MANDATORY: the only one essential method
// ===========================================================================
bool LoKi::Decays::Nodes::Or::operator() ( const LHCb::ParticleID& pid ) const 
{ return m_nodes.end() != std::find ( m_nodes.begin() , m_nodes.end() , pid ) ; }
// ===========================================================================
// MANDATORY: the specific printout
// ===========================================================================
std::ostream& LoKi::Decays::Nodes::Or::fillStream ( std::ostream& s ) const 
{
  s << " (" ;
  for ( LoKi::Decays::SubNodes::const_iterator node = m_nodes.begin() ;
        m_nodes.end() != node ; ++node ) 
  {
    if ( m_nodes.begin() != node ) { s << "||" ; }    
    s << *node ;
  }  
  return s << ") " ;
}
// ===========================================================================
// MANDATORY: check the validity 
// ===========================================================================
bool LoKi::Decays::Nodes::Or::valid () const
{ return !m_nodes.empty() && LoKi::Decays::valid ( m_nodes.begin() , m_nodes.end() ) ; }
// ===========================================================================
// MANDATORY: the proper validation of the node
// ===========================================================================
StatusCode LoKi::Decays::Nodes::Or::validate ( IParticlePropertySvc* svc ) const
{
  if ( m_nodes.empty() ) { return StatusCode::FAILURE ; }
  return LoKi::Decays::validate ( m_nodes.begin() , m_nodes.end() , svc ) ;
}
// ===========================================================================


// ===========================================================================
// And
// ===========================================================================
// constructor from two nodes 
// ===========================================================================
LoKi::Decays::Nodes::And::And
( const LoKi::Decays::iNode& n1 , 
  const LoKi::Decays::iNode& n2 ) 
  : LoKi::Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
}
// ===========================================================================
// constructor from three nodes 
// ===========================================================================
LoKi::Decays::Nodes::And::And
( const LoKi::Decays::iNode& n1 , 
  const LoKi::Decays::iNode& n2 ,
  const LoKi::Decays::iNode& n3 ) 
  :  LoKi::Decays::iNode () 
  , m_nodes ()
{
  m_nodes.push_back ( n1 ) ;
  m_nodes.push_back ( n2 ) ;  
  m_nodes.push_back ( n3 ) ;  
}
// ===========================================================================
// constructor from four nodes 
// ===========================================================================
LoKi::Decays::Nodes::And::And
( const LoKi::Decays::iNode& n1 , 
  const LoKi::Decays::iNode& n2 ,
  const LoKi::Decays::iNode& n3 , 
  const LoKi::Decays::iNode& n4 ) 
  : LoKi::Decays::iNode () 
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
LoKi::Decays::Nodes::And::And
( const LoKi::Decays::SubNodes& nodes ) 
  : LoKi::Decays::iNode () 
  , m_nodes ( nodes )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::And*
LoKi::Decays::Nodes::And::clone() const { return new And(*this) ; }
// ===========================================================================
// MANDATORY: the only one essential method
// ===========================================================================
bool LoKi::Decays::Nodes::And::operator() ( const LHCb::ParticleID& pid ) const 
{
  if ( m_nodes.empty() ) { return false ; }
  for ( LoKi::Decays::SubNodes::const_iterator node = m_nodes.begin() ;
        m_nodes.end() != node ; ++node ) 
  { if ( (*node != pid ) ) { return false; } }
  return true ;
}
// ===========================================================================
// MANDATORY: the specific printout
// ===========================================================================
std::ostream& LoKi::Decays::Nodes::And::fillStream ( std::ostream& s ) const 
{
  s << " (" ;
  for ( LoKi::Decays::SubNodes::const_iterator node = m_nodes.begin() ;
        m_nodes.end() != node ; ++node ) 
  {
    if ( m_nodes.begin() != node ) { s << "&&" ; }    
    s << *node ;
  }  
  return s << ") " ;
}
// ===========================================================================
// MANDATORY: check the validity 
// ===========================================================================
bool LoKi::Decays::Nodes::And::valid () const
{ return !m_nodes.empty() && LoKi::Decays::valid ( m_nodes.begin() , m_nodes.end() ) ; }
// ===========================================================================
// MANDATORY: the proper validation of the node
// ===========================================================================
StatusCode LoKi::Decays::Nodes::And::validate ( IParticlePropertySvc* svc ) const
{
  if ( m_nodes.empty() ) { return StatusCode::FAILURE ; }
  return LoKi::Decays::validate ( m_nodes.begin() , m_nodes.end() , svc ) ;
}
// ===========================================================================




// ===========================================================================
// constructor from the node 
// ===========================================================================
LoKi::Decays::Nodes::Not::Not ( const LoKi::Decays::iNode& node ) 
  : LoKi::Decays::iNode ( ) 
  , m_node ( node )  
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Decays::Nodes::Not*
LoKi::Decays::Nodes::Not::clone() const { return new Not(*this) ; }
// ============================================================================
// valid ? 
// ============================================================================
bool LoKi::Decays::Nodes::Not::valid() const { return m_node.valid() ; }
// ============================================================================
bool LoKi::Decays::Nodes::Not::operator() ( const LHCb::ParticleID& pid ) const
{ return ! m_node.node ( pid ) ; }
// ============================================================================
StatusCode  LoKi::Decays::Nodes::Not::validate ( IParticlePropertySvc* svc ) const 
{ return m_node.validate ( svc ) ; }
// ============================================================================

// ============================================================================
std::ostream& 
LoKi::Decays::Nodes::Not::fillStream ( std::ostream& s ) const
{ return s << " !(" << m_node << ") " ; }


// ============================================================================
LoKi::Decays::Nodes::Or& 
LoKi::Decays::Nodes::Or::operator|= ( const LoKi::Decays::iNode& node )
{ m_nodes.push_back ( node ) ; return *this ; }
// ============================================================================
LoKi::Decays::Nodes::Or& 
LoKi::Decays::Nodes::Or::operator+= ( const LoKi::Decays::iNode& node )
{ return (*this) |= node  ; }
// ============================================================================
LoKi::Decays::Nodes::And& 
LoKi::Decays::Nodes::And::operator&= ( const LoKi::Decays::iNode& node )
{ m_nodes.push_back ( node ) ; return *this ; }
// ============================================================================
LoKi::Decays::Nodes::And& 
LoKi::Decays::Nodes::And::operator+= ( const LoKi::Decays::iNode& node )
{ return (*this) &= node  ; }
// ============================================================================


// ============================================================================
/*  Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const LoKi::Decays::iNode& o1 , 
  const std::string&         o2 ) 
{ return o1 || LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const LoKi::Decays::iNode& o1 , 
  const LHCb::ParticleID&    o2 ) 
{ return o1 || LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const LoKi::Decays::iNode& o1 , 
  const LHCb::Decay::Item&   o2 ) 
{ return o1 || LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const LoKi::Decays::iNode& o1 , 
  const ParticleProperty*    o2 ) 
{ return o1 || LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const std::string&         o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const LHCb::ParticleID&    o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const LHCb::Decay::Item&   o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
LoKi::Decays::Nodes::Or operator||
( const ParticleProperty*    o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) || o1 ; }
// ============================================================================
/*  Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const LoKi::Decays::iNode& o1 , 
  const std::string&         o2 ) 
{ return o1 && LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const LoKi::Decays::iNode& o1 , 
  const LHCb::ParticleID&    o2 ) 
{ return o1 && LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const LoKi::Decays::iNode& o1 , 
  const LHCb::Decay::Item&   o2 ) 
{ return o1 && LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const LoKi::Decays::iNode& o1 , 
  const ParticleProperty*    o2 ) 
{ return o1 && LoKi::Decays::Nodes::Pid ( o2 ) ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const std::string&         o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const LHCb::ParticleID&    o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const LHCb::Decay::Item&   o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================
LoKi::Decays::Nodes::And operator&&
( const ParticleProperty*    o2 ,
  const LoKi::Decays::iNode& o1 ) 
{ return LoKi::Decays::Nodes::Pid ( o2 ) && o1 ; }
// ============================================================================


// ===========================================================================
// The END
// ===========================================================================
