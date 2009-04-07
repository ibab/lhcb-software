// $Id: CaloPinDigitAlg.cpp,v 1.6 2009-04-07 09:54:24 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/RndmGenerators.h"
// local
#include "Event/CaloAdc.h"
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"
#include "CaloPinDigitAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloPinDigitAlg
//
// @todo
// Treat SPD correctly 
// Add L0 data
// Led sequence
//
// 2007-02-06 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloPinDigitAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloPinDigitAlg::CaloPinDigitAlg( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_rndmSvc           (  0    )
{

  m_detectorName = name.substr( 0 , 4 );
  if ( name.substr( 0 , 3 ) == "Prs" ) m_detectorName = "Prs";  
  if ( name.substr( 0 , 3 ) == "Spd" ) m_detectorName = "Spd";  

  declareProperty("LedSignal" , m_signal );
  declareProperty("LedSpread" , m_spread );
  declareProperty("CoherentNoise"   , m_cNoise =   0.0 );
  declareProperty("IncoherentNoise" , m_iNoise =   1.0 );
  declareProperty("LedRate"         , m_rate   =   20 );
  declareProperty("SeparatePinContainer"  , m_separatePinContainer   = false);
  m_signal.push_back(100.);
  m_spread.push_back(2.);
  
}
//=============================================================================
// Destructor
//=============================================================================
CaloPinDigitAlg::~CaloPinDigitAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloPinDigitAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;





  if ( "Spd" == m_detectorName ) {
    m_calo  = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
    m_data  = LHCb::CaloAdcLocation::Spd;
    m_outputData = "None ";
    m_cNoise = 0.0;
    m_iNoise = 0.0;

    if(m_signal.size() != 1 || m_spread.size() != 1 ){
      error() << "Spd : requires only 1 LED signal  per PMT " << endreq;
    return StatusCode::FAILURE;
    }

  } else if ( "Prs" == m_detectorName ) {
    m_calo  = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_data  = LHCb::CaloAdcLocation::Prs;
    m_outputData = "None ";
    m_cNoise = 0.0;
    m_iNoise = 1.0;

    if(m_signal.size() != 1 || m_spread.size() != 1 ){
      error() << "Prs : requires only 1 LED signal  per PMT " << endreq;
    return StatusCode::FAILURE;
    }

  } else if ( "Ecal" == m_detectorName ) {
    m_calo  = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    m_data  = LHCb::CaloAdcLocation::FullEcal;    
    m_outputData = LHCb::CaloAdcLocation::EcalPin;
    m_cNoise = 0.3;
    m_iNoise = 1.2;

    if(m_signal.size() != 1 || m_spread.size() != 1 ){
      error() << "Ecal : requires only 1 LED signal  per PMT " << endreq;
    return StatusCode::FAILURE;
    }

  } else if ( "Hcal" == m_detectorName ) {
    m_calo  = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    m_data  = LHCb::CaloAdcLocation::FullHcal;
    m_outputData = LHCb::CaloAdcLocation::HcalPin;
    m_cNoise = 0.3;
    m_iNoise = 1.2;

    if(m_signal.size() > 2 || m_signal.size() < 1 || m_spread.size() != m_signal.size() ){
      error() << "Hcal : requires  1 or 2 LED  signal per PMT " << endreq;
    return StatusCode::FAILURE;
    }


  }  else {
    return( Error( "Invalid detector Name =  " + m_detectorName ) );
  }


  m_saturateAdc = m_calo->adcMax();  
  m_rndmSvc = svc< IRndmGenSvc>( "RndmGenSvc" , true );
  m_count = -1;

  
  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloPinDigitAlg::execute() {

  debug() << "==> Execute" << endmsg;
  ++m_count;

  
  if( m_count >= m_rate)m_count = 0;
  debug() << " Sequence " << m_count << "/" << m_rate << endreq;
  // Init
  Rndm::Numbers normale( rndmSvc() , Rndm::Gauss( 0.0 , 1.0) );
  
  
  // get the ADCs (normal cells) 
  LHCb::CaloAdcs* adcs = get<LHCb::CaloAdcs>( m_data );
  
  if( "Prs" == m_detectorName || "Spd" == m_detectorName ){
    LHCb::L0PrsSpdHits* spdBank = get<LHCb::L0PrsSpdHits>( LHCb::L0PrsSpdHitLocation::Spd );
    LHCb::L0PrsSpdHits* prsBank = get<LHCb::L0PrsSpdHits>( LHCb::L0PrsSpdHitLocation::Prs );
    prsBank->clear();// remove trigger on noise
    spdBank->clear();// remove trigger on noise
    debug() << "Name " << m_detectorName << adcs->size() << endreq;
    // Trivial monitoring system for Prs/Spd : 1 Led -> 1 Cell !!!
    // loop over Cells
    for(LHCb::CaloAdcs::const_iterator iCell = adcs->begin() ; iCell != adcs->end() ; ++iCell ) {
      int index = iCell - adcs->begin();
      debug() << " Prs/Spd " << index  << " " << m_count << endreq;
      if( index-m_count < 0 )continue;
      if( 0 !=  m_rate*( (int) (index-m_count)/m_rate)-(index-m_count) )continue;
      double signal = m_signal[0];
      double spread = normale() *  m_spread[0];
      int newAdc = (*iCell)->adc() + (int) floor( signal + spread +0.5 ) ;
      if ( newAdc > m_saturateAdc ) { newAdc = m_saturateAdc ; } 
      debug() << " Prs/Spd Add signal " << newAdc << endreq;
      (*iCell)->setAdc( newAdc );
      if ( newAdc > 0 ){
        debug() << "  insert SPD/Prs" << endreq;
        LHCb::CaloCellID prsId = (*iCell)->cellID();
        LHCb::CaloCellID spdId(0,prsId.area(),prsId.col(),prsId.row());
        LHCb::L0PrsSpdHit* spdHit = new LHCb::L0PrsSpdHit( spdId );
        LHCb::L0PrsSpdHit* prsHit = new LHCb::L0PrsSpdHit( prsId );
        spdBank->insert( spdHit );
        prsBank->insert( prsHit );
      }      
    }
    return StatusCode::SUCCESS;
  }


  
  LHCb::CaloAdcs* pinAdcs ;
  if(!m_separatePinContainer){
    pinAdcs = adcs;
  }else{
    pinAdcs= new LHCb::CaloAdcs();
    put( pinAdcs, m_outputData );
  }
  

  // ==============================
  // Create Noise for the pinDiode 
  // ============================== 
  const CaloVector<CaloPin>&  caloPins = m_calo->caloPins();

  // (in)coherent noise
  Rndm::Numbers rndCNoise( rndmSvc() , Rndm::Gauss( 0.0 , m_cNoise ) );// coherent noise
  Rndm::Numbers rndINoise( rndmSvc() , Rndm::Gauss( 0.0 , m_iNoise ) );// incoherent noise  
  double cNoise = rndCNoise();
  for( CaloVector<CaloPin>::const_iterator iPin = caloPins.begin() ; iPin != caloPins.end() ; ++iPin ) {
    CaloPin pin = *iPin;
    const LHCb::CaloCellID   id    = pin.id();
    int pinSignal  = (int) floor( cNoise + rndINoise() + 0.5 ); 
    std::vector<int> leds = pin.leds();
    debug() << " PIN " << iPin-caloPins.begin() << " id " << id 
            << " pedestal : " << pinSignal << " Leds : " << leds << endreq;
    LHCb::CaloAdc* pinAdc = new LHCb::CaloAdc(id , pinSignal );
    pinAdcs->insert( pinAdc ); 
  }
  debug() << " Initialized : " << pinAdcs->size() << " ADCs for pin Diode " << endreq;

  // ================//
  // Create LED data //
  // ================//
  const std::vector<CaloLed>&  caloLeds = m_calo->caloLeds();

  debug() << " Get " << caloLeds.size() << " LEDs " << endreq;
  

  // loop over LEDs
  for(std::vector<CaloLed>::const_iterator iLed = caloLeds.begin() ; iLed != caloLeds.end() ; ++iLed ) {
    CaloLed led = *iLed;
    int index = led.number();
    debug() << "Led index " << index << endreq;
    // LED firing : use m_rate & m_count
    if( index-m_count < 0 )continue;
    if( 0 !=  m_rate*( (int) (index-m_count)/m_rate)-(index-m_count) )continue;
    debug() << "---> is fired" << endreq;
    const std::vector<LHCb::CaloCellID>& cells = led.cells();
    //if( !ledIsFired ) continue;
    CaloPin cPin = caloPins[led.pin()];
    debug () << "-----> LED id " << led.number() 
             << " => "  << cells.size()  << " cells "
             << " => PIN id " << led.pin() <<  " => " << cPin.cells().size() <<" cells"<< endreq; 
    

    //
    std::vector<int> data;
    for( unsigned int irndm = 0 ; irndm < m_spread.size() ; ++irndm){
      double signal = m_signal[irndm];
      double spread = m_spread[irndm]*normale();
      data.push_back( (int) floor( signal + spread +0.5 ) );
    }
    

    debug() << " -----> Add data value " << data << endreq;
    // Add data to cells ADCs
    debug() << cells.size() << " connected cells " << endreq;
    for ( std::vector<LHCb::CaloCellID>::const_iterator iCell = cells.begin(); iCell != cells.end() ; ++iCell){
      LHCb::CaloAdc* adc = adcs->object( *iCell );
      if( NULL == adc){
        warning() << " ADC not found for cell " << *iCell << endreq;
        continue;
      }
      int newAdc = data[0] + adc->adc();

      // Hcal : 2 LEDs -> PMT
      if( 1 != data.size() ){
        int index = m_calo->caloPins()[(*iLed).pin()].index();
        if(m_calo->cellParam(*iCell).leds().size() ==  data.size() && (unsigned) index <  data.size() ){
          newAdc = data[ index ] + adc->adc();
        }
        else{
          warning() << " # Led signal requested does not match with # LED per PMT " << endreq;
        }        
      }
      if ( newAdc > m_saturateAdc ) { newAdc = m_saturateAdc ; } 
      verbose() << " -----> cell Adc " << *iCell <<" : " <<adc->adc() << " -> " << newAdc << endreq;
      adc->setAdc( newAdc );
    }

    // Add data to Corresponding Pin-Diode
    LHCb::CaloCellID pin = (*iLed).pin();
    LHCb::CaloAdc* pinAdc =  pinAdcs->object( pin );

    int newAdc = pinAdc->adc() + data[0]; // add data
    if( 1 != data.size() ){
      int index = m_calo->caloPins()[(*iLed).pin()].index();
      if( (unsigned) index < data.size() ){
        newAdc = data[index] + pinAdc->adc();
        }
      else{
        warning() << " # Led index does not match with LedSignal size " << endreq;
      }        
    }
    
    
    if ( newAdc > m_saturateAdc ) { newAdc = m_saturateAdc ; }  
    pinAdc->setAdc ( newAdc ); 
    debug() << " -----> pin Adc " << newAdc << endreq;
  } 
  verbose() << " The End " << endreq;
  return StatusCode::SUCCESS; 
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloPinDigitAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
