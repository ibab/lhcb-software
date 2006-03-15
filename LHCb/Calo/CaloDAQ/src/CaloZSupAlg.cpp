// $Id: CaloZSupAlg.cpp,v 1.1 2006-03-15 12:51:25 odescham Exp $

// LHCbDefinitions
#include "Kernel/SystemOfUnits.h"
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

// MANDATORY!!!
static const AlgFactory<CaloZSupAlg>          Factory ;
const       IAlgFactory& CaloZSupAlgFactory = Factory ;


//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloZSupAlg::CaloZSupAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm       ( name , pSvcLocator            )
{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("DetectorName"    , m_detectorName    ) ;
  declareProperty("OutputData"      , m_outputData      ) ;
  declareProperty("ZsupMethod"      , m_zsupMethod      ) ;
  declareProperty("ZsupThreshold"   , m_zsupThreshold   ) ;

  //=== Default values according to the name of the algorithm !
  if ( "EcalZSup" == name ) {
    m_detectorName     = DeCalorimeterLocation::Ecal;
    m_outputData       = LHCb::CaloAdcLocation::Ecal;
    m_zsupMethod       = "2D";
    m_zsupThreshold    = 20;
    m_inputToolName    = "CaloEnergyFromRaw/EcalEnergyFromRaw";
  } else if ( "HcalZSup" == name ) {
    m_detectorName     = DeCalorimeterLocation::Hcal;
    m_outputData       = LHCb::CaloAdcLocation::Hcal;
    m_zsupMethod       = "1D";
    m_zsupThreshold    = 4;
    m_inputToolName    = "CaloEnergyFromRaw/HcalEnergyFromRaw";
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
  
  // Retrieve the calorimeter we are working with.
  m_calo = getDet<DeCalorimeter>( m_detectorName );
  m_numberOfCells = m_calo->numberOfCells();

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

  m_adcTool = tool<ICaloEnergyFromRaw>( m_inputToolName );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloZSupAlg::execute() {

  bool isDebug   = msgLevel() <= MSG::DEBUG;
  bool isVerbose = msgLevel() <= MSG::VERBOSE;
  
  //*** some trivial printout

  if ( isDebug ) debug() << "Perform zero suppression to "
                         << m_outputData << endreq;

  //*** get the input data

  std::vector<LHCb::CaloAdc>& adcs = m_adcTool->adcs( );

  //***  prepare the output container

  LHCb::CaloAdcs* newAdcs = new LHCb::CaloAdcs();
  put( newAdcs, m_outputData );
  
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

  //** write tagged data as CaloAdc
  
  for( anAdc = adcs.begin(); adcs.end() != anAdc ; ++anAdc ) {
    LHCb::CaloCellID id = (*anAdc).cellID();
    index         = m_calo->cellIndex( id );
    if( DefaultFlag == caloFlags[index] ) { continue; }

    LHCb::CaloAdc* adc = new LHCb::CaloAdc( id, (*anAdc).adc() );
    newAdcs->insert( adc ) ;
    
    if( isVerbose ) {
      if ( NeighborFlag == caloFlags[index] ) {
        verbose() << id << " added as Neighbor." << endreq;
      } else {
        verbose() << id << " added as Seed.    " << endreq;
      }
    }    
  }

  debug() << format( "Have stored %5d CaloAdcs.", newAdcs->size() ) 
          << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
