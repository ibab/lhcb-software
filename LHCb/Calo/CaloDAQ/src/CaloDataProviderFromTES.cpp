// Include files

// local
#include "CaloDataProviderFromTES.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDataProviderFromTES
//
// 2008-08-22 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloDataProviderFromTES )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDataProviderFromTES::CaloDataProviderFromTES( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  ,m_calo(0)
  ,m_digCont(NULL)
  ,m_adcCont(NULL)
  ,m_ok(false)
  ,m_getRaw(true)
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
StatusCode CaloDataProviderFromTES::finalize ( ) {
     IIncidentSvc* inc = incSvc() ;
     if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
     return GaudiTool::finalize();
}

StatusCode CaloDataProviderFromTES::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Initialize " << name() << endmsg;

  // fill incident listener
  IIncidentSvc* inc = incSvc() ;
  if ( inc ) inc -> addListener  ( this , IncidentType::BeginEvent ) ;


  if ( "Ecal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  } else if ( "Hcal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  } else if ( "Prs" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  } else if ( "Spd" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs ); // Prs FE for SPD
  } else {
    error() << "Unknown detector name " << m_detectorName << endmsg;
    return StatusCode::FAILURE;
  }

  //
  if( m_loc.empty() ){
    if( fromAdc() ){
      m_loc = m_adcLoc;
    }else if( fromDigit() ){
      m_loc = m_digLoc;
    }else{
      Error("Unknown Data type " + m_data).ignore();
      return StatusCode::FAILURE;
    }
  }

  long nCells = m_calo->numberOfCells();
  long nPins  = m_calo->numberOfPins();
  m_adcs.reserve( nCells + nPins  );
  m_digits.reserve( nCells );
  clear();

  //

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << " Initialisation OK" << endmsg;
  return StatusCode::SUCCESS;
}

//-------------------------------------
bool CaloDataProviderFromTES::getBanks( ) {
  clear();
  m_getRaw = false;
  counter("Call for ADC containers") += 1;
  if( fromDigit() ){
    m_digCont = getIfExists<LHCb::CaloDigits>(evtSvc(),m_loc);
    if( m_digCont ){
      m_ok = true;
      if ( msgLevel( MSG::DEBUG) )debug() << "Found container " << m_loc << " Content size " << m_digCont->size() << endmsg;
    }else m_ok = false;
  }else if( fromAdc() ){
    m_adcCont = getIfExists<LHCb::CaloAdcs>(evtSvc(),m_loc);
    if( m_adcCont ){
      m_ok = true;
      if ( msgLevel( MSG::DEBUG) )debug() << "Found container " << m_loc << " Content size " << m_adcCont->size() << endmsg;
    }else m_ok = false;
  }
  if(!m_ok)counter("No container found") += 1;
  return m_ok;
}

//-------------------------------------
void CaloDataProviderFromTES::clear( ) {
  m_adcs.clear();
  m_digits.clear();
  m_readSources.clear();
  m_minADC = LHCb::CaloAdc(LHCb::CaloCellID() , 3840);
  m_minPinADC = LHCb::CaloAdc(LHCb::CaloCellID(), 3840);
  m_maxADC = LHCb::CaloAdc(LHCb::CaloCellID() , -256);
  m_maxPinADC = LHCb::CaloAdc(LHCb::CaloCellID(), -256);
}
void CaloDataProviderFromTES::cleanData(int feb ) {
  if(feb<0)return;
  CaloVector<LHCb::CaloAdc> temp;
  CaloVector<LHCb::CaloDigit> tempD;
  for(const auto& iadc : m_adcs) {
    if( m_calo->cellParam( iadc.cellID() ).cardNumber() == feb)continue;
    temp.addEntry( iadc, iadc.cellID() );
  }
  for(const auto& idig : m_digits) {
    if( m_calo->cellParam( idig.cellID() ).cardNumber() == feb)continue;
    tempD.addEntry( idig, idig.cellID() );
  }
  m_adcs.clear();
  m_digits.clear();
  for(const auto& iadc : temp ) m_adcs.addEntry(iadc, iadc.cellID() );
  for(const auto& idig : tempD) m_digits.addEntry(idig, idig.cellID() );
}
//---------
double CaloDataProviderFromTES::digit (LHCb::CaloCellID id, double def){
  double pedShift = m_calo->pedestalShift();
  if(m_getRaw)getBanks();
  if( fromDigit() ){
    if( !m_digCont )return def ;
    LHCb::CaloDigit* digit = m_digCont->object( id );
    if( !digit )return def;
    return digit->e();
  }
  if( fromAdc() ){
    if( !m_adcCont )return def ;
    LHCb::CaloAdc* adc = m_adcCont->object( id );
    if( !adc )return def;
    return ( (double) adc->adc() - pedShift ) * m_calo->cellGain( id );
  }
  return def;
}
//---------
int CaloDataProviderFromTES::adc (LHCb::CaloCellID id,int def){
  double pedShift = m_calo->pedestalShift();
  if(m_getRaw)getBanks();
  if( fromDigit() ){
    if( !m_digCont )return def ;
    LHCb::CaloDigit* digit = m_digCont->object( id );
    if( !digit )return def;
    return (int) (digit->e() / m_calo->cellGain( id ) + pedShift);
  }
  if(  fromAdc() ){
    if( !m_adcCont )return def ;
    LHCb::CaloAdc* adc = m_adcCont->object( id );
    if( !adc )return def;
    return adc->adc();
  }
  return def;
}
//---------


const CaloVector<LHCb::CaloAdc>& CaloDataProviderFromTES::adcs(int source,bool clean){
  return adcs({source},clean);
}
const CaloVector<LHCb::CaloDigit>& CaloDataProviderFromTES::digits(int source,bool clean){
  return digits({source},clean);
}


const CaloVector<LHCb::CaloAdc>& CaloDataProviderFromTES::adcs(std::vector<int> sources,bool clean){
  if(m_getRaw)getBanks();
  if(clean)clear();
  for(const auto& source : sources ) {
    if(checkSrc( source ))continue;
    if( fromDigit() ){
      if( !m_digCont )return m_adcs ;
      for(const auto& idig : *m_digCont) {
        LHCb::CaloCellID id = idig->cellID();
        if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
        LHCb::CaloAdc cAdc(id ,adc(id) );
        m_adcs.addEntry( cAdc , id);
        if( id.area() != CaloCellCode::PinArea ){
          if( cAdc.adc() < m_minADC.adc()  )m_minADC = cAdc ;
          if( cAdc.adc() > m_maxADC.adc()  )m_maxADC = cAdc ;
        }else{
          if( cAdc.adc() < m_minPinADC.adc() )m_minPinADC = cAdc ;
          if( cAdc.adc() > m_maxPinADC.adc() )m_maxPinADC = cAdc ;
        }
      }
    }else if( fromAdc() ){
      if( !m_adcCont )return m_adcs ;
      for(const auto& iadc : *m_adcCont){
        LHCb::CaloCellID id = iadc->cellID();
        if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
        LHCb::CaloAdc adc = *iadc;
        m_adcs.addEntry( adc , id);
        if( id.area() != CaloCellCode::PinArea ){
          if( adc.adc() < m_minADC.adc()  )m_minADC = adc ;
          if( adc.adc() > m_maxADC.adc()  )m_maxADC = adc ;
        }else{
          if( adc.adc() < m_minPinADC.adc() )m_minPinADC = adc ;
          if( adc.adc() > m_maxPinADC.adc() )m_maxPinADC = adc ;
        }
      }
    }
  }
  return m_adcs;
}
//---------
const CaloVector<LHCb::CaloDigit>& CaloDataProviderFromTES::digits(std::vector<int> sources,bool clean){
  if(m_getRaw)getBanks();
  if(clean)clear();
  for(const auto& source : sources) {
    if(checkSrc( source ))continue;
    if( fromDigit() ){
      if( !m_digCont )return m_digits ;
      for(const auto& idig : *m_digCont) {
        LHCb::CaloCellID id = idig->cellID();
        if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
        m_digits.addEntry( *idig , id);
      }
    }else if( fromAdc() ){
      if( !m_adcCont )return m_digits ;
      for(const auto& iadc : *m_adcCont) {
        LHCb::CaloCellID id = iadc->cellID();
        if( source != -1 && source != m_calo->cardToTell1 ( m_calo->cardNumber( id ) )) continue;
        m_digits.addEntry( LHCb::CaloDigit( id , digit(id) ) , id);
      }
    }
  }
  return m_digits;
}

bool CaloDataProviderFromTES::checkSrc(int source){
  return std::any_of( m_readSources.begin(), m_readSources.end(),
                      [&](int i) { return source == i; } );
}
