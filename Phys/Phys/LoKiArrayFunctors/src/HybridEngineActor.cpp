// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <fstream>
#include <sstream>
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
  // ==========================================================================
  // check the tool
  if ( !m_tool.top().validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::EngineActor::addCut/Fun(): LoKi::IHybridTool* is not connected!") ;  
  }
  // ==========================================================================
  // one more check 
  if ( name != m_tool.top()->name() )
  {
    return LoKi::Report::Error
      ("LoKi:Hybrid::EngineActor::addCut/Fun() : mismatch in LoKi::IHybridTool name!") ;  
  }
  // ==========================================================================
  // set the cut for the tool 
  m_tool.top() -> set ( cut ) ;
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
LoKi::Hybrid::EngineActor::EngineActor() : m_tool (  ) {} 
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::EngineActor::~EngineActor() {} 
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::releaseTool 
( const LoKi::IHybridTool*       tool ) 
{
  
  if ( m_tool.top().getObject() != tool ) 
  {
    m_tool.pop() ;
    return LoKi::Report::Error
      ("LoKi:Hybrid::EngineActor::releaseTool(): mismatch in tools " ) ;
  } 
  // nullify the pointer 
  m_tool.pop();

  // check if there are still tools on the stack
  if(m_tool.size()>0){
    std::stringstream msg;
    msg << "LoKi:Hybrid::EngineActor::releaseTool():  tool stack not empty after release.\n"
	<< " Number of tools left on stack: " << m_tool.size() << "   Next tool on stack: " << m_tool.top()->name() << std::endl
	<< " This is expected if you are using a chain of tools.";
    return LoKi::Report::Warning(msg.str(),StatusCode::SUCCESS) ;
  }
  //
  return LoKi::Report::Warning("LoKi:Hybrid::EngineActor::releaseTool(): Stack empty after release. All is well.",StatusCode::SUCCESS) ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::connectTool 
(       LoKi::IHybridTool*       tool )
{
  //
  //LoKi::Report::Assert
  //  ( !m_tool.validPointer() , "LoKi:Hybrid::EngineActor:: double lock?" ) ;
  //
  // put the tool on the stack
  m_tool.push(tool);
 
    //m_tool =  tool ;
  // 
  if ( !m_tool.top().validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi:Hybrid::EngineActor::connectTool(): Invalid LoKi::IHybridTool" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// predicates 
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
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process 
( const std::string&          name , 
  const LoKi::Types::PPCuts&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// functions 
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
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::PPFunc&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// maps
// ============================================================================
// propagate the map  to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::Maps&    cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the map to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::VMaps&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the map to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::PPMaps&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// pipes
// ============================================================================
// propagate the pipe to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::Pipes&   cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the pipe to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::VPipes&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the pipe to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::PPPipes& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// fun-vals 
// ============================================================================
// propagate the fun-val  to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::FunVals& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the fun-val to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&           name , 
  const LoKi::Types::VFunVals& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the fun-val to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&            name , 
  const LoKi::Types::PPFunVals& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// cut-vals 
// ============================================================================
// propagate the fun-val  to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&          name , 
  const LoKi::Types::CutVals& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the fun-val to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&           name , 
  const LoKi::Types::VCutVals& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the fun-val to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&            name , 
  const LoKi::Types::PPCutVals& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// sources
// ============================================================================
// propagate the source to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&           name , 
  const LoKi::Types::Sources&  cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the source to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&           name , 
  const LoKi::Types::VSources& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================
// propagate the source to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::EngineActor::process
( const std::string&            name , 
  const LoKi::Types::PPSources& cut  ) const { return _add ( name , cut ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
