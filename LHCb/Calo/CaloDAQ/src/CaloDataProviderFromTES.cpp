// $Id: CaloDataProviderFromTES.cpp,v 1.1 2008-10-27 18:14:26 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "CaloDataProviderFromTES.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDataProviderFromTES
//
// 2008-08-22 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloDataProviderFromTES );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDataProviderFromTES::CaloDataProviderFromTES( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    ,m_adcs()
    ,m_digits()
{
  declareInterface<ICaloDataProvider>(this);

  declareProperty("InputDataType"     , m_data = "CaloDigits" );
  declareProperty("InputDataLocation" , m_loc = "" );
  // set default detectorName
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_detectorName = name.substr( index, 4 ); 
  if ( name.substr(index,3) == "Prs" ) m_detectorName = "Prs";
  if ( name.substr(index,3) == "Spd" ) m_detectorName = "Spd";


  //
  if ( "Ecal" == m_detectorName ) {
    m_digLoc = LHCb::CaloDigitLocation::Ecal; 
    m_adcLoc = LHCb::CaloAdcLocation::Ecal; 
  } else if ( "Hcal" == m_detectorName ) {
    m_digLoc = LHCb::CaloDigitLocation::Hcal; 
    m_adcLoc = LHCb::CaloAdcLocation::Hcal; 
  } else if ( "Prs" == m_detectorName ) {
    m_digLoc = LHCb::CaloDigitLocation::Prs;
    m_adcLoc = LHCb::CaloAdcLocation::Prs;
  } else if ( "Spd" == m_detectorName ) {
    m_digLoc = LHCb::CaloDigitLocation::Spd; 
    m_adcLoc = LHCb::CaloAdcLocation::Spd; 
  }

}
//=============================================================================
// Destructor
//=============================================================================
CaloDataProviderFromTES::~CaloDataProviderFromTES() {} 

//=============================================================================
StatusCode CaloDataProviderFromTES::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize " << name() << endmsg;

  if ( "Ecal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  } else if ( "Hcal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  } else if ( "Prs" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  } else if ( "Spd" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs ); // Prs FE for SPD
  } else {
    error() << "Unknown detector name " << m_detectorName << endreq;
    return StatusCode::FAILURE;
  }

  //
  if( m_loc == ""){
    if( fromAdc() ){
      m_loc = m_adcLoc;
    }else if( fromDigit() ){
      m_loc = m_digLoc; 
    }else{
      Error("Unknown Data type " + m_data).ignore();
      return StatusCode::FAILURE;
    }    
  }
  
  m_pedShift = m_calo->pedestalShift();
  long nCells = m_calo->numberOfCells();
  long nPins  = m_calo->numberOfPins();
  m_adcs.reserve( nCells + nPins  );
  m_digits.reserve( nCells );
  clear();
  debug() << " Initialisation OK" << endreq;
  return StatusCode::SUCCESS;
}

//-------------------------------------
bool CaloDataProviderFromTES::getBanks( ) {  
  if( fromDigit() ){
    if( exist<LHCb::CaloDigits>(m_loc) ){
      m_digCont = get<LHCb::CaloDigits>(m_loc);
      if ( msgLevel( MSG::DEBUG) )debug() << "Found container " << m_loc << " Content size " << m_digCont->size() << endreq;
    }else return false;
  }else if( fromAdc() ){
    if( exist<LHCb::CaloAdcs>(m_loc) ){
      m_adcCont = get<LHCb::CaloAdcs>(m_loc);
      if ( msgLevel( MSG::DEBUG) )debug() << "Found container " << m_loc << " Content size " << m_adcCont->size() << endreq;
    }else return false;
  }
  return true;
}

//-------------------------------------
void CaloDataProviderFromTES::clear( ) {
  m_adcs.clear();
  m_digits.clear();
}
void CaloDataProviderFromTES::cleanData(int feb ) {
  if(feb<0)return;
  CaloVector<LHCb::CaloAdc> temp;
  CaloVector<LHCb::CaloDigit> tempD;
  for(CaloVector<LHCb::CaloAdc>::iterator iadc = m_adcs.begin();iadc!=m_adcs.end();++iadc){
    if( m_calo->cellParam( (*iadc).cellID() ).cardNumber() == feb)continue;  
    temp.addEntry( *iadc, (*iadc).cellID() );
  }  
  for(CaloVector<LHCb::CaloDigit>::iterator idig = m_digits.begin();idig!=m_digits.end();++idig){
    if( m_calo->cellParam( (*idig).cellID() ).cardNumber() == feb)continue;  
    tempD.addEntry( *idig, (*idig).cellID() );
  }
  m_adcs.clear();
  m_digits.clear();
  for(CaloVector<LHCb::CaloAdc>::iterator iadc = temp.begin();iadc!=temp.end();++iadc){
    m_adcs.addEntry(*iadc, (*iadc).cellID() );
  }
  for(CaloVector<LHCb::CaloDigit>::iterator idig = tempD.begin();idig!=tempD.end();++idig){
    m_digits.addEntry(*idig, (*idig).cellID() );
  }
}
//---------
double CaloDataProviderFromTES::digit (LHCb::CaloCellID id){

  if( fromDigit() ){
    if( NULL == m_digCont )return 0. ;
    LHCb::CaloDigit* digit = m_digCont->object( id );
    return digit->e();
  }else if( fromAdc() ){
    if( NULL == m_adcCont )return 0. ;
    LHCb::CaloAdc* adc = m_adcCont->object( id );
    return ( (double) adc->adc() - m_pedShift ) * m_calo->cellGain( id );
  }
  return 0.;
}
//---------
int CaloDataProviderFromTES::adc (LHCb::CaloCellID id){
  if( fromDigit() ){
    if( NULL == m_digCont )return 0 ;
    LHCb::CaloDigit* digit = m_digCont->object( id );
    return (int) (digit->e() / m_calo->cellGain( id ) + m_pedShift);
  }else if(  fromAdc() ){
    if( NULL == m_adcCont )return 0 ;
    LHCb::CaloAdc* adc = m_adcCont->object( id );
    return adc->adc();
  }
  return 0;
}
//---------
CaloVector<LHCb::CaloAdc>& CaloDataProviderFromTES::adcs(int source){
  clear();
  if( fromDigit() ){
    if( NULL == m_digCont )return m_adcs ;
    for(LHCb::CaloDigits::iterator idig = m_digCont->begin();idig!=m_digCont->end();idig++){
      LHCb::CaloCellID id = (*idig)->cellID();
      if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
      m_adcs.addEntry( LHCb::CaloAdc( id , adc(id) ) , id);
    }
  }else if( fromAdc() ){
    if( NULL == m_adcCont )return m_adcs ;
    for(LHCb::CaloAdcs::iterator iadc = m_adcCont->begin();iadc!=m_adcCont->end();iadc++){
      LHCb::CaloCellID id = (*iadc)->cellID();
      if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
      m_adcs.addEntry( *(*iadc) , id);
    }
  }
  return m_adcs;
}
//---------
CaloVector<LHCb::CaloDigit>& CaloDataProviderFromTES::digits(int source){
  clear();
  if( fromDigit() ){
    if( NULL == m_digCont )return m_digits ;
    for(LHCb::CaloDigits::iterator idig = m_digCont->begin();idig!=m_digCont->end();idig++){
      LHCb::CaloCellID id = (*idig)->cellID();
      if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
      m_digits.addEntry( *(*idig) , id);
    }
  }else if( fromAdc() ){
    if( NULL == m_adcCont )return m_digits ;
    for(LHCb::CaloAdcs::iterator iadc = m_adcCont->begin();iadc!=m_adcCont->end();iadc++){
      LHCb::CaloCellID id = (*iadc)->cellID();
      if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
      m_digits.addEntry( LHCb::CaloDigit( id , digit(id) ) , id);
    }
  }
  return m_digits;
}


