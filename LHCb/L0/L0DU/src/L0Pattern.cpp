// Include files 

// L0Event
#include "Event/L0DUReport.h"
// local
#include "L0Pattern.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0Pattern
//
// 2008-03-26 : Olivier Deschamps
//
// Produce L0 pattern to feed L0DU board test-benches
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Pattern )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Pattern::L0Pattern( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ) 
    , m_config(0)
    , m_emulator(0)
    , m_fromRaw(0)
    , m_datas(0)
    , m_bcid(0)
{
  declareProperty( "L0DUEmulatorTool"  , m_emulatorTool= "L0DUEmulatorTool");
  declareProperty( "L0DUFromRawTool"   , m_fromRawTool = "L0DUFromRawTool" );  
  declareProperty( "L0DUConfigTool"    , m_configTool  = "L0PatternConfig");
  declareProperty( "TCKList"           , m_list);
  declareProperty( "resetBCID"         , m_setbcid = true);
  
} 

//=============================================================================
// Destructor
//=============================================================================
L0Pattern::~L0Pattern() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0Pattern::initialize() {
  
  StatusCode sc = GaudiTupleAlg::initialize() ;
  if( sc.isFailure() ) 
  { return Error("Could not initialize base class CaudiAlgorithm",sc);}
  info() << "==> Initialize" << endmsg;
  
  // get the tools
  m_fromRaw   = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool , this );
  m_emulator  = tool<IL0DUEmulatorTool>(m_emulatorTool, m_emulatorTool);
  m_config    = tool<IL0DUConfigProvider>("L0DUMultiConfigProvider" , m_configTool);
  


  // Check
  if(m_list.empty())return Error("Empty list of TCK");
  for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
    for(std::vector<std::string>::iterator jt = it+1  ; jt != m_list.end() ; jt++){
      if( (*it) == (*jt) ){
        warning() << "The requested TCK " << *it << " appears twice in the TCKList " << endmsg;
        warning() << "Emulator may give wrong result due to  downscaling - please clean the list ..." << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Pattern::execute() {

  using namespace LHCb;


  Tuple ntp = nTuple(500 , "L0DU pattern" ,CLID_ColumnWiseTuple);
  
  
  // get processor data
  if(!m_fromRaw->decodeBank()){
    Error("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();
    return StatusCode::SUCCESS;
  } 
  if(m_fromRaw->version() == 0){
    Error("L0Pattern production only works with bank version > 0").ignore();
    return StatusCode::SUCCESS;
  }
  

  m_datas =  m_fromRaw->L0ProcessorDatas();
  

  
  // built 12 bits BCID 
  unsigned long sature = 0xDEB;
  if( m_bcid > sature )m_bcid=0 ;
  // set bcid to the L0DU data words
  if(m_setbcid){
    encode(m_bcid , L0DUBase::Electron::BCID     );
    encode(m_bcid , L0DUBase::Photon::BCID       );
    encode(m_bcid , L0DUBase::Hadron::BCID       );
    encode(m_bcid , L0DUBase::Pi0Global::BCID    );
    encode(m_bcid , L0DUBase::Pi0Local::BCID     );
    encode(m_bcid , L0DUBase::Sum::BCID          );
    encode(m_bcid , L0DUBase::Spd::BCID          );
    encode(m_bcid , L0DUBase::Muon1::BCID1       );
    encode(m_bcid , L0DUBase::Muon1::BCID2       );
    encode(m_bcid , L0DUBase::Muon3::BCID1       );
    encode(m_bcid , L0DUBase::Muon3::BCID2       );
    encode(m_bcid , L0DUBase::Muon5::BCID1       );
    encode(m_bcid , L0DUBase::Muon5::BCID2       );
    encode(m_bcid , L0DUBase::Muon7::BCID1       );
    encode(m_bcid , L0DUBase::Muon7::BCID2       );
    encode(m_bcid , L0DUBase::PileUp::BCID1      );
    encode(m_bcid , L0DUBase::PileUp::BCID2      );
  }
  


  // RSDA vs TCK
  std::vector<int> rsda,tcks,report;
  // Run emulator over various configuration
  for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
    if( "0x" != (*it).substr( 0, 2 ) ){
      Error("SKIP the requested TCK value " + *it + " (MUST be in hexadecimal format '0x" + *it + "')").ignore();
      continue;
    }
    int itck;
    std::istringstream is( (*it).c_str() );
    is >> std::hex >> itck;
    LHCb::L0DUConfig* config   = m_config->config( itck );
    if(NULL == config)return Error("Unknown TCK",StatusCode::SUCCESS);
    m_emulator->process(config , m_datas); 

    unsigned int rs = 0;
    if( m_fromRaw->version() == 1 ){
      rs = m_bcid & 0xFFF;
      rs |= ( config->emulatedDecision(LHCb::L0DUDecision::Physics) ) << 12;
    }
    else if( m_fromRaw->version() == 2 ){
      rs = (m_bcid & 0x3FF);
      rs |= ( config->emulatedDecision(LHCb::L0DUDecision::Physics) ) << 12;
      rs |= ( config->emulatedDecision(LHCb::L0DUDecision::Beam2)   ) << 11;
      rs |= ( config->emulatedDecision(LHCb::L0DUDecision::Beam1)   ) << 10;
    }

    rsda.push_back( rs );
    tcks.push_back( itck );
    // channel pattern
    long pattern = 0;
    LHCb::L0DUChannel::Map channels = config->channels();    
    for(LHCb::L0DUChannel::Map::iterator ic = channels.begin() ; ic != channels.end() ; ++ic ){
      LHCb::L0DUChannel* channel = (*ic).second;
      pattern |= channel->emulatedDecision()  << channel->id();
    }
    report.push_back(pattern);
  }
  StatusCode sc;
  sc=ntp->farray("rsda"    , rsda   ,"nConfig", m_list.size() );
  sc=ntp->farray("tck"     , tcks   ,"nConfig", m_list.size() );
  sc=ntp->farray("report"  , report ,"nConfig", m_list.size() );
  
  // DATA
  std::vector<int> word,lsb,msb,typ;
  for(L0ProcessorDatas::const_iterator idata = m_datas->begin(); idata != m_datas->end(); idata++ ) {
    word.push_back ( (*idata)->word() ) ;
    lsb.push_back( (*idata)->lsb() ) ;
    msb.push_back( (*idata)->msb() ) ;
    typ.push_back( (*idata)->key() ) ;
  }
  sc=ntp->farray("word" , word ,"nData", 24 );
  sc=ntp->farray("lsb"  , lsb  ,"nData", 24 );
  sc=ntp->farray("msb"  , msb  ,"nData", 24 );
  sc=ntp->farray("fiber", typ  ,"nData", 24 );
  sc=ntp->column("bcid" , m_bcid      );

  m_bcid++;
  sc = ntp->write();
  return sc ;
}

void L0Pattern::encode(unsigned int data ,  const unsigned int base[L0DUBase::Index::Size]){
  LHCb::L0ProcessorData* fiber = m_datas->object( base[ L0DUBase::Index::Fiber ]  )  ;
  unsigned int word = fiber->word();  
  word |= ( (data << base[L0DUBase::Index::Shift]) & base[L0DUBase::Index::Mask] );
  fiber->setWord( word);
  if( L0DUBase::Fiber::Empty != base[ L0DUBase::Index::Fiber2 ]  ) {
    fiber = m_datas->object( base[ L0DUBase::Index::Fiber2 ]  )  ;
    word = fiber->word();
    unsigned int val = data >> base[L0DUBase::Index::Offset];
    word |= ( ( val << base[L0DUBase::Index::Shift2]) & base[L0DUBase::Index::Mask2] );
    fiber->setWord( word);
  }
}


