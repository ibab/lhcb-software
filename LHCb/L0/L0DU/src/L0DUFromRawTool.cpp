// Include files 

// from Gaudi
#include "Event/L0DUReport.h"
// local
#include "L0DUFromRawTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUFromRawTool
//
// 2007-10-19 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUFromRawTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUFromRawTool::L0DUFromRawTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
: Decoder::ToolBase ( type, name , parent ),
  m_confTool(NULL),
  m_emuTool(NULL),
  m_condDB(NULL),
  m_banks(),
  m_vsn(0),
  m_report(),
  m_processorDatas(),
// DO NOT TOUCH !! IF YOU MODIFY THIS VALUE THIS WILL BREAK THE DC06 BACKWARD COMPATIBILITY
  m_tck(0xDC06), // default value for DC06 production (TCK was not implemented in Bank) 
  m_warning(true),
  m_size(0),
  m_roStatus(),
  m_data(NULL),
  m_source(0),
  m_slot("T0"),
  m_dumping(-1),
  m_count(0)
{
  declareInterface<IL0DUFromRawTool>(this);
  
  declareProperty( "EmulatorTool"            , m_emulatorType="L0DUEmulatorTool");
  declareProperty( "L0DUConfigProviderName"  , m_configName="L0DUConfig");
  declareProperty( "L0DUConfigProviderType"  , m_configType="L0DUMultiConfigProvider");
  declareProperty( "ForceNonZeroSupMuons"    , m_muonNoZsup=false);  // WARNING : for experts only
  declareProperty( "ForceSummarySize"        , m_sumSize=-1);        // WARNING : for experts only
  declareProperty( "ForceTCK"                , m_force = -1);        // WARNING : for experts only
  declareProperty( "FullWarning"             , m_warn = false);
  declareProperty( "FillDataMap"             , m_fill = true);    // EXPERT USAGE
  declareProperty( "EncodeProcessorData"     , m_encode = true);  // EXPERT USAGE
  declareProperty( "Emulate"                 , m_emu  = true);    // EXPERT USAGE
  declareProperty( "StatusOnTES"             , m_stat = true);    // EXPERT USAGE
  declareProperty( "DumpBank"                , m_dumping = -1);   // EXPERT USAGE
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
}
//=============================================================================
// Destructor
//=============================================================================
L0DUFromRawTool::~L0DUFromRawTool() {
  if(m_processorDatas)delete m_processorDatas;
} 



//=============================================================================
StatusCode L0DUFromRawTool::initialize(){

  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize" << endmsg;
  StatusCode sc = Decoder::ToolBase::initialize();
   if(sc.isFailure())return sc;


   //
   //m_slot = ( rootInTES() == "" ) ? "T0" : rootInTES();
   if ( (rootInTES() == "Prev7/") ||\
        (rootInTES() == "Prev6/") ||\
        (rootInTES() == "Prev5/") ||\
        (rootInTES() == "Prev4/") ||\
        (rootInTES() == "Prev3/") ||\
        (rootInTES() == "Prev2/") ||\
        (rootInTES() == "Prev1/") ||\
        (rootInTES() == "Next1/") ||\
        (rootInTES() == "Next2/") ||\
        (rootInTES() == "Next3/") ||\
        (rootInTES() == "Next4/") ||\
        (rootInTES() == "Next5/") ||\
        (rootInTES() == "Next6/") ||\
        (rootInTES() == "Next7/") )
     m_slot =rootInTES();
   else 
     m_slot ="T0";

   if(!m_encode)m_emu = false;
  // get the configuration provider tool
  m_confTool = tool<IL0DUConfigProvider>(m_configType , m_configName);
  m_emuTool  = tool<IL0DUEmulatorTool>(m_emulatorType, m_emulatorType);
  m_condDB   = tool<IL0CondDBProvider>("L0CondDBProvider");

  // create Empty ProcessorData
  // Built L0ProcessorData
  m_processorDatas = new LHCb::L0ProcessorDatas();
  for( int fiber = 0 ; fiber != (int) L0DUBase::Fiber::Empty ; fiber++){
    LHCb::L0ProcessorData* temp = new LHCb::L0ProcessorData ( (L0DUBase::Fiber::Type) fiber , L0DUBase::EmptyData );
    m_processorDatas->insert( temp );
  }

  if( m_force >= 0 ){
    warning() << " ========> WARNING : TCK WILL BE FORCED TO : " << format("0x%04X", m_force) 
              << " YOU ARE ASSUMMED TO KNOW WHAT YOU ARE DOING " << endmsg;
  }
  if( m_sumSize >= 0 ){
    warning() << " ========> WARNING : Number of summary report is FORCED TO : " << m_sumSize << endmsg;
  }
  if( m_muonNoZsup ){
    warning() << " ========> WARNING : Muons are assumed to be non zero-suppressed : " << endmsg;
  }
  
  return sc;
}


// ---------------------------------------------
bool L0DUFromRawTool::decodeBank(int ibank){
  bool ok = decoding(ibank);
  if(m_count < m_dumping )dump();
  m_count++;
  if( !ok ){
    m_report.clear();
    m_report.setValid( false );
  }
  if(m_stat)putStatusOnTES();
  return ok;
}

// ---------------------------------------------
bool L0DUFromRawTool::getL0DUBanksFromRaw( ){

  m_banks = NULL;

  m_roStatus = LHCb::RawBankReadoutStatus( LHCb::RawBank::L0DU );
  m_roStatus.addStatus( 0, LHCb::RawBankReadoutStatus::OK);

  LHCb::RawEvent* rawEvt = findFirstRawEvent() ;
  // if not existing complain
  if( NULL == rawEvt ){
    Warning("rawEvent not found in  '" + Gaudi::Utils::toString(m_rawEventLocations) +"' locations", StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing);
    return false;
  }      
  
  m_banks= &rawEvt->banks(   LHCb::RawBank::L0DU );


  // check whether error bank has been produced
  const std::vector<LHCb::RawBank*>* errBanks = &rawEvt->banks(   LHCb::RawBank::L0DUError );
  if( errBanks != 0 && errBanks->size() !=0 ){
    if(m_warn)Warning("L0DUError bank has been found ...",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( 0, LHCb::RawBankReadoutStatus::ErrorBank );
  }
  
   if ( msgLevel( MSG::DEBUG) )
     debug() << "Number of L0DU bank(s) found : " << m_banks->size() << endmsg; // should be == 1 for L0DU
  if( 0 == m_banks->size() ) {
    if(m_warn)Warning("READOUTSTATUS : no L0DU bank found in rawEvent",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing);
    return false;
  }
  if( 1 != m_banks->size() ){
    std::stringstream msg("");
    if(m_warn)Warning("READOUSTATUS : more than one L0DU bank has been found in the RawEvent",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::NonUnique);
  }  
  return true;
} 



bool L0DUFromRawTool::decoding(int ibank){

  using namespace L0DUBase::PredefinedData;
  // ------------------------
  // clear -------------------
  // ------------------------
  LHCb::L0DUConfig* config = NULL;
  m_dump.clear();
  m_dataMap.clear();
  m_report.clear();
  // reset
  for( LHCb::L0ProcessorDatas::iterator it = m_processorDatas->begin(); it != m_processorDatas->end() ; it++){
    (*it)->setWord (L0DUBase::EmptyData);
  }

  //-----------------------------
  // get bank -------------------
  //-----------------------------
  if( !getL0DUBanksFromRaw() )return false;
  std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); 
  LHCb::RawBank* bank = *itB+ibank;  
  if(NULL == bank){
    Error("Bank point to NULL ",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing);
    return false;
  }

  // Check Magic pattern
  if( LHCb::RawBank::MagicPattern != bank->magic() ) {
    Error("Bad MagicPattern",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::BadMagicPattern);
    return false;
  }



  // ------------------------
  // decoding ---------------
  // ------------------------

  // rawBank header :
  m_data = bank->data();
  m_size = bank->size();  // Bank size is in bytes
  m_vsn  = bank->version();
  m_report.setBankVersion( m_vsn );
  m_source = bank->sourceID();
  m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::OK);
  
  int nm        = 0 ;
  int np        = 0 ;


  if ( msgLevel( MSG::DEBUG) ){
    debug() << " <============= Start decoding bank =============>" << endmsg;
    debug() << "Decoding bank : " << bank << " Source : " << m_source << " Version : " 
            << m_vsn << " Size " << m_size << " (bytes) " << endmsg;
  }

  // Version 0 : preliminary version used for DC06 simulated data
  //--------------------------------------------------------------
  if(m_vsn == 0){

    
    if( m_force >= 0 ){
      std::stringstream msg("");
      msg << " TCK IS FORCED TO BE " << format("0x%04X", m_force);
      if( m_force != (int) m_tck){
        msg << " INCONSISTENT with assumed TCK for bank version 0 : " << format("0x%04X", m_tck) ;
        Warning( msg.str() , StatusCode::SUCCESS).ignore();
      }else{
        msg << " consistent with assumed TCK : " << format("0x%04X", m_tck) ;
        if ( msgLevel(MSG::DEBUG) ) debug() << msg.str() << endmsg;
      }
      m_tck = (unsigned int) m_force;    
    }    
    
    m_report.setTck( m_tck );

    unsigned int word;
    word = *m_data;
    m_dump.push_back(word);
    if ( msgLevel( MSG::VERBOSE) )verbose() << "first data word = " << format("0x%04X", word)<< endmsg;
    m_report.setDecisionValue( (word & 0x1) );
    m_report.setChannelsDecisionSummary( word >> 1 );
    if( !nextData() )return false;
    word = *m_data;
    m_report.setConditionsValueSummary( word );
    if( !nextData() )return false;
    word = *m_data;
    encode(Name[PuPeak1],  word & 0xFF     , L0DUBase::PileUp::Peak1 );
    encode(Name[PuPeak1Pos] , (word>>8) & 0xFF , L0DUBase::PileUp::Peak1Pos );
    if( !nextData() )return false;
    word = *m_data;
    encode(Name[PuPeak2], word & 0xFF     , L0DUBase::PileUp::Peak2 );
    encode(Name[PuPeak2Pos] , (word>>8) & 0xFF , L0DUBase::PileUp::Peak2Pos );
    if( !nextData() )return false;
    word = *m_data;
    encode(Name[PuHits],  word & 0xFF      , L0DUBase::PileUp::Hits );
    if( !nextData() )return false;
    word = *m_data;
    encode(Name[SumEt],  word & 0x3FFF         , L0DUBase::Sum::Et      );
    encode(Name[SpdMult] , (word>>14) & 0x3FFF , L0DUBase::Spd::Mult    );

    // No TCK mechanism at time of version 1 :  TCK = FFFE assumed
    if(m_warning){
      m_warning = false;
      std::stringstream tck("");
      tck << format("0x%04X", m_tck);
      if(m_warning)Warning("L0DU bank version = 0 --> the TCK value is forced to " + tck.str(),StatusCode::SUCCESS).ignore();
    } 
    if ( msgLevel( MSG::DEBUG) )debug() << "Loading configuration" << endmsg;

    config = m_confTool->config( m_tck , m_slot);

    if( NULL == config){
      std::stringstream tck("");
      tck << "Unable to load the configuration for tck = " <<  format("0x%04X", m_tck) << " --> Incomplete L0DUReport" ;
      Warning(tck.str(), StatusCode::SUCCESS).ignore();
      m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Unknown);      
    }else{      
      m_report.setConfiguration(config);
    }    
    if ( msgLevel( MSG::DEBUG) )debug() << "L0DU bank version 0 decoded" << endmsg;
  }
  // Version 1 : complete RawBank as described in EDMS 868071
  //---------------------------------------------------------
  else if(m_vsn == 1 || m_vsn == 2){  // version 2 = adding extra-decisions (beam1/beam2) in RSDA

    if ( msgLevel( MSG::VERBOSE) )verbose() << "first data word = " << format("0x%04X", *m_data)<< endmsg;
    m_dump.push_back(*m_data);
    // global header
    unsigned int itc      = (*m_data & 0x00000003)  >> 0;
    unsigned int iec      = (*m_data & 0x0000000C)  >> 2;
    m_pgaVsn              = (*m_data & 0x00000FF0)  >> 4;
    //OD neither in v1 nor v2    m_status              = (*m_data & 0x0000F000)  >> 12;
    if( m_vsn == 2){ // report size moved to global header
      nm                  = (*m_data & 0x00003000 ) >> 12;
      np                  = (*m_data & 0x0000C000 ) >> 14;
    }
    m_tck                 = (*m_data & 0xFFFF0000)  >> 16;

    if(m_sumSize >=0){
      itc = m_sumSize;
      iec = m_sumSize;
    }

    if( m_force >= 0 ){
      std::stringstream msg("");
      msg << " TCK IS FORCED TO BE " << format("0x%04X", m_force);
      if( m_force != (int) m_tck){
        msg << " INCONSISTENT with TCK in data : " << format("0x%04X", m_tck) ;
        Warning( msg.str() , StatusCode::SUCCESS).ignore();
      }else{
        msg << " consistent with TCK in data : " << format("0x%04X", m_tck) ;
        if ( msgLevel(MSG::DEBUG) ) debug() << msg.str() << endmsg;
      }
      m_tck = (unsigned int) m_force;    
    }
    
    m_report.setTck( m_tck );    

    if ( msgLevel( MSG::DEBUG) ){
      debug() << "-- Global header " << endmsg;
      debug() << "   -> TCK = " << m_tck << " [" << format("0x%04X", m_tck) << "]"   <<endmsg;
      debug() << "   -> Firmware version : " << m_pgaVsn << " [" <<  format("0x%04X", m_pgaVsn) << "]" <<endmsg;
      debug() << "   -> Number of Condition & Channel summaries are : " << iec << " / " << itc << " respectively " ;
      if(m_vsn == 2) debug() << "   -> Number of Previous/Next BX : " << nm << "/" << np << endmsg;
      if(m_sumSize>0) debug() << " (FORCED BY USER) " ;                        
      debug() <<endmsg;
    }
    
    //---------------------------------
    // get corresponding configuration
    //---------------------------------
    config = m_confTool->config( m_tck , m_slot );
    if( NULL == config){
      std::stringstream tck("");
      m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Unknown);      
      tck << " Unable to load the configuration for tck = " <<  format("0x%04X", m_tck) << " --> Incomplete L0DUReport" ;
      Warning(tck.str(), StatusCode::SUCCESS).ignore();
    }else{
      m_report.setConfiguration(config);
    }
    
    //---------------------------------
    
    // PGA3-block header
    if( !nextData() )return false;
    
    unsigned long pga3Status    = (*m_data & 0x001FFFFF ) >>  0;
    m_bcid3                    = (*m_data & 0x0FE00000 ) >> 21;
    unsigned int  nmu          = (*m_data & 0xF0000000 ) >> 28;
    
    if(m_muonNoZsup){
      std::stringstream msg("");
      if( 8 != nmu ){
        msg << "READOUTSTATUS : muons are supposed to be NON 0-suppressed but the bank content indicates " 
            << nmu << " muons only (will assume 8 muons for decoding)";
        m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Corrupted);      
        Warning(msg.str() , StatusCode::SUCCESS).ignore();
      }
      nmu = 8;
    }
    
    encode("MuonCU0(Status)",(pga3Status >> 0) & 0xF  , L0DUBase::Muon1::Status  );
    encode("MuonCU1(Status)",(pga3Status >> 4) & 0xF  , L0DUBase::Muon3::Status  );
    encode("MuonCU2(Status)",(pga3Status >> 8) & 0xF  , L0DUBase::Muon5::Status  );
    encode("MuonCU3(Status)",(pga3Status >> 12)& 0xF  , L0DUBase::Muon7::Status  );
    
    unsigned int pga3Size = 4 * ( 4 + ( int((nmu+3)/4) ) + ( int((nmu+1)/2) ) );
    if ( msgLevel( MSG::DEBUG) ){
      debug() << "-- PGA3 header -------------------------- "  << endmsg;
      debug() << "   -> BCID (&7F): " <<  m_bcid3  <<  " [" << format("0x%04X",  m_bcid3) << "]"   <<endmsg;
      debug() << "   -> Processors status : "  << pga3Status  <<  " [" << format("0x%04X",  pga3Status) << "]"   <<endmsg;
      debug() << "   -> Number of L0Muon in bank (0-sup) : " << nmu     ;
      if( m_muonNoZsup ) debug() << " (FORCED TO BE NON ZeroSuppressed BY USER) " ;
      debug() << endmsg;
      debug() << "   -> PGA3 block expected size : "<< pga3Size  << " (bytes) " <<endmsg;
    }
    
    // PGA3 processing
    if( !nextData() )return false;
    int mu1 = (*m_data & 0x0000007F ) >> 0;
    dataMap(Name[Muon1Pt] ,mu1  ,scale(L0DUBase::Muon1::Pt));
    dataMap(Name[Muon1Sgn],(*m_data & 0x00000080 ) >> 7  );
    int mu2 = (*m_data & 0x00007F00 ) >> 8;
    dataMap(Name[Muon2Pt] ,mu2  ,scale(L0DUBase::Muon2::Pt));
    dataMap(Name[Muon2Sgn],(*m_data & 0x00008000 ) >> 15 );
    dataMap(Name[Muon3Pt] ,(*m_data & 0x007F0000 ) >> 16 ,scale(L0DUBase::Muon3::Pt));
    dataMap(Name[Muon3Sgn],(*m_data & 0x00800000 ) >> 23 );
    dataMap(Name[DiMuonPt],(*m_data & 0xFF000000 ) >> 24, scale(L0DUBase::Muon1::Pt));
    //
    dataMap(Name[DiMuonProdPt], mu1*mu2,scale(L0DUBase::Muon1::Pt));

    if( !nextData() )return false;
    dataMap(Name[Muon1Add],(*m_data & 0x0000FFFF ) >>0   );
    dataMap(Name[Muon2Add],(*m_data & 0xFFFF0000 ) >>16  );
    
    if( !nextData() )return false;
    dataMap(Name[Muon3Add],(*m_data & 0x0000FFFF)  >> 0  );
    m_muCleanPattern             = (*m_data & 0x00FF0000)  >>16;
    if ( msgLevel( MSG::VERBOSE) ){
      verbose() << "-- PGA3 block -------------------------- " << endmsg;
      verbose() << "   ... L0Muon processed data decoded  - Muon cleaning pattern = " 
                << m_muCleanPattern <<  " [" << format("0x%04X",  m_muCleanPattern) << "] ..."   
                <<endmsg;
    }
    
    // PGA3 Input data
    int n[8]; 
    for(unsigned int imu =0 ; imu < nmu ; imu++){
      int odd = imu-int(imu/2)*2;
      if(odd == 0 && !nextData())return false;
      std::stringstream num("");
      n[imu] = ((*m_data  >> 16*odd) & 0x0000E000 ) >> 13;
      num << n[imu];
      unsigned int val = (*m_data  >> 16*odd) & 0x0000FFFF;
      if( n[imu] == 0)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon1::Address);
      else if( n[imu] == 1)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon2::Address);
      else if( n[imu] == 2)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon3::Address);
      else if( n[imu] == 3)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon4::Address);
      else if( n[imu] == 4)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon5::Address);
      else if( n[imu] == 5)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon6::Address);
      else if( n[imu] == 6)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon7::Address);
      else if( n[imu] == 7)encode("M"+num.str()+"(Add)", val , L0DUBase::Muon8::Address);
    }
    
    for(unsigned int imu =0 ; imu < nmu ; imu++){
      int biodd = imu-int(imu/4)*4;
      if(biodd == 0 && !nextData())return false;
      std::stringstream num("");
      num << n[imu];
      unsigned int pt = (*m_data >> 8*biodd) & 0x0000007F ;
      unsigned int sgn=((*m_data >> 8*biodd) & 0x00000080) >> 7;
      if( n[imu] == 0){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon1::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon1::Sign );
      }else if( n[imu] == 1){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon2::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon2::Sign );
      }else if( n[imu] == 2){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon3::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon3::Sign );
      }else if( n[imu] == 3){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon4::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon4::Sign );
      }else if( n[imu] == 4){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon5::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon5::Sign );
      }else if( n[imu] == 5){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon6::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon6::Sign );
      }else if( n[imu] == 6){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon7::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon7::Sign );
      }else if( n[imu] == 7){
        encode("M"+num.str()+"(Pt)" , pt  ,  L0DUBase::Muon8::Pt  );
        encode("M"+num.str()+"(Sgn)", sgn ,  L0DUBase::Muon8::Sign );
      }
    }
    
    if ( msgLevel( MSG::VERBOSE) )verbose() << "   ... L0Muon input data decoded  ( " << nmu << " ) ..." << endmsg;
    
    // PGA2-block header
    if( !nextData() )return false;
    m_rsda       = (*m_data & 0x0000FFFF );
    unsigned int pga2Status= (*m_data & 0x0FFF0000 ) >> 16;
    m_status               = (pga2Status & 0xE00   )  >> 12;
 
    // NOT IMPLEMENTED IN TELL1 F/W TO BE CHANGED IN EDMS !!!!
    //if( 0x1 & m_status){
    //  m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Tell1Error );
    //  m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Tell1Sync );
    //}

    if( m_vsn == 1 ){
      nm                     = (*m_data & 0x30000000 ) >> 28;
      np                     = (*m_data & 0xC0000000 ) >> 30;
    }    

    unsigned int beam1 = 0;
    unsigned int beam2 = 0;
    if( m_vsn == 1){
      m_bcid2                =  m_rsda & 0xFFF;
    }
    else if (m_vsn ==2){
      // BCID split : 10 LSB in RSDA + 2 in PGA2 header
      m_bcid2                =  (m_rsda & 0x3FF)  | ( (*m_data & 0x30000000 ) >> 18); 
      beam1     = (m_rsda >> 10) & 0x1;
      beam2     = (m_rsda >> 11) & 0x1;
    }
    unsigned int decision  = (m_rsda >> 12) & 1;
    unsigned int fb        = (m_rsda >> 13) & 1;
    unsigned int ttb       = (m_rsda >> 14) & 1;

    int decisionValue = 0x0;
    if( decision == 1)decisionValue |= LHCb::L0DUDecision::Physics;
    if( beam1    == 1)decisionValue |= LHCb::L0DUDecision::Beam1;
    if( beam2    == 1)decisionValue |= LHCb::L0DUDecision::Beam2;

    if ( msgLevel( MSG::DEBUG) ){
      debug() << "-- PGA2 header -------------------------- " << endmsg;
      debug() << "   -> RSDA : " <<  m_rsda << " [" << format("0x%04X",  m_rsda) << "]"   << endmsg;
      debug() << "   -> BCID ( & 0xFFF): "<< m_bcid2 << " [" << format("0x%04X",  m_bcid2) << "]"   << endmsg; 
      debug() << "   -> Decisions : [ physics : " << decision << "| beam1 : " << beam1 << " |  beam2 : " << beam2 << "]" <<endmsg;
      debug() << "   -> TTB : " << ttb  << "  FB : " << fb << endmsg;
      if(m_vsn == 1) debug() << "   -> Number of Previous/Next BX : " << nm << "/" << np << endmsg;
      debug() << "   -> L0DU Status : " << m_status  << " [" <<  format("0x%04X", m_status) << "]"   <<endmsg;
      debug() << "   -> Processors Status : " << (m_status&0x1FF)  << " [" <<  format("0x%04X", (m_status&0x1FF)) << "]"<<endmsg;
    }
    if ( msgLevel( MSG::VERBOSE) )verbose() << "-- PGA2 block -------------------------- " << endmsg;

    // update L0DUReport
    m_report.setDecisionValue( decisionValue );

    m_report.setTimingTriggerBit( (ttb == 1) );
    m_report.setForceBit( (fb == 1) );
    //
    encode("Electron(Status)",(pga2Status>>L0DUBase::Fiber::CaloElectron)  & 0x1    , L0DUBase::Electron::Status  );
    encode("Photon(Status)",(pga2Status>>L0DUBase::Fiber::CaloPhoton)    & 0x1      , L0DUBase::Photon::Status    );
    encode("Hadron(Status)",(pga2Status>>L0DUBase::Fiber::CaloHadron)    & 0x1      , L0DUBase::Hadron::Status    );
    encode("GlobalPi0(Status)", (pga2Status>>L0DUBase::Fiber::CaloPi0Local)  & 0x1  , L0DUBase::Pi0Global::Status );
    encode("LocalPi0(Status)",(pga2Status>>L0DUBase::Fiber::CaloPi0Global) & 0x1    , L0DUBase::Pi0Local::Status  );
    encode("Sum(Status)",(pga2Status>>L0DUBase::Fiber::CaloSumEt)     & 0x1         , L0DUBase::Sum::Status       );
    encode("Spd(Status)",(pga2Status>>L0DUBase::Fiber::CaloSpdMult)   & 0x1         , L0DUBase::Spd::Status       );
    encode("PU1(Status)",(pga2Status>>L0DUBase::Fiber::Pu1)           & 0x1         , L0DUBase::PileUp::Status1   );
    encode("PU2(Status)",(pga2Status>>L0DUBase::Fiber::Pu2)           & 0x1         , L0DUBase::PileUp::Status2   );
    
    // check the bank size is consistent
    
    unsigned int pga2Size = 4 * ( 7 + iec + 2*itc +  (itc+iec)*(nm+np) 
                                  + (int( (nm+1)/2) + int ((np+1)/2)));
    if ( msgLevel( MSG::DEBUG) )debug() << "   -> PGA2 bloc size should be " << pga2Size << " (bytes)" << endmsg;
    unsigned int allSize = 4 + pga2Size + pga3Size;
    if(m_size == allSize){
      if ( msgLevel( MSG::DEBUG) )debug() << "   -> The total expected size (header + PGA2 + PGA3) " << allSize 
                                          << " matches the actual bank size ________________ <** OK **> " <<endmsg;
    }else{
      std::stringstream msg("");
      msg << "READOUTSTATUS : the total expected size "  
          << " does NOT match the bank size __________________  <** POSSIBLE DATA CORRUPTION **>";
      if ( msgLevel( MSG::DEBUG) )
        debug() << " Expected size : " << allSize << " Actual size : " << m_size << " DO NOT MATCH" << endmsg;
      Error(msg.str() , StatusCode::SUCCESS).ignore(); 
      m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Corrupted ); 
    }
    
    if ( (0x7F & m_bcid2) == m_bcid3){
      if ( msgLevel( MSG::DEBUG) )debug() 
        << "   -> The PGA3 and PGA2 data are aligned _____________________________________ <** OK **> " 
        << endmsg;
    }else{
      if(m_warn)Warning("   -> The PGA3 and PGA2 data are NOT aligned ",StatusCode::SUCCESS).ignore();
      if ( msgLevel( MSG::DEBUG) )
        debug() << " BCIDs PGA2(LSB)/PGA3= " << (m_bcid2 & 0x7F) << " /"  << m_bcid3 << " NOT ALIGNED " << endmsg;
    }
    
    
    // PGA2 processing
    //clear summaries Maps
    m_ecs.clear();
    m_cds.clear();
    m_tcs.clear();
    
    for(unsigned int i = 0 ; i < itc ; i++){
      if( !nextData() )return false;
      m_tcs[std::make_pair(0,i)] = *m_data;
      if ( msgLevel( MSG::VERBOSE) )verbose() << "   ->  Channel PreDecision summary[BX=0, Word=" << i << "] = "
                                              << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg;
    }
    for(unsigned int i = 0 ; i < itc ; i++){
      if( !nextData() )return false;
      m_cds[std::make_pair(0,i)]=*m_data;
      if ( msgLevel( MSG::VERBOSE) )verbose() << "   -> Channel Decision summary[BX=0, Word=" << i << "] = "
                                              << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg;
    }
    for(unsigned int i = 0 ; i < iec ; i++){
      if( !nextData() )return false;
      m_ecs[std::make_pair(0,i)]=*m_data;
      if ( msgLevel( MSG::VERBOSE) )verbose() << "   ->  Elementary condition summary[BX=0, Word=" << i << "] = " 
                                              << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg; 
    }

    
    if ( msgLevel( MSG::VERBOSE) )verbose() << "   ... PGA2 decision processing decoded ..." <<endmsg;
    
    //PGA2 input data
    if( !nextData() )return false;
    int sumEt0 = (*m_data & 0x00003FFF );             
    encode(Name[SumEt]        , sumEt0                         , L0DUBase::Sum::Et  );
    m_sumEt[0] = sumEt0;

    if ( msgLevel( MSG::VERBOSE) )verbose() << "   -> SumEt[BX=0] = "<< m_sumEt[0] << endmsg;

    encode(Name[SpdMult],(*m_data & 0x0FFFC000 ) >> 14         , L0DUBase::Spd::Mult    );
    encode("PU(MoreInfo)",(*m_data & 0xF0000000 ) >> 28      , L0DUBase::PileUp::MoreInfo  );
    
    if( !nextData() )return false;
    encode(Name[ElectronEt] ,(*m_data & 0x000000FF ) >> 0      , L0DUBase::Electron::Et      );
    encode(Name[PhotonEt]   ,(*m_data & 0x0000FF00 ) >> 8      , L0DUBase::Photon::Et        );
    encode(Name[GlobalPi0Et],(*m_data & 0x00FF0000 ) >> 16     , L0DUBase::Pi0Global::Et     );
    encode(Name[LocalPi0Et] ,(*m_data & 0xFF000000 ) >> 24     , L0DUBase::Pi0Local::Et      );
    
    if( !nextData() )return false;
    encode(Name[HadronEt]   ,(*m_data & 0x000000FF ) >> 0      , L0DUBase::Hadron::Et        );
    encode(Name[PuPeak1],(*m_data & 0x0000FF00 ) >> 8      , L0DUBase::PileUp::Peak1     );
    encode(Name[PuPeak2],(*m_data & 0x00FF0000 ) >> 16     , L0DUBase::PileUp::Peak2     );
    encode(Name[PuHits] ,(*m_data & 0xFF000000 ) >> 24     , L0DUBase::PileUp::Hits      );
    
    if( !nextData() )return false;
    encode(Name[ElectronAdd] ,(*m_data & 0x0000FFFF ) >> 0     , L0DUBase::Electron::Address   );
    encode(Name[PhotonAdd]   ,(*m_data & 0xFFFF0000 ) >> 16    , L0DUBase::Photon::Address   );
    
    if( !nextData() )return false;
    encode(Name[GlobalPi0Add],(*m_data & 0x0000FFFF )          , L0DUBase::Pi0Global::Address );
    encode(Name[LocalPi0Add] ,(*m_data & 0xFFFF0000 ) >> 16    , L0DUBase::Pi0Local::Address  );
    
    if( !nextData() )return false;
    encode(Name[HadronAdd]  ,(*m_data & 0x0000FFFF )           , L0DUBase::Hadron::Address );
    encode(Name[PuPeak1Pos] ,(*m_data & 0x00FF0000 ) >> 16     , L0DUBase::PileUp::Peak1Pos );
    encode(Name[PuPeak2Pos] ,(*m_data & 0xFF000000 ) >> 24     , L0DUBase::PileUp::Peak2Pos );
    
    if ( msgLevel( MSG::VERBOSE) )verbose() << "   ... L0Calo & L0PileUp input data decoded ..." <<endmsg;
    
    
    // Previous/Next BX
    for(int im = nm ; im > 0 ; im--){
      for(unsigned int i = 0 ; i < itc ; i++){
        if( !nextData() )return false;
        m_cds[std::make_pair(-im,i)] = *m_data;
        if( msgLevel( MSG::VERBOSE) )verbose() << "   -> Trigger Channel Summary[BX="<< -im << ", Word=" <<i << "] = " 
                                               << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg;
      }
      for(unsigned int i = 0 ; i < iec ; i++){
        if( !nextData() )return false;
        m_ecs[std::make_pair(-im,i)] = *m_data;
        if ( msgLevel( MSG::VERBOSE) )verbose() << "   -> Elementary Condition Summary[BX="<< -im << ", Word=" <<i << "] = " 
                  << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg;
      }
    }
    if( msgLevel( MSG::VERBOSE) )verbose() << "   ... " << nm << " previous BX summaries decoded ..." << endmsg;  
    
    
    // SumEt
    for(int im = nm ; im > 0 ; im--){
      int odd = (nm-im)%2;
      if(odd == 0 &&  !nextData() )return false;
      m_sumEt[-im]= (*m_data >> 16*odd) & 0x3FFF ;
      if( msgLevel( MSG::VERBOSE) )verbose() << "   -> SumEt[Bx=" << -im << "] = " <<  m_sumEt[-im] << endmsg;
    }
    dataMap("Sum(Et,Prev2)",m_sumEt[-2],scale(L0DUBase::Sum::Et));
    dataMap("Sum(Et,Prev1)",m_sumEt[-1],scale(L0DUBase::Sum::Et));

    if( msgLevel( MSG::VERBOSE) )verbose() << "   ... " << nm << " previous BX sumET decoded ... " << endmsg;

    
    for(int ip = 0 ; ip < np ; ip++){
      for(unsigned int i = 0 ; i < itc ; i++){
        if( !nextData() )return false;
        m_cds[std::make_pair(ip+1,i)] = *m_data;
        if( msgLevel( MSG::VERBOSE) )verbose() << "   -> Trigger Channel Summary[BX="<< ip+1 << ", Word=" <<i << "] = " 
                                               << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg;
      }
      for(unsigned int i = 0 ; i < iec ; i++){
        if( !nextData() )return false;
        m_ecs[std::make_pair(ip+1,i)] = *m_data;
        if( msgLevel( MSG::VERBOSE) )verbose() << "   -> Elementary Condition Summary[BX="<< ip+1 << ", Word=" <<i << "] = "
                                               << *m_data << " [0x" << format("0x%04X",  *m_data) << "]"  <<endmsg;
      }
    }
    if( msgLevel( MSG::VERBOSE) )verbose() << "   ... " <<  np << " next BX summaries decoded ..." << endmsg;  
    
    for(int ip = 0 ; ip < np ; ip++){
      int odd = ip%2;
      if(odd == 0 &&  !nextData() )return false;
      m_sumEt[ip+1]                        = (*m_data >> 16*odd) & 0x3FFF ;
      if( msgLevel( MSG::VERBOSE) )verbose() << " SumEt[Bx=" << ip+1 << "] = " <<  m_sumEt[ip+1] << endmsg;
    }
    if( msgLevel( MSG::VERBOSE) )verbose() << "   ... " << nm << " next BX sumET decoded ..." << endmsg;
    dataMap("Sum(Et,Next1)",m_sumEt[1],scale(L0DUBase::Sum::Et));
    dataMap("Sum(Et,Next2)",m_sumEt[2],scale(L0DUBase::Sum::Et));

    // check the size 
    if( msgLevel( MSG::VERBOSE) )verbose() << " <============= Decoding completed successfuly =============>" << endmsg;
    
    // Print all data values extracted from rawBank
    if ( msgLevel( MSG::VERBOSE) ){
      for(std::map<std::string, std::pair<unsigned int,double> >::iterator imap = m_dataMap.begin();
          imap!=m_dataMap.end();imap++){
        verbose() << "   --> Data = (value,scale) : " << imap->first << " = " <<  imap->second << endmsg;
      }
    }
    
    // -----------------------------------------------
    // BUILDING output (L0ProcessorData & L0Report)
    // -----------------------------------------------
    if ( msgLevel( MSG::VERBOSE) )verbose() << " ... fill processor Data ..." <<endmsg;
    // encode BCIDs from input data in rawBank
    if(m_encode)fillBCIDData();
    //  emulate the config for later usage (monitoring) 
    if(m_emu){
      if(NULL != config)m_emuTool->process(config , m_processorDatas).ignore();
      // template config :
      LHCb::L0DUConfig* temp =  m_confTool->config( LHCb::L0DUTemplateConfig::TCKValue , m_slot);
      if( NULL != temp &&  0 != temp->channels().size() ){
        m_emuTool->process(temp, m_processorDatas).ignore();
      }
    }
    // add data value to L0DUReport embeded map if not available via L0DUConfig (i.e. status bit or no emulation or unknown TCK)
    for( std::map<std::string, std::pair<unsigned int,double> >::iterator it = m_dataMap.begin();
         m_dataMap.end() != it ; ++it){ 
      std::string name = it->first;
      if( "" == name)continue;
      unsigned int    di = (it->second).first;
      double sc = (it->second).second;
      if( m_report.dataDigit(name) != di )m_report.addToData(name,di, sc);
    }
    
    

    // Fill report with the consecutive BXs info
    if ( msgLevel( MSG::VERBOSE) )verbose() << " ... filling L0DU Report with consecutive BXs ..." << endmsg;
    m_report.setChannelsDecisionSummaries( m_cds );
    m_report.setConditionsValueSummaries(  m_ecs );
    m_report.setChannelsPreDecisionSummaries( m_tcs );  
    m_report.setSumEt( m_sumEt );    
    m_report.setBcid( m_bcid2 );
    if ( msgLevel( MSG::VERBOSE) )verbose() 
      << " <==== Building output (emulated ProcessorData & L0DUReport) completed successfuly ====>" 
      << endmsg;
    
    //---------------------------------------------------------
    // End of coding version = 1
    //---------------------------------------------------------
    
  }  else{
    Error(" Unknown bank version ",StatusCode::SUCCESS).ignore();
    if ( msgLevel( MSG::DEBUG) )debug() << " Unknown bank version : " << m_vsn << endmsg;
    m_roStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Corrupted);
    return false;
  }


  
  return true;
}


unsigned int L0DUFromRawTool::data(std::string name){
  // look for data in dataMap :
  std::map<std::string, std::pair<unsigned int,double> >::iterator it = m_dataMap.find(name);
  if( m_dataMap.end() != it) return ((*it).second).first;
  // not found : look for in config->data() (e.g. compound)
  return m_report.dataDigit( name );
}

double L0DUFromRawTool::dataScaled(std::string name){
  // look for data in dataMap :
  std::map<std::string, std::pair<unsigned int,double> >::iterator it = m_dataMap.find(name);
  if( m_dataMap.end() != it) return double(((*it).second).first)*((*it).second).second;
  // not found : look for in config->data() (e.g. compound)
  return m_report.dataValue( name );
}



double L0DUFromRawTool::scale(const unsigned int base[L0DUBase::Index::Size]){
  return m_condDB->scale( base[L0DUBase::Index::Scale] );
}




void L0DUFromRawTool::fillBCIDData(){
  encode("",m_bcid2                        , L0DUBase::Electron::BCID    );
  encode("",m_bcid2                        , L0DUBase::Photon::BCID    );
  encode("",m_bcid2                        , L0DUBase::Hadron::BCID    );
  encode("",m_bcid2                        , L0DUBase::Pi0Global::BCID    );
  encode("",m_bcid2                        , L0DUBase::Pi0Local::BCID    );
  encode("",m_bcid2                        , L0DUBase::Sum::BCID    );
  encode("",m_bcid2                        , L0DUBase::Spd::BCID    );
  encode("",m_bcid3                        , L0DUBase::Muon1::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon1::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon2::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon2::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon3::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon3::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon4::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon4::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon5::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon5::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon6::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon6::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon7::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon7::BCID2);
  encode("",m_bcid3                        , L0DUBase::Muon8::BCID1);
  encode("",m_bcid3                        , L0DUBase::Muon8::BCID2);
  encode("",m_bcid2                        , L0DUBase::PileUp::BCID1    );
  encode("",m_bcid2                        , L0DUBase::PileUp::BCID2    );
}






void L0DUFromRawTool::putStatusOnTES(){
  // Readout Status
  typedef LHCb::RawBankReadoutStatus Status;
  typedef LHCb::RawBankReadoutStatuss Statuss;  
  Statuss* statuss = getOrCreate<Statuss,Statuss>( LHCb::RawBankReadoutStatusLocation::Default );
  Status* status = statuss->object ( m_roStatus.key() );
  if( NULL == status ){
    status = new Status( m_roStatus  );
    statuss->insert( status );
  } else {
    std::stringstream type("");
    type << LHCb::RawBank::typeName(m_roStatus.key()) ;
    
    if ( msgLevel( MSG::DEBUG) )debug() << "Status for bankType " <<  type.str()  << " already exists" << endmsg;
    if( status->status() != m_roStatus.status() ){
      Warning("Status for bankType " +  type.str() + " already exists  with different status value -> merge both"
              , StatusCode::SUCCESS).ignore();
      for( std::map< int, long >::iterator it = m_roStatus.statusMap().begin() ; it != m_roStatus.statusMap().end() ; ++it){
        status->addStatus((*it).first , (*it).second);
      }
    } 
  }
}




