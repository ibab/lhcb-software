// Include files 

// from LHCb
#include "Event/ODIN.h"
// local
#include "L0DUEmulatorTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUEmulatorTool
//
// 2007-10-10 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUEmulatorTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUEmulatorTool::L0DUEmulatorTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_report(),
    m_muCleaning(),
    m_muZeroSup(),
    m_muHighest(),
    m_muPattern(0),
    m_decoder(NULL),
    m_condDB(NULL),
    m_odin(NULL),
    m_config(NULL),
    m_begEvent(true)
{
  declareInterface<IL0DUEmulatorTool>(this);
  declareProperty( "MuonCleaning" , m_muCleaning = false);
  declareProperty( "MuonZeroSup"  , m_muZeroSup    = false);
  m_nMu = 3;
  m_muHighest.reserve( m_nMu );
}
//=============================================================================
// Destructor
//=============================================================================
L0DUEmulatorTool::~L0DUEmulatorTool() {} 

//=============================================================================

StatusCode L0DUEmulatorTool::initialize(){
  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize  L0EmulatorTool" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  // register incident
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;

  // get tools
  m_decoder  = tool<IL0ProcessorDataDecoder>("L0ProcessorDataDecoder","L0ProcessorDataDecoder",this);
  m_condDB   = tool<IL0CondDBProvider>("L0CondDBProvider");
  m_odin     = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);
  return sc;
}


//=============================================================================
StatusCode L0DUEmulatorTool::process(LHCb::L0DUConfig* config,  LHCb::L0ProcessorDatas* datas){
  m_config = config;
  //  if(!m_decoder -> setL0ProcessorData(datas))return StatusCode::FAILURE;
  if(!m_decoder -> setL0ProcessorData(datas)){
    Warning("L0ProcessorData decoder failed",StatusCode::SUCCESS).ignore();
    counter("Error with Processor data")+=1;
  }  
  return processing();
}

StatusCode L0DUEmulatorTool::process(LHCb::L0DUConfig* config,  std::vector<std::string> dataLocs){
  m_config = config;
  //  if(!m_decoder -> setL0ProcessorData(dataLocs))return StatusCode::FAILURE;
  if(!m_decoder -> setL0ProcessorData(dataLocs)){
    Warning("L0ProcessorData decoder failed",StatusCode::SUCCESS).ignore();
    counter("Error with Processor data")+=1;
}  
  return processing();
}


//=============================================================================
StatusCode L0DUEmulatorTool::fillData(){
  if(m_config == NULL)return StatusCode::FAILURE;
  m_config->clearDataValue(); // reset data + emulation flag
  
  LHCb::L0DUElementaryData::Map& dataMap = m_config->data();
  using namespace L0DUBase::PredefinedData;

  setDataValue( dataMap[Name[ElectronEt ]] , L0DUBase::Electron::Et        );
  setDataValue( dataMap[Name[PhotonEt]]    , L0DUBase::Photon::Et          );
  setDataValue( dataMap[Name[HadronEt]]    , L0DUBase::Hadron::Et          );
  setDataValue( dataMap[Name[GlobalPi0Et]] , L0DUBase::Pi0Global::Et       );
  setDataValue( dataMap[Name[LocalPi0Et]]  , L0DUBase::Pi0Local::Et        );
  setDataValue( dataMap[Name[SumEt]]       , L0DUBase::Sum::Et             );
  setDataValue( dataMap[Name[SpdMult]]     , L0DUBase::Spd::Mult           );
  setDataValue( dataMap[Name[PuPeak1]]     , L0DUBase::PileUp::Peak1       );
  setDataValue( dataMap[Name[PuPeak2]]     , L0DUBase::PileUp::Peak2       );
  setDataValue( dataMap[Name[PuHits ]]     , L0DUBase::PileUp::Hits        );
  setDataValue( dataMap[Name[ElectronAdd]] , L0DUBase::Electron::Address   );
  setDataValue( dataMap[Name[PhotonAdd]]   , L0DUBase::Photon::Address     );
  setDataValue( dataMap[Name[HadronAdd]]   , L0DUBase::Hadron::Address     );
  setDataValue( dataMap[Name[GlobalPi0Add]], L0DUBase::Pi0Global::Address  );
  setDataValue( dataMap[Name[LocalPi0Add]] , L0DUBase::Pi0Local::Address   );
  // PileUp
  setDataValue(dataMap[Name[PuPeak1Pos]]   , L0DUBase::PileUp::Peak1Pos );
  setDataValue(dataMap[Name[PuPeak2Pos]]   , L0DUBase::PileUp::Peak2Pos );



  // The Muon case
  m_muPattern = 0;
  std::vector<int> muonVec;
  int pt1 = digit( L0DUBase::Muon1::Pt   );
  int pt2 = digit( L0DUBase::Muon2::Pt   );
  muonVec.push_back( pt1 );
  if(m_muCleaning && pt1 == pt2)  m_muPattern |= 1<<1 ; else muonVec.push_back( pt2 );

  int pt3 = digit( L0DUBase::Muon3::Pt   );
  int pt4 = digit( L0DUBase::Muon4::Pt   );
  muonVec.push_back(  pt3 );
  if(m_muCleaning && pt3 == pt4) m_muPattern |= 1<<3  ; else muonVec.push_back( pt4 );

  int pt5 = digit( L0DUBase::Muon5::Pt   );
  int pt6 = digit( L0DUBase::Muon6::Pt   );
  muonVec.push_back(  pt5 );
  if(m_muCleaning && pt5 == pt6)  m_muPattern |= 1<<5 ; else muonVec.push_back( pt6 );

  int pt7 = digit( L0DUBase::Muon7::Pt   );
  int pt8 = digit( L0DUBase::Muon8::Pt   );
  muonVec.push_back(  pt7 );
  if(m_muCleaning && pt7 == pt8)  m_muPattern |= 1<<7 ; else muonVec.push_back( pt8 );

  // get the 3 highest muon
  m_muHighest.clear();
  for(int i=0 ; i != m_nMu ; i++){
    int maxPt = -1;    
    int imax = -1;
    for(unsigned int  j=0 ; j != muonVec.size() ; j++){
      bool ok=true;
      for(unsigned int k=0 ; k !=m_muHighest.size() ; k++){
        if( (int) j == m_muHighest[k])
          ok=false;
      }
      if(!ok)continue;
      int pt = muonVec[j];
      if( maxPt < pt ){
        maxPt= pt ;
        imax=j;
      }
    }
    if(-1 == imax){
      Error("Error in muon processing").ignore();
    }else{
      m_muHighest.push_back(imax);
    }
  }
  if( msgLevel(MSG::VERBOSE))verbose() << "Muon sorted " << endmsg;

  int dimuon = 0;
  int dimuonP= 0;
  double ptScale = scale(L0DUBase::Muon1::Pt);
  double adScale = scale(L0DUBase::Muon1::Address);
  double sgScale = scale(L0DUBase::Muon1::Sign);
  long   ptMax   = max(  L0DUBase::Muon1::Pt);
  long   adMax   = max(  L0DUBase::Muon1::Address);
  long   sgMax   = max(  L0DUBase::Muon1::Sign);
  for(unsigned int i = 0; i != m_muHighest.size();++i){
    int k = m_muHighest[i];
    int pt = muonVec[k];
    if(i<2){
      dimuon += pt;
      if( i==0)dimuonP=pt;
      else 
        dimuonP *= pt;
    }
    std::stringstream num("");
    num << i+1;
    dataMap["Muon"+num.str()+"(Pt)"]->setOperand( pt , ptScale , ptMax );
    int add = 0;
    int sgn = 0;
    if( k == 0 ){
      add = digit( L0DUBase::Muon1::Address)  | (0 << 13)  ;
      sgn = digit( L0DUBase::Muon1::Sign    ) ;
    }else if( k == 1 ){
      add = digit( L0DUBase::Muon2::Address)  | (1 << 13)  ;
      sgn = digit( L0DUBase::Muon2::Sign    ) ;
    }else if( k == 2 ){
      add = digit( L0DUBase::Muon3::Address)  | (2 << 13)  ;
      sgn = digit( L0DUBase::Muon3::Sign    ) ;
    }else if( k == 3 ){
      add = digit( L0DUBase::Muon4::Address)  | (3 << 13)  ;
      sgn = digit( L0DUBase::Muon4::Sign    ) ;
    }else if( k == 4 ){
      add = digit( L0DUBase::Muon5::Address)  | (4 << 13)  ;
      sgn = digit( L0DUBase::Muon5::Sign    ) ;
    }else if( k == 5 ){
      add = digit( L0DUBase::Muon6::Address)  | (5 << 13)  ;
      sgn = digit( L0DUBase::Muon6::Sign    ) ;
    }else if( k == 6 ){
      add = digit( L0DUBase::Muon7::Address)  | (6 << 13)  ;
      sgn = digit( L0DUBase::Muon7::Sign    ) ;
    }else if( k == 7 ){
      add = digit( L0DUBase::Muon8::Address)  | (7 << 13)  ;
      sgn = digit( L0DUBase::Muon8::Sign    ) ;
    }
    dataMap["Muon"+num.str()+"(Add)"]->setOperand(add , adScale , adMax);
    dataMap["Muon"+num.str()+"(Sgn)"]->setOperand(sgn , sgScale , sgMax);
  }
  dataMap[Name[DiMuonPt]]->setOperand( dimuon , ptScale , ptMax*2+1);  
  dataMap[Name[DiMuonProdPt]]->setOperand( dimuonP , ptScale , (ptMax+1)*(ptMax+1)-1);  
  if( msgLevel(MSG::VERBOSE))verbose() << "DiMuon OK " << endmsg;

  // -------------------------------------
  // Data processing of user-defined data
  // ------------------------------------  
  for( LHCb::L0DUElementaryData::Map::iterator idata = dataMap.begin();idata != dataMap.end() ; idata++){
    LHCb::L0DUElementaryData* data = (*idata).second;
    // Fill RAM(BCID)
    const std::string dataName = data->name();
    int index = dataName.rfind("(BCID)");
    if( (int) dataName.rfind("RAM") != (int) std::string::npos && index != (int) std::string::npos ){
      std::string ram = dataName.substr(0,index);
      // get BCID from ODIN
      m_odin->getTime();
      unsigned int odBX = 0;
      const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
      if( NULL != odin ){
        odBX = odin->bunchId();
      }else{
        Warning( "Emtpy location for ODIN '"+ LHCb::ODINLocation::Default +"'" ,StatusCode::SUCCESS).ignore();
      }
      const int ramBcid = m_condDB->RAMBCID( ram , odBX);
      //info() << "--------------- " << odBX << " " << ram << " " << ramBcid << endmsg;
      data->setOperand( ramBcid  , m_condDB->scale(L0DUBase::Type::Digit) , L0DUBase::RAMBCID::Max  );
    }
    if( data->type() != LHCb::L0DUElementaryData::Compound )continue;
    StatusCode sc = dataTree(data, dataMap);
    if(sc.isFailure())return sc;
  }       
              
  if( msgLevel(MSG::VERBOSE)) {
    verbose() << "User-defined data filled " << endmsg;
    verbose() << " ---> data summary " << endmsg;
    for( LHCb::L0DUElementaryData::Map::iterator idata = dataMap.begin();idata != dataMap.end() ; idata++){
      verbose() << "Data :  " << ((*idata).second)->summary() <<endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}

//===========================================================================================================
void L0DUEmulatorTool::setDataValue(LHCb::L0DUElementaryData* l0Data, 
                                    const unsigned int  base[L0DUBase::Index::Size]){
  l0Data->setOperand( digit( base ) , scale(base) , max(base)  );
  if( msgLevel(MSG::VERBOSE))
    verbose() << "Set Data digit " << l0Data->name() << " : " <<digit(base) << " : scale  = " << scale(base) << endmsg;
}
//===========================================================================================================
StatusCode  L0DUEmulatorTool::dataTree(LHCb::L0DUElementaryData* data, LHCb::L0DUElementaryData::Map dataMap   ){  
  if( data->type() != LHCb::L0DUElementaryData::Compound )return StatusCode::SUCCESS;
  std::vector<std::string>::const_iterator iop = data->operandsName().begin();
  double scale = 0;
  for(iop =  data->operandsName().begin() ; iop != data->operandsName().end() ; iop++){
    LHCb::L0DUElementaryData* preData = dataMap[ *iop ];
    // process pre-Data (if not yet done)
    LHCb::L0DUElementaryData::Map::key_compare kc = dataMap.key_comp();
    if( kc( data->name(), preData->name() ))dataTree( preData, dataMap);
    //    if( std::map::key_compare<std::string>(data->name(), preData->name() ))dataTree( preData, dataMap);
    // set the same scale as the 1st pre-data
    if(iop ==  data->operandsName().begin()){ 
      scale = preData->scale();
      data->setScale( scale );
    }
    data->addOperand( preData->digit() );
    data->setSaturation( preData->saturation() + data->saturation() );
    // check the scale is the same for the other operands
    if( scale != preData->scale() ){ 
      error() << "Cannot combine data with different scales in the compound data "<< *iop << endmsg;
      return StatusCode::FAILURE;
    }
    scale = preData->scale();
  }
  return StatusCode::SUCCESS;
}
 
//===========================================================================================================
StatusCode L0DUEmulatorTool::processing(){
  if(fillData().isFailure())return StatusCode::FAILURE;  
  m_config->setCompleted(true);

  // downscaling counters updated once per event (paranoid pretection against multi-call of the emulator processing  / event)
  
  std::map<unsigned int,bool>::iterator it = m_procMap.find( m_config->tck() );
  if( it == m_procMap.end() || (*it).second ){
    m_config->updateCounters( true );
    m_procMap[ m_config->tck() ]=false;
  } else {
    m_config->updateCounters( false );
  } 
  
  m_config->emulate();  // process the actual emulation  @ each processing (no longer onDemand becayse of downscaling)

  // output
  if( msgLevel(MSG::VERBOSE)) {
    verbose() << "End of processing ... summary : " << endmsg;
    verbose() << m_config->summary() << endmsg;
  }
  //
  return StatusCode::SUCCESS;
}
//===========================================================================================================
const LHCb::L0DUReport L0DUEmulatorTool::emulatedReport(){
  m_report.clear();
  m_report.setConfiguration(m_config);
  if( !m_config->emulated() ){
    error() << " Report is requested but the emulator has not been processed ... return empty report" << endmsg;
    return m_report;
  }
  
  m_report.setDecisionValue( m_config->emulatedDecisionValue()  );
  m_report.setSumEt( digit(L0DUBase::Sum::Et) );

  
  LHCb::L0DUElementaryCondition::Map& conditionMap  = m_config->conditions();
  LHCb::L0DUChannel::Map&             channelMap    = m_config->channels();

  for( LHCb::L0DUChannel::Map::iterator ichan = channelMap.begin() ; ichan != channelMap.end() ; ichan++){
    LHCb::L0DUChannel* channel = (*ichan).second ;
    m_report.setChannelDecision(channel->name(), channel->emulatedDecision() );
    m_report.setChannelPreDecision(channel->name(), channel->emulatedPreDecision() );
  }

  for( LHCb::L0DUElementaryCondition::Map::iterator icond = conditionMap.begin() ; icond != conditionMap.end() ; icond++){
    LHCb::L0DUElementaryCondition* condition = (*icond).second ;
    if(condition->reported())m_report.setConditionValue(condition->name(), condition->emulatedValue() );
  }
  return m_report;
}

unsigned long L0DUEmulatorTool::digit(const unsigned int   base[L0DUBase::Index::Size]){
  return m_decoder->digit(base);
}
double L0DUEmulatorTool::scale(const unsigned int base[L0DUBase::Index::Size]){
  return m_condDB->scale(base[L0DUBase::Index::Scale]);
}

long L0DUEmulatorTool::max(const unsigned int base[L0DUBase::Index::Size]){
  long  w1 = base[L0DUBase::Index::Mask] >> base[L0DUBase::Index::Shift];
  long  w2 = base[L0DUBase::Index::Mask2] >> base[L0DUBase::Index::Shift2];
  long  w  = w1 | (w2 << base[L0DUBase::Index::Offset]);
  return  w;
}




//=============================================================================
const std::vector<unsigned int> L0DUEmulatorTool::bank(unsigned int version){

  std::vector<unsigned int> l0Block;


  emulatedReport(); // emulate the report
  if( !m_config->emulated() ){
    error() << " rawBank is requested but the emulator has not been processed ... return empty bank" << endmsg;
    return l0Block;
  }
  

  // Version 0 : preliminary version used for DC06 simulated data
  //--------------------------------------------------------------
  if( 0 == version ){

    unsigned int word = ( (m_report.channelsDecisionSummary() << 1) | (m_report.decisionValue() & 0x1 )) & 0xFFFF;
    
    l0Block.push_back( word ); // decision Pattern (first 32 bits)
    l0Block.push_back( m_report.conditionsValueSummary() ); // condition Pattern (first 32 bits)
    l0Block.push_back( digit( L0DUBase::PileUp::Peak1 ) |
                       digit(L0DUBase::PileUp::Peak1Pos ) << 8 ); // PileUp Peak1
    l0Block.push_back( digit(L0DUBase::PileUp::Peak2 ) |
                       digit(L0DUBase::PileUp::Peak2Pos ) << 8 ); // PileUp Peak2
    l0Block.push_back( digit(L0DUBase::PileUp::Hits    ) ); // PileUp #Hits
    l0Block.push_back( digit(L0DUBase::Sum::Et )    |
                       digit(L0DUBase::Spd::Mult ) << 14  ); // SumEt + SpdMult

  }
  // Version 1 : complete RawBank as described in EDMS 868071
  //---------------------------------------------------------
  else  if( 1 == version || 2 == version ){

    // simulation : no previous/next report
    unsigned int nBxp = 0;
    unsigned int nBxm = 0;


    // global header ( simulation : no PGA f/w version )
    unsigned int ec_size = (m_report.conditionsValueSummaries().size() ) & 0x3 ;
    unsigned int tc_size = (m_report.channelsDecisionSummaries().size() ) & 0x3;
    unsigned int tck     = m_report.tck() & 0xFFFF; 
    unsigned int word  =  tc_size  |  ec_size << 2 | tck << 16;
    if( 2 == version ) word |= ( nBxm & 0x3 ) << 12 | ( nBxp & 0x3 ) << 14;    
    l0Block.push_back( word );

    // ------------
    // PGA3-Block
    // -----------
    std::vector<int> ptMu;
    std::vector<int> adMu;
    ptMu.push_back( digit( L0DUBase::Muon1::Pt   ) | ( digit( L0DUBase::Muon1::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon2::Pt   ) | ( digit( L0DUBase::Muon2::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon3::Pt   ) | ( digit( L0DUBase::Muon3::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon4::Pt   ) | ( digit( L0DUBase::Muon4::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon5::Pt   ) | ( digit( L0DUBase::Muon5::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon6::Pt   ) | ( digit( L0DUBase::Muon6::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon7::Pt   ) | ( digit( L0DUBase::Muon7::Sign    ) << 7 ) );
    ptMu.push_back( digit( L0DUBase::Muon8::Pt   ) | ( digit( L0DUBase::Muon8::Sign    ) << 7 ) );
    adMu.push_back( digit( L0DUBase::Muon1::Address)  | (0 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon2::Address)  | (1 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon3::Address)  | (2 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon4::Address)  | (3 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon5::Address)  | (4 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon6::Address)  | (5 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon7::Address)  | (6 << 13)  );
    adMu.push_back( digit( L0DUBase::Muon8::Address)  | (7 << 13)  );

    unsigned int nmuons  =0;
    unsigned int muPt[2]={0,0};
    unsigned int muAdd[4]={0,0,0,0};
    for(int i=0 ; i !=8 ; i++){
      // 0-suppression
      if( 0 != ptMu[ i  ] || !m_muZeroSup ){ 
        int ipt = (int) nmuons/4;
        int iad = (int) nmuons/2;
        muPt[ipt]  |= ( ptMu[ i ] << (nmuons - ipt*4)*8 );
        muAdd[iad] |= ( adMu[ i ] << (nmuons - iad*2)*16);
        nmuons++;
      }      
    }
    
    // Fill MuonBlock
    l0Block.push_back( digit(L0DUBase::Muon1::Status)       |
                       digit(L0DUBase::Muon3::Status) << 4  |
                       digit(L0DUBase::Muon5::Status) << 8  |
                       digit(L0DUBase::Muon7::Status) << 12 |
                       nmuons << 28 );
    // Processed muon
    std::vector<int> muonMax = m_muHighest;
    int i1 = m_muHighest[0];
    int i2 = m_muHighest[1];
    int i3 = m_muHighest[2];
    unsigned int dimu =  (ptMu[i1] & 0x7F) + (ptMu[i2] & 0x7F);
    l0Block.push_back( ptMu[i1] | ptMu[i2] << 8 | ptMu[i3] << 16 | dimu << 24 );
    l0Block.push_back( adMu[i1] | adMu[i2] << 16 );
    l0Block.push_back( adMu[i3] |  m_muPattern << 16);

    // Zero-suppressed input muons
    for(unsigned int i=0; i < (nmuons+1)/2 ;++i){
      l0Block.push_back(muAdd[i]);
    }
    for(unsigned int i=0; i < (nmuons+3)/4 ;++i){
      l0Block.push_back(muPt[i]);
    }

    // ------------
    // PGA2-Block
    // -----------
    unsigned int l0Id = 0 ;

    unsigned int rsda = 0;
    if( version == 1 ){
      rsda = (l0Id & 0xFFF) ;
      rsda |= ( m_report.decision(LHCb::L0DUDecision::Physics) ) << 12;
    }
    else if( version == 2 ){
      rsda = (l0Id & 0x3FF);
      rsda |= ( m_report.decision(LHCb::L0DUDecision::Physics) ) << 12;
      rsda |= ( m_report.decision(LHCb::L0DUDecision::Beam2)   ) << 11;
      rsda |= ( m_report.decision(LHCb::L0DUDecision::Beam1)   ) << 10;
    }
    
    unsigned int status = 
      digit(L0DUBase::Electron::Status) << L0DUBase::Fiber::CaloElectron  |
      digit(L0DUBase::Photon::Status)   << L0DUBase::Fiber::CaloPhoton    |         
      digit(L0DUBase::Hadron::Status)   << L0DUBase::Fiber::CaloHadron    |          
      digit(L0DUBase::Pi0Local::Status) << L0DUBase::Fiber::CaloPi0Local  |          
      digit(L0DUBase::Pi0Global::Status)<< L0DUBase::Fiber::CaloPi0Global |          
      digit(L0DUBase::Sum::Status)      << L0DUBase::Fiber::CaloSumEt     |          
      digit(L0DUBase::Spd::Status)      << L0DUBase::Fiber::CaloSpdMult   |          
      digit(L0DUBase::PileUp::Status1)  << L0DUBase::Fiber::Pu1           |          
      digit(L0DUBase::PileUp::Status2)  << L0DUBase::Fiber::Pu2      ;    
    // simulation : no L0DU status bit
    int L0DUstatus = 0;
    status |= (L0DUstatus & 0x7) << 9;
    int w =  rsda | ( status  ) << 16 ;
    if( 1 == version ){
      int bx = ( l0Id & 0x300 ) << 18;
      w |= bx ;
    }
    if(  1 == version ) 
      w |= ( nBxm << 28 )  | ( nBxp <<30 ) ;

    l0Block.push_back( w ); // header
    // PGA2-Block
    for(unsigned int itc = 0 ; itc <tc_size ; ++itc ){
      l0Block.push_back( m_report.channelsPreDecisionSummary(itc)  & 0xFFFFFFFF ); // Channel summary
    }
    for(unsigned int itc = 0 ; itc < tc_size ; ++itc ){
      l0Block.push_back( m_report.channelsDecisionSummary(itc)  & 0xFFFFFFFF ); // Channel summary
    }
    for(unsigned int iec = 0 ; iec < ec_size ; ++iec ){
      l0Block.push_back( m_report.conditionsValueSummary(iec) & 0xFFFFFFFF ); // Condition Summary
    }
    l0Block.push_back( digit(L0DUBase::Sum::Et )            |
                       digit(L0DUBase::Spd::Mult ) << 14    |
                       digit(L0DUBase::PileUp::MoreInfo ) << 28 ); 
    l0Block.push_back( digit(L0DUBase::Electron::Et ) |
                       digit(L0DUBase::Photon::Et )    << 8 |
                       digit(L0DUBase::Pi0Global::Et ) << 16 |
                       digit(L0DUBase::Pi0Local::Et )  << 24 );
    l0Block.push_back( digit(L0DUBase::Hadron::Et ) |
                       digit( L0DUBase::PileUp::Peak1 ) << 8 |
                       digit( L0DUBase::PileUp::Peak2 ) << 16 |
                       digit( L0DUBase::PileUp::Hits )  << 24 );
#ifdef __INTEL_COMPILER       // Disable ICC warning
  #pragma warning(disable:68) // integer conversion resulted in a change of sign
  #pragma warning(push)
#endif
    l0Block.push_back( digit(L0DUBase::Electron::Address ) | 0x2 << 14 |
                       digit(L0DUBase::Photon::Address ) << 16 | 0x2 << 30 );
    l0Block.push_back( digit(L0DUBase::Pi0Global::Address ) | 0x2 << 14 |
                       digit(L0DUBase::Pi0Local::Address ) << 16 | 0x2 << 30 );
#ifdef __INTEL_COMPILER // Re-enable ICC warning 68
  #pragma warning(pop)
#endif
    l0Block.push_back( digit(L0DUBase::Hadron::Address ) | 0x3 << 14 |
                       digit( L0DUBase::PileUp::Peak1Pos) << 16 |
                       digit( L0DUBase::PileUp::Peak2Pos) << 24 );
    // put here the summaries for the sequence of consecutive BX if any
    
  }
  else{
    warning() << "L0DU RawBank version " << version << " is not defined " << endmsg;
  }
  return l0Block;
}



