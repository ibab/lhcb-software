// $Id: EventTuple.cpp,v 1.4 2009-06-01 15:40:27 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"

#include "Kernel/IEventTupleTool.h"            // Interface

// local
#include "EventTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventTuple
//
// 2008-07-01 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventTuple )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventTuple::EventTuple( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  m_toolList.push_back( "TupleToolEventInfo" );
  m_collectionName = name + "/ETC" ;
  declareProperty("ToolList", m_toolList, "List of tools to be used" );
  declareProperty("TupleName", m_tupleName="EventTuple", "Name of nTuple" );
  declareProperty("CollectionName", m_collectionName = "",
                  "Default is evtColPath()");
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
    m_tools.push_back(tool<IEventTupleTool>(*s,this));
    info() << *s << ", " ;
  }
  info() << endmsg ;

  if (produceEvtCols ()) {
    if (m_collectionName == ""){
      m_collectionName =  evtColPath() ;
    }
    info() << "Will be writing an ETC with name " << m_collectionName
           << "/" << m_tupleName << endmsg ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventTuple::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  counter("Event")++;
  StatusCode sc = StatusCode::SUCCESS ;

  Tuple tuple = ( produceEvtCols () ? evtCol(m_tupleName,m_collectionName) : nTuple( m_tupleName ));
  if (msgLevel(MSG::VERBOSE)) verbose() << "Got tuple" << endmsg ;
  if (produceEvtCols()){
    // pick up the location of the event --
    // this is what makes the tag collection a collection...
    DataObject* pObject = get<DataObject>("/Event");
    if (0!=pObject) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Filling Address " << endmsg ;
      sc = tuple->column("Address", pObject->registry()->address() );
      if (!sc) {
        err() << "Error writing address" << endmsg ;
        return sc;
      }
    }
    else {
      Error("    not able to retrieve IOpaqueAddress");
      return StatusCode::FAILURE ;
    }
  }

  tuple->column( "EventInSequence", counter("Event").nEntries()).ignore();
  for ( std::vector<IEventTupleTool*>::iterator i = m_tools.begin() ; i!= m_tools.end() ; ++i){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Filling " << (*i)->name() << endmsg ;
    sc = (*i)->fill(tuple);
    if (msgLevel(MSG::VERBOSE)) verbose() << (*i)->name() << " returns " << sc << endmsg ;
    if (!sc) return sc;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Writing tuple" << endmsg ;
  return tuple->write();
}

//=============================================================================
