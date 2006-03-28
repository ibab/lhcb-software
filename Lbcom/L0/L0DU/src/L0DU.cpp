// $Id: L0DU.cpp,v 1.6 2006-03-28 11:40:15 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Kernel/SystemOfUnits.h"
//DAQEvent
#include "Event/RawEvent.h"
// local
#include "L0DU.h"

// Interface
#include "Event/IL0Candidate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DU
//
// 2004-01-13 : Olivier Deschamps
//
// L0DU Decision 
//
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( L0DU );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DU::L0DU( const std::string& name,
            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
    , m_CaloDataLocation ( LHCb::L0ProcessorDataLocation::Calo )
    , m_MuonDataLocation ( LHCb::L0ProcessorDataLocation::Muon )
    , m_PileUpDataLocation ( LHCb::L0ProcessorDataLocation::PileUp )
    , m_L0DUReportLocation ( LHCb::L0DUReportLocation::Default  ) 
    , m_L0DUChannelLocation (  LHCb::L0DUChannelLocation::Default) 
    , m_data()
    , m_conditions()
    , m_channels()
    , m_ADCconvert(true)
    , m_reconfigure(false)
    , m_reparametrize(false)
    , m_writeOnTES(false)
    , m_fillRaw(true)
{    
  declareProperty( "CaloData"                , m_CaloDataLocation   );
  declareProperty( "MuonData"                , m_MuonDataLocation   );
  declareProperty( "PileUpData"              , m_PileUpDataLocation );
  declareProperty( "OutputData"              , m_L0DUReportLocation );
  declareProperty( "Data"                    , m_data     );
  declareProperty( "Conditions"              , m_conditions );
  declareProperty( "Channels"                , m_channels );
  declareProperty( "ADCconvert"              , m_ADCconvert = true );
  declareProperty( "ResetConfiguration"      , m_reconfigure = false );
  declareProperty( "ResetParametrisation"    , m_reparametrize = false   );
  declareProperty( "WriteOnTES"              , m_writeOnTES = false   );
  declareProperty( "FillRaw"                 , m_fillRaw = true );

  // configure DAQ
  m_rawSrcID = 0 ;// rawBank srcID
  m_rawVsn   = 0 ;// rawBank Vsn
  m_rawBankType  = LHCb::RawBank::L0DU; // rawBank Type
}

//=======================================================================
  // -------------------------------------------------------------------//
  // Default configuration & parametrisation is hardcoded here. //
  // -------------------------------------------------------------------//
  // The whole architecture is possibly (re)configurable through options file 
  // -> options overwrite hard-coded default if m_reconfigure = true 
  // The thresholds and accept rates are (re)parametrisable through options file 
  // -> options overwrite hard-coded default if  m_reparametrize = true

StatusCode L0DU::setDefault(){
  StatusCode sc= StatusCode::SUCCESS;
  // Tuning for DC'04 data (Eduardo Rodrigues, 14/03/2005)
  // L0DU Elementary Conditions
  //                  Condition Name       Data                Comp.  Threshold    
  sc=setDefaultCondition("Total_Et"      , "Sum(Et)"             , ">" , 5.00 * GeV );
  sc=setDefaultCondition("Global_SPD"    , "Spd(Mult)"           , "<" , 280        );
  sc=setDefaultCondition("Global_PU"     , "PUHits(Mult)"        , "<" , 112        );
  sc=setDefaultCondition("Peak2_Sum"     , "PUPeak2(Cont)"       , "<" , 3.00       );
  sc=setDefaultCondition("Electron_High" , "Electron(Et)"        , ">" , 2.60 * GeV );
  sc=setDefaultCondition("Electron_Low"  , "Electron(Et)"        , ">" , 2.60 * GeV );
  sc=setDefaultCondition("Photon_High"   , "Photon(Et)"          , ">" , 2.30 * GeV );
  sc=setDefaultCondition("Photon_Low"    , "Photon(Et)"          , ">" , 2.30 * GeV );
  sc=setDefaultCondition("Hadron_High"   , "Hadron(Et)"          , ">" , 3.50 * GeV );
  sc=setDefaultCondition("Hadron_Low"    , "Hadron(Et)"          , ">" , 3.50 * GeV );
  sc=setDefaultCondition("LocalPi0_High" , "LocalPi0(Et)"        , ">" , 4.30 * GeV );
  sc=setDefaultCondition("LocalPi0_Low"  , "LocalPi0(Et)"        , ">" , 4.30 * GeV );
  sc=setDefaultCondition("GlobalPi0_High", "GlobalPi0(Et)"       , ">" , 3.70 * GeV );
  sc=setDefaultCondition("GlobalPi0_Low" , "GlobalPi0(Et)"       , ">" , 3.70 * GeV );
  sc=setDefaultCondition("Muon_High"     , "Muon1(Pt)"           , ">" , 1.30 * GeV );
  sc=setDefaultCondition("Muon_Low"      , "Muon1(Pt)"           , ">" , 1.30 * GeV );
  sc=setDefaultCondition("DiMuon_High"   , "Muon1(Pt)+Muon2(Pt)" , ">" , 1.50 * GeV );
  sc=setDefaultCondition("DiMuon_Low"    , "Muon1(Pt)+Muon2(Pt)" , ">" , 1.50 * GeV );

  // Define Global conditions  
  std::vector<std::string> sumEtCond;
  sumEtCond.push_back("Total_Et" );
  std::vector<std::string> globalConds = sumEtCond;
  globalConds.push_back("Global_PU");
  globalConds.push_back("Global_SPD");
  globalConds.push_back("Peak2_Sum");

  // Define L0 Channels
  //                  Channel Name          Global        Condition    Accept Rate (%)  
  sc=setDefaultChannel("L0electron_High" , globalConds , "Electron_High" , 100 );
  sc=setDefaultChannel("L0electron_Low"  , globalConds , "Electron_Low"  , 0   );
  sc=setDefaultChannel("L0photon_High"   , globalConds , "Photon_High"   , 100 );
  sc=setDefaultChannel("L0photon_Low"    , globalConds , "Photon_Low"    , 0   );
  sc=setDefaultChannel("L0localPi0_High" , globalConds , "LocalPi0_High" , 100 );
  sc=setDefaultChannel("L0localPi0_Low"  , globalConds , "LocalPi0_Low"  , 0   );
  sc=setDefaultChannel("L0globalPi0_High", globalConds , "GlobalPi0_High", 100 );
  sc=setDefaultChannel("L0globalPi0_Low" , globalConds , "GlobalPi0_Low" , 0   );
  sc=setDefaultChannel("L0hadron_High"   , globalConds , "Hadron_High"   , 100 );
  sc=setDefaultChannel("L0hadron_Low"    , globalConds , "Hadron_Low"    , 0   );
  sc=setDefaultChannel("L0muon_High"     , globalConds , "Muon_High"     , 100 );
  sc=setDefaultChannel("L0muon_Low"      , globalConds , "Muon_Low"      , 0   );
  sc=setDefaultChannel("L0dimuon_High"   , sumEtCond   , "DiMuon_High"   , 100 );
  sc=setDefaultChannel("L0dimuon_Low"    , sumEtCond   , "DiMuon_Low"    , 0   );
  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
L0DU::~L0DU() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0DU::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  //----------------------------
  // Set-up L0DU Elementary Data
  //----------------------------
  dataSetup();

  //------------------------------------------------
  //  Set-up a default configuration/parametrisation 
  //------------------------------------------------
  if( !m_reconfigure )  {
    sc=setDefault();
    if( sc.isFailure() ){ return Error("Could not set-up the default L0DU algorithm",sc);}
  }
  
  //-------------------------------------------------------
  // Set-up user-defined compound data (defined in options)
  //-------------------------------------------------------
  for( std::vector<std::string>::iterator idata = m_data.begin() ; 
       idata != m_data.end() ; idata++){
    L0DUMakerTools*  L0DUElementaryDataMaker 
      = tool<L0DUMakerTools>( "L0DUMakerTools/"+ *idata ,this);
    sc = L0DUElementaryDataMaker -> makeData( *idata , m_dataCont );
    if( sc.isFailure() ){ return Error("Could not initialize L0DU algorithm",sc);}
  }
  
  //------------------------------------------------------------------------------------
  // Set-up user-defined Elementary Conditions (defined in options Elementary Conditions
  //------------------------------------------------------------------------------------
  for(std::vector<std::string>::iterator icond = m_conditions.begin() ; 
      icond != m_conditions.end() ; icond++){
    L0DUMakerTools*  L0DUConditionMaker 
      = tool<L0DUMakerTools>( "L0DUMakerTools/"+ *icond ,this);
    sc = L0DUConditionMaker -> makeCondition(*icond , 
                                             m_dataCont,
                                             m_conditionsCont );
    if( sc.isFailure() ){ return Error("Could not initialize L0DU algorithm",sc);}
    m_conditionsMap[m_conditionsMap.size()]= *icond ;
  }

  //-----------------------------------------------------------
  // Set-up user-defined L0DU Channels (defined in via options) 
  //-----------------------------------------------------------
  for(std::vector<std::string>::iterator ichan = m_channels.begin() ; 
      ichan != m_channels.end() ; ichan++){
    L0DUMakerTools*  L0DUChannelMaker =
      tool<L0DUMakerTools>( "L0DUMakerTools/"+ *ichan ,this);
    sc = L0DUChannelMaker -> makeChannel(*ichan ,
                                         m_conditionsCont,
                                         m_channelsCont );
    if( sc.isFailure() ){ return Error("Could not initialize L0DU algorithm",sc);}
    m_channelsMap[m_channelsMap.size()]=*ichan ;
  }

  //---------------------
  // L0Candidate tool
  //---------------------
  m_CaloCandidate   = tool<IL0Candidate>("L0CandidateTool","L0CaloCandidate",this);
  m_MuonCandidate   = tool<IL0Candidate>("L0CandidateTool","L0MuonCandidate",this);
  m_PileUpCandidate = tool<IL0Candidate>("L0CandidateTool","L0PileUpCandidate",this);


  // SetUp L0Candidate tool instances
  m_CaloCandidate   -> setUp(m_CaloDataLocation     , m_ADCconvert);
  m_MuonCandidate   -> setUp(m_MuonDataLocation     , m_ADCconvert);
  m_PileUpCandidate -> setUp(m_PileUpDataLocation   , m_ADCconvert);

  //---------------------
  // L0DU set-up PrintOut
  //---------------------
  printOut();

  return sc;
};


//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DU::execute() {
  
  //-------------------------------
  // Proccess Data  : (built L0DUElementaryData from L0 candidates)
  dataProcessing() ;

  //--------------------------------
  // Process L0DU channels decision  - Fill L0DuReport w/ decision pattern
  LHCb::L0DUReport* report = new LHCb::L0DUReport( );
  put (report , m_L0DUReportLocation );
  if(m_writeOnTES){
    m_channelsOnTES = new LHCb::L0DUChannels();
    put(  m_channelsOnTES ,   m_L0DUChannelLocation);
  }
      
  for( std::map<int,std::string>::iterator ichan = m_channelsMap.begin() ; 
       ichan != m_channelsMap.end() ; ichan++){
    LHCb::L0DUChannel* channel = m_channelsCont[ (*ichan).second ];
    channel -> process( );
    report -> setChannel(channel->name(), channel->decision() );
    // Print full output for debugging
    debug() << channel->summary() << endreq;

    if(m_writeOnTES){
      LHCb::L0DUChannel* channelOnTES = channel->clone();
      m_channelsOnTES->push_back( channelOnTES );
    }
  }
  
  // fill L0DuReport with ElementaryConditions pattern as well
  for(std::map<int,std::string>::iterator icond = m_conditionsMap.begin() ; 
      icond != m_conditionsMap.end() ; icond++){
    LHCb::L0DUElementaryCondition* condition = m_conditionsCont[ (*icond).second ];
    report -> setCondition(condition->name(), condition->value() );
  }

  debug() << "L0DU report = " << report->report() << endreq;

  if(m_fillRaw){
    //-------------------------------- // 
    // fill rawBank with L0Block
    // to be finalised when L0Block format and content is defined
    std::vector<unsigned int> l0Block;
    l0Block.push_back( report -> report() ); // decision Pattern
    l0Block.push_back( report -> conditionPattern() ); // condition Pattern
    l0Block.push_back( m_PileUpCandidate->digitValue( L0DUBase::PileUp::Peak1 ) |
                       m_PileUpCandidate->digitValue(L0DUBase::PileUp::Peak1Pos ) << 8 ); // PileUp Peak1
    l0Block.push_back( m_PileUpCandidate->digitValue(L0DUBase::PileUp::Peak2 ) |
                       m_PileUpCandidate->digitValue(L0DUBase::PileUp::Peak2Pos ) << 8 ); // PileUp Peak2
    l0Block.push_back( m_PileUpCandidate->digitValue(L0DUBase::PileUp::Hits    ) ); // PileUp #Hits
    l0Block.push_back( m_CaloCandidate->digitValue(L0DUBase::Sum::Et )    |
                       m_CaloCandidate->digitValue(L0DUBase::Spd::Mult ) << 14  ); // SumEt + SpdMult
    LHCb::RawEvent* raw = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    raw->addBank(m_rawSrcID , m_rawBankType , m_rawVsn , l0Block);
  }
  
  return StatusCode::SUCCESS;   
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DU::finalize() {

  debug()<< "==> Finalize" << endreq;

  // Delete
  for(LHCb::L0DUElementaryData::Map::iterator idata = m_dataCont.begin();
      idata != m_dataCont.end() ; idata++){
    delete (*idata).second;
  }
  for(LHCb::L0DUElementaryCondition::Map::iterator icond = m_conditionsCont.begin();
      icond != m_conditionsCont.end() ; icond++){
    delete (*icond).second;
  }
  for(LHCb::L0DUChannel::Map::iterator ichan = m_channelsCont.begin();
      ichan != m_channelsCont.end() ; ichan++){
    delete (*ichan).second;
  }
  
  m_dataCont.clear();
  m_channelsCont.clear();
  m_conditionsCont.clear();

  return GaudiAlgorithm::finalize();
}


//=============================================================================
StatusCode L0DU::dataProcessing()
{
  /*
    Decode L0Processors words + Processing (3 highest muon + DiMuon ...)
  */

  //intra-quarter ghost cleaning for muon ?
  bool cleanGhost = false;

  /*
    Pre-processing : Data Mapping
  */


  // -------------------
  // Data processing 
  // -------------------
  m_dataCont["Electron(Et)"] ->setOperand( m_CaloCandidate->value( L0DUBase::Electron::Et   ) );
  m_dataCont["Photon(Et)"]   ->setOperand( m_CaloCandidate->value( L0DUBase::Photon::Et     ) );
  m_dataCont["Hadron(Et)"]   ->setOperand( m_CaloCandidate->value( L0DUBase::Hadron::Et     ) );
  m_dataCont["GlobalPi0(Et)"]->setOperand( m_CaloCandidate->value( L0DUBase::Pi0Global::Et  ) );
  m_dataCont["LocalPi0(Et)"] ->setOperand( m_CaloCandidate->value( L0DUBase::Pi0Local::Et   ) );
  m_dataCont["Sum(Et)"]      ->setOperand( m_CaloCandidate->value( L0DUBase::Sum::Et        ) );
  m_dataCont["Spd(Mult)"]    ->setOperand( m_CaloCandidate->value( L0DUBase::Spd::Mult      ) );
  m_dataCont["PUPeak1(Cont)"]->setOperand( m_PileUpCandidate->value(L0DUBase::PileUp::Peak1 ) );
  m_dataCont["PUPeak2(Cont)"]->setOperand( m_PileUpCandidate->value(L0DUBase::PileUp::Peak2 ) );
  m_dataCont["PUHits(Mult)"] ->setOperand( m_PileUpCandidate->value(L0DUBase::PileUp::Hits  ) );

  m_dataCont["Electron(Add)"] ->setOperand( m_CaloCandidate->value( L0DUBase::Electron::Address  ) );
  m_dataCont["Photon(Add)"]   ->setOperand( m_CaloCandidate->value( L0DUBase::Photon::Address    ) );
  m_dataCont["Hadron(Add)"]   ->setOperand( m_CaloCandidate->value( L0DUBase::Hadron::Address    ) );
  m_dataCont["GlobalPi0(Add)"]->setOperand( m_CaloCandidate->value( L0DUBase::Pi0Global::Address ) );
  m_dataCont["LocalPi0(Add)"] ->setOperand( m_CaloCandidate->value( L0DUBase::Pi0Local::Address  ) );
  m_dataCont["PUPeak1(Add)"]  ->setOperand( m_PileUpCandidate->value( L0DUBase::PileUp::Peak1Pos ) );
  m_dataCont["PUPeak2(Add)"]  ->setOperand( m_PileUpCandidate->value( L0DUBase::PileUp::Peak2Pos ) );

  // The Muon case
  std::map<std::string,double> muonMap;

  muonMap["M0"]     = m_MuonCandidate->value( L0DUBase::Muon1::Pt      ) ;
  muonMap["M0(Add)"]= m_MuonCandidate->value( L0DUBase::Muon1::Address ) ;
  muonMap["M0(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon1::Sign    ) ;
  muonMap["M1"]     = m_MuonCandidate->value( L0DUBase::Muon2::Pt      ) ;
  muonMap["M1(Add)"]= m_MuonCandidate->value( L0DUBase::Muon2::Address ) ;
  muonMap["M1(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon2::Sign    ) ;
  muonMap["M2"]     = m_MuonCandidate->value( L0DUBase::Muon3::Pt      ) ;
  muonMap["M2(Add)"]= m_MuonCandidate->value( L0DUBase::Muon3::Address ) ;
  muonMap["M2(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon3::Sign    ) ;
  muonMap["M3"]     = m_MuonCandidate->value( L0DUBase::Muon4::Pt      ) ;
  muonMap["M3(Add)"]= m_MuonCandidate->value( L0DUBase::Muon4::Address ) ;
  muonMap["M3(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon4::Sign    ) ;
  muonMap["M4"]     = m_MuonCandidate->value( L0DUBase::Muon5::Pt      ) ;
  muonMap["M4(Add)"]= m_MuonCandidate->value( L0DUBase::Muon5::Address ) ;
  muonMap["M4(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon5::Sign    ) ;
  muonMap["M5"]     = m_MuonCandidate->value( L0DUBase::Muon6::Pt      ) ;
  muonMap["M5(Add)"]= m_MuonCandidate->value( L0DUBase::Muon6::Address ) ;
  muonMap["M5(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon6::Sign    ) ;
  muonMap["M6"]     = m_MuonCandidate->value( L0DUBase::Muon7::Pt      ) ;
  muonMap["M6(Add)"]= m_MuonCandidate->value( L0DUBase::Muon7::Address ) ;
  muonMap["M6(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon7::Sign    ) ;
  muonMap["M7"]     = m_MuonCandidate->value( L0DUBase::Muon8::Pt      ) ;
  muonMap["M7(Add)"]= m_MuonCandidate->value( L0DUBase::Muon8::Address ) ;
  muonMap["M7(Sgn)"]= m_MuonCandidate->value( L0DUBase::Muon8::Sign    ) ;

  // intra-quarter ghost cleaning (if required)
  if( cleanGhost && muonMap["M0(Add)"] == muonMap["M1(Add)"] ) muonMap["M1"]=0;
  if( cleanGhost && muonMap["M2(Add)"] == muonMap["M3(Add)"] ) muonMap["M2"]=0;
  if( cleanGhost && muonMap["M4(Add)"] == muonMap["M5(Add)"] ) muonMap["M5"]=0;
  if( cleanGhost && muonMap["M6(Add)"] == muonMap["M7(Add)"] ) muonMap["M7"]=0; 

  
  // Select the 3 highest Muons
  for(int i=0 ; i !=3 ; i++){
    std::stringstream num("");
    num << (i+1);
    double maxPt = -99999;
    int imax = 0;
    for(int  j=0 ; j !=8 ; j++){
      std::stringstream num("");
      num << j;
      if( maxPt < muonMap["M"+num.str()]){
        maxPt= muonMap["M"+num.str()];
        imax=j;
      }
    }
    std::stringstream numax("");
    numax << imax;
    m_dataCont["Muon"+num.str()+"(Pt)"]->setOperand(muonMap["M"+numax.str()]);
    m_dataCont["Muon"+num.str()+"(Add)"]->setOperand(muonMap["M"+numax.str()+"(Add)"]);
    m_dataCont["Muon"+num.str()+"(Sgn)"]->setOperand(muonMap["M"+numax.str()+"(Sgn)"]);
    muonMap["M"+numax.str()]=-9999999;
  }
  double dimuon = m_dataCont["Muon1(Pt)"]->value()+m_dataCont["Muon2(Pt)"]->value();
  m_dataCont["Muon1(Pt)+Muon2(Pt)"]->setOperand( dimuon );
  muonMap.clear();

  /*
    Assign operand(s) value for compound L0DU Data 
  */
                                             
  for( std::vector<std::string>::iterator idata = m_data.begin() ; 
       idata != m_data.end() ; idata++){
    LHCb::L0DUElementaryData* data = m_dataCont[ *idata ];

    data->operands().clear();    // reset
    std::vector<std::string> operandsName = data->operandsName();
    for(std::vector<std::string>::iterator iop = operandsName.begin() ; 
        iop != operandsName.end() ; iop++){
      // Check the operand exists and is corectly mapped
      LHCb::L0DUElementaryData::Map::iterator icheck = m_dataCont.find( *iop );
      if( icheck == m_dataCont.end() ){
        fatal() << " The requested operand '" <<  *iop 
                << "' has not been defined. " << endreq;
        return StatusCode::FAILURE;
      }
      data->addOperand( m_dataCont[ *iop ]->value() );
    }
  }   
  return StatusCode::SUCCESS; 
}



//=============================================================================
void L0DU::dataSetup()
{

  // Fill L0DUElementaryData container with base data (fields of L0PU words) 

  // 1- content fields (Et/Pt/Multiplicity) ...
  predefinedData("Electron(Et)"   );
  predefinedData("Photon(Et)"     );
  predefinedData("Hadron(Et)"     );
  predefinedData("LocalPi0(Et)"   );
  predefinedData("GlobalPi0(Et)"  );
  predefinedData("Sum(Et)"        );
  predefinedData("Spd(Mult)"          );
  predefinedData("Muon1(Pt)"          );
  predefinedData("Muon2(Pt)"          );
  predefinedData("Muon3(Pt)"          );
  predefinedData("Muon1(Pt)+Muon2(Pt)");
  predefinedData("PUHits(Mult)"       );
  predefinedData("PUPeak1(Cont)"      );
  predefinedData("PUPeak2(Cont)"      );

  // 2- addresses Fields
  predefinedData("Electron(Add)"      );
  predefinedData("Photon(Add)"        );
  predefinedData("Hadron(Add)"        );
  predefinedData("LocalPi0(Add)"      );
  predefinedData("GlobalPi0(Add)"     );
  predefinedData("Muon1(Add)"         );
  predefinedData("Muon2(Add)"         );
  predefinedData("Muon3(Add)"         );
  predefinedData("PUPeak1(Add)"       );
  predefinedData("PUPeak2(Add)"       );
  // 3 - additional elementary data field (muon sign ... status or more info if required...)
  predefinedData("Muon1(Sgn)"         );
  predefinedData("Muon2(Sgn)"         );
  predefinedData("Muon3(Sgn)"         );  
  
}


//=============================================================================
void L0DU::predefinedData(std::string name)
{
  LHCb::L0DUElementaryData* data = new LHCb::L0DUElementaryData();
  data -> init(name,"Id",name);
  m_dataCont[name]=data ;
}

//=============================================================================
StatusCode L0DU::setDefaultCondition(std::string name , 
                               std::string data , 
                               std::string comparator , 
                               double threshold)
{
  L0DUMakerTools*  condition  = tool<L0DUMakerTools>( "L0DUMakerTools/"+name,this);

  StringProperty eCp1("Data"        , data       ) ;
  StringProperty eCp2("Comparator"  , comparator );
  DoubleProperty eCp3("Threshold"   , threshold  );
  StatusCode sc ;
  sc = condition -> setProperty(eCp1);
  sc = condition -> setProperty(eCp2);
  if( !m_reparametrize) sc = condition -> setProperty(eCp3);
  sc = condition -> makeCondition(name,m_dataCont,m_conditionsCont);  
  m_conditionsMap[m_conditionsMap.size()]=name;
  return sc;
}
//=============================================================================
StatusCode L0DU::setDefaultChannel(std::string name , std::vector<std::string> conditions ,std::string addCondition,
                             unsigned int acceptRate)
{
  L0DUMakerTools*   channel = tool<L0DUMakerTools>( "L0DUMakerTools/"+name,this);

  conditions.push_back(addCondition);    

  StringArrayProperty c1("Conditions", conditions);
  UnsignedIntegerProperty c2("AcceptRate",acceptRate);
  StatusCode sc ;
  sc =  channel -> setProperty(c1);
  if( !m_reparametrize) sc =  channel -> setProperty(c2);
  sc =  channel -> makeChannel(name , m_conditionsCont , m_channelsCont);
  m_channelsMap[m_channelsMap.size()]=name;
  return sc;
}

//=============================================================================
void L0DU::printOut()
{  
  //--------------------
  // Print Out
  info() << "=================================================="<< endreq;   
  info() << "           L0DU algorithm set-up "<< endreq;   
  info() << "=================================================="<< endreq;   
  info() << "  "<< endreq;
  if( m_reconfigure ){
    info() << " Configuration & parametrisation are set via options file.  "<< endreq;     
  }else {
    info() << " Hardcoded configuration is used. "<< endreq;     
    if( m_reparametrize ){ 
      info() << " parametrisation - thresholds and rates - are set through options file. "<< endreq;     
    }
  }
  if( m_ADCconvert ) info() << " Transverse energy and momenta are converted into MeV"<< endreq;

  info() << "  "<< endreq;
  info() << " - Algorithm is based on  " << m_channelsCont.size() << " Channels :"<< endreq;
  info() << "  "<< endreq;
  for(LHCb::L0DUChannel::Map::iterator iL0 = m_channelsCont.begin() ; iL0 != m_channelsCont.end() ; iL0++){ 
    LHCb::L0DUChannel* channel = (*iL0).second;
    info() << "    - "  << channel->name() 
        <<"  - accept rate = " << channel->acceptRate() << " / " << LHCb::L0DUCounter::Scale << endreq;
    debug() << "     " 
           <<"  - # of conditions = " << channel->elementaryConditions().size()<< " : " << endreq;
    LHCb::L0DUElementaryCondition::Map conditions =  channel->elementaryConditions();
    for (LHCb::L0DUElementaryCondition::Map::iterator 
           icond = conditions.begin();icond != conditions.end() ; icond++){
      LHCb::L0DUElementaryCondition* condition = (*icond).second;     
      debug() 
          << "           + "   
          << condition->data()->name() << " " << condition->comparator() << " " <<  condition->threshold() 
          << endreq;       
    }
  }
  info() << "  "<< endreq;
  info() << "=================================================="<< endreq;   
}
