// $Id: TupleToolTrigger.cpp,v 1.20 2010-07-07 17:14:47 tskwarni Exp $
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
  : TupleToolTriggerBase ( type, name , parent )
  , m_routingBits(0)
{
  declareInterface<IEventTupleTool>(this);
  //declareProperty( "FillL0", m_fillL0=true, "Fill L0" ); now in the base class
  //declareProperty( "FillHlt", m_fillHlt=true, "Fill Hlt" ); now in the base class
  //depracated, use VerboseHlt1, VerboseHlt2 or VerboseL0
  //declareProperty( "AllIntermediateSteps", m_allSteps=false, "Fill also intermediate steps" );
  for ( unsigned int i = 32 ; i < 96 ; i++)
  {
    m_routingBits.push_back(i);
  }
  declareProperty( "RoutingBits", m_routingBits, "Routing bits to fill" );
  
}

//=========================================================================
//  
//=========================================================================
StatusCode TupleToolTrigger::initialize ( ) {
  StatusCode sc = TupleToolTriggerBase::initialize();
  
  if (m_doStripping || m_stripping.size()!=0) 
  {
    return Error("You should use TupleToolStripping for that", StatusCode::FAILURE);
  }
  
  
  return sc ;
}
//=============================================================================
StatusCode TupleToolTrigger::fillBasic( Tuples::Tuple& tuple ) {
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger Basic" << endmsg ;
  
  const std::string prefix=fullName();
  //fill the L0 global
  if (m_doL0)
  {    
    if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) )
    {
      LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
      if ( ! tuple->column( prefix+"L0Global", report->decision() ) ) return StatusCode::FAILURE;
      if (msgLevel(MSG::DEBUG)) debug() << "L0 decision:  " << report->decision() << endreq;
    } else if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default, false) )
    {
      LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default, false);
      if ( ! tuple->column( prefix+"L0Global", report->decision() ) ) return StatusCode::FAILURE;
      if (msgLevel(MSG::DEBUG)) debug() << "L0 decision:  " << report->decision() << endreq;
    } 
  }
  
  //fill the HLT global
  if (m_doHlt1 || m_doHlt2)
  {
    if( exist<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default ) )
    { 
      const LHCb::HltDecReports* decReports = 
        get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
      //fill the HLT1 global
      if( !tuple->column( prefix+"Hlt1Global", (decReports->decReport("Hlt1Global"))? 
                          (decReports->decReport("Hlt1Global")->decision()):0 )) return StatusCode::FAILURE;
      
      //fill the HLT2 global
      if( !tuple->column( prefix+"Hlt2Global", (decReports->decReport("Hlt2Global"))? 
                          (decReports->decReport("Hlt2Global")->decision()):0 )) return StatusCode::FAILURE; 
    } else if( exist<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default, false ) )
    { 
      const LHCb::HltDecReports* decReports = 
        get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default, false );
      //fill the HLT1 global
      if( !tuple->column( prefix+"Hlt1Global", (decReports->decReport("Hlt1Global"))? 
                          (decReports->decReport("Hlt1Global")->decision()):0 )) return StatusCode::FAILURE;
      
      //fill the HLT2 global
      if( !tuple->column( prefix+"Hlt2Global", (decReports->decReport("Hlt2Global"))? 
                          (decReports->decReport("Hlt2Global")->decision()):0 )) return StatusCode::FAILURE; 
    } 
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "Done" << endmsg ;
  
  return StatusCode::SUCCESS;
}
//=============================================================================

//=============================================================================
StatusCode TupleToolTrigger::fillVerbose( Tuples::Tuple& tuple ) 
{
  bool test=true;
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger Verbose" << endmsg ;
  
  //fill the L0 verbose
  if (m_doL0 && m_verboseL0)
  {
    test&=fillL0(tuple);
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger L0 " << test << endmsg ;
  
  //fill the HLT1 verbose
  if (m_doHlt1 && m_verboseHlt1)
  {
    test&=fillHlt(tuple, "Hlt1");
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger Hlt1 " << test << endmsg ;
  
  //fill the HLT2 verbose
  if (m_doHlt2 && m_verboseHlt2)
  {
    test&=fillHlt(tuple, "Hlt2");
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger Hlt2 " << test << endmsg ;

  test &= fillRoutingBits(tuple);
  if (msgLevel(MSG::DEBUG)) debug() << "Tuple Tool Trigger RoutingBits " << test << endmsg ;

  if (!test) Warning("Failure from Trigger::fillVerbose");
  return StatusCode(test);
}

//=============================================================================
StatusCode TupleToolTrigger::fillL0( Tuples::Tuple& tuple )
{
  return fillHlt(tuple,"L0");
}
//============================================================================
StatusCode TupleToolTrigger::fillHlt( Tuples::Tuple& tuple, const std::string & level)
{
  const std::string prefix=fullName();
  
  std::string loca = LHCb::HltDecReportsLocation::Default;
  if( level == "L0" )loca="HltLikeL0/DecReports";
  bool userootintes = exist<LHCb::HltDecReports>( loca );
  if( exist<LHCb::HltDecReports>( loca, userootintes ) )
  { 
    const LHCb::HltDecReports* decReports = get<LHCb::HltDecReports>( loca, userootintes );
    
    unsigned int nsel = 0 ;
    
    std::vector<std::string> names = std::vector<std::string>(0);
    
    if(level=="L0") names=m_l0;
    else if(level=="Hlt1") names=m_hlt1;
    else if(level=="Hlt2") names=m_hlt2;
    else return StatusCode::FAILURE;
    
    for ( std::vector<std::string>::const_iterator n = names.begin() ; n!= names.end() ; ++n)
    {
      int found = -1 ;
      if (msgLevel(MSG::DEBUG))  debug() << " In verbose HLT, Trying to fill  " << (*n)   << endmsg; 
      // individual Hlt trigger lines
      for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin();
          it!=decReports->end();++it)
      {
        if (msgLevel(MSG::VERBOSE))  verbose() << " Hlt trigger name= " << it->first  << endmsg;
        if ( ( it->first == *n ) )
        {
          if (msgLevel(MSG::DEBUG))  debug() << " Hlt trigger name= " << it->first  
                                             << " decision= " << it->second.decision() << endmsg;
          found = it->second.decision() ;
          //only print all the names if output level is verbose otherwise this is a waste of time
          if(!msgLevel(MSG::VERBOSE)) break;
        }
      }
      if (msgLevel(MSG::VERBOSE)) verbose() << "Added " << *n << " " << found 
                                            << " to " << nsel << endmsg ;
      bool isDecision = ( n->find("Decision") == n->length()-8  ) ; // 8 is length of Decision
      if (isDecision && found>0) nsel++ ;
      //if (isDecision || m_allSteps)
      //{
      if ( ! tuple->column(prefix+*n, found ) ) return StatusCode::FAILURE;
      //}
    }
    if ( ! tuple->column(prefix+level+"nSelections" , nsel ) ) return StatusCode::FAILURE;
  }
  else return Warning("No HltDecReports at "+loca,StatusCode::FAILURE,2);
  if (msgLevel(MSG::DEBUG)) debug() << "Done " << prefix+level << endmsg ;
  return StatusCode::SUCCESS ;
}
//============================================================================
StatusCode TupleToolTrigger::fillRoutingBits( Tuples::Tuple& tuple ) 
{
  const std::string prefix=fullName();
  if (msgLevel(MSG::DEBUG)) debug() << "RoutingBits" << endmsg ;
  bool userootintes = exist<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  if (exist<LHCb::RawEvent>(LHCb::RawEventLocation::Default, userootintes))
  {
    LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default, userootintes);
    std::vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,m_routingBits);
    if (msgLevel(MSG::DEBUG)) debug() << yes << endmsg ;
    if (!tuple->farray(prefix+"RoutingBits", yes, prefix+"MaxRoutingBits" , m_routingBits.size() )){
      Warning("Failure to fill routing bits");
      return StatusCode::FAILURE ;
    }
    if (msgLevel(MSG::DEBUG)) debug() << "RoutingBits OK " << endmsg ;
  } else {
    userootintes = exist<LHCb::RawEvent>(LHCb::RawEventLocation::Trigger);
    if (exist<LHCb::RawEvent>(LHCb::RawEventLocation::Trigger, userootintes))
    {
      LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default, userootintes);
      std::vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,m_routingBits);
      if (msgLevel(MSG::DEBUG)) debug() << yes << endmsg ;
      if (!tuple->farray(prefix+"RoutingBits", yes, prefix+"MaxRoutingBits" , m_routingBits.size() )){
        Warning("Failure to fill routing bits");
        return StatusCode::FAILURE ;
      }
      if (msgLevel(MSG::DEBUG)) debug() << "RoutingBits OK " << endmsg ;
    }
  }
  return StatusCode::SUCCESS ;
}
