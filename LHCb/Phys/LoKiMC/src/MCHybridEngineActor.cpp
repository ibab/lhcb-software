// $Id: MCHybridEngineActor.cpp,v 1.1 2007-07-25 15:16:07 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCHybridEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::HybridEngineActor
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @date 2004-06-29 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================
// helper method to descrease number of lines 
// ============================================================================
template <class TYPE>
inline StatusCode LoKi::Hybrid::MCEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::MCEngineActor::addCut/Fun(): LoKi::IMCHybridTool* is not connected!") ;  
  };
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::MCEngineActor::addCut/Fun() : mismatch in LoKi::IMCHybridTool name!") ;  
  };
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::MCEngineActor& LoKi::Hybrid::MCEngineActor::instance() 
{
  static LoKi::Hybrid::MCEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::MCEngineActor::MCEngineActor() : m_tool ( 0 ) {} ;
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::MCEngineActor::~MCEngineActor() {} ;
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngineActor::releaseTool 
( const LoKi::IMCHybridTool*       tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi::Hybrid::MCEngineActor::releaseTool(): mismatch in tools " ) ;
  } ;
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngineActor::connectTool 
(       LoKi::IMCHybridTool*       tool )
{
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::MCEngineActor::releaseTool(): Invalid LoKi::IHybridTool" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngineActor::process
( const std::string&          name , 
  const LoKi::Types::MCCuts&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngineActor::process
( const std::string&          name , 
  const LoKi::Types::MCVCuts& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngineActor::process
( const std::string&          name , 
  const LoKi::Types::MCFunc&  func ) const { return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::MCEngineActor::process
( const std::string&          name , 
  const LoKi::Types::MCVFunc& func ) const { return _add ( name , func ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
