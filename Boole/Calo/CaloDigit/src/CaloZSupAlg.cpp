// $Id: CaloZSupAlg.cpp,v 1.10 2005-01-14 15:45:51 cattanem Exp $

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// Event 
#include "Event/MCTruth.h"

// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"

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
  , m_zsupMethod        ( "1D"  )
  , m_zsup2D            ( false )
  , m_zsupThreshold     ( 6     )
  , m_triggerEtScale    ( 20 * MeV )

{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("DetectorName"    , m_detectorName    ) ;
  declareProperty("InputData"       , m_inputData       ) ;
  declareProperty("OutputData"      , m_outputData      ) ;
  declareProperty("InputMCData"     , m_inputMCData     ) ;
  declareProperty("ZsupMethod"      , m_zsupMethod      ) ;
  declareProperty("ZsupThreshold"   , m_zsupThreshold   ) ;
  declareProperty("BankType"        , m_bankType        ) ;

  //=== Default values according to the name of the algorithm !
  if ( "SpdZSup" == name ) {
    m_detectorName   = "/dd/Structure/LHCb/Spd";
    m_inputData      =  CaloDigitLocation::FullSpd;
    m_outputData     = CaloDigitLocation::Spd;
    m_inputMCData    = MCCaloDigitLocation::Spd;
    m_zsupThreshold  = 1;
    m_triggerName    = L0PrsSpdHitLocation::Spd;
    m_triggerThreshold = 0.1 * MeV;
    m_triggerIsBit   = true;
  } else if ( "PrsZSup" == name ) {
    m_detectorName     = "/dd/Structure/LHCb/Prs";
    m_inputData        = CaloDigitLocation::FullPrs;
    m_outputData       = CaloDigitLocation::Prs;
    m_inputMCData      = MCCaloDigitLocation::Prs;
    m_zsupThreshold    = 15;
    m_triggerName      = L0PrsSpdHitLocation::Prs;
    m_triggerIsBit   = true;
  } else if ( "EcalZSup" == name ) {
    m_detectorName     = "/dd/Structure/LHCb/Ecal";
    m_inputData        = CaloDigitLocation::FullEcal;
    m_outputData       = CaloDigitLocation::Ecal;
    m_inputMCData      = MCCaloDigitLocation::Ecal;
    m_zsupMethod       = "2D";
    m_zsupThreshold    = 20;
    m_triggerThreshold = 0.;
    m_triggerName      = L0CaloAdcLocation::Ecal;
    m_triggerIsBit   = false;

    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.04 );
    m_corrArea.push_back( 1.08 );
    
  } else if ( "HcalZSup" == name ) {
    m_detectorName     = "/dd/Structure/LHCb/Hcal";
    m_inputData        = CaloDigitLocation::FullHcal;
    m_outputData       = CaloDigitLocation::Hcal;
    m_inputMCData      = MCCaloDigitLocation::Hcal;
    m_zsupThreshold    = 4;
    m_triggerThreshold = 0.;
    m_triggerName      = L0CaloAdcLocation::Hcal;
    m_triggerIsBit   = false;

    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.05 );
    
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
         << " Threshold " << m_zsupThreshold
         << " threshold " << m_triggerThreshold;
  if ( 0 < m_corrArea.size() ) {
    info() << " Correction factor: ";
    for ( unsigned int kk = 0 ; m_corrArea.size() > kk ; kk++ ) {
      info() << m_corrArea[kk] << " ";
    }
  }
  info() << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloZSupAlg::execute() {

  bool isDebug   = msgLevel() <= MSG::DEBUG;
  bool isVerbose = msgLevel() <= MSG::VERBOSE;
  
  //*** some trivial printout

  if ( isDebug ) debug() << "Perform digitization from "
                         << m_inputData << " to " << m_outputData << endreq;

  //*** get the input data

  CaloDigits* allDigits = get<CaloDigits>( m_inputData );
  
  //***  prepare the output container

  CaloDigits* digits = new CaloDigits();
  StatusCode sc = put( digits, m_outputData );
  if( sc.isFailure() ) { return sc ; } 
  L0CaloAdcs* trigBank = NULL;
  L0PrsSpdHits* bitsBank = NULL;
  if ( "" != m_triggerName ) {
    if ( m_triggerIsBit ) {
      bitsBank = new L0PrsSpdHits();
      sc = put( bitsBank, m_triggerName );
      if( sc.isFailure() ) { return sc ; } 
    } else {
      trigBank = new L0CaloAdcs();
      sc = put( trigBank, m_triggerName );
      if( sc.isFailure() ) { return sc ; } 
    }
  }

  /// set MCtruth for the container!

  MCCaloDigits* mcd = get<MCCaloDigits>( m_inputMCData );
  sc = setMCTruth( digits , mcd );
  if( sc.isFailure() )  {
    error() << "Could not set MCTruth information!" << endreq ;
    return sc ;
  }

  if ( isDebug ) debug() << "Processing " << allDigits->size() 
                         << " Digits." << endreq;

  enum {
    DefaultFlag   ,
    NeighborFlag  ,
    SeedFlag       };

  CaloDigits::const_iterator aDig;
  std::vector<int> caloFlags    ( m_numberOfCells, DefaultFlag ) ;

  int index;
  
  // == Apply the threshold. If 2DZsup, tag also the neighbours

  for( aDig = allDigits->begin(); allDigits->end() != aDig ; ++aDig ) {
    if( 0 != *aDig ) {
      CaloCellID id = (*aDig)->cellID();
      index         = m_calo->cellIndex( id );
      double gain   = m_calo->cellGain( id ) ;
      double energy = (*aDig)->e();
      int    digAdc = int ( floor( energy/gain + .9) );
      int trigVal   = 0;
      if ( m_triggerIsBit ) {
        if ( m_triggerThreshold < energy ) {
          L0PrsSpdHit* myHit = new L0PrsSpdHit( id );
          bitsBank->add( myHit );
        }
      } else {
        energy = energy * m_corrArea[ id.area() ] * m_calo->cellSine( id );
        trigVal = (int)floor( energy / m_triggerEtScale + .5 );
        if ( 255 < trigVal ) trigVal = 255;
        if ( 0   > trigVal ) trigVal = 0;
        if ( 0 < trigVal ) {
          L0CaloAdc* trigAdc = new L0CaloAdc( id, trigVal );
          trigBank->insert( trigAdc );
        }
      }
      
      if( isDebug ) {
        if( m_zsupThreshold <= digAdc || 0 !=  trigVal) {
          debug() << id 
                  << format( " Energy %10.2f MeV gain %6.2f adc %4d Trig %3d",
                             (*aDig)->e()/MeV, gain, digAdc, trigVal );
          if (  m_zsupThreshold <= digAdc ) debug() << " seed";
          debug() << endreq;
        }
      }
      
      if( m_zsupThreshold <= digAdc ) {
        caloFlags[index] = SeedFlag ;
        if( m_zsup2D ) {
          CaloNeighbors::const_iterator neighbor =
            m_calo->zsupNeighborCells( id ).begin() ;
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
  }

  //** write tagged data as CaloDigit
  
  for( aDig = allDigits->begin(); allDigits->end() != aDig ; ++aDig ) {
    CaloCellID id = (*aDig)->cellID();
    index         = m_calo->cellIndex( id );
    if( DefaultFlag == caloFlags[index] ) { continue; }

    CaloDigit* digit = new CaloDigit( id, (*aDig)->e() );
    digits->add( digit ) ;
    
    if( isVerbose ) {
      if ( NeighborFlag == caloFlags[index] ) {
        verbose() << id << " added as Neighbor.";
      } else {
        verbose() << id << " added as Seed.    ";
      }
      verbose() << format( " Energy %9.2f MeV", (*aDig)->e()/MeV ) << endreq;
    }    
  }

  debug() << format( "Have stored %5d digits.", digits->size() ) 
          << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
