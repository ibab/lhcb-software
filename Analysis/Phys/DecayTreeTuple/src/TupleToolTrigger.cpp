// $Id: TupleToolTrigger.cpp,v 1.4 2008-05-04 18:53:45 gligorov Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTrigger.h"

#include "Event/L0DUReport.h"
//#include "Event/HltSummary.h"
#include "Event/HltEnums.h"
#include "Kernel/IHltSummaryTool.h"

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
  declareProperty( "VerboseL0",         m_verboseL0=false );
  declareProperty( "VerboseAlleys",     m_verboseAlleys=false );
  declareProperty( "VerboseSelections", m_verboseSelections=false );
  declareProperty( "FillL0", m_fillL0=true );
  declareProperty( "FillHlt", m_fillHLT=false );
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
  if (m_fillHLT)
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
        if (msgLevel(MSG::VERBOSE)) 
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
  // HLT

  if( !tuple->column( "HltDecision", m_summaryTool->decision() )) return StatusCode::FAILURE;

  /// @todo need to add more granularity in the alleys. 

  if ( m_verboseAlleys){
    std::vector<std::string> sels = m_summaryTool->confStringVector("HltAlleys/InputSelections");
    
    for ( std::vector<std::string>::const_iterator i = sels.begin() ; i!=sels.end() ; ++i){
      if ( ! tuple->column( "Hlt"+(*i), m_summaryTool->selectionDecision(*i) ) ) return StatusCode::FAILURE;
      if (msgLevel(MSG::VERBOSE)) verbose() << *i << " says " << m_summaryTool->selectionDecision(*i) << endmsg ;
    }
  }
  
  if ( m_verboseSelections){
    for ( int i = LHCb::HltEnums::HltSelEntry ; i!= LHCb::HltEnums::HltSelLastSelection ; ++i){
      std::string s = LHCb::HltEnums::HltSelectionSummaryEnumToString(i) ;
      if ( ! tuple->column( s, m_summaryTool->selectionDecision(s) ) ) 
        return StatusCode::FAILURE;
      if (msgLevel(MSG::VERBOSE)) verbose() << s << " says " 
                                            << m_summaryTool->selectionDecision(s) << endmsg ; 
    } 
  }
  return StatusCode::SUCCESS;
}

