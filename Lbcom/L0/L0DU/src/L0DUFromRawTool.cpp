// $Id: L0DUFromRawTool.cpp,v 1.4 2008-02-07 17:24:52 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/L0DUReport.h"
// local
#include "L0DUFromRawTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUFromRawTool
//
// 2007-10-19 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUFromRawTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUFromRawTool::L0DUFromRawTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_report(),
    m_reportLocal(),
    m_processorDatas(),
    // DO NOT TOUCH !! IF YOU MODIFY THIS VALUE THIS WILL BREAK THE DC06 BACKWARD COMPATIBILITY
    m_tck(0xDC06), // default value for DC06 production (TCK was not implemented in Bank) 
    m_warning(true)
{
  declareInterface<IL0DUFromRawTool>(this);
  
  declareProperty( "RawLocation"    , m_rawLocation = LHCb::RawEventLocation::Default   );
  declareProperty( "EmulatorTool"   , m_emulatorType="L0DUEmulatorTool");
  declareProperty( "L0DUConfigProviderName"  , m_configName="L0DUConfig");
}
//=============================================================================
// Destructor
//=============================================================================
L0DUFromRawTool::~L0DUFromRawTool() {} 



//=============================================================================
StatusCode L0DUFromRawTool::initialize(){
  debug() << "Initialize  L0EmulatorTool" << endreq;
  StatusCode sc = GaudiTool::initialize();
   if(sc.isFailure())return sc;

  // get the configuration provider tool
  m_confTool = tool<IL0DUConfigProvider>("L0DUMultiConfigProvider" , m_configName);
  m_emuTool  = tool<IL0DUEmulatorTool>(m_emulatorType, m_emulatorType,this);
  m_condDB   = tool<IL0CondDBProvider>("L0CondDBProvider");

  // create Empty ProcessorData
  // Built L0ProcessorData
  m_processorDatas = new LHCb::L0ProcessorDatas();
  for( int fiber = 0 ; fiber != (int) L0DUBase::Fiber::Empty ; fiber++){
    LHCb::L0ProcessorData* temp = new LHCb::L0ProcessorData ( (L0DUBase::Fiber::Type) fiber , L0DUBase::EmptyData );
    m_processorDatas->insert( temp );
  }

  

  return sc;
}


// ---------------------------------------------
bool L0DUFromRawTool::getL0DUBanksFromRaw( ){

  m_banks = NULL;
  LHCb::RawEvent* rawEvt = NULL ;



  if( !exist<LHCb::RawEvent>( m_rawLocation ) ){
    warning() << "rawEvent not found at location '" << rootInTES() + m_rawLocation << "'" << endmsg;
    return false;
  }      
  
  rawEvt= get<LHCb::RawEvent>( rootInTES() + m_rawLocation );
  m_banks= &rawEvt->banks(   LHCb::RawBank::L0DU );

  
  debug() << "Number of L0DU bank(s) found : " << m_banks->size() << endreq; // should be == 1 for L0DU
  if( 0 == m_banks->size() ) {
    Error("No L0DU bank found in rawEvent",StatusCode::SUCCESS).ignore();
    return false;
  }
  if( 1 != m_banks->size() )Warning("More than one L0DU bank has been found in the RawEvent", StatusCode::SUCCESS).ignore();
  return true;
} 


bool L0DUFromRawTool::decodeBank(int ibank){


  // ------------------------
  // clear -------------------
  // ------------------------
  LHCb::L0DUConfig* config = NULL;
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
    error() << "Bank point to NULL " << endreq;
    return false;
  }


  // ------------------------
  // decoding ---------------
  // ------------------------

  // rawBank header :
  unsigned int* data = bank->data();
  m_size                = bank->size();  // Bank size is in bytes
  m_vsn                 = bank->version();
  unsigned int sourceID = bank->sourceID();

  int nm        = 0 ;
  int np        = 0 ;


  if ( msgLevel( MSG::DEBUG) ){
    debug() << " <============= Start decoding bank =============>" << endreq;
    debug() << "Decoding bank : " << bank << " Source : " << sourceID << " Version : " 
            << m_vsn << " Size " << m_size << " (bytes) " << endreq;



  }

  // Version 0 : preliminary version used for DC06 simulated data
  //--------------------------------------------------------------
  if(m_vsn == 0){
    unsigned int word;
    word = *data;
    m_report.setDecision( (bool) (word & 0x1) );
    m_report.setChannelsDecisionSummary( word >> 1 );
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    word = *data;
    m_report.setConditionsValueSummary( word );
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    word = *data;
    m_dataMap["PUPeak1(Cont)"]=  word & 0xFF;
    m_dataMap["PUPeak1(Add)" ]= (word>>8) & 0xFF;
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    word = *data;
    m_dataMap["PUPeak2(Cont)"]=  word & 0xFF;
    m_dataMap["PUPeak2(Add)" ]= (word>>8) & 0xFF;
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    word = *data;
    m_dataMap["PUHits(Mult)"]=  word & 0xFF;
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    word = *data;
    m_dataMap["Sum(Et)"]=  word & 0x3FFF;
    m_dataMap["Spd(Mult)" ]= (word>>14) & 0x3FFF;

    // No TCK mechanism at time of version 1 :  TCK = FFFE assumed
    if ( msgLevel( MSG::DEBUG) )debug() <<"For rawBank version 0 the TCK is forced to : " << m_tck  << endreq;
    if(m_warning){
      m_warning = false;
      std::stringstream tck("");
      tck << format("0x%04X", m_tck);
      info() << "For L0DU bank version = 0 the TCK value is forced to " << tck.str() << endreq;
    } 
    if ( msgLevel( MSG::DEBUG) )debug() << "Loading configuration" << endreq;
    config = m_confTool->config( m_tck );

    if( NULL == config){
      error() << " Unable to load the configuration for tck = " << m_tck << endreq;
      return false;
    }
    m_report.setConfiguration(config);
    if ( msgLevel( MSG::DEBUG) )debug() << "L0DU bank version 0 decoded" << endreq;
  }
  // Version 1 : complete RawBank as described in EDMS 868071
  //---------------------------------------------------------
  else if(m_vsn == 1){  

    // global header
    unsigned int itc      = (*data & 0x00000003)  >> 0;
    unsigned int iec      = (*data & 0x0000000C)  >> 2;
    m_pgaVsn              = (*data & 0x00000FF0)  >> 4;
    m_status              = (*data & 0x0000F000)  >> 12;
    m_tck                 = (*data & 0xFFFF0000)  >> 16;
    
    debug() << "-- Global header ==>  TCK = " << m_tck << " L0DU Status : " 
            << m_status << " Firmware version : " << m_pgaVsn << endreq;
    verbose() << "      --> Number of Condition & Channel summaries are : " 
              << iec << " / " << itc << " respectively " <<endreq;
    
    //---------------------------------
    // get corresponding configuration
    //---------------------------------
    config = m_confTool->config( m_tck);
    if( NULL == config){
      error() << " Unable to load the configuration for tck = " << m_tck << endreq;
      return false;
    }
    m_report.setConfiguration(config);
    //---------------------------------
    
    // PGA3-block header
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    
    unsigned int pga3Status    = (*data & 0x001FFFFF ) >>  0;
    m_bcid3                    = (*data & 0x0FE00000 ) >> 21;
    unsigned int  nmu          = (*data & 0xF0000000 ) >> 28;
    
    
    m_dataMap["MuonCU0(Status)"] = (pga3Status >> 0) & 0xF;
    m_dataMap["MuonCU1(Status)"] = (pga3Status >> 4) & 0xF;
    m_dataMap["MuonCU2(Status)"] = (pga3Status >> 8) & 0xF;
    m_dataMap["MuonCU3(Status)"] = (pga3Status >> 12)& 0xF;
    
    
    debug() << "-- PGA3 header ==> Processors status : "  << pga3Status  
            << "  BCID : " <<  m_bcid3 << endreq;
    debug() << "      --> Number of L0Muon on bank (0-sup) : " << nmu     << endreq;
    unsigned int pga3Size = 4 * ( 4 + ( int((nmu+3)/4) ) + ( int((nmu+1)/2) ) );
    debug() << "  PGA3 block size should be : "<< pga3Size  << " (bytes) " <<endreq;
    
    
    // PGA3 processing
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Muon1(Pt)"]      = (*data & 0x0000007F ) >> 0;
    m_dataMap["Muon1(Sgn)"]     = (*data & 0x00000080 ) >> 7;
    m_dataMap["Muon2(Pt)"]      = (*data & 0x00007F00 ) >> 8;
    m_dataMap["Muon2(Sgn)"]     = (*data & 0x00008000 ) >> 15;
    m_dataMap["Muon3(Pt)"]      = (*data & 0x007F0000 ) >> 16;
    m_dataMap["Muon3(Sgn)"]     = (*data & 0x00800000 ) >> 23;
    m_dataMap["DiMuon(Pt)"]     = (*data & 0xFF000000 ) >> 24;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Muon1(Add)"]      = (*data & 0x0000FFFF ) >>0;
    m_dataMap["Muon2(Add)"]      = (*data & 0xFFFF0000 ) >>16;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Muon3(Add)"]      = (*data & 0x0000FFFF)  >> 0;
    m_muCleanPattern             = (*data & 0x00FF0000)  >>16;
    
    verbose() << " L0Muon processed data decoded  - Muon cleaning pattern = " 
              << m_muCleanPattern << endreq;
    
    // PGA3 Input data
    int n[8]; 
    for(unsigned int imu =0 ; imu < nmu ; imu++){
      int odd = imu-int(imu/2)*2;
      if(odd == 0 && NULL == ++data){
        error()<<"No more data in bank --> CORRUPTION" <<endreq; 
        return false;
      }
      std::stringstream num("");
      n[imu] = ((*data  >> 16*odd) & 0x0000E000 ) >> 13;
      num << n[imu];
      m_dataMap["M"+num.str()+"(Add)"] = (*data  >> 16*odd) & 0x0000FFFF ;
    }
    
    for(unsigned int imu =0 ; imu < nmu ; imu++){
      int biodd = imu-int(imu/4)*4;
      if(biodd == 0 && NULL == ++data){error()<<"No more data in bank --> CORRUPTION" 
                                              <<endreq; return false;}
      std::stringstream num("");
      num << n[imu];
      m_dataMap["M"+num.str()+"(Pt)"] =   (*data >> 8*biodd) & 0x0000007F ;
      m_dataMap["M"+num.str()+"(Sgn)"] = ((*data >> 8*biodd) & 0x00000080) >> 7;
    }
    
    verbose() << " L0Muon input data decoded  ( " << nmu << " )" << endreq;
    
    // PGA2-block header
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_rsda       = (*data & 0x0000FFFF );
    unsigned int pga2Status= (*data & 0x0FFF0000 ) >> 16;
    nm                     = (*data & 0x30000000 ) >> 28;
    np                     = (*data & 0xC0000000 ) >> 30;
    m_bcid2                =  m_rsda & 0xFFF;
    unsigned int decision  = (m_rsda >> 12) & 1;
    debug() << "-- PGA2 header ==> Processor Status : "  << pga2Status << " BCID : "<< m_bcid2 << endreq;
    debug() << "      --> RSDA : " <<  m_rsda << " Decision : " << decision << endreq;
    verbose() << "      --> Number of Previous/Next BX : " << nm << "/" << np << endreq;
    // update L0DUReport
    m_report.setDecision( (bool) decision );
    
    
    m_dataMap["Electron(Status)"] = (pga2Status>>L0DUBase::Fiber::CaloElectron)  & 0x1;
    m_dataMap["Photon(Status)"]   = (pga2Status>>L0DUBase::Fiber::CaloPhoton)    & 0x1;
    m_dataMap["Hadron(Status)"]   = (pga2Status>>L0DUBase::Fiber::CaloHadron)    & 0x1;
    m_dataMap["GlobalPi0(Status)"]= (pga2Status>>L0DUBase::Fiber::CaloPi0Local)  & 0x1;
    m_dataMap["LocalPi0(Status)"] = (pga2Status>>L0DUBase::Fiber::CaloPi0Global) & 0x1;
    m_dataMap["Sum(Status)"]      = (pga2Status>>L0DUBase::Fiber::CaloSumEt)     & 0x1;
    m_dataMap["Spd(Status)"]      = (pga2Status>>L0DUBase::Fiber::CaloSpdMult)   & 0x1;
    m_dataMap["PU1(Status)"]      = (pga2Status>>L0DUBase::Fiber::Pu1)           & 0x1;
    m_dataMap["PU2(Status)"]      = (pga2Status>>L0DUBase::Fiber::Pu2)           & 0x1;
    
    // check the bank size is consistent
    
    unsigned int pga2Size = 4 * ( 7 + iec + 2*itc +  (itc+iec)*(nm+np) 
                                  + (int( (nm+1)/2) + int ((np+1)/2)));
    debug() << "  PGA2 block size should be " << pga2Size << " (bytes)" << endreq;
    unsigned int allSize = 4 + pga2Size + pga3Size;
    if(m_size == allSize){
      debug() << "---> The total expected size (header + PGA2 + PGA3) " << allSize 
              << " matches the actual bank size ________________ <** OK **> " <<endreq;
    }else{
    error() << "---> The total expecte size "<<allSize 
            << " does NOT match the bank size __________________  <** POSSIBLE DATA CORRUPTION **>" 
            << endreq;
    }
    
    if ( (0x7F & m_bcid2) == m_bcid3){
      debug() 
        << "---> The PGA3 and PGA2 data are aligned _____________________________________ <** OK **> " 
        << endreq;
    }else{
      error() << "---> The PGA3 and PGA2 data are NOT aligned  BCIDs (MSB)= " 
              <<  (m_bcid2 & 0x7F) << " /"  << m_bcid3 << endreq;
    }
    
    
    // PGA2 processing
    //clear summaries Maps
    m_ecs.clear();
    m_tcs.clear();
    m_fcs.clear();
    
    for(unsigned int i = 0 ; i < itc ; i++){
      if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
      m_fcs[std::make_pair(0,i)] = *data;
    }
    for(unsigned int i = 0 ; i < itc ; i++){
      if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
      m_tcs[std::make_pair(0,i)]=*data;
    }
    for(unsigned int i = 0 ; i < iec ; i++){
      if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
      m_ecs[std::make_pair(0,i)]=*data;
    }
    
    
    verbose() << "PGA2 decision processing decoded " <<endreq;
    
    //PGA2 input (*data
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    
    m_dataMap["Sum(Et)"]      = (*data & 0x00003FFF );
    m_sumEt[0] = m_dataMap["Sum(Et)"];
    m_dataMap["Spd(Mult)"]    = (*data & 0x0FFFC000 ) >> 14;
    m_dataMap["PU(MoreInfo)"] = (*data & 0xF0000000 ) >> 28;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Electron(Et)" ] = (*data & 0x000000FF ) >> 0;
    m_dataMap["Photon(Et)"   ] = (*data & 0x0000FF00 ) >> 8;
    m_dataMap["GlobalPi0(Et)"] = (*data & 0x00FF0000 ) >> 16;
    m_dataMap["LocalPi0(Et)" ] = (*data & 0xFF000000 ) >> 24;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Hadron(Et)"   ] = (*data & 0x000000FF ) >> 0;
    m_dataMap["PUPeak1(Cont)"] = (*data & 0x0000FF00 ) >> 8;
    m_dataMap["PUPeak2(Cont)"] = (*data & 0x00FF0000 ) >> 16;
    m_dataMap["PUHits(Mult)" ] = (*data & 0xFF000000 ) >> 24;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Electron(Add)" ] = (*data & 0x0000FFFF ) >> 0;
    m_dataMap["Photon(Add)"   ] = (*data & 0xFFFF0000 ) >> 16;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["GlobalPi0(Add)"] = (*data & 0x0000FFFF );
    m_dataMap["LocalPi0(Add)" ] = (*data & 0xFFFF0000 ) >> 16;
    
    if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
    m_dataMap["Hadron(Add)"  ]  = (*data & 0x0000FFFF );
    m_dataMap["PUPeak1(Add)" ]  = (*data & 0x00FF0000 ) >> 16;
    m_dataMap["PUPeak2(Add)" ]  = (*data & 0xFF000000 ) >> 24;
    
    verbose() << " L0Calo & L0PileUp input data decoded " <<endreq;
    
    
    // Previous/Next BX
    for(unsigned int im = nm ; im > 0 ; im--){
      for(unsigned int i = 0 ; i < itc ; i++){
        if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
        m_tcs[std::make_pair(im,i)] = *data;
      }
      for(unsigned int i = 0 ; i < iec ; i++){
        if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
        m_ecs[std::make_pair(im,i)] = *data;
      }
    }
    verbose() << nm << " Previous BX summaries decoded " << endreq;  
    
    
    // SumEt
    for(unsigned int im = nm ; im > 0 ; im--){
      int odd = im-int(im/2)*2;
      if(odd == 0 &&  NULL == ++data){error()<<"No more data in bank --> CORRUPTION" 
                                             <<endreq; return false;}
      std::stringstream num("");
      num << im;
      m_sumEt[im]= (*data >> 16*odd) & 0x3FFF ;
    }
    verbose() << nm << " Previous BX sumET decoded " << endreq;
    

    
    for(int ip = 0 ; ip < np ; ip++){
      for(unsigned int i = 0 ; i < itc ; i++){
        if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
        m_tcs[std::make_pair(ip,i)] = *data;
      }
      for(unsigned int i = 0 ; i < iec ; i++){
        if( NULL == ++data){error()<<"No more data in bank --> CORRUPTION" <<endreq; return false;}
        m_ecs[std::make_pair(ip,i)] = *data;
      }
    }
    verbose() << np << " Next BX summaries decoded " << endreq;  
    
    for(int ip = 0 ; ip < np ; ip++){
      int odd = ip-int(ip/2)*2;
      if(odd == 0 && NULL == ++data){error()<<"No more data in bank --> CORRUPTION" 
                                            <<endreq; return false;}
      std::stringstream num("");
      num << ip;
      m_sumEt[ip]                        = (*data >> 16*odd) & 0x3FFF ;
    }
    verbose() << nm << " Next BX sumET decoded " << endreq;
    
    // check the size 
    verbose() << " <============= Decoding completed successfuly =============>" << endreq;
    
    // Print all data values extracted from rawBank
    if ( msgLevel( MSG::VERBOSE) ){
      for(std::map<std::string, unsigned int>::iterator imap = m_dataMap.begin();
          imap!=m_dataMap.end();imap++){
        verbose() << "Data " << (*imap).first << " = " <<  (*imap).second << endreq;
      }
    }
    
    // -----------------------------------------------
    // BUILDING output (L0ProcessorData & L0Report)
    // -----------------------------------------------
    verbose() << "fill processor Data" <<endreq;
    // fill L0ProcessorData with input data in rawBank
    fillProcessorData();
    //  emulate the config accordingly for later usage (monitoring) 
    if(NULL != config)m_emuTool->process(config , m_processorDatas);
    // Fill report with the consecutive BXs info
    verbose() << "filling L0DU Report with consecutive BX" << endreq;
    m_report.setChannelsDecisionSummaries( m_tcs );
    m_report.setConditionsValueSummaries(  m_ecs );
    m_report.setSumEt( m_sumEt );    
    m_report.setFiredChannelsSummaries( m_fcs );  
    verbose() 
      << " <==== Building output (emulated ProcessorData & L0DUReport) completed successfuly ====>" 
      << endreq;
    
    //---------------------------------------------------------
    // End of coding version = 1
    //---------------------------------------------------------
    
  }  else{
    error() << " Unknown bank version " << m_vsn << endreq;
    return false;
  }
  
  return true;
}


const unsigned int L0DUFromRawTool::data(std::string name){
  std::map<std::string, unsigned int>::iterator it = m_dataMap.find(name);
  if(m_dataMap.end() == it){
    warning() << " Requested data " << name  << " is unknown " << endreq;
    return 0;
  }
  return (*it).second;
}

const double L0DUFromRawTool::scale(unsigned int base){
  return m_condDB->scale( base );
}




void L0DUFromRawTool::fillProcessorData(){



  encode(m_dataMap["Electron(Et)"]      , L0DUBase::Electron::Et      );
  encode(m_dataMap["Electron(Add)"]     , L0DUBase::Electron::Address );
  encode(m_bcid2                        , L0DUBase::Electron::BCID    );
  encode(m_dataMap["Electron(Status)"]  , L0DUBase::Electron::Status  );
  encode(m_dataMap["Photon(Et)"]        , L0DUBase::Photon::Et      );
  encode(m_dataMap["Photon(Add)"]       , L0DUBase::Photon::Address );
  encode(m_bcid2                        , L0DUBase::Photon::BCID    );
  encode(m_dataMap["Photon(Status)"]    , L0DUBase::Photon::Status  );
  encode(m_dataMap["Hadron(Et)"]        , L0DUBase::Hadron::Et      );
  encode(m_dataMap["Hadron(Add)"]       , L0DUBase::Hadron::Address );
  encode(m_bcid2                        , L0DUBase::Hadron::BCID    );
  encode(m_dataMap["Hadron(Status)"]    , L0DUBase::Hadron::Status  );
  encode(m_dataMap["GlobalPi0(Et)"]     , L0DUBase::Pi0Global::Et      );
  encode(m_dataMap["GlobalPi0(Add)"]    , L0DUBase::Pi0Global::Address );
  encode(m_bcid2                        , L0DUBase::Pi0Global::BCID    );
  encode(m_dataMap["GlobalPi0(Status)"] , L0DUBase::Pi0Global::Status  );
  encode(m_dataMap["LocalPi0(Et)"]      , L0DUBase::Pi0Local::Et      );
  encode(m_dataMap["LocalPi0(Add)"]     , L0DUBase::Pi0Local::Address );
  encode(m_bcid2                        , L0DUBase::Pi0Local::BCID    );
  encode(m_dataMap["LocalPi0(Status)"]  , L0DUBase::Pi0Local::Status  );
  encode(m_dataMap["Sum(Et)"]           , L0DUBase::Sum::Et      );
  encode(m_bcid2                        , L0DUBase::Sum::BCID    );
  encode(m_dataMap["Sum(Status)"]       , L0DUBase::Sum::Status  );
  encode(m_dataMap["Spd(Mult)"]         , L0DUBase::Spd::Mult      );
  encode(m_bcid2                        , L0DUBase::Spd::BCID    );
  encode(m_dataMap["Spd(Status)"]       , L0DUBase::Spd::Status  );

  encode(m_dataMap["M0(Pt)"]            , L0DUBase::Muon1::Pt      );
  encode(m_dataMap["M0(Sgn)"]           , L0DUBase::Muon1::Sign   );
  encode(m_dataMap["M0(Add)"]           , L0DUBase::Muon1::Address);
  encode(m_bcid3                        , L0DUBase::Muon1::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon1::BCID2);
  encode(m_dataMap["MuonCU0(Status)"]   , L0DUBase::Muon1::Status  );
  encode(m_dataMap["M1(Pt)"]            , L0DUBase::Muon2::Pt      );
  encode(m_dataMap["M1(Sgn)"]           , L0DUBase::Muon2::Sign   );
  encode(m_dataMap["M1(Add)"]           , L0DUBase::Muon2::Address);
  encode(m_bcid3                        , L0DUBase::Muon2::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon2::BCID2);
  encode(m_dataMap["M2(Pt)"]            , L0DUBase::Muon3::Pt      );
  encode(m_dataMap["M2(Sgn)"]           , L0DUBase::Muon3::Sign   );
  encode(m_dataMap["M2(Add)"]           , L0DUBase::Muon3::Address);
  encode(m_bcid3                        , L0DUBase::Muon3::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon3::BCID2);
  encode(m_dataMap["MuonCU1(Status)"]   , L0DUBase::Muon3::Status  );
  encode(m_dataMap["M3(Pt)"]            , L0DUBase::Muon4::Pt      );
  encode(m_dataMap["M3(Sgn)"]           , L0DUBase::Muon4::Sign   );
  encode(m_dataMap["M3(Add)"]           , L0DUBase::Muon4::Address);
  encode(m_bcid3                        , L0DUBase::Muon4::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon4::BCID2);
  encode(m_dataMap["M4(Pt)"]            , L0DUBase::Muon5::Pt      );
  encode(m_dataMap["M4(Sgn)"]           , L0DUBase::Muon5::Sign   );
  encode(m_dataMap["M4(Add)"]           , L0DUBase::Muon5::Address);
  encode(m_bcid3                        , L0DUBase::Muon5::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon5::BCID2);
  encode(m_dataMap["MuonCU2(Status)"]   , L0DUBase::Muon5::Status  );
  encode(m_dataMap["M5(Pt)"]            , L0DUBase::Muon6::Pt      );
  encode(m_dataMap["M5(Sgn)"]           , L0DUBase::Muon6::Sign   );
  encode(m_dataMap["M5(Add)"]           , L0DUBase::Muon6::Address);
  encode(m_bcid3                        , L0DUBase::Muon6::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon6::BCID2);
  encode(m_dataMap["M6(Pt)"]            , L0DUBase::Muon7::Pt      );
  encode(m_dataMap["M6(Sgn)"]           , L0DUBase::Muon7::Sign   );
  encode(m_dataMap["M6(Add)"]           , L0DUBase::Muon7::Address);
  encode(m_bcid3                        , L0DUBase::Muon7::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon7::BCID2);
  encode(m_dataMap["MuonCU3(Status)"]   , L0DUBase::Muon7::Status  );
  encode(m_dataMap["M7(Pt)"]            , L0DUBase::Muon8::Pt      );
  encode(m_dataMap["M7(Sgn)"]           , L0DUBase::Muon8::Sign   );
  encode(m_dataMap["M7(Add)"]           , L0DUBase::Muon8::Address);
  encode(m_bcid3                        , L0DUBase::Muon8::BCID1);
  encode(m_bcid3                        , L0DUBase::Muon8::BCID2);
  encode(m_dataMap["PUPeak1(Cont)"]     , L0DUBase::PileUp::Peak1 );
  encode(m_dataMap["PUPeak2(Cont)"]     , L0DUBase::PileUp::Peak2 );
  encode(m_dataMap["PUPeak1(Add)"]      , L0DUBase::PileUp::Peak1Pos );
  encode(m_dataMap["PUPeak2(Add)"]      , L0DUBase::PileUp::Peak2Pos );
  encode(m_dataMap["PUHits(Mult)"]      , L0DUBase::PileUp::Hits );
  encode(m_dataMap["PU(MoreInfo)"]      , L0DUBase::PileUp::MoreInfo );
  encode(m_bcid2                        , L0DUBase::PileUp::BCID1    );
  encode(m_bcid2                        , L0DUBase::PileUp::BCID2    );
  encode(m_dataMap["PU1(Status)"]       , L0DUBase::PileUp::Status1  );
  encode(m_dataMap["PU2(Status)"]       , L0DUBase::PileUp::Status2  );
}




void L0DUFromRawTool::encode(unsigned int data ,  const unsigned int base[L0DUBase::Index::Size]){
  LHCb::L0ProcessorData* fiber = m_processorDatas->object( base[ L0DUBase::Index::Fiber ]  )  ;
  unsigned int word = fiber->word();  
  word |= ( (data << base[L0DUBase::Index::Shift]) & base[L0DUBase::Index::Mask] );
  fiber->setWord( word);
  if( L0DUBase::Fiber::Empty != base[ L0DUBase::Index::Fiber2 ]  ) {
    fiber = m_processorDatas->object( base[ L0DUBase::Index::Fiber2 ]  )  ;
    word = fiber->word();
    unsigned int val = data >> base[L0DUBase::Index::Offset];
    word |= ( ( val << base[L0DUBase::Index::Shift2]) & base[L0DUBase::Index::Mask2] );
    fiber->setWord( word);
  }
}
