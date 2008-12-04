// $Id: CaloReadoutTool.cpp,v 1.31 2008-12-04 13:01:48 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "CaloReadoutTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloReadoutTool
//
// base class for Calo readout tools 
// (CaloEnergyFromRaw, CaloTriggerAdcsFromRaw and CaloTriggerBitsFromRaw)
//
// 2007-02-01 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloReadoutTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloReadoutTool::CaloReadoutTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_first(true)
{
  declareInterface<ICaloReadoutTool>(this);

  declareProperty( "DetectorName"   , m_detectorName );
  declareProperty( "PackedIsDefault", m_packedIsDefault = false);
  declareProperty( "DetectorSpecificHeader", m_extraHeader = false);
  declareProperty( "CleanWhenCorruption", m_cleanCorrupted = false);
  m_getRaw = true;
}
//=============================================================================
// Destructor
//=============================================================================
CaloReadoutTool::~CaloReadoutTool() {}

//=========================================================================
//  Get required CaloBanks (short or packed format) - Fill m_banks
//=========================================================================
bool CaloReadoutTool::getCaloBanksFromRaw( ) {

  
  m_readSources.clear();

  m_banks = NULL;
  LHCb::RawEvent* rawEvt = NULL ;
  m_raw = LHCb::RawEventLocation::Default;
  if ( msgLevel( MSG::DEBUG) )debug() << "raw location :: " << rootInTES() + m_raw << endmsg;  
  if( exist<LHCb::RawEvent>( m_raw ) ){
    rawEvt= get<LHCb::RawEvent>( m_raw );
  }else  {
    if(m_first)info()<<"WARNING : rawEvent not found at location  (message will be suppressed)'" << rootInTES() << m_raw <<endreq;
    m_first=false;
    return false;
  }
      
  m_packed =false;
  if( !m_packedIsDefault){
    if ( msgLevel( MSG::DEBUG) )debug() << "Banks of short type are requested as default" << endreq;
    m_banks= &rawEvt->banks(  m_shortType );
    m_status = LHCb::RawBankReadoutStatus( m_shortType);
  }else{
    if ( msgLevel( MSG::DEBUG) )debug() << "Banks of paked type are requested as default" << endreq;
    m_banks= &rawEvt->banks(  m_packedType );
    m_status = LHCb::RawBankReadoutStatus( m_packedType);
  }
  
  
  if ( 0 == m_banks || 0 == m_banks->size() ) {
    if( !m_packedIsDefault){      
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of short type has not been found ... try packed type" << endreq;
      m_banks = &rawEvt->banks( m_packedType );
      m_status = LHCb::RawBankReadoutStatus( m_packedType);
    }else{
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of packed type has not been found ... try short type" << endreq;
      m_banks = &rawEvt->banks( m_shortType );
      m_status = LHCb::RawBankReadoutStatus( m_shortType);
    }    
    
    if ( 0 == m_banks || 0 == m_banks->size() ){
      info() << "WARNING : None of short and packed banks have been found " << endreq;
      return false;
    }else{
      if( !m_packedIsDefault){      
        if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of packed type has been found" << endreq;
        m_packed = true;
      }else{
        if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of short type has found" << endreq;
      }
    }
  }else{
    if( !m_packedIsDefault){      
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of short type has been found" << endreq;
    }else{
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of packed type has found" << endreq;
      m_packed =true;
    }
  }


  // check whether the associated Error Bank is present or not
  const std::vector<LHCb::RawBank*>* errBanks= &rawEvt->banks(  m_errorType );
  for( std::vector<LHCb::RawBank*>::const_iterator itB = errBanks->begin(); itB != errBanks->end() ; ++itB ) {
    m_status.addStatus( (*itB)->sourceID() , LHCb::RawBankReadoutStatus::ErrorBank );
  }

  
  // check banks integrity

  std::vector<int> sources;
  for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); itB != m_banks->end() ; ++itB ) {
    if(NULL == *itB)continue;
    sources.push_back( (*itB)->sourceID() );
  }  

  if(m_packed){  // TELL1 format : 1 source per TELL1
    std::vector<Tell1Param>& tell1s = m_calo->tell1Params();
    for(std::vector<Tell1Param>::iterator itt = tell1s.begin() ; itt != tell1s.end() ; itt++){
      bool ok=false;
      for(std::vector<int>::iterator it = sources.begin() ; it != sources.end() ; it++){
        if( (*itt).number() == *it){ok=true;break;}
      }
      ok ? 
        m_status.addStatus( (*itt).number()  , LHCb::RawBankReadoutStatus::OK) : 
        m_status.addStatus( (*itt).number()  , LHCb::RawBankReadoutStatus::Missing);
    }
  } else { // Offline format : single source 0
    (sources.size() != 0) ? 
      m_status.addStatus( 0 , LHCb::RawBankReadoutStatus::OK) : 
      m_status.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing);
  }


  return true;
}



//========================
//  Check FE-Cards is PIN
//========================
bool CaloReadoutTool::checkCards(int nCards, std::vector<int> feCards ){
  bool check = true;
  if ( msgLevel( MSG::DEBUG) )debug() << nCards-feCards.size() 
                                      << "FE-Cards have been read among the " << nCards << " expected"<< endreq; 
  if( 0 != feCards.size() ){
    for(unsigned int iFe = 0 ; iFe <  feCards.size();++iFe){ 
      if ( msgLevel( MSG::DEBUG) )debug() << " Unread FE-Cards : " << m_calo->cardCode( feCards[iFe] ) 
                                          << "  - Is it a PinDiode readout FE-Card ? " 
                                          << m_calo->isPinCard( feCards[iFe] ) << endreq;
      if ( !m_calo->isPinCard( feCards[iFe] ) ){
        std::stringstream s("");
        s << m_calo->cardCode( feCards[iFe] )  ;
        Warning(" The standard FE-Card " + s.str() + " expected in TELL1 bank has not been read !!").ignore();
        check = false;
      }
    }    
  }
  return check;
}




//===========================
//  Find Card number by code
//===========================
int CaloReadoutTool::findCardbyCode(std::vector<int> feCards , int code){
  for(unsigned int iFe = 0 ; iFe <  feCards.size();++iFe){ 
    if( code == m_calo->cardCode( feCards[iFe] ) ){
      int crate  = m_calo->cardParam( feCards[ iFe ] ).crate();
      int slot   = m_calo->cardParam( feCards[ iFe ] ).slot();
      if ( msgLevel( MSG::DEBUG) )debug() <<" FE-Card [code : " << code << " | crate : " << crate << " slot : " << slot 
                                          << "] has been found with (num : " << feCards[iFe] <<")  in condDB" << endreq;
      return iFe;
      break;
    }        
  }
  std::stringstream c("");
  c<<code;
  Error("FE-Card [code : " + c.str() + "] does not match the condDB cabling scheme  ",StatusCode::SUCCESS).ignore();
  return -1;
}    


void CaloReadoutTool::putStatusOnTES(){
  // Readout Status
  typedef LHCb::RawBankReadoutStatus Status;
  typedef LHCb::RawBankReadoutStatuss Statuss;  
  Statuss* statuss = getOrCreate<Statuss,Statuss>( LHCb::RawBankReadoutStatusLocation::Default );
  Status* status = statuss->object ( m_status.key() );
  if( NULL == status ){
    status = new Status( m_status  );
    statuss->insert( status );
  } else {
    std::stringstream type("");
    type << LHCb::RawBank::typeName(m_status.key()) ;
    
    if ( msgLevel( MSG::DEBUG) )debug() << "Status for bankType " <<  type.str()  << " already exists" << endreq;
    if( status->status() != m_status.status() ){
      Warning("Status for bankType " +  type.str() + " already exists  with different status value -> merge both"
              , StatusCode::SUCCESS).ignore();
      for( std::map< int, long >::iterator it = m_status.statusMap().begin() ; it != m_status.statusMap().end() ; ++it){
        status->addStatus((*it).first , (*it).second);
      }
    } 
  }
}


void CaloReadoutTool::checkCtrl(int ctrl,int sourceID){

  if ( msgLevel( MSG::DEBUG) )debug()<< "Control word :" << ctrl << endreq;
  
  if( 0 != (0x1& ctrl) || 0 != (0x20& ctrl) || 0 != (0x40& ctrl)){
    if(msgLevel(MSG::DEBUG))debug() << "Tell1 error bits have been detected in data" << endreq;
    if( 0 != (0x1  & ctrl))m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Tell1Error );
    if( 0 != (0x20 & ctrl))m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Tell1Sync  );      
    if( 0 != (0x40 & ctrl))m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Tell1Link  );
  }
}

bool CaloReadoutTool::checkSrc(int source){

  
  bool read = false;

  for(std::vector<int>::iterator it = m_readSources.begin() ; it != m_readSources.end() ; ++it){
    if( source == *it){
      read = true;
      break;
    }    
  }

  if(read){
    std::stringstream s("");
    s<< source;
    Warning("Another bank with same sourceID " + s.str() + " has already been read").ignore();
    m_status.addStatus(source, LHCb::RawBankReadoutStatus::NonUnique );
  }
  else{
    m_readSources.push_back(source);
  }
  return read;
}
