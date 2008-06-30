// $Id: L0DUEmulatorTool.cpp,v 1.5 2008-06-30 08:20:16 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "L0DUEmulatorTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUEmulatorTool
//
// 2007-10-10 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUEmulatorTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUEmulatorTool::L0DUEmulatorTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_report(),
    m_muonMap(),
    m_muonCleaning(),
    m_muZeroSup(),
    m_muonMaxIndices(),
    m_muonCleaningPattern()

{
  declareInterface<IL0DUEmulatorTool>(this);

  declareProperty( "MuonCleaning" , m_muonCleaning = false);
  declareProperty( "MuonZeroSup"  , m_muZeroSup    = false);
  
}
//=============================================================================
// Destructor
//=============================================================================
L0DUEmulatorTool::~L0DUEmulatorTool() {} 

//=============================================================================

StatusCode L0DUEmulatorTool::initialize(){
  debug() << "Initialize  L0EmulatorTool" << endreq;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  m_decoder   = tool<IL0ProcessorDataDecoder>("L0ProcessorDataDecoder","L0ProcessorDataDecoder",this);
  m_condDB = tool<IL0CondDBProvider>("L0CondDBProvider");
  return sc;
}


//=============================================================================
StatusCode L0DUEmulatorTool::process(LHCb::L0DUConfig* config,  LHCb::L0ProcessorDatas* datas){
  m_config = config;
  if(!m_decoder -> setL0ProcessorData(datas))return StatusCode::FAILURE;
  return processing();
}

StatusCode L0DUEmulatorTool::process(LHCb::L0DUConfig* config,  std::vector<std::string> dataLocs){
  m_config = config;
  if(!m_decoder -> setL0ProcessorData(dataLocs))return StatusCode::FAILURE;
  return processing();
}


//=============================================================================
StatusCode L0DUEmulatorTool::fillData(){

  m_config->clearDataValue();
  
  LHCb::L0DUElementaryData::Map& dataMap = m_config->data();


  // -------------------------------------
  // Data processing of hardcoded data
  // ------------------------------------
  // Calo
  setDataValue( dataMap["Electron(Et)"] , L0DUBase::Electron::Et        );
  setDataValue( dataMap["Photon(Et)"]   , L0DUBase::Photon::Et          );
  setDataValue( dataMap["Hadron(Et)"]   , L0DUBase::Hadron::Et          );
  setDataValue( dataMap["GlobalPi0(Et)"], L0DUBase::Pi0Global::Et       );
  setDataValue( dataMap["LocalPi0(Et)"] , L0DUBase::Pi0Local::Et        );
  setDataValue( dataMap["Sum(Et)"]      , L0DUBase::Sum::Et             );
  setDataValue( dataMap["Spd(Mult)"]    , L0DUBase::Spd::Mult           );
  setDataValue( dataMap["PUPeak1(Cont)"], L0DUBase::PileUp::Peak1       );
  setDataValue( dataMap["PUPeak2(Cont)"], L0DUBase::PileUp::Peak2       );
  setDataValue( dataMap["PUHits(Mult)"] , L0DUBase::PileUp::Hits        );
  setDataValue( dataMap["Electron(Add)"], L0DUBase::Electron::Address   );
  setDataValue( dataMap["Photon(Add)"]  , L0DUBase::Photon::Address     );
  setDataValue( dataMap["Hadron(Add)"]  , L0DUBase::Hadron::Address     );
  setDataValue( dataMap["GlobalPi0(Add)"],L0DUBase::Pi0Global::Address  );
  setDataValue( dataMap["LocalPi0(Add)"] ,L0DUBase::Pi0Local::Address   );
  // PileUp
  setDataValue(dataMap["PUPeak1(Add)"]  , L0DUBase::PileUp::Peak1Pos );
  setDataValue(dataMap["PUPeak2(Add)"]  , L0DUBase::PileUp::Peak2Pos );




  // The Muon case
  m_muonMap.clear();
  m_muonMap["M0"]     = digit( L0DUBase::Muon1::Pt      ) ;
  m_muonMap["M0(Add)"]= digit( L0DUBase::Muon1::Address)  | (0 << 13)  ;
  m_muonMap["M0(Sgn)"]= digit( L0DUBase::Muon1::Sign    ) ;
  m_muonMap["M1"]     = digit( L0DUBase::Muon2::Pt      ) ;
  m_muonMap["M1(Add)"]= digit( L0DUBase::Muon2::Address)  | (1 << 13) ;
  m_muonMap["M1(Sgn)"]= digit( L0DUBase::Muon2::Sign    ) ;
  m_muonMap["M2"]     = digit( L0DUBase::Muon3::Pt      ) ;
  m_muonMap["M2(Add)"]= digit( L0DUBase::Muon3::Address)  | (2 << 13) ;
  m_muonMap["M2(Sgn)"]= digit( L0DUBase::Muon3::Sign    ) ;
  m_muonMap["M3"]     = digit( L0DUBase::Muon4::Pt      ) ;
  m_muonMap["M3(Add)"]= digit( L0DUBase::Muon4::Address)  | (3 << 13) ;
  m_muonMap["M3(Sgn)"]= digit( L0DUBase::Muon4::Sign    ) ;
  m_muonMap["M4"]     = digit( L0DUBase::Muon5::Pt      ) ;
  m_muonMap["M4(Add)"]= digit( L0DUBase::Muon5::Address)  | (4 << 13) ;
  m_muonMap["M4(Sgn)"]= digit( L0DUBase::Muon5::Sign    ) ; 
  m_muonMap["M5"]     = digit( L0DUBase::Muon6::Pt      ) ;
  m_muonMap["M5(Add)"]= digit( L0DUBase::Muon6::Address)  | (5 << 13) ;
  m_muonMap["M5(Sgn)"]= digit( L0DUBase::Muon6::Sign    ) ;
  m_muonMap["M6"]     = digit( L0DUBase::Muon7::Pt      ) ;
  m_muonMap["M6(Add)"]= digit( L0DUBase::Muon7::Address)  | (6 << 13) ;
  m_muonMap["M6(Sgn)"]= digit( L0DUBase::Muon7::Sign    ) ;
  m_muonMap["M7"]     = digit( L0DUBase::Muon8::Pt      ) ;
  m_muonMap["M7(Add)"]= digit( L0DUBase::Muon8::Address)  | (7 << 13) ;
  m_muonMap["M7(Sgn)"]= digit( L0DUBase::Muon8::Sign    ) ;
  //
  verbose() << "Muon data mapped " << endreq;
  

  // intra-quarter ghost cleaning (if required)
  m_muonCleaningPattern = 0;
  if( cleanMuon(0,1) ) {m_muonMap["M1"]=0; m_muonCleaningPattern |= 1<<1;}
  if( cleanMuon(2,3) ) {m_muonMap["M3"]=0; m_muonCleaningPattern |= 1<<3;}
  if( cleanMuon(4,5) ) {m_muonMap["M5"]=0; m_muonCleaningPattern |= 1<<5;}
  if( cleanMuon(6,7) ) {m_muonMap["M7"]=0; m_muonCleaningPattern |= 1<<7;}

  verbose() << "Muon cleaning done " << endreq;
  
  m_muonMaxIndices.clear();
  // Select the 3 highest Muons
  for(int i=0 ; i !=3 ; i++){
    std::stringstream num("");
    num << (i+1);
    int maxPt = -1;    
    int imax = -1;
    for(int  j=0 ; j !=8 ; j++){
      bool ok=true;
      for(unsigned int k=0 ; k !=m_muonMaxIndices.size() ; k++)if( j== m_muonMaxIndices[k])ok=false;
      if(!ok)continue;
      std::stringstream num("");
      num << j;
      if( maxPt < m_muonMap["M"+num.str()]){
        maxPt= m_muonMap["M"+num.str()];
        imax=j;
      }
    }
    if(-1 == imax){
      Error("Error in muon processing").ignore();
    }else{
      std::stringstream numax("");
      numax << imax;
      m_muonMaxIndices.push_back(imax);
      //
      dataMap["Muon"+num.str()+"(Pt)"]->setOperand(m_muonMap["M"+numax.str()] , scale(L0DUBase::Muon1::Pt) 
                                                   ,max(L0DUBase::Muon1::Pt) );
      dataMap["Muon"+num.str()+"(Add)"]->setOperand(m_muonMap["M"+numax.str()+"(Add)"], scale(L0DUBase::Muon1::Address) 
                                                    ,max(L0DUBase::Muon1::Address));
      dataMap["Muon"+num.str()+"(Sgn)"]->setOperand(m_muonMap["M"+numax.str()+"(Sgn)"], scale(L0DUBase::Muon1::Sign) 
                                                    ,max(L0DUBase::Muon1::Sign) );
    }
  }
  verbose() << "Muon sorted " << endreq;

  const unsigned long dimuon = dataMap["Muon1(Pt)"]->digit()+dataMap["Muon2(Pt)"]->digit();
  dataMap["DiMuon(Pt)"]->setOperand( dimuon , scale( L0DUBase::Muon1::Pt)  , max(L0DUBase::Muon1::Pt)*2+1);

  verbose() << "DiMuon OK " << endreq;


  // -------------------------------------
  // Data processing of user-defined data
  // ------------------------------------  
  for( LHCb::L0DUElementaryData::Map::iterator idata = dataMap.begin();idata != dataMap.end() ; idata++){
    LHCb::L0DUElementaryData* data = (*idata).second;
    if( data->type() != LHCb::L0DUElementaryData::Compound )continue;
    StatusCode sc = dataTree(data, dataMap);
    if(sc.isFailure())return sc;
  }  
       
              
  if( msgLevel(MSG::VERBOSE)) {
    verbose() << "User-defined data filled " << endreq;
    verbose() << " ---> data summary " << endreq;
    for( LHCb::L0DUElementaryData::Map::iterator idata = dataMap.begin();idata != dataMap.end() ; idata++){
      verbose() << "Data :  " << ((*idata).second)->summary() <<endreq;
    }
  }
  
  return StatusCode::SUCCESS;
}

//===========================================================================================================
void L0DUEmulatorTool::setDataValue(LHCb::L0DUElementaryData* l0Data, 
                                    const unsigned int  base[L0DUBase::Index::Size]){
  l0Data->setOperand( digit( base ) , scale(base) , max(base)  );
  verbose() << "Set Data digit " << l0Data->name() << " << " <<digit(base) << " : scale to MeV = " << scale(base) << endreq;
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
      error() << "Cannot combine data with different scales in the compound data "<< *iop << endreq;
      return StatusCode::FAILURE;
    }
    scale = preData->scale();
  }
  return StatusCode::SUCCESS;
}
 
//===========================================================================================================
StatusCode L0DUEmulatorTool::processing(){
  if(fillData().isFailure())return StatusCode::FAILURE;  
  m_config->emulate();
  //
  verbose() << "End of processing ... summary : " << endreq;
  verbose() << m_config->summary() << endreq;
  //
  return StatusCode::SUCCESS;
}
//===========================================================================================================
const LHCb::L0DUReport L0DUEmulatorTool::emulatedReport(){
  m_report.clear();
  m_report.setConfiguration(m_config);
  
  if( !m_config->emulated() ){
    error() << " Report is requested but the emulator has not been processed ... return empty report" << endreq;
    return m_report;
  }
  
  m_report.setDecision( m_config->emulatedDecision()  );
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
    m_report.setConditionValue(condition->name(), condition->emulatedValue() );
  }

  return m_report;
}
  
//==================================================
bool L0DUEmulatorTool::cleanMuon(int i, int j){
  if(!m_muonCleaning)return false;
  std::stringstream is("");
  std::stringstream js("");
  is << i;
  js << j;
  bool ghost =  (m_muonMap["M"+is.str()+"(Pt)"] == m_muonMap["M"+js.str()+"(Pt)"]); // muon ghost definition
  return ghost;
}
 

const unsigned long L0DUEmulatorTool::digit(const unsigned int   base[L0DUBase::Index::Size]){
  return m_decoder->digit(base);
}
const double L0DUEmulatorTool::scale(const unsigned int base[L0DUBase::Index::Size]){
  return m_condDB->scale(base[L0DUBase::Index::Scale]);
}

const long L0DUEmulatorTool::max(const unsigned int base[L0DUBase::Index::Size]){
  long  w1 = base[L0DUBase::Index::Mask] >> base[L0DUBase::Index::Shift];
  long  w2 = base[L0DUBase::Index::Mask2] >> base[L0DUBase::Index::Shift2];
  long  w  = w1 | (w2 << base[L0DUBase::Index::Offset]);
  return  w;
}




//=============================================================================
const std::vector<unsigned int> L0DUEmulatorTool::bank(unsigned int version){

  std::vector<unsigned int> l0Block;

  if( !m_config->emulated() ){
    error() << " rawBank is requested but the emulator has not been processed ... return empty bank" << endreq;
    return l0Block;
  }

  emulatedReport(); // emulate the report
  

  // Version 0 : preliminary version used for DC06 simulated data
  //--------------------------------------------------------------
  if( 0 == version ){

    unsigned int word = ( (m_report.channelsDecisionSummary() << 1) | m_report.decision() ) & 0xFFFF;
    
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
  else  if(1 == version){
    // global header
    unsigned int ec_size = (m_report.conditionsValueSummaries().size() ) & 0x3 ;
    unsigned int tc_size = (m_report.channelsDecisionSummaries().size() ) & 0x3;
    unsigned int tck     = m_report.tck() & 0xFFFF; 
    unsigned int word  =  tc_size  |  ec_size << 2 | tck << 16;
    l0Block.push_back( word );



    // ------------
    // PGA3-Block
    // -----------
    unsigned int nmuons  =0;
    unsigned int muPt[2]={0,0};
    unsigned int muAdd[4]={0,0,0,0};
    for(int i=0 ; i !=8 ; i++){
      std::stringstream num("");
      num << i;
      std::string mu = "M"+num.str();
      // 0-suppression
      if( 0 != m_muonMap[ mu  ] || !m_muZeroSup ){ 
        int ipt = (int) nmuons/4;
        int iad = (int) nmuons/2;
        muPt[ipt]  |= (( m_muonMap[ mu  ]  | (m_muonMap[ mu +"(Sgn)" ] << 7)   )     << (nmuons - ipt*4)*8 );
        muAdd[iad] |= (( m_muonMap[ mu +"(Add)" ]  ) << (nmuons - iad*2)*16);
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
    std::vector<int> muonMax = m_muonMaxIndices;
    std::stringstream mu1("");
    std::stringstream mu2("");
    std::stringstream mu3("");
    mu1 << muonMax[0];
    mu2 << muonMax[1];
    mu3 << muonMax[2];
    
    
    unsigned int dimu =  (int)m_muonMap[ "M"+mu1.str()] +   (int)m_muonMap["M"+mu2.str()] ;
    l0Block.push_back( (int)m_muonMap[ "M"+mu1.str()]       | (int)m_muonMap["M"+mu1.str()+"(Sgn)"] << 7  |
                       (int)m_muonMap[ "M"+mu2.str()] << 8  | (int)m_muonMap["M"+mu2.str()+"(Sgn)"] << 15 |
                       (int)m_muonMap[ "M"+mu3.str()] << 16 | (int)m_muonMap["M"+mu3.str()+"(Sgn)"] << 23 | dimu << 24 );
    
                       

    l0Block.push_back(  (int)m_muonMap[ "M"+mu1.str()+"(Add)"]      |  (int)m_muonMap[ "M"+mu2.str()+"(Add)"] << 16 );
    l0Block.push_back(  (int)m_muonMap[ "M"+mu3.str()+"(Add)"]      |  m_muonCleaningPattern << 16);

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
    unsigned int rsda = l0Id | (m_report.decision() << 12);
    unsigned int nBxp = 0;
    unsigned int nBxm = 0;
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
    

    l0Block.push_back( ( rsda | status << 16 | nBxm << 28 |  nBxp <<30)  ); // header
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
    l0Block.push_back( digit(L0DUBase::Electron::Address ) | 0x2 << 14 |
                       digit(L0DUBase::Photon::Address ) << 16 | 0x2 << 30 );
    l0Block.push_back( digit(L0DUBase::Pi0Global::Address ) | 0x2 << 14 |
                       digit(L0DUBase::Pi0Local::Address ) << 16 | 0x2 << 30 );
    l0Block.push_back( digit(L0DUBase::Hadron::Address ) | 0x3 << 14 |
                       digit( L0DUBase::PileUp::Peak1Pos) << 16 |
                       digit( L0DUBase::PileUp::Peak2Pos) << 24 );
    // put here the summaries for the sequence of consecutive BX if any
    
  }
  else{
    warning() << "L0DU RawBank version " << version << " is not defined " << endreq;
  }
  return l0Block;
}



