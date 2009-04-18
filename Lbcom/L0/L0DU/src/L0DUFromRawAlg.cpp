// $Id: L0DUFromRawAlg.cpp,v 1.6 2009-04-18 00:17:12 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// DAQEvent
#include "Event/RawEvent.h"
// L0Event
#include "Event/L0DUReport.h"
#include "Event/L0ProcessorData.h"

// local
#include "L0DUFromRawAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUFromRawAlg
//
// Extract & decode L0DU Block from rawEvent
// - version 0 : L0DU Report only is extracted
//
// 2006-05-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUFromRawAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUFromRawAlg::L0DUFromRawAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ReportLocation"       , m_L0DUReportLocation =  LHCb::L0DUReportLocation::Default );
  declareProperty( "ProcessorDataLocation", m_proDataLoc         =  LHCb::L0ProcessorDataLocation::L0DU );
  declareProperty( "L0DUFromRawToolType"  , m_fromRawTool = "L0DUFromRawTool" );
  declareProperty( "ProcessorDataOnTES"   , m_proc = true );
}
//=============================================================================
// Destructor
//=============================================================================
L0DUFromRawAlg::~L0DUFromRawAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DUFromRawAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  // get the decoding tool
  m_fromRaw = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool  );

  // init
  m_evt     = 0;
  m_size    = 0;
  m_sizeMax = 0;
  m_sizeMin = 99999999;
  


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUFromRawAlg::execute() {


  // decode the bank

  if(!m_fromRaw->decodeBank())Warning("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();
  
  LHCb::L0DUReport rep = m_fromRaw->report();
  // put the report and processor data on TES
  LHCb::L0DUReport* report = new LHCb::L0DUReport( rep );
  put (report , m_L0DUReportLocation );

  
  // clone Processor Data and put it on TES
  if( m_proc){
    LHCb::L0ProcessorDatas* datas = new LHCb::L0ProcessorDatas();
    put (datas  , m_proDataLoc );
    for(LHCb::L0ProcessorDatas::iterator it = m_fromRaw->L0ProcessorDatas()->begin();
        it != m_fromRaw->L0ProcessorDatas()->end(); it++){
      LHCb::L0ProcessorData* data = new  LHCb::L0ProcessorData( **it );
      datas->insert (data);
    }  
  }
  
  m_evt++;
  m_size += m_fromRaw->size();
  if( m_fromRaw->size() > m_sizeMax )m_sizeMax = m_fromRaw->size();
  if( m_fromRaw->size() < m_sizeMin )m_sizeMin = m_fromRaw->size();

  // print out (CHECKS)
  if(report->configuration() != NULL && ( msgLevel( MSG::DEBUG) ) ){
    debug() << "Bank size : " << m_fromRaw->size() << " (bytes) " << endreq;
    LHCb::L0DUChannel::Map& channels = m_fromRaw->report().configuration()->channels();
    debug() << "________________ Trigger decision from raw ____________________ L0-yes = " 
              << m_fromRaw->report().decision() << endreq;
    verbose() << "Rebuilt decision from summary : " << m_fromRaw->report().decisionFromSummary() << endreq;
    for(LHCb::L0DUChannel::Map::iterator it = channels.begin();channels.end()!=it;it++){
      std::string name = ((*it).second)->name();
      verbose() << "Channel Decision " << name << " : " << m_fromRaw->report().channelDecisionByName( name ) << endreq;
    }
    LHCb::L0DUElementaryCondition::Map& conds = m_fromRaw->report().configuration()->conditions();
    for(LHCb::L0DUElementaryCondition::Map::iterator it = conds.begin();conds.end()!=it;it++){
    std::string name = ((*it).second)->name();
    verbose() << "Condition Value " << name << " : " << m_fromRaw->report().conditionValueByName( name ) << endreq;
    }
    // This works only with L0DU rawBank version > 0
    if(m_fromRaw->version() != 0){
      bool emul = m_fromRaw->report().configuration()->emulatedDecision ();
      debug() << "_________________ EMULATION using data from raw_________________ L0-yes = " << emul ;
      
      if( !emul) debug() << "(Downscaled ? " << m_fromRaw->report().configuration()->isDownscaled() << ") " << endreq;
      verbose() << m_fromRaw->report().configuration()->emulate()->summary()  << endreq;
    }
   }

  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUFromRawAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  //
  info() << " - ------------------------------------------------------------------" << endreq;
  info() << " - ========> Final summary of L0DUFromRawAlg" << endreq;
  info() << " - Total number of events processed           : " << m_evt << endreq;
  info() << " - Average bank size : " << (double) m_size/ (double) m_evt 
         << " (bytes)  | min= " << m_sizeMin << "  | max= " << m_sizeMax <<endreq;
  info() << " - ------------------------------------------------------------------" << endreq;



  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
