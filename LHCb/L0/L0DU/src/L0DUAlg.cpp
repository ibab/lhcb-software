// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
//DAQEvent
#include "Event/RawEvent.h"
// local
#include "L0DUAlg.h"

#include "Event/L0DUBase.h"
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUAlg
//
// 2007-10-25 : Olivier Deschamps
//
// Run the L0DU Emulator according to a Trigger Configuration Key (TCK)
// and produce L0DUReport on TES AND/OR rawBank
//
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( L0DUAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUAlg::L0DUAlg( const std::string& name,
            ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator ) 
  , m_config(0)
  , m_confTool(0)
  , m_emulator(0)
{    
  declareProperty( "ProcessorDataLocations"  , m_dataLocations   );
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::Calo);
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::Muon);
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::PileUp);
 // configure DAQ
  declareProperty( "BankVersion"             , m_rawVsn   = 2 );
  declareProperty( "EmulatorTool"            , m_emulatorType="L0DUEmulatorTool");
  //
  declareProperty( "RawLocation"             , m_rawLocation    = LHCb::RawEventLocation::Default   );
  declareProperty( "L0DUReportLocation"          , m_reportLocation = LHCb::L0DUReportLocation::Default );
  //
  declareProperty( "TCK"                     , m_tck="");
  declareProperty( "L0DUConfigProviderName"  , m_configName="L0DUConfig");
  declareProperty( "L0DUConfigProviderType"  , m_configType="L0DUMultiConfigProvider");
  declareProperty( "BankSourceID"            ,  m_rawSrcID = 0);

  m_rawBankType  = LHCb::RawBank::L0DU; // rawBank Type


  if( context() == "Emulation" )m_reportLocation = LHCb::L0DUReportLocation::Emulated;
  

}


//=============================================================================
// Destructor
//=============================================================================
L0DUAlg::~L0DUAlg() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0DUAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  //---------------------------
  if(m_tck == ""){
    error() << "The Trigger Configuration Key is undefined" << endmsg;
    return StatusCode::FAILURE;
  }
  if( "0x" != m_tck.substr( 0, 2 ) ){
    error() << "The requested TCK value " << m_tck << " MUST be in hexadecimal format '0x" << m_tck << "'" << endmsg;
    return StatusCode::FAILURE;
  }


  //----------------------------
  // Get the L0DUAlg emulator tool
  //---------------------------
  m_emulator = tool<IL0DUEmulatorTool>(m_emulatorType,m_emulatorType);
  
  //--------------------------------
  // Get the L0DUAlg configuration tool
  //--------------------------------
  int itck;
  std::istringstream is( m_tck.c_str() );
  is >> std::hex >> itck;
  m_confTool = tool<IL0DUConfigProvider>(m_configType , m_configName );
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << " loading the configuration for TCK = " << m_tck << " /  " << itck << endmsg;
  m_config   = m_confTool->config( itck );
  if( NULL == m_config){
    error() << " Unable to load the configuration for TCK = " << m_tck << " /  " << itck << endmsg;
    return StatusCode::FAILURE;
  }

  info()<< "The L0DUConfig (TCK=" << m_tck << ") have been succesfully loaded" << endmsg;    

  return sc;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUAlg::execute() {


  // process the emulator
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Emulator processing ( Data = " << m_dataLocations << ", TCK = " << m_tck << " )" <<endmsg;
  StatusCode sc = m_emulator->process(m_config, m_dataLocations );
  if(sc.isFailure()){
    Error("Cannot process the emulator").ignore();
    return sc;
  }
  
  // push Report and Config on TES
  if(m_writeOnTES){
    if ( msgLevel(MSG::DEBUG) ) debug() <<"Push L0DUReport on TES" << endmsg;
    LHCb::L0DUReport* report = new LHCb::L0DUReport(m_emulator->emulatedReport());
    std::string loc = dataLocation( m_reportLocation );
    put (report   , loc );
  }
  
  //push bank in RawBuffer
  if(m_writeBanks){
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Insert RawBank in rawEvent" << endmsg;
    const std::vector<unsigned int> block = m_emulator->bank(m_rawVsn);
    LHCb::RawEvent* raw = getOrCreate<LHCb::RawEvent,LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    raw->addBank(m_rawSrcID , m_rawBankType , m_rawVsn , block);
  } 

  return StatusCode::SUCCESS ;
}



