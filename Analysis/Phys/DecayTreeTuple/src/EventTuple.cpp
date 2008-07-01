// $Id: EventTuple.cpp,v 1.1 2008-07-01 14:50:02 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Kernel/IEventTupleTool.h"            // Interface

// local
#include "EventTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventTuple
//
// 2008-07-01 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventTuple );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventTuple::EventTuple( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  m_toolList.push_back( "TupleToolEventInfo" );
  declareProperty("ToolList", m_toolList );
  declareProperty( "TupleName", m_tupleName="DecayTree" );
 }
//=============================================================================
// Destructor
//=============================================================================
EventTuple::~EventTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventTuple::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  info() << "Tools to be used : " ;
  for (std::vector<std::string>::const_iterator s = m_toolList.begin() ; s!=m_toolList.end() ; ++s){
    m_tools.push_back(tool<IEventTupleTool>(*s));
    info() << *s << ", " ;
  }
  info() << endmsg ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventTuple::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Tuple tuple = nTuple( m_tupleName, m_tupleName );
  
  StatusCode sc = StatusCode::SUCCESS ;
  for ( std::vector<IEventTupleTool*>::iterator i = m_tools.begin() ; i!= m_tools.end() ; ++i){
    sc = (*i)->fill(tuple);
    if (!sc) return sc;
  }
  return tuple->write();

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventTuple::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
