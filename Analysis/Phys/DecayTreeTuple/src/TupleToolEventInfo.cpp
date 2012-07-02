// $Id: TupleToolEventInfo.cpp,v 1.8 2010-06-02 12:01:44 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/ILHCbMagnetSvc.h"

// local
#include "TupleToolEventInfo.h"

#include "Event/ODIN.h" // event & run number
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/ITupleTool.h"
#include "Event/RecVertex.h"
#include "Event/VertexBase.h"
#include "Event/Track.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolEventInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolEventInfo::TupleToolEventInfo( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : TupleToolBase ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  declareProperty("InputLocation", 
                  m_pvLocation = LHCb::RecVertexLocation::Primary,
                  "PV location to be used. If empty, take default");
  declareProperty("Mu", m_mu);
}

//=============================================================================
StatusCode TupleToolEventInfo::initialize( )
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  m_magSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
  return sc;
}

//=============================================================================
StatusCode TupleToolEventInfo::fill( Tuples::Tuple& tuple )
{
  const std::string prefix = fullName();

  // Load the ODIN
  LHCb::ODIN* odin(NULL);
  if( exist<ODIN>( LHCb::ODINLocation::Default ) )
  {
    odin = get<ODIN>( LHCb::ODINLocation::Default );
  }
  else if( exist<ODIN>( LHCb::ODINLocation::Default, false ) )
  {
    odin = get<ODIN>( LHCb::ODINLocation::Default, false );
  }
  else
  {
    // should always be available ...
    return Error( "Cannot load the ODIN data object", StatusCode::SUCCESS );
  }

  // Number PVs
  unsigned int nPVs = 0 ;
  if (exist<RecVertex::Container>(m_pvLocation)) 
    nPVs = (get<RecVertex::Container>(m_pvLocation))->size();

  LHCb::L0DUReport* report = NULL;
  if(exist<L0DUReport>( LHCb::L0DUReportLocation::Default) )
  {
    report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
  }
  else if(exist<L0DUReport>( LHCb::L0DUReportLocation::Default, false) )
  {
    report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default,false);
  }
  else
  {
    Warning("Can't get LHCb::L0DUReportLocation::Default (" +
            LHCb::L0DUReportLocation::Default + ")").ignore();
  }

  LHCb::HltDecReports* decreport = NULL;
  if(exist<HltDecReports>( LHCb::HltDecReportsLocation::Default) )
  {
    decreport = get<LHCb::HltDecReports>(LHCb::HltDecReportsLocation::Default);
  }
  else if(exist<HltDecReports>( LHCb::HltDecReportsLocation::Default, false) )
  {
    decreport = get<LHCb::HltDecReports>(LHCb::HltDecReportsLocation::Default, false);
  }
  else
  {
    Warning("Can't get LHCb::HltDecReportsLocation::Default (" +
            LHCb::HltDecReportsLocation::Default + ")").ignore();
  }

  bool test = true;

  // Fill the tuple

  if (!m_mu.empty()) test &= tuple->column( prefix+"Mu", m_mu[odin->runNumber()] );

  test &= tuple->column( prefix+"runNumber",    odin->runNumber() );
  test &= tuple->column( prefix+"eventNumber",  odin->eventNumber() );
  test &= tuple->column( prefix+"BCID",         odin->bunchId() );
  test &= tuple->column( prefix+"BCType",       odin->bunchCrossingType() );
  test &= tuple->column( prefix+"OdinTCK",      odin->triggerConfigurationKey() );
  test &= tuple->column( prefix+"L0DUTCK",      report ? report->tck() : 0 );
  test &= tuple->column( prefix+"HLTTCK",       decreport ? decreport->configuredTCK() : 0 );
  test &= tuple->column( prefix+"GpsTime",      odin->gpsTime() );
  if ( isVerbose() )
  {
    const Gaudi::Time gtime = odin->eventTime();
    test &= tuple->column( prefix+"GpsYear",  gtime.year(false) );
    test &= tuple->column( prefix+"GpsMonth", gtime.month(false) );
    test &= tuple->column( prefix+"GpsDay", gtime.day(false) );
    test &= tuple->column( prefix+"GpsHour", gtime.hour(false) );
    test &= tuple->column( prefix+"GpsMinute", gtime.minute(false) );
    test &= tuple->column( prefix+"GpsSecond", gtime.second(false)+gtime.nsecond()/1000000000. );
    test &= tuple->column( prefix+"TriggerType", odin->triggerType() );
  }
  test &= tuple->column( prefix+"Primaries", nPVs );
  if (m_magSvc) test &= tuple->column( prefix+"Polarity", (short)(m_magSvc->isDown()?-1:1) );

  if( msgLevel( MSG::VERBOSE ) ) verbose() << "Returns " << test << endreq;
  return StatusCode(test);
}
