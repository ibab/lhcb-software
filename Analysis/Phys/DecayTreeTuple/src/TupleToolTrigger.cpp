// $Id: TupleToolTrigger.cpp,v 1.5 2008-06-25 12:18:26 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTrigger.h"

#include "Event/L0DUReport.h"
#include "Kernel/IHltSummaryTool.h"
#include "Kernel/IANNSvc.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"


//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTrigger );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTrigger::TupleToolTrigger( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_summaryTool(0)
{
  declareInterface<IEventTupleTool>(this);
  declareProperty( "VerboseL0",   m_verboseL0=false );
  declareProperty( "VerboseHlt1", m_verboseHlt1=false );
  declareProperty( "VerboseHlt2", m_verboseHlt2=false );
  declareProperty( "FillL0", m_fillL0=true );
  declareProperty( "FillHlt", m_fillHlt=true );
  declareProperty( "Hlt1MajorKey", m_hlt1MajorKey = "Hlt1SelectionID"); 
  declareProperty( "Hlt2MajorKey", m_hlt2MajorKey = "Hlt2SelectionID");

}

//=========================================================================
//  
//=========================================================================
StatusCode TupleToolTrigger::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_summaryTool = tool<IHltSummaryTool>("HltSummaryTool",this);


  return StatusCode::SUCCESS ;
}
//=============================================================================
StatusCode TupleToolTrigger::fill( Tuples::Tuple& tuple ) {
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger" << endmsg ;
  
  if (m_fillL0)
  	if (!fillL0(tuple)) return StatusCode::FAILURE ;
  if (m_fillHlt)
  	if (!fillHlt(tuple)) return StatusCode::FAILURE ;
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode TupleToolTrigger::fillL0( Tuples::Tuple& tuple ) {
  if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ){
    LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
    if ( ! tuple->column( "L0Decision", report->decision() ) ) return StatusCode::FAILURE;
    if (msgLevel(MSG::DEBUG)) debug() << "L0 decision:  " << report->decision() << endreq;
    if ( m_verboseL0 ){
      LHCb::L0DUChannel::Map channels = report->configuration()->channels();
      for(LHCb::L0DUChannel::Map::const_iterator it = channels.begin();
          it!=channels.end();it++){
        if ( ! tuple->column( "L0_"+(*it).first , 
                              report->channelDecision( ((*it).second)->id() )))
          return StatusCode::FAILURE;
        if (msgLevel(MSG::VERBOSE)) verbose() << (*it).first << " : " 
                    << report->channelDecision( ((*it).second)->id() ) << endreq;
      } 
    }
  } else {
    Warning("Can't get LHCb::L0DUReportLocation::Default (" +
	    LHCb::L0DUReportLocation::Default + ")" );
    if ( ! tuple->column( "L0Decision", -1 ) ) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode TupleToolTrigger::fillHlt( Tuples::Tuple& tuple ) {
  // Hlt

  if( !tuple->column( "HltDecision", m_summaryTool->decision() )) return StatusCode::FAILURE;

  StatusCode sc = StatusCode::SUCCESS ;

  if ( m_verboseHlt1)        sc = fillHlt(tuple,m_hlt1MajorKey);
  if ( sc && m_verboseHlt2 ) sc = fillHlt(tuple,m_hlt2MajorKey);
  return sc ;
}
//============================================================================
StatusCode TupleToolTrigger::fillHlt( Tuples::Tuple& tuple, const std::string & major ) {
  std::vector<std::string> names = svc<IANNSvc>("HltANNSvc")->names(major);
  for (std::vector<std::string>::const_iterator s = names.begin() ; s!=names.end() ; ++s){
    if ( ! tuple->column(*s, m_summaryTool->selectionDecision(*s) ) ) 
      return StatusCode::FAILURE;
    if (msgLevel(MSG::VERBOSE)) verbose() << major << " :: " << *s << " says " 
                                          << m_summaryTool->selectionDecision(*s) << endmsg ; 
  }
  return StatusCode::SUCCESS;
}
