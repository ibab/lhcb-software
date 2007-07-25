// $Id: GenHybridEngineActor.cpp,v 1.1 2007-07-25 15:07:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenHybridEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::GenEngineActor
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
inline StatusCode LoKi::Hybrid::GenEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::GenEngineActor::addCut/Fun(): LoKi::IGenHybridTool* is not connected!") ;  
  };
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::GenEngineActor::addCut/Fun() : mismatch in LoKi::IGenHybridTool name!") ;  
  };
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::GenEngineActor& LoKi::Hybrid::GenEngineActor::instance() 
{
  static LoKi::Hybrid::GenEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::GenEngineActor::GenEngineActor() : m_tool ( 0 ) {} 
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::GenEngineActor::~GenEngineActor() {} 
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngineActor::releaseTool 
( const LoKi::IGenHybridTool*       tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi::Hybrid::GenEngineActor::releaseTool(): mismatch in tools " ) ;
  } ;
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngineActor::connectTool 
(       LoKi::IGenHybridTool*       tool )
{
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::GenEngineActor::releaseTool(): Invalid LoKi::IHybridTool" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngineActor::process
( const std::string&          name , 
  const LoKi::Types::GCuts&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngineActor::process
( const std::string&          name , 
  const LoKi::Types::GVCuts&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngineActor::process
( const std::string&          name , 
  const LoKi::Types::GFunc&   func ) const { return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::GenEngineActor::process
( const std::string&          name , 
  const LoKi::Types::GVFunc&  func ) const { return _add ( name , func ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
