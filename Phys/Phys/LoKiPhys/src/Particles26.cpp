// $Id$
// ============================================================================
// Include files 
// ============================================================================
// DaVinci Interfaces
// ============================================================================
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Interface.h"
#include "LoKi/Child.h"
#include "LoKi/Particles26.h"
// ============================================================================
#include  "LoKi/GetTools.h"
// ============================================================================
/** @file 
 *  Implementation file for functions form file LoKi/Particles26
 *  @date 2009-04-24 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// the invalid particle:
  const LHCb::Particle*      const s_PARTICLE = 0 ;
  // ==========================================================================
  /// the invalid vertex:
  const LHCb::VertexBase*    const s_VERTEX   = 0 ;
  // ==========================================================================
  /// the invalid tool
  const IDistanceCalculator* const s_TOOL     = 0 ;
  // ==========================================================================
  /// the the valid tool name
  inline std::string toolName
  ( const IAlgTool*    tool ,
    const std::string& nick )
  {
    if ( 0 == tool || !nick.empty() ) { return nick ; }
    //
    const std::string& name = tool -> name() ;
    const bool pub = ( 0 == name.find ("ToolSvc.") ) ;
    const std::string::size_type ldot = name.rfind ('.') ;
    //
    if ( std::string::npos != ldot )
    {
      return !pub ?
        tool -> type () + "/" + std::string ( name , ldot ) :
        tool -> type () + "/" + std::string ( name , ldot ) + ":PUBLIC" ;
    }
    //
    return tool->type() ;
  }
  // ==========================================================================
  /// the the valid tool name
  inline std::string toolName
  ( const LoKi::Interface<IDistanceCalculator>& dc   ,
    const std::string&                          nick )
  {
    const IAlgTool* tool = dc.getObject() ;
    return toolName ( tool , nick ) ;
  }
  // ==========================================================================
}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCA::DOCA
( const size_t               i1    ,
  const size_t               i2    ,
  const IDistanceCalculator* dc    ,
  const bool                 allow ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_eval   ( s_PARTICLE , dc , allow )
  , m_first  ( i1 )
  , m_second ( i2 )
  , m_nick   ("")
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCA::DOCA
( const size_t                                i1    ,
  const size_t                                i2    ,
  const LoKi::Interface<IDistanceCalculator>& dc    , 
  const bool                                  allow ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_eval   ( s_PARTICLE , dc , allow )
  , m_first  ( i1 )
  , m_second ( i2 )
  , m_nick   ( "" )
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCA::DOCA
( const size_t       i1    ,
  const size_t       i2    ,
  const std::string& nick  ,
  const bool         allow ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_eval   ( s_PARTICLE , s_TOOL , allow )
  , m_first  ( i1   )
  , m_second ( i2   )
  , m_nick   ( nick )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCA::result_type
LoKi::Particles::DOCA::operator()
  ( LoKi::Particles::DOCA::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  /// get the first daughter
  const LHCb::Particle* first = LoKi::Child::child( p , firstIndex() ) ;
  if ( 0 == first ) 
  {
    Error ("Invalid first daughter particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  /// get the second daughter
  const LHCb::Particle* second = LoKi::Child::child( p , secondIndex() ) ;
  if ( 0 == second ) 
  {
    Error ("Invalid second daughter particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  // tool is valid?
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result
  return doca ( first , second ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCA::fillStream ( std::ostream& s ) const
{  return s << "DOCA("     << firstIndex() << "," << secondIndex() << ",'"
            << toolName () << "',"
            << ( allow() ?  "True" : "False" ) 
            << ")" ; 
}
// ============================================================================
// get toolname 
// ============================================================================
std::string LoKi::Particles::DOCA::toolName() const 
{ return ::toolName ( tool() , nickname() ) ; }
// ============================================================================
StatusCode LoKi::Particles::DOCA::loadTool () const 
{
  // tool is valid?
  if ( !tool() )
  {
    const IDistanceCalculator* dc = 
      LoKi::GetTools::distanceCalculator ( *this , nickname() ) ;
    /// finally set the tool
    setTool ( dc ) ;
  }
  Assert( !(!tool()) , "Unable to locate tool!" ) ;
  //
  return StatusCode::SUCCESS ;
}




// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2::DOCAChi2
( const size_t               i1 ,
  const size_t               i2 ,
  const IDistanceCalculator* dc ,
  const bool                 allow ) 
  : LoKi::Particles::DOCA( i1 , i2 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2::DOCAChi2
( const size_t                                i1    ,
  const size_t                                i2    ,
  const LoKi::Interface<IDistanceCalculator>& dc    ,
  const bool                                  allow ) 
  : LoKi::Particles::DOCA( i1 , i2 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCAChi2::DOCAChi2
( const size_t       i1    ,
  const size_t       i2    ,
  const std::string& nick  ,
  const bool         allow ) 
  : LoKi::Particles::DOCA( i1 , i2 , nick , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCAChi2::result_type
LoKi::Particles::DOCAChi2::operator()
  ( LoKi::Particles::DOCAChi2::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  /// get the first daughter
  const LHCb::Particle* first = LoKi::Child::child( p , firstIndex() ) ;
  if ( 0 == first ) 
  {
    Error ("Invalid first daughter particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  /// get the second daughter
  const LHCb::Particle* second = LoKi::Child::child( p , secondIndex() ) ;
  if ( 0 == second ) 
  {
    Error ("Invalid second daughter particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  // tool is valid?
  if ( !tool() ) { loadTool () ; }
  
  // evaluate the result
  return chi2 ( first , second ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCAChi2::fillStream ( std::ostream& s ) const
{ return s << "DOCACHI2(" << firstIndex() << "," << secondIndex() << ",'"
           << toolName () << "',"
           << ( allow() ? "True" : "False" ) 
           << ")" ; }
// ============================================================================

// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAMax::DOCAMax
( const IDistanceCalculator* dc    , 
  const bool                 allow )
  : LoKi::Particles::DOCA( 1 , 1 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAMax::DOCAMax
( const LoKi::Interface<IDistanceCalculator>& dc    ,
  const bool                                  allow )
  : LoKi::Particles::DOCA( 1 , 1 , dc , allow ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCAMax::DOCAMax
( const std::string& nick ,
  const bool         allow )
  : LoKi::Particles::DOCA( 1 , 1 , nick , allow ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCAMax::result_type
LoKi::Particles::DOCAMax::operator()
  ( LoKi::Particles::DOCAMax::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  const SmartRefVector<LHCb::Particle>& daugs = p->daughters() ;
  if ( daugs.empty() ) 
  {
    Error ("Empty list of daughetrs, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  
  // check the tool 
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result
  return docamax ( daugs ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCAMax::fillStream ( std::ostream& s ) const
{ return s << "DOCAMAX(" << "'" << toolName () << "',"
           << ( allow() ? "True" : "False" ) 
           << ")" ; }
// ============================================================================

// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2Max::DOCAChi2Max
( const IDistanceCalculator* dc )
  : LoKi::Particles::DOCAMax( dc ) 
{}
// ============================================================================
// constructor from two indices and the tool
// ============================================================================
LoKi::Particles::DOCAChi2Max::DOCAChi2Max
( const LoKi::Interface<IDistanceCalculator>& dc )
  : LoKi::Particles::DOCAMax( dc ) 
{}
// ============================================================================
// constructor from two indices and the tool nickname
// ============================================================================
LoKi::Particles::DOCAChi2Max::DOCAChi2Max
( const std::string& nick )
  : LoKi::Particles::DOCAMax( nick ) 
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::DOCAChi2Max::result_type
LoKi::Particles::DOCAChi2Max::operator()
  ( LoKi::Particles::DOCAChi2Max::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ("Invalid particle, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  const SmartRefVector<LHCb::Particle>& daugs = p->daughters() ;
  if ( daugs.empty() ) 
  {
    Error ("Empty list of daughetrs, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  
  // check the tool 
  if ( !tool() ) { loadTool() ; }
  
  // evaluate the result
  return docachi2max ( daugs ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::DOCAChi2Max::fillStream ( std::ostream& s ) const
{ return s << "DOCACHI2MAX(" << "'" << toolName () << "')" ; }

// ============================================================================
// Constructor from the daughter index & tool 
// ============================================================================
LoKi::Particles::ChildIP::ChildIP
( const size_t               index , 
  const IDistanceCalculator* tool  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_eval  ( s_VERTEX , tool ) 
  , m_index ( index )
  , m_nick  () 
{}
// ============================================================================
// Constructor from the daughter index & tool 
// ============================================================================
LoKi::Particles::ChildIP::ChildIP
( const size_t                                index , 
  const LoKi::Interface<IDistanceCalculator>& tool  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_eval  ( s_VERTEX , tool ) 
  , m_index ( index )
  , m_nick  () 
{}
// ============================================================================
// Constructor from the daughter index & tool 
// ============================================================================
LoKi::Particles::ChildIP::ChildIP
( const size_t        index , 
  const std::string&  nick  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_eval  ( s_VERTEX , s_TOOL ) 
  , m_index ( index )
  , m_nick  ( nick  ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::ChildIP::~ChildIP(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ChildIP*
LoKi::Particles::ChildIP::clone() const 
{ return new LoKi::Particles::ChildIP(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildIP::result_type 
LoKi::Particles::ChildIP::operator() 
  ( LoKi::Particles::ChildIP::argument p ) const 
{
  m_eval.setVertex ( s_VERTEX ) ;
  
  if ( 0 == p ) 
  {
    Error("LHCb::Particle* points to NULL; returning InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                         // RETURN
  }
  
  const LHCb::Vertex* vertex = p->endVertex();  
  if ( 0 == vertex ) 
  {
    Error ("LHCb::Particle* has a NULL endVertex; returning InvalidDistance ");
    return LoKi::Constants::InvalidDistance ;                         // RETURN
  }
  
  /// get the daughter
  const LHCb::Particle* daughter  = LoKi::Child::child ( p , m_index ) ;
  if ( 0 == daughter ) 
  {
    Error ( "Invalid daughter particle, return InvalidDistance") ;
    return LoKi::Constants::InvalidDistance ;                         // RETURN
  }
  
  // tool is valid?
  if ( !tool() ) { loadTool () ; }  

  //
  m_eval.setVertex (   vertex ) ;
  
  const double ip = m_eval.ip ( daughter ) ;
  
  m_eval.setVertex ( s_VERTEX ) ; // re-set the vertex 
  
  return ip ;
}
// ============================================================================
// Load the tool 
// ============================================================================
StatusCode LoKi::Particles::ChildIP::loadTool () const 
{
  // tool is valid?
  if ( !tool() )
  {
    const IDistanceCalculator* dc = 
      LoKi::GetTools::distanceCalculator ( *this , m_nick ) ;
    /// finally set the tool
    setTool ( dc ) ;
  }
  Assert( !(!tool()) , "Unable to locate tool!" ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// get toolname 
// ============================================================================
std::string LoKi::Particles::ChildIP::toolName() const 
{ return ::toolName ( tool() , m_nick ) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::ChildIP::fillStream ( std::ostream& s ) const
{ return s << " CHILDIP( " << m_index <<",'" << toolName () << "')" ; }
// ============================================================================





// ============================================================================
// Constructor from the daughter index & tool 
// ============================================================================
LoKi::Particles::ChildIPChi2::ChildIPChi2
( const size_t               index , 
  const IDistanceCalculator* tool  ) 
  : LoKi::Particles::ChildIP ( index , tool ) 
{}
// ============================================================================
// Constructor from the daughter index & tool 
// ============================================================================
LoKi::Particles::ChildIPChi2::ChildIPChi2
( const size_t                                index , 
  const LoKi::Interface<IDistanceCalculator>& tool  ) 
  : LoKi::Particles::ChildIP ( index , tool ) 
{}
// ============================================================================
// Constructor from the daughter index & tool 
// ============================================================================
LoKi::Particles::ChildIPChi2::ChildIPChi2
( const size_t        index , 
  const std::string&  nick  ) 
  : LoKi::Particles::ChildIP ( index , nick ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::ChildIPChi2::~ChildIPChi2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ChildIPChi2*
LoKi::Particles::ChildIPChi2::clone() const 
{ return new LoKi::Particles::ChildIPChi2(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildIPChi2::result_type 
LoKi::Particles::ChildIPChi2::operator() 
  ( LoKi::Particles::ChildIPChi2::argument p ) const 
{
  m_eval.setVertex ( s_VERTEX ) ;
  
  if ( 0 == p ) 
  {
    Error("LHCb::Particle* points to NULL; returning InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN
  }
  
  const LHCb::Vertex* vertex = p->endVertex();  
  if ( 0 == vertex ) 
  {
    Error ("LHCb::Particle* has a NULL endVertex; returning InvalidChi2");
    return LoKi::Constants::InvalidChi2 ;                         // RETURN
  }
  
  /// get the daughter
  const LHCb::Particle* daughter  = LoKi::Child::child ( p , m_index ) ;
  if ( 0 == daughter ) 
  {
    Error ( "Invalid daughter particle, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN
  }
  
  // tool is valid?
  if ( !tool() ) { loadTool () ; }  

  //
  m_eval.setVertex (   vertex ) ;
  
  const double chi2 = m_eval.ipchi2 ( daughter ) ;
  
  m_eval.setVertex ( s_VERTEX ) ; // reset the vertex 
  
  return chi2 ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::ChildIPChi2::fillStream ( std::ostream& s ) const
{ return s << " CHILDIPCHI( " << m_index <<",'" << toolName () << "')" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
