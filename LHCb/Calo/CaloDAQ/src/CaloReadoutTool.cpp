// $Id: CaloReadoutTool.cpp,v 1.18 2007-12-11 21:10:55 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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

  m_banks = NULL;
  LHCb::RawEvent* rawEvt = NULL ;
  m_raw = LHCb::RawEventLocation::Default;
  if ( msgLevel( MSG::DEBUG) )debug() << "raw location :: " << rootInTES() + m_raw << endmsg;  
  if( exist<LHCb::RawEvent>( m_raw ) ){
    rawEvt= get<LHCb::RawEvent>( m_raw );
  }else  {
    warning() << "rawEvent not found at location '" << rootInTES() + m_raw 
              << "'" << endmsg;
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
      Error("None of short and packed banks have been found ").ignore();
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

  // check banks integrity

  std::vector<int> sources;
  for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); itB != m_banks->end() ; ++itB ) {
    if(NULL == *itB)continue;
    sources.push_back( (*itB)->sourceID() );
  }  

  if(m_packed){  // TELL1 format : 1 source per TELL1
    std::vector<Tell1Param>& tell1s = m_calo->tell1Params();
    for(std::vector<int>::iterator it = sources.begin() ; it != sources.end() ; it++){
      bool ok=false;
      for(std::vector<Tell1Param>::iterator itt = tell1s.begin() ; itt != tell1s.end() ; itt++){
        if( (*itt).number() == *it){ok=true;break;}
      }
      ok ? 
        m_status.addStatus( *it , LHCb::RawBankReadoutStatus::OK) : 
        m_status.addStatus( *it , LHCb::RawBankReadoutStatus::Missing);
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
        warning() << " The standard FE-Card " << m_calo->cardCode( feCards[iFe] )  
                 << " expected in TELL1 bank has not been read !!"<< endreq;
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
  error() << "  FE-Card [code : " << code << "] does not match the condDB cabling scheme  " << endreq;
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
    debug() << "Status for bankType " <<  LHCb::RawBank::BankType(m_status.key()) << " already exists" << endreq;
    if( status->status() != m_status.status() ){
      error() << "Status for bankType " <<  LHCb::RawBank::BankType(m_status.key()) << " already exists" << endreq;
      error() << " with different status value !!! "
              << status->status() << " / " << m_status.status()
              << endreq;
    } 
  }
}
