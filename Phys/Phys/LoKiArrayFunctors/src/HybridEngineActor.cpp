// $Id: HybridEngineActor.cpp,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HybridEngineActor.h"
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
inline StatusCode LoKi::Hybrid::EngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::EngineActor::addCut/Fun(): LoKi::IHybridTool* is not connected!") ;  
  };
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::EngineActor::addCut/Fun() : mismatch in LoKi::IHybridTool name!") ;  
  };
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::EngineActor& LoKi::Hybrid::EngineActor::instance() 
{
  static LoKi::Hybrid::EngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::EngineActor::EngineActor() : m_tool ( 0 ) {} ;
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::EngineActor::~EngineActor() {} ;
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::releaseTool 
( const LoKi::IHybridTool*       tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi:Hybrid::EngineActor::releaseTool(): mismatch in tools " ) ;
  } ;
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::connectTool 
(       LoKi::IHybridTool*       tool )
{
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi:Hybrid::EngineActor::releaseTool(): Invalid LoKi::IHybridTool" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::Cuts&    cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::VCuts&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process 
( const std::string&          name , 
  const LoKi::Types::ACuts&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::Func&    cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::VFunc&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::AFunc&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
