// $Id: TupleToolEventInfo.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
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
  , m_ttool(0)
{
  declareInterface<IEventTupleTool>(this);

  declareProperty( "ETCTupleName", m_etcTupleName="ETC" );
}

//=============================================================================

StatusCode TupleToolEventInfo::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_ttool = tool<ITupleTool>( "TupleTool", this );
  if( !m_ttool ){
    Error("Can't initialize the ITupleTool tool");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolEventInfo::fill( Tuples::Tuple& tuple ) {
  int run = -1;
  int ev = -1;

  LHCb::ODIN* odin(0);

  if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
    odin = get<ODIN>( LHCb::ODINLocation::Default );
    run = odin->runNumber();
    ev = odin->eventNumber();
  } else {
    Error("Can't get LHCb::ODINLocation::Default (" +
	  LHCb::ODINLocation::Default + ")" );
  }

  if( msgLevel( MSG::DEBUG ) )
    debug() << "Event " << ev << ", run " << run << endreq;

  bool test = true;
  test &= tuple->column( "runNumber", run );
  test &= tuple->column( "eventNumber", ev );

//   if( test ){
//     DataObject* pObject = get<DataObject>("/Event");
//     if (pObject!=0 && m_ttool ) {
//       std::cout << "BOOKING ETC" << std::endl;
//       Tuples::Tuple etc = m_ttool->evtCol( Tuples::TupleID(m_etcTupleName)
// 					   , m_etcTupleName );
//       //      Tuple etc = nTuple("bob");//evtCol( m_etcTupleName, m_etcTupleName );
//       std::cout << "address : " << pObject->registry()->address() << std::endl;
//       test &= etc->column( "Address", pObject->registry()->address() );
//       std::cout << "test " << std::boolalpha << test << std::endl;
//       test &= etc->column( "runNumber", run );
//       std::cout << "test " << std::boolalpha << test << std::endl;
//       test &= etc->column( "eventNumber", ev );
//       test &= etc->write();       
//       std::cout << "test " << std::boolalpha << test << std::endl;
//     } else {
//       Error("not able to retrieve IOpaqueAddress");
//       return StatusCode::FAILURE;
//     }
//   }
//  std::cout << "RETURNS " <<std::boolalpha<< test  << std::endl;
  return StatusCode(test);
}
