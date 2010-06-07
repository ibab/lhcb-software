
// Include files 
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// event model
#include "Event/EventCountFSR.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/LumiCounters.h"
// local
#include "GetIntegratedLuminosity.h"
//-----------------------------------------------------------------------------
// Implementation file for class : GetIntegratedLuminosity
//
// Yasmine Amhis 
// 
// 2010-05-26
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GetIntegratedLuminosity );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetIntegratedLuminosity::GetIntegratedLuminosity( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_nHandled    ( 0 ),
    m_count_input(0),
    m_count_output(0),
    m_incSvc(0)
{
  declareProperty( "WriteCountersDetails", m_countersDetails = false );
  declareProperty( "IntegratorToolName" , m_ToolName          = "LumiIntegrator" );  
  
}
//=============================================================================
// Destructor
//=============================================================================
GetIntegratedLuminosity::~GetIntegratedLuminosity() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetIntegratedLuminosity::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  // prepare tool
  m_integratorTool = tool<ILumiIntegrator>( "LumiIntegrator", m_ToolName );
  //prepare the FSR
  m_eventFSR = new LHCb::EventCountFSR();
  m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" , true );
  sc=service("IncidentSvc", m_incSvc, false);
  if(!sc.isSuccess() || m_incSvc== NULL) return StatusCode::FAILURE;
  m_incSvc->addListener( this, IncidentType::BeginEvent);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetIntegratedLuminosity::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_count_input++;
  m_eventFSR->setInput(m_count_input);
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetIntegratedLuminosity::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  StatusCode sc = StatusCode::SUCCESS ;
  
  std::string fromlumi = "FromLumi_";
  Tuple tuple = nTuple("LumiTuple");
  
  double  IntegratedLuminosity = -1;
  
 
  info() << "From Lumi: Number of input events  :" << m_eventFSR->input() << endmsg;
  info() << "From Lumi: Number of output events :" << m_eventFSR->output() << endmsg; 
  
  if( m_eventFSR->input() ==  m_eventFSR->output() ){
    IntegratedLuminosity = -16;
    info() << "From Lumi: For this set of data the integrated luminosity is : "  << IntegratedLuminosity << endmsg; 
    //store the integrated lumi
    tuple->column("IntegratedLuminosity", IntegratedLuminosity);
    LHCb::LumiIntegral::ValuePair defValue ( -1, 0 );
    for ( int key = 0; key < LHCb::LumiCounters::LastGlobal; key++ ) {
      if ( m_integratorTool->integral().hasInfo(key) ) {
	LHCb::LumiIntegral::ValuePair value = m_integratorTool->integral().info( key, defValue );
	std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
	if ( value.first != -1 ) {
	  info() << "From Lumi: The counter " << counterName 
		 << ":  with the Key " 
		 << key << "  was used : " << value.first << " times and the summed value is :  " << value.second << endmsg;
	  //write the counters in the LumiTuple
	  if(m_countersDetails == true){
	    tuple->column(fromlumi+counterName+"_Key" ,key);
	    tuple->column(fromlumi+counterName+ "_Counter", value.first);
	    tuple->column(fromlumi+counterName+ "_Value", value.second);
	    
	  }//debug mode 
	  
	}//check the first value
      }//take the key
    }//loop over the keys
    
  }//check that all the events were processed
  else {
    info() <<  "Some events were not processed  " << endmsg; 
    info() <<  "This integrated luminosity will not be computed " << endmsg; 
    tuple->column("IntegratedLuminosity", -1);
  }
    sc  = tuple->write();
  return sc; 
  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

