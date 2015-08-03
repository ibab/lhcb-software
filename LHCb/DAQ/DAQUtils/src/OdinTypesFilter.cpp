// Include files 

// from LHCb
#include "Event/ODIN.h"
// local
#include "OdinTypesFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OdinTypesFilter
//
// 2008-02-05 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OdinTypesFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OdinTypesFilter::OdinTypesFilter( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    ,m_all(0)
    ,m_acc(0)
{

  declareProperty("BXTypes"      , m_bxs);
  declareProperty("TriggerTypes" , m_trs);
  declareProperty("ReadoutTypes" , m_ros);
  declareProperty("TAEWindowLessThan" , m_winmax=99);
  declareProperty("TAEWindowMoreThan" , m_winmin=-1);
  declareProperty("CalibrationTypes", m_cls);
  declareProperty("Logical"      , m_log="AND"); 

  m_bxs.push_back("ALL");
  m_trs.push_back("ALL");
  m_ros.push_back("ALL");
  m_cls.push_back("ALL");
}
//=============================================================================
// Destructor
//=============================================================================
OdinTypesFilter::~OdinTypesFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OdinTypesFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // check properties
  if(m_log != "AND" && m_log != "OR"){
    return Error( "The Logical operator should be 'AND' or 'OR' " );
  }




  for(std::vector<std::string>::iterator icl = m_cls.begin(); icl != m_cls.end() ; ++icl){ 
    if(*icl == "ALL")continue;
    int k = 0;
    std::string  cl;
    bool ok = false;
    int max = LHCb::ODIN::CalibrationTypeMask >> LHCb::ODIN::CalibrationTypeBits;
    while(k <= max && !ok){
      std::stringstream s("");
      s << (LHCb::ODIN::CalibrationTypes) k;
      cl = s.str();
      if( cl == *icl)ok=true;
      k++;
    }
    if( !ok ){
      error() << "The requested CalibrationType '" << *icl << "' is not a valid type" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  

  for(std::vector<std::string>::iterator ibx = m_bxs.begin(); ibx != m_bxs.end() ; ++ibx){ 
    if(*ibx == "ALL")continue;
    int k = 0;
    std::string  bx;
    bool ok = false;
    int max = LHCb::ODIN::BXTypeMask >> LHCb::ODIN::BXTypeBits;
    while(k <= max && !ok){
      std::stringstream s("");
      s << (LHCb::ODIN::BXTypes) k;
      bx = s.str();
      if( bx == *ibx)ok=true;
      k++;
    }
    if( !ok ){
      error() << "The requested BXType '" << *ibx << "' is not a valid type" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  for(std::vector<std::string>::iterator itr = m_trs.begin(); itr != m_trs.end() ; ++itr){
    if(*itr == "ALL")continue;
    int k = 0;
    std::string  tr;
    bool ok = false;
    int max = LHCb::ODIN::TriggerTypeMask >> LHCb::ODIN::TriggerTypeBits;
    while(k <= max && !ok){
      std::stringstream s("");
      s << (LHCb::ODIN::TriggerType) k;
      tr = s.str();
      if( tr == *itr)ok=true;
      k++;
    }
    if( !ok ){
      error() << "The requested TriggerType '" << *itr << "' is not a valid type" << endmsg;
      return StatusCode::FAILURE;
    }
  }


  for(std::vector<std::string>::iterator iro = m_ros.begin(); iro != m_ros.end() ; ++iro){
    if(*iro == "ALL")continue;
    int k = 0;
    std::string  ro;
    bool ok = false;
    int max = LHCb::ODIN::ReadoutTypeMask >> LHCb::ODIN::ReadoutTypeBits;
    while(k <= max && !ok){
      std::stringstream s("");
      s << (LHCb::ODIN::ReadoutTypes) k;
      ro = s.str();
      if( ro == *iro)ok=true;
      k++;
    }
    if( !ok ){
      error() << "The requested ReadoutType '" << *iro << "' is not a valid type" << endmsg;
      return StatusCode::FAILURE;
    }
  }  

  // selection :
  info() << "Accepted BXTypes : " << m_bxs << endmsg;
  info() << m_log << endmsg;
  info() << "Accepted TriggerTypes : " << m_trs << endmsg;
  info() << m_log << endmsg;
  info() << "Accepted ReadoutTypes : " << m_ros << endmsg;
  info() << m_log << endmsg;
  info() << "Accepted CalibrationTypes : " << m_cls << endmsg;
  info() << m_log << endmsg;
  info() << "TAE Window in [" << m_winmin+1 << "," << m_winmax-1 <<"]"<< endmsg;



  // warns trivial requests 
  if((m_bxs.empty() || m_trs.empty()  || m_ros.empty() || m_winmin>=m_winmax  || m_cls.empty()) && m_log == "AND")
    Warning("BXTypes, TriggerTypes, ReadoutTypes or TAEWindow is empty : ALL events will be rejected !!"
            ,StatusCode::SUCCESS).ignore();
  if((m_bxs.empty() && m_trs.empty()) && m_ros.empty() && m_winmin>=m_winmax && m_cls.empty() && m_log == "OR")
    Warning("BXTypes, TriggerTypes, ReadoutTypes and TAEWindow are empties : ALL events will be rejected !!"
            ,StatusCode::SUCCESS).ignore();
  if( *(m_bxs.begin()) == "ALL" 
      && *(m_cls.begin()) == "ALL" 
      && *(m_trs.begin()) == "ALL" 
      && *(m_ros.begin()) == "ALL" 
      && m_winmin<0 && m_winmax>7
      && m_log == "AND")
    Warning("OdinTypesFilter has no effect : ALL events will be accepted !!"
            ,StatusCode::SUCCESS).ignore();
  if(( *(m_bxs.begin()) == "ALL"       ||*(m_cls.begin()) == "ALL"       
       || *(m_trs.begin()) == "ALL"  || *(m_ros.begin()) == "ALL" || (m_winmin<0&&m_winmax>7)) && m_log == "OR")
    Warning("OdinTypesFilter has no effect : ALL events will be accepted !!"
            ,StatusCode::SUCCESS).ignore();

  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OdinTypesFilter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_all++;



  // treat trivial requests
  setFilterPassed(true);
  if( *(m_bxs.begin()) == "ALL" &&*(m_cls.begin()) == "ALL" && *(m_trs.begin()) == "ALL" && *(m_ros.begin()) == "ALL" 
      &&  (m_winmin<0 && m_winmax>7) && m_log == "AND"){
    m_acc++;
    return StatusCode::SUCCESS;
  }
  
  if(( *(m_bxs.begin()) == "ALL" ||*(m_cls.begin()) == "ALL" || *(m_trs.begin()) == "ALL" || *(m_ros.begin()) == "ALL" 
       || (m_winmin<0 && m_winmax>7)) && m_log == "OR"){
    m_acc++;
    return StatusCode::SUCCESS;
  }
  
  setFilterPassed(false);
  if( (m_bxs.empty() || m_cls.empty() || m_trs.empty() || m_ros.empty() || m_winmin>=m_winmax) && m_log =="AND")return StatusCode::SUCCESS;
  if( (m_bxs.empty() && m_cls.empty() && m_trs.empty() && m_ros.empty() && m_winmin>=m_winmax) && m_log =="OR")return StatusCode::SUCCESS;

  

  // get ODIN
  LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if( odin == NULL )
    return Error("ODIN cannot be loaded - FilterPassed = false",StatusCode::SUCCESS);
  
  std::stringstream clType("");
  std::stringstream bxType("");
  std::stringstream trType("");
  std::stringstream roType("");
  clType << (LHCb::ODIN::CalibrationTypes) odin->calibrationType();
  bxType << (LHCb::ODIN::BXTypes) odin->bunchCrossingType();
  trType << (LHCb::ODIN::TriggerType) odin->triggerType();
  roType << (LHCb::ODIN::ReadoutTypes) odin->readoutType();


  if ( msgLevel(MSG::DEBUG) ) 
    debug() << " Trigger Type : " << trType.str() << " BXType : " << bxType.str() << endmsg;

  bool clPass =  false;
  for(std::vector<std::string>::iterator icl = m_cls.begin(); icl != m_cls.end() ; ++icl){
    if( clType.str() == *icl || "ALL" == *icl ){
      clPass = true;
      break;
    }    
  } 

  bool bxPass =  false;
  for(std::vector<std::string>::iterator ibx = m_bxs.begin(); ibx != m_bxs.end() ; ++ibx){
    if( bxType.str() == *ibx || "ALL" == *ibx ){
      bxPass = true;
      break;
    }    
  } 
  
  bool trPass = false;
  for(std::vector<std::string>::iterator itr = m_trs.begin(); itr != m_trs.end() ; ++itr){
    if( trType.str() == *itr || "ALL" == *itr ){
      trPass = true;
      break;
    }    
  }

  bool roPass = false;
  for(std::vector<std::string>::iterator iro = m_ros.begin(); iro != m_ros.end() ; ++iro){
    if( roType.str() == *iro || "ALL" == *iro ){
      roPass = true;
      break;
    }    
  }

  bool taePass = false;
  if((int) odin->timeAlignmentEventWindow()>m_winmin && (int) odin->timeAlignmentEventWindow()<m_winmax)taePass=true;
  
  if(m_log == "AND")setFilterPassed( trPass && bxPass && roPass && taePass && clPass);
  if(m_log == "OR")setFilterPassed( trPass || bxPass || roPass || taePass || clPass);

  if(filterPassed() )m_acc++;

  counter("ODIN Filter ("+m_log+")")+=int(filterPassed());
  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OdinTypesFilter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;


  info() << "Accepted BXTypes : " << m_bxs << endmsg;
  info() << m_log << endmsg;
  info() << "Accepted TriggerTypes : " << m_trs << endmsg;
  info() << m_log << endmsg;
  info() << "Accepted CalibrationTypes : " << m_cls << endmsg;
  info() << m_log << endmsg;
  info() << "Accepted ReadoutTypes : " << m_ros << endmsg;
  info() << m_log << endmsg;
  info() << "TAE Window in [" << m_winmin << "," << m_winmax <<"]"<< endmsg;
  info() << "   ---> " << m_acc << " accepted events among " << m_all << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
