// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Status.h"
#include "LoKi/ILoKiSvc.h"
// ============================================================================
/** @file 
 *  Implementation of functors deaing with generic tools
 *  @see ICheckTool 
 *  @see IAcceptTool
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-08-02 
 */
// ============================================================================
// constructor from tool nickname 
// ============================================================================
LoKi::Status::Check::Check
( const std::string& nickname ) 
  : LoKi::AuxFunBase ( std::tie ( nickname ) )
  , LoKi::Functor<void,bool>()
  , m_nickname ( nickname ) 
  , m_tool     () 
{
  if ( gaudi() ) { getTool() ; }
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Status::Check::~Check()
{ 
  if ( m_tool && !gaudi() ) 
  {
    // Warning("Manual release of ICheckTool") ;
    m_tool.reset () ; 
  }
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Status::Check* LoKi::Status::Check::clone() const 
{ return new LoKi::Status::Check ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Status::Check::result_type 
LoKi::Status::Check::operator() ( /* LoKi::Status::Check::argument */ ) const 
{
  if ( !m_tool ) { getTool() ; }
  Assert ( !(!m_tool) , "Invalid ICheckTool-interface" ) ;
  //
  return m_tool->check().isSuccess() ;
}
// ============================================================================
void LoKi::Status::Check::getTool() const 
{
  ILoKiSvc* l = lokiSvc() ;
  Assert ( 0 != l   , "ILoKiSvc* points to NULL!" ) ;
  SmartIF<IToolSvc> tsvc ( l ) ;
  Assert ( !(!tsvc) , "IToolSvc* points to NULL!" ) ;
  // aquire tool 
  ICheckTool*       ctool  = 0  ;
  const IInterface* parent = 0 ;
  StatusCode sc = tsvc->retrieveTool ( m_nickname , ctool , parent , true ) ;
  Assert ( sc.isSuccess() , "Unable to acquire the tool '" + m_nickname + "'" , sc );
  m_tool = ctool ;
  Assert ( !(!m_tool)     , "Unable to acquire the tool '" + m_nickname + "'" );
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Status::Check::fillStream ( std::ostream& s ) const 
{
  s << " CHECK("   ;
  Gaudi::Utils::toStream ( m_nickname , s ) ;
  return s << ") " ;
}
// ============================================================================

// ============================================================================
// constructor from tool nickname 
// ============================================================================
LoKi::Status::Accept::Accept
( const std::string& nickname ) 
  : LoKi::AuxFunBase ( std::tie ( nickname ) )
  , LoKi::Functor<void,bool>()
  , m_nickname ( nickname ) 
  , m_tool     () 
{
  if ( gaudi() ) { getTool() ; }
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Status::Accept::~Accept()
{ 
  if ( m_tool && !gaudi() ) 
  {
    // Warning("Manual release of IAccept") ;
    m_tool.reset () ; 
  }
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Status::Accept* LoKi::Status::Accept::clone() const 
{ return new LoKi::Status::Accept ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Status::Accept::result_type 
LoKi::Status::Accept::operator() ( /* LoKi::Status::Check::argument */ ) const 
{
  if ( !m_tool ) { getTool() ; }
  Assert ( !(!m_tool) , "Invalid IAccept-interface" ) ;
  //
  return m_tool->accept() ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Status::Accept::fillStream ( std::ostream& s ) const 
{
  s << " ACCEPT("   ;
  Gaudi::Utils::toStream ( m_nickname , s ) ;
  return s << ") " ;
}
// ============================================================================
void LoKi::Status::Accept::getTool() const 
{
  ILoKiSvc* l = lokiSvc() ;
  Assert ( 0 != l   , "ILoKiSvc* points to NULL!" ) ;
  SmartIF<IToolSvc> tsvc ( l ) ;
  Assert ( !(!tsvc) , "IToolSvc* points to NULL!" ) ;
  // aquire tool 
  IAccept*       ctool  = 0  ;
  const IInterface* parent = 0 ;
  StatusCode sc = tsvc->retrieveTool ( m_nickname , ctool , parent , true ) ;
  Assert ( sc.isSuccess() , "Unable to aquire the tool '" + m_nickname + "'" , sc );
  m_tool = ctool ;
  Assert ( !(!m_tool)     , "Unable to aquire the tool '" + m_nickname + "'" );  
}


// ============================================================================
// The END 
// ============================================================================
