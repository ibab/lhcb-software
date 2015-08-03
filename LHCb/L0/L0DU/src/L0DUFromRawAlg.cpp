// Include files 

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
  , m_fromRaw(NULL)
  , m_fromRaw2(NULL)
{
  declareProperty( "L0DUReportLocation"   , m_L0DUReportLocation =  LHCb::L0DUReportLocation::Default );
  declareProperty( "ProcessorDataLocation", m_procDataLocation   =  LHCb::L0ProcessorDataLocation::L0DU );
  declareProperty( "L0DUFromRawToolType"  , m_fromRawTool = "L0DUFromRawTool" );
  declareProperty( "Hlt1"                 , m_hlt1        = false );
  declareProperty( "CompareTools"         , m_compare     = false );
  declareProperty( "EnsureKnownTCK"       , m_ensureKnownTCK = false );
}

  //=============================================================================
  // Destructor
  //=============================================================================
L0DUFromRawAlg::~L0DUFromRawAlg() {} 


//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DUFromRawAlg::initialize() {
  StatusCode sc = L0FromRawBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if ( m_hlt1 &&
       m_fromRawTool == "L0DUFromRawTool" ) { 
    m_fromRawTool = "L0DUFromRawHlt1Tool";
  }
  
  // get the decoding tool
  m_fromRaw = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool  , this);
  if ( m_compare ) m_fromRaw2 = tool<IL0DUFromRawTool>( "L0DUFromRawTool"  , "L0DUFromRawTool" , this);

  if ( !m_hlt1 ) {    
    // decode the bank
    if ( m_fromRaw->_setProperty("RawEventLocations",Gaudi::Utils::toString(rawEventLocations()) ).isFailure() )
      return Error("Unable to set RawEventLocations in L0DUFromRawTool",StatusCode::SUCCESS,50);
    if ( m_fromRaw->_setProperty("StatusOnTES",Gaudi::Utils::toString(m_statusOnTES)).isFailure() )
      return Error("Unable to set StatusOnTES in L0DUFromRawTool",StatusCode::SUCCESS,50);
    //if ( m_fromRaw->_setProperty("UseRootInTES",Gaudi::Utils::toString(m_useRootInTES)).isFailure() ) //no longer needed!
    //   return Error("Unable to set UseRootInTES in L0DUFromRawTool",StatusCode::SUCCESS,50);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUFromRawAlg::execute() {


  
  if(!m_fromRaw->decodeBank())Warning("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();

  if (m_ensureKnownTCK) {
    if (m_fromRaw->report().configuration() == NULL) 
      return Error("TCK not recognized. Run with L0Conf().EnsureKnownTCK=False to ignore this error",StatusCode::FAILURE);
  }

  // L0DUReport on TES
  if( m_writeOnTES ){
    LHCb::L0DUReport rep = m_fromRaw->report();
    // put the report and processor data on TES
    LHCb::L0DUReport* report = new LHCb::L0DUReport( rep );
    std::string loc = dataLocation( m_L0DUReportLocation);
    put (report , loc , IgnoreRootInTES);
  }

  if ( m_compare ) compareReports();

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
    if( m_fromRaw->report().configuration() != NULL && msgLevel(MSG::VERBOSE) ) {
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

//=========================================================================
//  Compare the report produced by two versions of the tool
//=========================================================================
void L0DUFromRawAlg::compareReports ( ) {
  if( !m_fromRaw2->decodeBank()){
    Warning("RawBank decoding failed - no report comparaison").ignore();
    return;
  } 
  
  LHCb::L0DUReport rep  = m_fromRaw->report(); 
  LHCb::L0DUReport rep2 = m_fromRaw2->report();
  
  if ( rep.decisionValue() != rep2.decisionValue() ) {
    info() << "!= decisionValue : hlt " << rep.decisionValue() << " pro " << rep2.decisionValue()  << endmsg;
  }
  if ( rep.timingTriggerBit() != rep2.timingTriggerBit() ) {
    info() << "!= timingTriggerBit : hlt " << rep.timingTriggerBit() << " pro " << rep2.timingTriggerBit()  << endmsg;
  }
  if ( rep.forceBit() != rep2.forceBit() ) {
    info() << "!= forceBit : hlt " << rep.forceBit() << " pro " << rep2.forceBit()  << endmsg;
  }
  if ( rep.tck() != rep2.tck() ) {
    info() << "!= tck : hlt " << rep.tck() << " pro " << rep2.tck()  << endmsg;
  }
  if ( rep.sumEt() != rep2.sumEt() ) {
    info() << "!= sumEt : hlt " << rep.sumEt() << " pro " << rep2.sumEt()  << endmsg;
  }
  if ( rep.channelsPreDecisionSummaries() != rep2.channelsPreDecisionSummaries() ) {
    info() << "!= channelsPreDecisionSummaries : hlt " << rep.channelsPreDecisionSummaries() 
           << " pro " << rep2.channelsPreDecisionSummaries()  << endmsg;
  }
  if ( rep.channelsDecisionSummaries() != rep2.channelsDecisionSummaries() ) {
    info() << "!= channelsDecisionSummaries : hlt " << rep.channelsDecisionSummaries() 
           << " pro " << rep2.channelsDecisionSummaries()  << endmsg;
  }
  if ( rep.conditionsValueSummaries() != rep2.conditionsValueSummaries() ) {
    info() << "!= conditionsValueSummaries : hlt " << rep.conditionsValueSummaries() 
           << " pro " << rep2.conditionsValueSummaries()  << endmsg;
  }
  if ( rep.valid() != rep2.valid() ) {
    info() << "!= valid : hlt " << rep.valid() << " pro " << rep2.valid()  << endmsg;
  }
  if ( rep.bankVersion() != rep2.bankVersion() ) {
    info() << "!= bankVersion : hlt " << rep.bankVersion() << " pro " << rep2.bankVersion()  << endmsg;
  }
  if ( rep.dataMap() != rep2.dataMap() ) {
    for(std::map<std::string, std::pair<int,double> >::const_iterator imap = rep.dataMap().begin();
        imap!= rep.dataMap().end();imap++){
      if ( int(rep2.dataDigit(imap->first)) != imap->second.first ) {
        info() << "!= dataMap : For name " << imap->first 
               << " hlt: " << imap->second.first << " pro " <<   rep2.dataDigit(imap->first) << endmsg;
      }
    }
    for(std::map<std::string, std::pair<int,double> >::const_iterator imap = rep2.dataMap().begin();
        imap!= rep2.dataMap().end();imap++){
      if ( int(rep.dataDigit(imap->first)) != imap->second.first ) {
        info() << "!= dataMap : For name " << imap->first 
               << " pro: " << imap->second.first << " hlt " <<   rep.dataDigit(imap->first) << endmsg;
      }
    }
  }
  if ( rep.bcid() != rep2.bcid() ) {
    info() << "!= bcid : hlt " << rep.bcid() << " pro " << rep2.bcid()  << endmsg;
  }
  if ( rep.configuration() != rep2.configuration() ) {
    info() << "!= configuration : hlt " << rep.configuration() << " pro " << rep2.configuration()  << endmsg;
  }
}
//=============================================================================
