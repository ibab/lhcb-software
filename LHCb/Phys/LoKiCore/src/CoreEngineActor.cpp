// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CoreEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::Hybrid::CoreEngineActor
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *
 *  @date 2008-09-28 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// helper method to descrease number of lines 
// ============================================================================
template <class TYPE>
inline StatusCode LoKi::Hybrid::CoreEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::CoreEngineActor::addCut/Fun(): LoKi::Hybrid::ICoreAntiFactory* is not connected!") ;  
  }
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::CoreEngineActor::addCut/Fun() : mismatch in LoKi::Hybrid::ICoreAntiFactory name!") ;  
  }
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::CoreEngineActor& LoKi::Hybrid::CoreEngineActor::instance() 
{
  static LoKi::Hybrid::CoreEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::CoreEngineActor::CoreEngineActor() : m_tool ( 0 ) {} 
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::CoreEngineActor::~CoreEngineActor() {} 
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::releaseTool 
( const LoKi::Hybrid::ICoreAntiFactory*       tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi::Hybrid::CoreEngineActor::releaseTool(): mismatch in tools " ) ;
  } 
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::connectTool 
(       LoKi::Hybrid::ICoreAntiFactory*       tool )
{
  //
  LoKi::Report::Assert 
    ( !m_tool.validPointer() , "LoKi::Hybrid::CoreEngineActor: double lock?" )  ;
  //
  // substitute the tool 
  //
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::CoreEngineActor::releaseTool(): Invalid LoKi::Hybrid::ICoreAntiFactory" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&        name , 
  const LoKi::Types::FCuts& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&        name , 
  const LoKi::Types::FFunc& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&        name , 
  const LoKi::Types::XCuts& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&        name , 
  const LoKi::Types::XFunc& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&        name , 
  const LoKi::Types::XMaps& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&           name , 
  const LoKi::Types::XFunVals& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::CoreEngineActor::process
( const std::string&           name , 
  const LoKi::Types::XSources& cut  ) const 
{ return _add ( name , cut ) ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
