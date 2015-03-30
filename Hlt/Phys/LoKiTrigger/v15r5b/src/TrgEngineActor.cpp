// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrgEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::TrgEngineActor
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
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// helper method to descrease number of lines 
// ============================================================================
template <class TYPE>
inline StatusCode LoKi::Hybrid::TrgEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::TrgEngineActor::addCut/Fun(): LoKi::ITrgFunctorAntiFactory* is not connected!") ;  
  }
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::TrgEngineActor::addCut/Fun(): mismatch in LoKi::ITrgFunctorAntiFactory name!" ) ;  
  }
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::TrgEngineActor& 
LoKi::Hybrid::TrgEngineActor::instance() 
{
  static LoKi::Hybrid::TrgEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::TrgEngineActor::TrgEngineActor() : m_tool ( 0 ) {} 
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::TrgEngineActor::~TrgEngineActor() {} 
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::releaseTool 
( const LoKi::ITrgFunctorAntiFactory* tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi:Hybrid::TrgEngineActor::releaseTool(): mismatch in tools " ) ;
  } 
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::connectTool 
(       LoKi::ITrgFunctorAntiFactory* tool )
{
  LoKi::Report::Assert 
    ( !m_tool.validPointer() ,
      "LoKi::Hybrid::TrgEngineActor: double lock?" ) ;
  //
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::TrgEngineActor::releaseTool(): Invalid LoKi::ITrgFunctorAntiFactory" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TC_Cuts&  cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TC_Func&  func ) const { return _add ( name , func ) ; }
// ============================================================================
// functional part 
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TC_Maps&     func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TC_Pipes&    func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TC_FunVals&  func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrgEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TC_Sources&  func ) const 
{ return _add ( name , func ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
