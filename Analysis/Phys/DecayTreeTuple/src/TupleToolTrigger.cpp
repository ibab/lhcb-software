// $Id: TupleToolTrigger.cpp,v 1.16 2009-11-06 13:25:53 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTrigger.h"

#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"
#include "Kernel/IANNSvc.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Kernel/ReadRoutingBits.h"

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
{
  declareInterface<IEventTupleTool>(this);
  declareProperty( "VerboseL0",   m_verboseL0=true, "Fill L0 details" );
  declareProperty( "VerboseHlt1", m_verboseHlt1=false, "Fill Hlt1 details" );
  declareProperty( "VerboseHlt2", m_verboseHlt2=false, "Fill Hlt2 details" );
  declareProperty( "FillL0", m_fillL0=true, "Fill L0" );
  declareProperty( "FillHlt", m_fillHlt=true, "Fill Hlt" );
  declareProperty( "AllIntermediateSteps", m_allSteps=false, "Fill also intermediate steps" );
  declareProperty( "FillGlobal", m_fillGlobal = false, "Fill Hlt1Global and Hlt2Global" );
  for ( unsigned int i = 32 ; i < 96 ; i++){
    m_routingBits.push_back(i);
  }
  declareProperty( "RoutingBits", m_routingBits, "Routing bits to fill" );
  
}

//=========================================================================
//  
//=========================================================================
StatusCode TupleToolTrigger::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if (!m_fillHlt && ( m_fillGlobal || m_verboseHlt1 || m_verboseHlt2 || !m_routingBits.empty() || m_allSteps) ){
    Error("Inconsistent options. Please switch FillHlt on");
    return StatusCode::FAILURE ;
  }  
  
  return sc ;
}
//=============================================================================
StatusCode TupleToolTrigger::fill( Tuples::Tuple& tuple ) {
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger" << endmsg ;
  
  if (m_fillL0)
  	if (!fillL0(tuple)) return StatusCode::FAILURE ;
  if (m_fillHlt){
    if (!(fillHlt(tuple,"Hlt1", m_verboseHlt1))) return StatusCode::FAILURE;
    if (!(fillHlt(tuple,"Hlt2", m_verboseHlt2))) return StatusCode::FAILURE; 
    if (!m_routingBits.empty()) if (!(fillRoutingBits(tuple))) return StatusCode::FAILURE; 
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Done" << endmsg ;
  
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
//============================================================================
StatusCode TupleToolTrigger::fillHlt( Tuples::Tuple& tuple, const std::string & level, bool individual ) {
  if( exist<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default ) ){ 
    const LHCb::HltDecReports* decReports = 
      get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
    if (m_fillGlobal) {
      if( !tuple->column( level+"Global", (decReports->decReport(level+"Global"))? 
                          (decReports->decReport(level+"Global")->decision()):0 )) return StatusCode::FAILURE;
    }
    if ( individual) {
      unsigned int nsel = 0 ;
      std::vector<std::string> names = svc<IANNSvc>("HltANNSvc")->keys(level+"SelectionID");
      for ( std::vector<std::string>::const_iterator n = names.begin() ; n!= names.end() ; ++n){
        bool found = false ;
        // individual Hlt trigger lines
        for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin();
            it!=decReports->end();++it){
          if ( ( it->first == *n ) ){
            if (msgLevel(MSG::DEBUG))  debug() << " Hlt trigger name= " << it->first  
                                               << " decision= " << it->second.decision() << endmsg;
            found = it->second.decision() ;
          }
        }
        if (msgLevel(MSG::VERBOSE)) verbose() << "Added " << *n << " " << found 
                                              << " to " << nsel << endmsg ;
        bool isDecision = ( n->find("Decision") == n->length()-8  ) ; // 8 is length of Decision
        if (isDecision && found) nsel++ ;
        if (isDecision || m_allSteps){
          if ( ! tuple->column(*n, found ) ) return StatusCode::FAILURE;
        }
      }
      if ( ! tuple->column(level+"nSelections" , nsel ) ) return StatusCode::FAILURE;
    }
  } else Warning("No HltDecReports at "+LHCb::HltDecReportsLocation::Default,StatusCode::FAILURE,1);
  if (msgLevel(MSG::DEBUG)) debug() << "Done " << level << endmsg ;
  return StatusCode::SUCCESS ;
}
//============================================================================
StatusCode TupleToolTrigger::fillRoutingBits( Tuples::Tuple& tuple ) {
  if (exist<LHCb::RawEvent>(LHCb::RawEventLocation::Default)){
    LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    std::vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,m_routingBits);
    if (!tuple->farray("RoutingBits", yes, "MaxRoutingBits" , m_routingBits.size() )) return StatusCode::FAILURE ;
  }
  return StatusCode::SUCCESS ;
}
