// $Id: CaloZSupAlg.cpp,v 1.5 2006-11-23 13:38:32 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// local
#include "CaloZSupAlg.h"

// ============================================================================
/** @file CaloZSupAlg.cpp
 *
 *    Calorimeter Zero suppression.
 *
 *    @author: Olivier Callot
 *    @date:   10 October 2002
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( CaloZSupAlg );

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloZSupAlg::CaloZSupAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm       ( name , pSvcLocator            )
{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("DetectorName"    , m_detectorName       ) ;
  declareProperty("OutputADCData"   , m_outputADCData      ) ;
  declareProperty("OutputDigitData" , m_outputDigitData    ) ;
  declareProperty("ZsupMethod"      , m_zsupMethod         ) ;
  declareProperty("ZsupThreshold"   , m_zsupThreshold      ) ;
  declareProperty("OutputType"      , m_outputType = "Digits"  ) ;



  m_digitOnTES =false  ;
  m_adcOnTES =false  ;
  
    


  //=== Default values according to the name of the algorithm !
  m_inputToolType = "CaloEnergyFromRaw";
  m_inputToolName = name + "Tool";
  if ( "Ecal" == name.substr( 0 , 4 ) ) {
    m_detectorName     = DeCalorimeterLocation::Ecal;
    m_outputADCData    = LHCb::CaloAdcLocation::Ecal;
    m_outputDigitData  = LHCb::CaloDigitLocation::Ecal;
    m_zsupMethod       = "2D";
    m_zsupThreshold    = 20;
  } else if ( "Hcal" == name.substr( 0 , 4 ) ) {
    m_detectorName     = DeCalorimeterLocation::Hcal;
    m_outputADCData    = LHCb::CaloAdcLocation::Hcal;
    m_outputDigitData  = LHCb::CaloDigitLocation::Hcal;
    m_zsupMethod       = "1D";
    m_zsupThreshold    = 4;
  }
};

//=============================================================================
// Standard destructor
//=============================================================================
CaloZSupAlg::~CaloZSupAlg() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloZSupAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return sc;

  if( m_outputType == "Digits" || 
      m_outputType == "CaloDigits" || 
      m_outputType == "Both") m_digitOnTES = true;
  if(m_outputType == "ADCs" || 
     m_outputType == "CaloAdcs" || 
     m_outputType == "Both")m_adcOnTES = true;
  if( !m_adcOnTES && !m_digitOnTES ){
    error() << "CaloZSupAlg configured to produce ** NO ** output" << endreq;
    return StatusCode::FAILURE;
  }  
  if( m_digitOnTES )debug() <<  "CaloZSupAlg will produce CaloDigits on TES" 
                            << endreq;
  if( m_adcOnTES )debug() <<  "CaloZSupAlg will produce CaloAdcs on TES" 
                          << endreq;
  
  // Retrieve the calorimeter we are working with.
  m_calo = getDet<DeCalorimeter>( m_detectorName );
  m_numberOfCells = m_calo->numberOfCells();
  m_pedShift      = m_calo->pedestalShift();
  
  //*** A few check of the parameters

  if ( "NO" != m_zsupMethod && 
       "1D" != m_zsupMethod && 
       "2D" != m_zsupMethod) {
    error() << "Unknown Z-sup mode" << m_zsupMethod
            << " (must be NO,1D or 2D)" << endreq;
    return StatusCode::FAILURE;
  }

  m_zsup2D = ( "2D" == m_zsupMethod );

  if ( "NO" == m_zsupMethod ) {
    if( -1000 != m_zsupThreshold ) {
      info() << " Threshold is reset from "<< m_zsupThreshold
             << " to " << -1000 << endreq; }
    m_zsupThreshold = -1000 ; // no threshold at all !
  }

  info() << "Calorimeter has " <<  m_numberOfCells
         << " cells. Zsup method "  << m_zsupMethod 
         << " Threshold " << m_zsupThreshold << endreq;

  m_adcTool = tool<ICaloEnergyFromRaw>( m_inputToolType , m_inputToolName,this);

  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloZSupAlg::execute() {

  bool isDebug   = msgLevel() <= MSG::DEBUG;
  bool isVerbose = msgLevel() <= MSG::VERBOSE;
  
  //*** some trivial printout

  if ( isDebug && m_adcOnTES) debug() << "Perform zero suppression - return CaloAdcs on TES at "
                                      << m_outputADCData << endreq;
  if ( isDebug && m_digitOnTES) debug() << "Perform zero suppression - return CaloDigits on TES at "
                                      << m_outputDigitData << endreq;


  //*** get the input data

  std::vector<LHCb::CaloAdc>& adcs = m_adcTool->adcs( );

  //***  prepare the output containers
  LHCb::CaloAdcs* newAdcs=0;
  LHCb::CaloDigits* newDigits=0;
  if(m_adcOnTES){
    newAdcs = new LHCb::CaloAdcs();
    put( newAdcs, m_outputADCData );
  }else delete newAdcs;
  if(m_digitOnTES) {
    newDigits = new LHCb::CaloDigits();
    put( newDigits, m_outputDigitData );
  }else delete newDigits;



  
  if ( isDebug ) debug() << "Processing " << adcs.size() 
                         << " Digits." << endreq;

  enum {
    DefaultFlag   ,
    NeighborFlag  ,
    SeedFlag       };

  std::vector<LHCb::CaloAdc>::const_iterator anAdc;
  std::vector<int> caloFlags    ( m_numberOfCells, DefaultFlag ) ;

  int index;
  
  // == Apply the threshold. If 2DZsup, tag also the neighbours

  for( anAdc = adcs.begin(); adcs.end() != anAdc ; ++anAdc ) {
    LHCb::CaloCellID id = (*anAdc).cellID();
    index         = m_calo->cellIndex( id );
    int    digAdc = (*anAdc).adc();
    if( m_zsupThreshold <= digAdc ) {
      if( isDebug ) {
        debug() << id 
                << format( " Energy adc %4d", digAdc );
        if (  m_zsupThreshold <= digAdc ) debug() << " seed";
        debug() << endreq;
      }
      
      caloFlags[index] = SeedFlag ;
      if( m_zsup2D ) {
        CaloNeighbors::const_iterator neighbor =  m_calo->zsupNeighborCells( id ).begin() ;
        while ( neighbor != m_calo->zsupNeighborCells( id ).end() ) {
          int neigh = m_calo->cellIndex(*neighbor);
          if( SeedFlag != caloFlags[neigh] ) {
            caloFlags[neigh] = NeighborFlag ;
          }
          neighbor++;
        }
      }
    }
  }

  //** write tagged data as CaloAdc or CaloDigits according to m_digitsOutput
  
  for( anAdc = adcs.begin(); adcs.end() != anAdc ; ++anAdc ) {
    LHCb::CaloCellID id = (*anAdc).cellID();
    index         = m_calo->cellIndex( id );
    if( DefaultFlag == caloFlags[index] ) { continue; }
    
    if(m_adcOnTES){
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( id, (*anAdc).adc() );
      newAdcs->insert( adc ) ;
    }

    if(m_digitOnTES){
      double e = ( double( (*anAdc).adc() ) - m_pedShift ) * m_calo->cellGain( id );
      LHCb::CaloDigit* digit = new LHCb::CaloDigit(id,e);
      newDigits->insert( digit ) ;
    }
    
    if( isVerbose ) {
      if ( NeighborFlag == caloFlags[index] ) {
        verbose() << id << " added as Neighbor." << endreq;
      } else {
        verbose() << id << " added as Seed.    " << endreq;
      }
    }    
  }
  if(m_adcOnTES){
    debug() << format( "Have stored %5d CaloAdcs.", newAdcs->size() ) 
            << endreq;
  }
  if(m_digitOnTES){
    debug() << format( "Have stored %5d CaloDigits.", newDigits->size() ) 
            << endreq;
  }


  return StatusCode::SUCCESS;
};

//=============================================================================
