// $Id: TupleToolTrigger.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTrigger.h"

#include "Event/L0DUReport.h"

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
{
  declareInterface<IEventTupleTool>(this);
  declareProperty( "VerboseL0", m_verboseL0=false );
}

//=============================================================================

StatusCode TupleToolTrigger::fill( Tuples::Tuple& tuple ) {
  int dec = -1;
  LHCb::L0DUReport* report = 0;

  if( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ){
    report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
    dec = report->decision();
    debug() << "L0 decsion:  " << dec << endreq;
  } else {
    Warning("Can't get LHCb::L0DUReportLocation::Default (" +
	    LHCb::L0DUReportLocation::Default + ")" );
  }

  if( ! tuple->column( "L0Decision", dec ) ) return StatusCode::FAILURE;

  /*
  if( m_verboseL0 && report ){
    std::vector<std::string> names = report->channelNames();
    for( std::vector<std::string>::const_iterator it = names.begin();
	 names.end()!=it; ++it ){
      int d=-1;
      d = report->channelDecisionByName( *it );
      std::string n = "L0Decision_" + *it;
      if( !tuple->column( n, d ) ) return StatusCode::FAILURE;
    }
  }
  */
  return StatusCode::SUCCESS;
}
