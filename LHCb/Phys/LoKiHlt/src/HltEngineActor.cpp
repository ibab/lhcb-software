// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::Hybrid::HltEngineActor
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @date 2008-09-28 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// helper method to descrease number of lines 
// ============================================================================
template <class TYPE>
inline StatusCode LoKi::Hybrid::HltEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::HltEngineActor::addCut/Fun(): LoKi::Hybrid::IHltAntiFactory* is not connected!") ;  
  }
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::HltEngineActor::addCut/Fun() : mismatch in LoKi::Hybrid::IHltAntiFactory name!") ;  
  }
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::HltEngineActor& LoKi::Hybrid::HltEngineActor::instance() 
{
  static LoKi::Hybrid::HltEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::HltEngineActor::HltEngineActor() : m_tool ( 0 ) {} 
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::HltEngineActor::~HltEngineActor() {} 
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::releaseTool 
( const LoKi::Hybrid::IHltAntiFactory*       tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi::Hybrid::HltEngineActor::releaseTool(): mismatch in tools " ) ;
  } 
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::connectTool 
(       LoKi::Hybrid::IHltAntiFactory*       tool )
{
  //
  LoKi::Report::Assert 
    ( !m_tool.validPointer() , 
      "LoKi::Hybrid::HltEngineActor: double lock?" ) ;                      
  //
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::HltEngineActor::releaseTool(): Invalid LoKi::Hybrid::IHltAntiFactory" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::process
( const std::string&          name , 
  const LoKi::Types::L0_Cuts& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::process
( const std::string&          name , 
  const LoKi::Types::L0_Func& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::process
( const std::string&          name , 
  const LoKi::Types::ODIN_Cuts& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::process
( const std::string&          name , 
  const LoKi::Types::ODIN_Func& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::process
( const std::string&           name , 
  const LoKi::Types::HLT_Cuts& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::HltEngineActor::process
( const std::string&           name , 
  const LoKi::Types::HLT_Func& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// The END 
// ============================================================================
