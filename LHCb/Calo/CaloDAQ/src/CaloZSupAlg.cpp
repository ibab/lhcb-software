// $Id: CaloZSupAlg.cpp,v 1.15 2009-10-12 16:03:54 odescham Exp $

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
  declareProperty("ZsupNeighbor"    , m_zsupNeighbor=-256  ) ;
  declareProperty("OutputType"      , m_outputType = "Digits"  ) ;
  declareProperty( "Extension"      ,  m_extension = "" );
  declareProperty( "StatusOnTES"    , m_statusOnTES = true);



  m_digitOnTES =false  ;
  m_adcOnTES =false  ;
  
    


  //=== Default values according to the name of the algorithm !
  m_inputToolType = "CaloEnergyFromRaw";
  m_inputToolName = name + "Tool";
  if ( "Ecal" == name.substr( 0 , 4 ) ) {
    m_detectorName     = DeCalorimeterLocation::Ecal;
    m_outputADCData    = LHCb::CaloAdcLocation::Ecal   + m_extension;
    m_outputDigitData  = LHCb::CaloDigitLocation::Ecal + m_extension;
    m_zsupMethod       = "2D";
    m_zsupThreshold    = 20;
    m_zsupNeighbor     = -5; // reject large negative noise
  } else if ( "Hcal" == name.substr( 0 , 4 ) ) {
    m_detectorName     = DeCalorimeterLocation::Hcal;
    m_outputADCData    = LHCb::CaloAdcLocation::Hcal   + m_extension;
    m_outputDigitData  = LHCb::CaloDigitLocation::Hcal + m_extension;
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


  std::string out( m_outputType );
  std::transform( m_outputType.begin() , m_outputType.end() , out.begin () , ::toupper ) ;
  m_outputType = out;
  if( m_outputType == "DIGITS" ||  m_outputType == "CALODIGITS" || 
      m_outputType == "DIGIT"  ||  m_outputType == "CALODIGIT"  || 
      m_outputType == "BOTH") m_digitOnTES = true;
  if(m_outputType == "ADCS" ||  m_outputType == "CALOADCS" || 
     m_outputType == "ADC"  ||  m_outputType == "CALOADC"  || 
     m_outputType == "BOTH")m_adcOnTES = true;
  if( !m_adcOnTES && !m_digitOnTES ){
    error() << "CaloZSupAlg configured to produce ** NO ** output (outputType = '" << m_outputType <<"')" << endmsg;
    return StatusCode::FAILURE;
  }  
  if( m_digitOnTES )debug() <<  "CaloZSupAlg will produce CaloDigits on TES" 
                            << rootInTES() + m_outputDigitData
                            << endmsg;
  if( m_adcOnTES )debug() <<  "CaloZSupAlg will produce CaloAdcs on TES" 
                          << rootInTES() + m_outputADCData 
                          << endmsg;
  
  // Retrieve the calorimeter we are working with.
  debug() << " get DeCalorimeter from " << m_detectorName << endmsg;
  m_calo = getDet<DeCalorimeter>( m_detectorName );  
  m_numberOfCells = m_calo->numberOfCells();
  m_pedShift      = m_calo->pedestalShift();
  
  //*** A few check of the parameters
  if ( "NO" != m_zsupMethod && 
       "1D" != m_zsupMethod && 
       "2D" != m_zsupMethod) {
    error() << "Unknown Z-sup mode" << m_zsupMethod
            << " (must be NO,1D or 2D)" << endmsg;
    return StatusCode::FAILURE;
  }

  m_zsup2D = ( "2D" == m_zsupMethod );

  if ( "NO" == m_zsupMethod ) {
    if( -1000 != m_zsupThreshold ) {
      info() << " Threshold is reset from "<< m_zsupThreshold
             << " to " << -1000 << endmsg; }
    m_zsupThreshold = -1000 ; // no threshold at all !
  }

  info() << "Calorimeter has " <<  m_numberOfCells
         << " cells. Zsup method "  << m_zsupMethod 
         << " Threshold " << m_zsupThreshold << endmsg;

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
                                      << rootInTES() + m_outputADCData << endmsg;
  if ( isDebug && m_digitOnTES) debug() << "Perform zero suppression - return CaloDigits on TES at "
                                      << rootInTES() + m_outputDigitData << endmsg;


  //*** get the input data

  const std::vector<LHCb::CaloAdc>& adcs = m_adcTool->adcs( );
  if(m_statusOnTES)m_adcTool->putStatusOnTES();

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
                         << " Digits." << endmsg;

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
      if( isVerbose ) {
        verbose() << id 
                << format( " Energy adc %4d", digAdc );
        if (  m_zsupThreshold <= digAdc ) debug() << " seed";
        verbose() << endmsg;
      }
      
      caloFlags[index] = SeedFlag ;
      if( m_zsup2D ) {
        CaloNeighbors::const_iterator neighbor =  m_calo->neighborCells( id ).begin() ;
        while ( neighbor != m_calo->neighborCells( id ).end() ) {
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
    if( NeighborFlag == caloFlags[index] && (*anAdc).adc() < m_zsupNeighbor)continue;
    
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
        verbose() << id << " added as Neighbor." << endmsg;
      } else {
        verbose() << id << " added as Seed.    " << endmsg;
      }
    }    
  }
  if(m_adcOnTES){
    if(isDebug)debug() << format( "Have stored %5d CaloAdcs.", newAdcs->size() ) 
            << endmsg;
  }
  if(m_digitOnTES){
    if(isDebug)debug() << format( "Have stored %5d CaloDigits.", newDigits->size() ) 
            << endmsg;
  }


  return StatusCode::SUCCESS;
};

//=============================================================================
