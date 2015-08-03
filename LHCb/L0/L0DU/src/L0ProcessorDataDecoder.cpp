// ============================================================================

// Detector Element
// local
#include "L0ProcessorDataDecoder.h"

/** @file 
 *  Implementation file for class : L0ProcessorDataDecoder
 *  
 *  purpose : extract relevant information from L0ProcessorData
 *
 * 2006-02-17 : Olivier Deschamps
 */

DECLARE_TOOL_FACTORY( L0ProcessorDataDecoder )

// ============================================================================
L0ProcessorDataDecoder::L0ProcessorDataDecoder 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaudiTool ( type, name , parent )
  , m_dataContainer(NULL)
  , m_ok(false)
  , m_condDB(NULL)
{
  declareInterface<IL0ProcessorDataDecoder> ( this ) ;  
}
// ============================================================================
StatusCode L0ProcessorDataDecoder::initialize () 
{
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Initialize L0ProcessorDataDecoder" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;
  m_condDB = tool<IL0CondDBProvider>("L0CondDBProvider");
  m_dataContainer = new LHCb::L0ProcessorDatas();
  return StatusCode::SUCCESS;
}


StatusCode L0ProcessorDataDecoder::finalize () 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "release L0ProcessoDataDecoder" << endmsg;
  delete m_dataContainer;
  return GaudiTool::finalize();
}



// ============================================================================
/// destructor
// ============================================================================
L0ProcessorDataDecoder::~L0ProcessorDataDecoder () {} 
// ============================================================================
// ============================================================================
bool L0ProcessorDataDecoder::setL0ProcessorData(std::vector<LHCb::L0ProcessorDatas*> datass){
  m_dataContainer->clear();
  m_ok=true;
  for(std::vector<LHCb::L0ProcessorDatas*>::iterator it=datass.begin();datass.end()!=it;it++){
    LHCb::L0ProcessorDatas* datas = *it;
    if(NULL == datas){
      Warning("Data container points to NULL ",StatusCode::SUCCESS).ignore();
      m_ok = false;
      break;
    }
    for(LHCb::L0ProcessorDatas::iterator itt=datas->begin();datas->end()!=itt;itt++){
      m_dataContainer->insert(*itt);
    }
  }
  return m_ok;
}

// ============================================================================
bool L0ProcessorDataDecoder::setL0ProcessorData(LHCb::L0ProcessorDatas* datas){
  std::vector<LHCb::L0ProcessorDatas*> datass;
  datass.push_back(datas);
  return setL0ProcessorData(datass);
}

bool L0ProcessorDataDecoder::setL0ProcessorData(std::vector<std::string> dataLocs){
  m_dataContainer->clear();
  m_ok=true;
  for(std::vector<std::string>::iterator it=dataLocs.begin();dataLocs.end()!=it;it++){
    const LHCb::L0ProcessorDatas* datas = getIfExists<LHCb::L0ProcessorDatas>( *it ) ;
    if( NULL == datas ){
      Warning("L0ProcessorData container not found at " + *it , StatusCode::SUCCESS).ignore();
      m_ok=false;
      break;
    }
    if( msgLevel(MSG::VERBOSE) ) verbose() << "inserting data from " << *it << endmsg;
    
    for(LHCb::L0ProcessorDatas::const_iterator itt=datas->begin();datas->end()!=itt;itt++){
          m_dataContainer->insert(*itt);
    } 
  } 
  if( msgLevel(MSG::VERBOSE) ) verbose() << "ALL DATA INSERTED" << endmsg;
  return m_ok;
} 

// ============================================================================
bool L0ProcessorDataDecoder::setL0ProcessorData(std::string dataLoc ){
  std::vector<std::string> dataLocs;
  dataLocs.push_back(dataLoc);
  return setL0ProcessorData(dataLocs);
}



// ============================================================================
double L0ProcessorDataDecoder::value( const unsigned int base[L0DUBase::Index::Size]){
  return ((double) digit(base))*m_condDB->scale(base[L0DUBase::Index::Scale]);
}


unsigned long L0ProcessorDataDecoder::digit( const unsigned int base[L0DUBase::Index::Size]){
  
  //  if(!m_ok)return 0;

  LHCb::L0ProcessorData* fiber = m_dataContainer->object( base[ L0DUBase::Index::Fiber ]  )  ;
  if( 0 == fiber ){ 
    Warning("Fiber "+ Gaudi::Utils::toString(base[ L0DUBase::Index::Fiber ]) +" not found ",StatusCode::SUCCESS).ignore();
    m_ok=false;
    return 0;
  }
  
  unsigned long val = ( ( fiber->word()   &  base[L0DUBase::Index::Mask]  ) >> base[L0DUBase::Index::Shift]  ) ; 

  if( L0DUBase::Fiber::Empty != base[ L0DUBase::Index::Fiber2 ]  ) {
    LHCb::L0ProcessorData* fiber2= m_dataContainer->object( base[ L0DUBase::Index::Fiber2 ]  )  ;
    if( 0 == fiber2 ){ 
      Warning("Data ( " + Gaudi::Utils::toString(base) + " ) not found in the container ",StatusCode::SUCCESS).ignore();
      m_ok=false;
      return 0;
    }
    unsigned long val2 =( ( fiber2->word() & base[ L0DUBase::Index::Mask2 ]  )  >> base[ L0DUBase::Index::Shift2 ]) ;
    val |=  (val2  << base[ L0DUBase::Index::Offset ]  ); 
  }
  m_ok=true;
  return val;
}

