// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

#include "Event/MCParticle.h"

// Calo/CaloGen
#include "Kernel/CaloCellID.h"
#include "CaloKernel/CaloVector.h"

// Event/CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/MCCaloHit.h"

#include <boost/utility/string_ref.hpp>

// local
#include "CaloSignalAlg.h"

// ============================================================================
/** @file CaloSignalAlg.cpp
 *
 *  Calorimeter Signal processing: Get MCCaloSignal and produce the deposits
 *  for this beam crossing, which will later be digitized.
 *  Handles the spill-over
 *
 *  @author: Olivier Callot
 *  @date:   21 February 2001
 */
// ============================================================================

namespace {
       int getGlobalTimeOffset( boost::string_ref rootInTES ) {
           if ( rootInTES.starts_with("Prev") && rootInTES.ends_with("/") ) {
              rootInTES.remove_prefix(4);
              rootInTES.remove_suffix(1);
              return - std::stoi( rootInTES.data() );
           }
           if ( rootInTES.starts_with("Next") && rootInTES.ends_with("/") ) {
              rootInTES.remove_prefix(4);
              rootInTES.remove_suffix(1);
              return std::stoi( rootInTES.data() );
           }
           return 0;
       }
}

DECLARE_ALGORITHM_FACTORY( CaloSignalAlg )

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloSignalAlg::CaloSignalAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
  , m_storePrevious(false)
  , m_calo(NULL)
  , m_rndmSvc(NULL) 
{
  //** Declare the algorithm's properties which can be set at run time and 
  //** default values
  declareProperty( "PreviousData"      , m_previousData       = ""   ) ;
  declareProperty( "PreviousDigits"    , m_previousDigits     = ""   ) ;
  declareProperty( "MinimalDeposit"    , m_minimalDeposit     = 0.1  ) ;
  declareProperty( "BackgroundScaling" , m_backgroundScaling  = 0.25 ) ;
  declareProperty( "IgnoreTimeInfo"    , m_ignoreTimeInfo     = false) ;

  std::string begName = name.substr( 0, 8 );
  bool normal =  "TAE" != context() && "" == rootInTES();
  if ( !normal ) m_backgroundScaling = 0.;  // no bkg in TAE events

  if ( "SpdSigna" == begName ) {
    m_detectorName   = DeCalorimeterLocation::Spd;
    m_inputData      = LHCb::MCCaloHitLocation::Spd ;
    m_outputData     = LHCb::MCCaloDigitLocation::Spd ;
    if ( normal ) m_previousDigits = "Prev/" + LHCb::MCCaloDigitLocation::Spd;
    m_minimalDeposit = 0.0;   //== Full history
  } else if ( "PrsSigna" == begName ) {
    m_detectorName   = DeCalorimeterLocation::Prs;
    m_inputData      = LHCb::MCCaloHitLocation::Prs ;
    m_outputData     = LHCb::MCCaloDigitLocation::Prs ;
    if ( normal ) m_previousDigits = "Prev/" + LHCb::MCCaloDigitLocation::Prs;
  } else if ( "EcalSign" == begName ) {
    m_detectorName   = DeCalorimeterLocation::Ecal;
    m_inputData      = LHCb::MCCaloHitLocation::Ecal ;
    m_outputData     = LHCb::MCCaloDigitLocation::Ecal ;
  } else if ( "HcalSign" == begName ) {
    m_detectorName   = DeCalorimeterLocation::Hcal;
    m_inputData      = LHCb::MCCaloHitLocation::Hcal ;
    m_outputData     = LHCb::MCCaloDigitLocation::Hcal ;
  }
  //== This is needed only for normal processing.
  if ( normal ) m_previousData   = "Prev/" + m_inputData;
 }

//=============================================================================
// Standard destructor
//=============================================================================
CaloSignalAlg::~CaloSignalAlg() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloSignalAlg::initialize() {
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) { return sc; }
  
  // Retrieve the calorimeter we are working with.
  m_calo = getDet<DeCalorimeter>( m_detectorName );
  
  // Initialize the random number service
  
  m_rndmSvc = svc<IRndmGenSvc>("RndmGenSvc", true  );

  m_storePrevious  = ( "" != m_previousDigits ) ;
  m_minimalDeposit = m_minimalDeposit / m_calo->activeToTotal() ;

  info() << "Initialised. ";
  if ( m_storePrevious )  info() << "Store also previous BX. ";
  if ( m_ignoreTimeInfo ) info() << "Ignore time information. ";
  info() << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode CaloSignalAlg::execute() {

  bool isVerbose = ( MSG::VERBOSE >= msgLevel() );

  // some trivial printout
  if(msgLevel(MSG::DEBUG)) debug() << "Perform signal processing from "
                                   << m_inputData << " to " << rootInTES()+ m_outputData << endmsg;
  
  // prepare the output container
  LHCb::MCCaloDigits* mcDigits = new LHCb::MCCaloDigits();
  put( mcDigits , m_outputData );

  LHCb::MCCaloDigits* mcPrevDigits = 0;
  if ( m_storePrevious ) {
    mcPrevDigits  = new LHCb::MCCaloDigits();
    put( mcPrevDigits , m_previousDigits, IgnoreRootInTES );
  }
  
  // get the input data
  if(!exist<LHCb::MCCaloHits>( m_inputData, IgnoreRootInTES ))
    return StatusCode::SUCCESS;
  LHCb::MCCaloHits* hits = get<LHCb::MCCaloHits>( m_inputData, IgnoreRootInTES );

  // initialize the background random number
  Rndm::Numbers BackgroundFraction ( m_rndmSvc , Rndm::Flat( 0.0 , 1. ) );
  
  //== Vector with direct access via CellId.
  CaloVector<LHCb::MCCaloDigit*> mcDigitPtr( 0 );
  CaloVector<LHCb::MCCaloDigit*> mcPrevDigitPtr( 0 );
  CaloVector<LHCb::MCCaloDigit*>::iterator vi;

  //== Vector to decide if an old BX can contribute.
  std::vector<bool> validBX;
  for ( unsigned int kk=0 ; 50 > kk; kk++ ) {
    if ( m_backgroundScaling > BackgroundFraction() ) {
      validBX.push_back( true );
    } else {
      validBX.push_back( false );
    } 
  }

  //--------------------------------
  // Process the input container.
  //--------------------------------

  LHCb::MCCaloHits::const_iterator hitIt;
  LHCb::MCCaloDigit* myDig;
  LHCb::MCCaloHit* hit;
  int    timeBin;
  double storedE;
  int    storeType;

  int globalTimeOffset = getGlobalTimeOffset( rootInTES() ) ; // in time bin number
  for( hitIt = hits->begin(); hits->end() != hitIt ; ++hitIt ) {
    hit = *hitIt;
    LHCb::CaloCellID id = hit->cellID() ;
    if ( !m_calo->valid( id ) ) continue; 

    // Set the timeBin (careful of bad rounding), If in time, store.
    // if late, get a selected fraction of it, and assign it at a
    // symmetric location (-x,-y) to decorelate from the current tracks.
    
    storeType = 0;
    storedE   = hit->activeE();
    timeBin   = int( floor( hit->time() + .5 + globalTimeOffset ) );//== Now in time bin number...

    if ( m_ignoreTimeInfo ) timeBin = 0;
      
    if ( 0 == timeBin ) {
      // Threshold for storing history (ADC count) converted back to activeE. 
      if ( m_minimalDeposit * m_calo->cellGain( id ) <= storedE ) {
        storeType = 1;
      } else {
        storeType = 2;
      }
    } else if ( 2 <= timeBin ) {
      //== Keep the contribution of old BX, according to the probability.
      if ( validBX[  timeBin%validBX.size() ] ) {
        Gaudi::XYZPoint center = m_calo->cellCenter( id );
        Gaudi::XYZPoint newCenter( -center.x(), -center.y(), center.z() );
        LHCb::CaloCellID id = m_calo->Cell( newCenter );
        storeType = 3;
      }
    }
    //== Need to store ?
    if ( 0 != storeType ) {
      myDig = mcDigitPtr[id];
      if ( 0 == myDig ) {
        myDig = new LHCb::MCCaloDigit( );
        myDig->setCellID( id );
        mcDigitPtr.addEntry( myDig, id );
      }
      myDig->addActiveE( storedE );
      if ( 1 == storeType ) myDig->addToHits( *hitIt );

      if ( isVerbose ) {
        std::string text;
        if ( 1 == storeType ) text = " stored";
        if ( 2 == storeType ) text = "  added";
        if ( 3 == storeType ) text = "    bkg";
        const LHCb::MCParticle* part = hit->particle();
        verbose() << id << text 
                  << format( " %8.2f MeV, time %2d Bin=%2d, MCPart %4d ID%8d (e= %9.3f GeV)",
                             storedE/Gaudi::Units::MeV, hit->time(), timeBin, 
                             (int) part->index(), part->particleID().pid(),
                             part->momentum().e()/Gaudi::Units::GeV ) 
                  << endmsg;
      }
    } // store ?
  } // hit loop

  //--------------------------------
  // == Process the spill-over data, but only if container specified
  //--------------------------------
  if( !m_previousData.empty() ) {
    if ( exist<LHCb::MCCaloHits>( m_previousData, IgnoreRootInTES ) ) {
      LHCb::MCCaloHits* spillOver = get<LHCb::MCCaloHits>( m_previousData, IgnoreRootInTES );
      for( hitIt = spillOver->begin(); spillOver->end() != hitIt ; ++hitIt ) {
        hit = *hitIt;
        const LHCb::CaloCellID id = hit->cellID() ;
        if ( ! m_calo->valid( id ) ) { continue; }
        
        storeType = 0;
        storedE   = hit->activeE();
        timeBin   = int( floor( hit->time() + .5 + globalTimeOffset ) );//== time bin number...
        if ( m_ignoreTimeInfo ) timeBin = 0;

        if ( 1 == timeBin ) {
          storeType = 4;
          myDig = mcDigitPtr[id];
          if ( 0 == myDig ) {
            myDig = new LHCb::MCCaloDigit( );
            myDig->setCellID( id );
            mcDigitPtr.addEntry( myDig, id );
          }
        } else if ( m_storePrevious && 0 == timeBin ) {
          myDig = mcPrevDigitPtr[id];
          if ( 0 == myDig ) {
            myDig = new LHCb::MCCaloDigit( );
            myDig->setCellID( id );
            mcPrevDigitPtr.addEntry( myDig, id );
          }
          storeType = 5;
        }
        
        if ( 0 != storeType ) {
          myDig->addActiveE( storedE );
          if ( isVerbose ) {
            std::string text;
            if ( 4 == storeType ) text = "  spill";
            if ( 5 == storeType ) text = " PREV  ";
            
            const LHCb::MCParticle* part = hit->particle();
            verbose() << id << text 
                      << format( " %8.2f MeV, time %2d Bin=%3d, MCPart %4d ID%8d (e= %9.3f GeV)",
                                 storedE/Gaudi::Units::MeV, hit->time(), timeBin,
                                 (int) part->index(), part->particleID().pid(),
                                 part->momentum().e()/Gaudi::Units::GeV ) 
                      << endmsg;
          }
        }
      }
    }
  }

  /// copy data into output container 

  for( vi = mcDigitPtr.begin(); mcDigitPtr.end() != vi; ++vi ){
    LHCb::MCCaloDigit* digit = *vi ;
    if( 0 != digit ) mcDigits->insert( digit );
  }

  if(msgLevel(MSG::DEBUG)) debug() << "-- Stored " << mcDigits->size() << " MCDigits";

  if ( m_storePrevious ) {
    for( vi = mcPrevDigitPtr.begin(); mcPrevDigitPtr.end() != vi; ++vi ){
      LHCb::MCCaloDigit* digit = *vi ;
      if( 0 != digit )  mcPrevDigits->insert( digit );
    }
    if(msgLevel(MSG::DEBUG)) debug() << " (plus " << mcPrevDigits->size() << " in Prev container)";
  }
  if(msgLevel(MSG::DEBUG)) debug() << endmsg;
  
  return StatusCode::SUCCESS;
}

// ============================================================================
