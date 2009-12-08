// $Id: TupleToolEventInfo.cpp,v 1.4 2009-12-08 23:03:47 gligorov Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolEventInfo.h"

#include "Event/ODIN.h" // event & run number

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
  : GaudiTool ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);

}

//=============================================================================

StatusCode TupleToolEventInfo::fill( Tuples::Tuple& tuple ) {
  int run = -1;
  int ev = -1;
  int bcid = -1;

  LHCb::ODIN* odin(0);

  if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
    odin = get<ODIN>( LHCb::ODINLocation::Default );
    run = odin->runNumber();
    ev = odin->eventNumber();
    bcid = odin->bunchId();
  } else {
    Error("Can't get LHCb::ODINLocation::Default (" +
	  LHCb::ODINLocation::Default + ")" );
  }

  if( msgLevel( MSG::DEBUG ) )
    debug() << "Event " << ev << ", run " << run << endreq;

  bool test = true;
  test &= tuple->column( "runNumber", run );
  test &= tuple->column( "eventNumber", ev );
  test &= tuple->column( "BCID", bcid );
  if( msgLevel( MSG::VERBOSE ) )
    verbose() << "Returns " << test << endreq;
  return StatusCode(test);
}
