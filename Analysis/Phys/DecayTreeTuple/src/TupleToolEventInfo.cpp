// $Id: TupleToolEventInfo.cpp,v 1.7 2010-04-28 15:24:27 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolEventInfo.h"

#include "Event/ODIN.h" // event & run number
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/ITupleTool.h"


//#include "GaudiAlg/TupleObj.h"
// #include "GaudiAlg/GaudiTupleAlg.h"


#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolEventInfo );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolEventInfo::TupleToolEventInfo( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : TupleToolBase ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);

}

//=============================================================================

StatusCode TupleToolEventInfo::fill( Tuples::Tuple& tuple ) 
{
  const std::string prefix=fullName();
  
 
  int run = -1;
  int ev = -1;
  int bcid = -1;
  int bctyp = -1;
  unsigned int odintck = 0; 
  unsigned int l0dutck = 0;
  unsigned int hlttck = 0;
  ulonglong gpstime = 0;
  Gaudi::Time gtime ;

  LHCb::ODIN* odin(0);

  if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
    odin = get<ODIN>( LHCb::ODINLocation::Default );
    run = odin->runNumber();
    ev = odin->eventNumber();
    bcid = odin->bunchId();
    odintck = odin->triggerConfigurationKey();
    gpstime = odin->gpsTime();
    gtime = odin->eventTime();
    bctyp = odin->bunchCrossingType() ;
  } else {
    Error("Can't get LHCb::ODINLocation::Default (" +
	  LHCb::ODINLocation::Default + ")" );
  }

  if(exist<L0DUReport>( LHCb::L0DUReportLocation::Default) ) {
    LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
    l0dutck = report->tck();
  } else {
    Warning("Can't get LHCb::L0DUReportLocation::Default (" +
    LHCb::L0DUReportLocation::Default + ")");
  }

  if(exist<HltDecReports>( LHCb::HltDecReportsLocation::Default) ) {
    LHCb::HltDecReports* decreport = get<LHCb::HltDecReports>(LHCb::HltDecReportsLocation::Default);
    hlttck = decreport->configuredTCK();
  } else {
    Warning("Can't get LHCb::HltDecReportsLocation::Default (" +
    LHCb::HltDecReportsLocation::Default + ")");
  }

  if( msgLevel( MSG::DEBUG ) )
    debug() << "Event " << ev << ", run " << run << endreq;

  bool test = true;
  test &= tuple->column( prefix+"runNumber", run );
  test &= tuple->column( prefix+"eventNumber", ev );
  test &= tuple->column( prefix+"BCID", bcid );
  test &= tuple->column( prefix+"BCType", bctyp );
  test &= tuple->column( prefix+"OdinTCK", odintck );
  test &= tuple->column( prefix+"L0DUTCK", l0dutck );
  test &= tuple->column( prefix+"HLTTCK", hlttck );
  test &= tuple->column( prefix+"GpsTime", (double)gpstime );
  if ( isVerbose()){
    test &= tuple->column( prefix+"GpsYear",  gtime.year(false) );
    test &= tuple->column( prefix+"GpsMonth", gtime.month(false) );
    test &= tuple->column( prefix+"GpsDay", gtime.day(false) );
    test &= tuple->column( prefix+"GpsHour", gtime.hour(false) );
    test &= tuple->column( prefix+"GpsMinute", gtime.minute(false) );
    test &= tuple->column( prefix+"GpsSecond", gtime.second(false)+gtime.nsecond()/1000000000. );
  }
  if( msgLevel( MSG::VERBOSE ) )
    verbose() << "Returns " << test << endreq;
  return StatusCode(test);
}
