// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackEngineActor.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hybrid::TrackEngineActor
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
inline StatusCode LoKi::Hybrid::TrackEngineActor::_add 
( const std::string& name , const TYPE& cut ) const 
{ 
  // check the tool
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::TrackEngineActor::addCut/Fun(): LoKi::ITrackFunctorAntiFactory* is not connected!") ;  
  }
  // one more check 
  if ( name != m_tool->name() )
  {
    return LoKi::Report::Error
      ("LoKi::Hybrid::TrackEngineActor::addCut/Fun(): mismatch in LoKi::ITrackFunctorAntiFactory name!" ) ;  
  }
  // set the cut for the tool 
  m_tool->set ( cut ) ;
  // 
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// accessor to the static instance 
// ============================================================================
LoKi::Hybrid::TrackEngineActor& 
LoKi::Hybrid::TrackEngineActor::instance() 
{
  static LoKi::Hybrid::TrackEngineActor s_holder ;
  return s_holder ;
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hybrid::TrackEngineActor::TrackEngineActor() : m_tool ( 0 ) {} ;
// ============================================================================
// destructor
// ============================================================================
LoKi::Hybrid::TrackEngineActor::~TrackEngineActor() {} ;
// ============================================================================
// disconnect the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::releaseTool 
( const LoKi::ITrackFunctorAntiFactory* tool ) 
{
  if ( m_tool.getObject() != tool ) 
  {
    m_tool = 0 ;
    return LoKi::Report::Error
      ("LoKi:Hybrid::TrackEngineActor::releaseTool(): mismatch in tools " ) ;
  } 
  // nullify the pointer 
  m_tool = 0 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// connect the hybrid tool for code translation 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::connectTool 
(       LoKi::ITrackFunctorAntiFactory* tool )
{
  // substitute the tool 
  m_tool =  tool ;
  // 
  if ( !m_tool.validPointer() ) 
  {
    return LoKi::Report::Error
      ( "LoKi::Hybrid::TrackEngineActor::releaseTool(): Invalid LoKi::ITrackFunctorAntiFactory" ) ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// propagate the cut to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TrCuts&  cut  ) const { return _add ( name , cut  ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&          name , 
  const LoKi::Types::TrFunc&  func ) const { return _add ( name , func ) ; }
// ============================================================================
// functional part 
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrMaps&     func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrPipes&    func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrFunVals&  func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrElements& func ) const 
{ return _add ( name , func ) ; }
// ============================================================================
// propagate the function to the tool 
// ============================================================================
StatusCode LoKi::Hybrid::TrackEngineActor::process
( const std::string&             name , 
  const LoKi::Types::TrSources&  func ) const 
{ return _add ( name , func ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
