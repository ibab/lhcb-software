// $Id: L0DUAlg.cpp,v 1.3 2007-11-02 11:04:24 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
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
DECLARE_ALGORITHM_FACTORY( L0DUAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUAlg::L0DUAlg( const std::string& name,
            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{    
  declareProperty( "DataLocations"                , m_dataLocations   );
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::Calo);
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::Muon);
  m_dataLocations.push_back(LHCb::L0ProcessorDataLocation::PileUp);
 // configure DAQ
  declareProperty( "BankVersion"             , m_rawVsn   = 1 );
  declareProperty( "ConfigProviderType"      , m_configType = "L0DUConfigProvider");
  declareProperty( "ConfigProviderName"      , m_configName = "L0DUConfig");
  declareProperty( "EmulatorTool"            , m_emulatorType="L0DUEmulatorTool");
  declareProperty( "TCK"                     , m_tck=0xFFFF);
  //
  declareProperty( "StoreInBuffer"           , m_fillRaw = true );
  declareProperty( "RawLocation"             , m_rawLocation    = LHCb::RawEventLocation::Default   );
  declareProperty( "WriteOnTES"              , m_writeOnTES = true   );
  declareProperty( "ReportLocation"          , m_reportLocation = LHCb::L0DUReportLocation::Default );
  //
  declareProperty( "TCK"                     , m_tck=0xFFFF);

  m_rawSrcID = 0 ;// hardcoded rawBank srcID
  m_rawBankType  = LHCb::RawBank::L0DU; // rawBank Type


}


//=============================================================================
// Destructor
//=============================================================================
L0DUAlg::~L0DUAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0DUAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;


  //---------------------------
  // check TCK
  if(m_tck>0xFFFF){
    error() << "The requested Trigger Configuration Key (TCK) " << m_tck << " is NOT a 16-bit word" << endreq;
    return StatusCode::FAILURE; 
  }  
  //----------------------------
  // Get the L0DUAlg emulator tool
  //---------------------------
  m_emulator = tool<IL0DUEmulatorTool>(m_emulatorType,m_emulatorType,this);
  
  //--------------------------------
  // Get the L0DUAlg configuration tool
  //--------------------------------
  m_confTool = tool<IL0DUConfigProvider>(m_configType , m_configName);
  m_config   = m_confTool->config( m_tck );

  
  return sc;
};


//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUAlg::execute() {


  // process the emulator
  debug() << "Emulator processing ( Data = " << m_dataLocations << ", TCK = " << m_tck << " )" <<endreq;
  StatusCode sc = m_emulator->process(m_config, m_dataLocations );
  if(sc.isFailure())return sc;

  // push Report and Config on TES
  debug() <<"Push L0DUReport on TES" << endreq;
  if(m_writeOnTES){
    LHCb::L0DUReport* report = new LHCb::L0DUReport(m_emulator->report());
    put (report   , rootInTES() + m_reportLocation );//non-const
  }
  
  //push bank in RawBuffer
  if(m_fillRaw){
    const std::vector<unsigned int> block = m_emulator->bank(m_rawVsn);
    LHCb::RawEvent* raw = getOrCreate<LHCb::RawEvent,LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    raw->addBank(m_rawSrcID , m_rawBankType , m_rawVsn , block);
  }

  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUAlg::finalize() {
  debug()<< "==> Finalize" << endreq;
  return GaudiAlgorithm::finalize();
}



