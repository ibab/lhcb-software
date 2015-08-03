// Include files 
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
DECLARE_TOOL_FACTORY( CaloReadoutTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloReadoutTool::CaloReadoutTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : Decoder::ToolBase ( type, name , parent )
  , m_banks(0)
  , m_calo(0)
  , m_packed(false)
  , m_ok(false)
  , m_first(true)
{
  declareInterface<ICaloReadoutTool>(this);

  declareProperty( "DetectorName"   , m_detectorName );
  declareProperty( "PackedIsDefault", m_packedIsDefault = false);
  declareProperty( "DetectorSpecificHeader", m_extraHeader = false);
  declareProperty( "CleanWhenCorruption", m_cleanCorrupted = false);
  m_getRaw = true;
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Calo, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
}
//=============================================================================
// Destructor
//=============================================================================
CaloReadoutTool::~CaloReadoutTool() {}

//=========================================================================
//  Get required CaloBanks (short or packed format) - Fill m_banks
//=========================================================================
StatusCode CaloReadoutTool::initialize(){

  StatusCode sc = Decoder::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "==> Initialize " << name() << endmsg;
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;
  
  return sc;
}
StatusCode CaloReadoutTool::finalize(){
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
  return GaudiTool::finalize();
}

  


bool CaloReadoutTool::getCaloBanksFromRaw( ) {

  m_readSources.clear();
  m_banks = NULL;

  // Retrieve the RawEvent:
  LHCb::RawEvent* rawEvt = findFirstRawEvent() ;
  
  if( rawEvt == NULL ) {
    if( m_first && msgLevel( MSG::DEBUG ) )
      debug() << "WARNING : RawEvent not found at locations: " 
              << m_rawEventLocations << " (message will be suppressed)" << endmsg;
    m_first=false;
    return false;
  }

  m_packed =false;
  if( !m_packedIsDefault){
    if ( msgLevel( MSG::DEBUG) )debug() << "Banks of short type are requested as default" << endmsg;
    m_banks= &rawEvt->banks(  m_shortType );
    m_status = LHCb::RawBankReadoutStatus( m_shortType);
  }else{
    if ( msgLevel( MSG::DEBUG) )debug() << "Banks of paked type are requested as default" << endmsg;
    m_banks= &rawEvt->banks(  m_packedType );
    m_status = LHCb::RawBankReadoutStatus( m_packedType);
  }
  
  
  if ( 0 == m_banks || 0 == m_banks->size() ) {
    if( !m_packedIsDefault){      
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of short type has not been found ... try packed type" << endmsg;
      m_banks = &rawEvt->banks( m_packedType );
      m_status = LHCb::RawBankReadoutStatus( m_packedType);
    }else{
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of packed type has not been found ... try short type" << endmsg;
      m_banks = &rawEvt->banks( m_shortType );
      m_status = LHCb::RawBankReadoutStatus( m_shortType);
    }    
    
    if ( 0 == m_banks || 0 == m_banks->size() ){
      if ( msgLevel( MSG::DEBUG) )debug() << "WARNING : None of short and packed banks have been found "<<endmsg;
      counter("No " + m_detectorName + " bank found") += 1;
      return false;
    }else{
      if( !m_packedIsDefault){      
        if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of packed type has been found" << endmsg;
        m_packed = true;
      }else{
        if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of short type has found" << endmsg;
      }
    }
  }else{
    if( !m_packedIsDefault){      
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of short type has been found" << endmsg;
    }else{
      if ( msgLevel( MSG::DEBUG) )debug()<< " Requested banks of packed type has found" << endmsg;
      m_packed =true;
    }
  }


  // check whether the associated Error Bank is present or not
  const std::vector<LHCb::RawBank*>* errBanks= &rawEvt->banks(  m_errorType );
  for( std::vector<LHCb::RawBank*>::const_iterator itB = errBanks->begin(); itB != errBanks->end() ; ++itB ) {
    m_status.addStatus( (*itB)->sourceID() , LHCb::RawBankReadoutStatus::ErrorBank );
  }

  
  // check banks integrity + Magic pattern
  std::vector<int> sources;
  for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); itB != m_banks->end() ; ++itB ) {
    if(NULL == *itB)continue;
    sources.push_back( (*itB)->sourceID() );
    if( LHCb::RawBank::MagicPattern != (*itB)->magic() ) {
      Error("Bad MagicPattern for sourceID " + Gaudi::Utils::toString( (*itB)->sourceID()),StatusCode::SUCCESS).ignore();
      m_status.addStatus( (*itB)->sourceID() , LHCb::RawBankReadoutStatus::BadMagicPattern);
    }  
  }
  
  if(m_packed){  // TELL1 format : 1 source per TELL1
    const std::vector<Tell1Param>& tell1s = m_calo->tell1Params();
    for(std::vector<Tell1Param>::const_iterator itt = tell1s.begin() ; itt != tell1s.end() ; itt++){
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
                                      << "FE-Cards have been read among the " << nCards << " expected"<< endmsg; 
  if( 0 != feCards.size() ){
    for(unsigned int iFe = 0 ; iFe <  feCards.size();++iFe){ 
      if ( msgLevel( MSG::DEBUG) )debug() << " Unread FE-Cards : " << m_calo->cardCode( feCards[iFe] ) 
                                          << "  - Is it a PinDiode readout FE-Card ? " 
                                          << m_calo->isPinCard( feCards[iFe] ) << endmsg;
      if ( m_calo->isPmtCard( feCards[iFe] ) ){
        Warning(" The standard (PMT) FE-Card " + Gaudi::Utils::toString( m_calo->cardCode( feCards[iFe] ) ) 
                + " expected in TELL1 bank has not been read !!").ignore();
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
                                          << "] has been found with (num : " << feCards[iFe] <<")  in condDB" << endmsg;
      return iFe;
      break;
    }        
  }
  Error("FE-Card [code : " + Gaudi::Utils::toString(code) 
        + "] does not match the condDB cabling scheme  ",StatusCode::SUCCESS).ignore();
  return -1;
}    


void CaloReadoutTool::putStatusOnTES(){
  // Readout Status
  typedef LHCb::RawBankReadoutStatus Status;
  typedef LHCb::RawBankReadoutStatuss Statuss;  
  if( msgLevel( MSG::DEBUG) )debug()<< "Creating container at " 
                                    << LHCb::RawBankReadoutStatusLocation::Default << endmsg;
  Statuss* statuss = getOrCreate<Statuss,Statuss>( LHCb::RawBankReadoutStatusLocation::Default );
  Status* nstatus = statuss->object ( m_status.key() );
  if( NULL == nstatus ){
    if( msgLevel( MSG::DEBUG))debug() << "Inserting new status for bankType " 
                                   <<Gaudi::Utils::toString( m_status.key())
                                   <<endmsg;
    nstatus = new Status( m_status  );
    statuss->insert( nstatus );
  } else {
    if ( msgLevel( MSG::DEBUG) )debug() << "Status for bankType " <<  Gaudi::Utils::toString( m_status.key())
                                        << " already exists" << endmsg;
    if( nstatus->status() != m_status.status() ){
      Warning("Status for bankType " +  LHCb::RawBank::typeName(m_status.key()) 
              + " already exists  with different status value -> merge both"
              , StatusCode::SUCCESS).ignore();
      std::map< int, long > smap = m_status.statusMap();
      for( std::map< int, long >::iterator it = smap.begin() ; it != smap.end() ; ++it){
        nstatus->addStatus((*it).first , (*it).second);
      }
    }
  }
}


void CaloReadoutTool::checkCtrl(int ctrl,int sourceID){

  if ( msgLevel( MSG::DEBUG) )debug()<< "Control word :" << ctrl << endmsg;
  
  if( 0 != (0x1& ctrl) || 0 != (0x20& ctrl) || 0 != (0x40& ctrl)){
    if(msgLevel(MSG::DEBUG))debug() << "Tell1 error bits have been detected in data" << endmsg;
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
    Warning("Another bank with same sourceID " + Gaudi::Utils::toString( source ) + " has already been read").ignore();
    m_status.addStatus(source, LHCb::RawBankReadoutStatus::NonUnique );
  }
  else{
    m_readSources.push_back(source);
  }
  return read;
}
