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
  : LoKi::Functor<void,bool>()
  , m_nickname ( nickname ) 
  , m_tool     () 
{
  //
  ILoKiSvc* l = lokiSvc() ;
  Assert ( 0 != l   , "ILoKiSvc* points to NULL!" ) ;
  SmartIF<IToolSvc> tsvc ( l ) ;
  Assert ( !(!tsvc) , "IToolSvc* points to NULL!" ) ;
  // aquire tool 
  ICheckTool*       ctool  = 0  ;
  const IInterface* parent = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nickname , ctool , parent , true ) ;
  Assert ( sc.isSuccess() , "Unable to aquire the tool '" + nickname + "'" , sc );
  m_tool = ctool ;
  Assert ( !(!m_tool)     , "Unable to aquire the tool '" + nickname + "'" );
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Status::Check::~Check(){  m_tool.release() ; }
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
  Assert ( !(!m_tool) , "Invalid ICheckTool-interface" ) ;
  //
  return m_tool->check().isSuccess() ;
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
  : LoKi::Functor<void,bool>()
  , m_nickname ( nickname ) 
  , m_tool     () 
{
  //
  ILoKiSvc* l = lokiSvc() ;
  Assert ( 0 != l   , "ILoKiSvc* points to NULL!" ) ;
  SmartIF<IToolSvc> tsvc ( l ) ;
  Assert ( !(!tsvc) , "IToolSvc* points to NULL!" ) ;
  // aquire tool 
  IAccept*       ctool  = 0  ;
  const IInterface* parent = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nickname , ctool , parent , true ) ;
  Assert ( sc.isSuccess() , "Unable to aquire the tool '" + nickname + "'" , sc );
  m_tool = ctool ;
  Assert ( !(!m_tool)     , "Unable to aquire the tool '" + nickname + "'" );
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Status::Accept::~Accept(){  m_tool.release() ; }
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


// ============================================================================
// The END 
// ============================================================================
