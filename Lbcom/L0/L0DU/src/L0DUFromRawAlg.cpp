// $Id: L0DUFromRawAlg.cpp,v 1.10 2010-01-20 21:13:34 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
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
DECLARE_ALGORITHM_FACTORY( L0DUFromRawAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUFromRawAlg::L0DUFromRawAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : L0FromRawBase ( name , pSvcLocator )
{
  declareProperty( "L0DUReportLocation"   , m_L0DUReportLocation =  LHCb::L0DUReportLocation::Default );
  declareProperty( "ProcessorDataLocation", m_procDataLocation   =  LHCb::L0ProcessorDataLocation::L0DU );
  declareProperty( "L0DUFromRawToolType"  , m_fromRawTool = "L0DUFromRawTool" );
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
  m_fromRaw = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool  , this);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUFromRawAlg::execute() {


  // decode the bank
  std::string rawEventLocation;
  if ( selectRawEventLocation(rawEventLocation).isFailure() )
    return Error("No valid raw event location found",StatusCode::SUCCESS,50);
  if ( m_fromRaw->_setProperty("RawLocation",rawEventLocation ).isFailure() )
    return Error("Unable to set RawLocation in L0DUFromRawTool",StatusCode::SUCCESS,50);
  if ( m_fromRaw->_setProperty("StatusOnTES",Gaudi::Utils::toString(m_statusOnTES)).isFailure() )
    return Error("Unable to set StatusOnTES in L0DUFromRawTool",StatusCode::SUCCESS,50);
  if ( m_fromRaw->_setProperty("UseRootInTES",Gaudi::Utils::toString(m_useRootInTES)).isFailure() )
    return Error("Unable to set UseRootInTES in L0DUFromRawTool",StatusCode::SUCCESS,50);
  
  if(!m_fromRaw->decodeBank())Warning("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();

  // L0DUReport on TES
  if( m_writeOnTES ){
    LHCb::L0DUReport rep = m_fromRaw->report();
    // put the report and processor data on TES
    LHCb::L0DUReport* report = new LHCb::L0DUReport( rep );
    std::string loc = dataLocation( m_L0DUReportLocation);
    put (report , loc , IgnoreRootInTES);
  }

  // Clone Processor Data and put it on TES 
  // WARNING : PROCESSOR DATA ARE NOT CONTEXT DEPENDANT
  if( m_writeProcData){
    LHCb::L0ProcessorDatas* datas = new LHCb::L0ProcessorDatas();
    put (datas  , m_procDataLocation , IgnoreRootInTES);
    for(LHCb::L0ProcessorDatas::iterator it = m_fromRaw->L0ProcessorDatas()->begin();
        it != m_fromRaw->L0ProcessorDatas()->end(); it++){
      LHCb::L0ProcessorData* data = new  LHCb::L0ProcessorData( **it );
      datas->insert (data);
    }  
  }
  
  counter("L0DU RawBank Size (Bytes)") += m_fromRaw->size();

  // print out (CHECKS)
  if( msgLevel( MSG::DEBUG)){
    debug() << "Bank size : " << m_fromRaw->size() << " (bytes) - Bank version : " << m_fromRaw->report().bankVersion()<< endmsg;
    debug() << "________________ L0DU decisions ____________________  " << endmsg;
    int mask = LHCb::L0DUDecision::Any;
    int typ  = 0x1;
    while(mask != 0x0){
      debug()   << " -- Decision type = " << LHCb::L0DUDecision::Name[typ] << " -- " << endmsg;
      debug()   << "    o decision bit from raw                          : " <<  m_fromRaw->report().decision(typ) << endmsg;
      debug()   << "    o decision re-built from summary report          : " 
                <<   m_fromRaw->report().decisionFromSummary(typ) << endmsg;
      if( m_fromRaw->version() != 0 && 
          m_fromRaw->report().configuration() != NULL &&
          m_fromRaw->report().configuration()->completed()
          ){
        debug()   << "    o emulated decision from configuration ( "
                  << format("0x%04X",m_fromRaw->report().configuration()->tck() ) << ") : "
                  <<   m_fromRaw->report().configuration()->emulatedDecision(typ) 
                  << " (Downscaling decision : " << m_fromRaw->report().configuration()->isDownscaled() << ") " << endmsg;
      }
      typ  = typ  << 1;
      mask = mask >> 1;
    }
    if( m_fromRaw->report().configuration() != NULL ){
      verbose() << "________________ Channels decision ____________________  " << endmsg;
      LHCb::L0DUChannel::Map& channels = m_fromRaw->report().configuration()->channels();
      for(LHCb::L0DUChannel::Map::iterator it = channels.begin();channels.end()!=it;it++){
        std::string name = ((*it).second)->name();
        verbose() << " -- Channel Decision '" << name << "'"
                  << " (Decision type  : " << LHCb::L0DUDecision::Name[((*it).second)->decisionType()] << ") : " 
                  << m_fromRaw->report().channelDecisionByName( name ) << endmsg;
      }
      LHCb::L0DUElementaryCondition::Map& conds = m_fromRaw->report().configuration()->conditions();
      for(LHCb::L0DUElementaryCondition::Map::iterator it = conds.begin();conds.end()!=it;it++){
        std::string name = ((*it).second)->name();
        verbose() << "    - Condition Value " << name << " : " << m_fromRaw->report().conditionValueByName( name ) << endmsg;
      }
    }
  }  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUFromRawAlg::finalize() {
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
