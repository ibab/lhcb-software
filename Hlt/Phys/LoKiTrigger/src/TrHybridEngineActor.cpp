// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrHybridEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::TrEngineActor
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
inline StatusCode LoKi::Hybrid::TrEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::TrEngineActor::addCut/Fun(): LoKi::ITrHybridTool* is not connected!") ;  
  }
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::TrEngineActor::addCut/Fun() : mismatch in LoKi::ITrHybridTool name!") ;  
  }
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::TrEngineActor& 
LoKi::Hybrid::TrEngineActor::instance() 
{
  static LoKi::Hybrid::TrEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::TrEngineActor::TrEngineActor() : m_tool ( 0 ) {} ;
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::TrEngineActor::~TrEngineActor() {} ;
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::releaseTool 
( const LoKi::ITrHybridTool*       tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi:Hybrid::TrEngineActor::releaseTool(): mismatch in tools " ) ;
  } ;
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::connectTool 
(       LoKi::ITrHybridTool*       tool )
{
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::TrEngineActor::releaseTool(): Invalid LoKi::ITrHybridTool" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TrCuts&  cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::RVCuts&  cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TTrCuts& cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TrVCuts& cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::RVVCuts& cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TrFunc&  func ) const { return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::RVFunc&  func ) const { return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TTrFunc& func ) const { return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TrVFunc& func ) const { return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&          name , 
  const LoKi::Types::RVVFunc& func ) const { return _add ( name , func ) ; }
// ============================================================================
// functional part 
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrMaps&     func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrPipes&    func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrFunVals&  func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrElements& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrSources&  func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// get the reference tracks    ( for Trigger/Hlt)
// ============================================================================
std::vector<LHCb::Track*>*     
LoKi::Hybrid::TrEngineActor::tracks   () const 
{
  // check the tool
  LoKi::Report::Assert
    ( m_tool.validPointer() ,
      "LoKi::Hybrid::TrEngineActor::tracks(): LoKi::ITrHybridTool* is not connected!") ;
  // use the tool 
  return m_tool->tracks() ;
}
// ============================================================================
// get the reference vertices  ( for Trigger/Hlt)
// ============================================================================
std::vector<LHCb::RecVertex*>* 
LoKi::Hybrid::TrEngineActor::vertices () const 
{
  // check the tool
  LoKi::Report::Assert
    ( m_tool.validPointer() ,
      "LoKi::Hybrid::TrEngineActor::vertices(): LoKi::ITrHybridTool* is not connected!") ;
  // use the tool 
  return m_tool->vertices() ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
