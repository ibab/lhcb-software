// $Id: CaloSignalAlg.cpp,v 1.2 2003-11-18 10:20:24 ocallot Exp $
// STL
#include <string>
#include <algorithm>
#include <stdio.h>
/// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
/// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IDataProviderSvc.h"
// Event/LHCbEvent
#include "Event/MCParticle.h"
// Calo/CaloGen
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloException.h"
/// Event/CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/MCCaloHit.h"
/// Det/CaloDet
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CaloSignalAlg.h"

// ============================================================================
/** @file CaloSignalAlgs.cpp
 *
 *  Calorimeter Signal processing: Get MCCaloSignal and produce the deposits
 *  for this beam crossing, which will later be digitized.
 *  Handles (or will in the future...) the spill-over
 *
 *  @author: Olivier Callot
 *  @date:   21 February 2001
 */
// ============================================================================

/// MANDATORY!!!
static const AlgFactory<CaloSignalAlg>          Factory ;
const       IAlgFactory& CaloSignalAlgFactory = Factory ;


//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloSignalAlg::CaloSignalAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : CaloAlgorithm ( name , pSvcLocator ) 
{
  //** Declare the algorithm's properties which can be set at run time and 
  //** default values
  declareProperty( "PreviousData"      , m_previousData       = ""   ) ;
  declareProperty( "PreviousDigits"    , m_previousDigits     = ""   ) ;
  declareProperty( "MinimalDeposit"    , m_minimalDeposit     = 0.1  ) ;
  declareProperty( "BackgroundScaling" , m_backgroundScaling  = 0.25 ) ;
  declareProperty( "IgnoreTimeInfo"    , m_ignoreTimeInfo     = false) ;
  declareProperty( "BackgroundData"    , m_backgroundData     = ""   ) ;

  if ( "SpdSignal" == name ) {
    setDetData(    "/dd/Structure/LHCb/Spd" );
    setInputData(  MCCaloHitLocation::Spd );
    setOutputData( MCCaloDigitLocation::Spd );
    m_previousData    = "Prev/" + MCCaloHitLocation::Spd;
    m_previousDigits  = "Prev/" + MCCaloDigitLocation::Spd;
    m_minimalDeposit  = 0.0;   //== Full history
    m_backgroundData  = "LHCBackground/" + MCCaloHitLocation::Spd;
  } else if ( "PrsSignal" == name ) {
    setDetData(    "/dd/Structure/LHCb/Prs" );
    setInputData(  MCCaloHitLocation::Prs );
    setOutputData( MCCaloDigitLocation::Prs );
    m_previousData    = "Prev/" + MCCaloHitLocation::Prs;
    m_previousDigits  = "Prev/" + MCCaloDigitLocation::Prs;
    m_backgroundData  = "LHCBackground/" + MCCaloHitLocation::Prs;
  } else if ( "EcalSignal" == name ) {
    setDetData(    "/dd/Structure/LHCb/Ecal" );
    setInputData(  MCCaloHitLocation::Ecal );
    setOutputData( MCCaloDigitLocation::Ecal );
    m_previousData    = "Prev/" + MCCaloHitLocation::Ecal;
    m_backgroundData  = "LHCBackground/" + MCCaloHitLocation::Ecal;
  } else if ( "HcalSignal" == name ) {
    setDetData(    "/dd/Structure/LHCb/Hcal" );
    setInputData(  MCCaloHitLocation::Hcal );
    setOutputData( MCCaloDigitLocation::Hcal );
    m_previousData    = "Prev/" + MCCaloHitLocation::Hcal;
    m_backgroundData  = "LHCBackground/" + MCCaloHitLocation::Hcal;
  }

 };

//=============================================================================
// Standard destructor
//=============================================================================
CaloSignalAlg::~CaloSignalAlg() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloSignalAlg::initialize() {
  
  MsgStream msg(msgSvc(), name());
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) { 
    return Error("Can not initialize the base class  CaloAlgorithm!", sc); 
  }
  
  
  // Retrieve the calorimeter we are working with.
  m_calo = get( detSvc() , detData() , (DeCalorimeter*) 0 );
  if( 0 == m_calo ) { return StatusCode::FAILURE ; }
  
  // Initialize the random number service
  
  m_rndmSvc = 0;
  StatusCode status = service("RndmGenSvc", m_rndmSvc , true  ) ;
  
  if ( status.isFailure() ) {
    return Error( "Can not initialize the RndmGenSvc", status ) ;
  }
  if ( 0 == m_rndmSvc     ) {
    return Error( "Can not initialize the RndmGenSvc"        ) ;
  }

  m_storePrevious  = ( "" != m_previousDigits ) ;
  m_minimalDeposit = m_minimalDeposit / m_calo->activeToTotal() ;

  msg << MSG::INFO << "Initialised. ";
  if ( m_storePrevious )  msg << "Store also previous BX. ";
  if ( m_ignoreTimeInfo ) msg << "Ignore time information. ";
  msg << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================

StatusCode CaloSignalAlg::execute() {

  MsgStream  msg( msgSvc(), name() );
  bool verbose = ( MSG::VERBOSE >= msg.level() );

  // some trivial printout
  msg << MSG::DEBUG     << "Perform signal processing from "
      << inputData  ()  << " to "
      << outputData ()  << endreq;
  
  // prepare the output container
  MCCaloDigits* mcDigits = new MCCaloDigits();
  StatusCode sc = put( mcDigits , outputData() );
  if( sc.isFailure() ) { return sc ; }

  MCCaloDigits* mcPrevDigits = 0;
  if ( m_storePrevious ) {
    mcPrevDigits  = new MCCaloDigits();
    StatusCode sc = put( mcPrevDigits , m_previousDigits );
    if( sc.isFailure() ) { return sc ; }
  }
  
  // get the input data
  MCCaloHits* hits = get( eventSvc() , inputData() , hits );
  if( 0 == hits      ) { return StatusCode::FAILURE ; }

  // initialize the background random number
  Rndm::Numbers BackgroundFraction ( m_rndmSvc , Rndm::Flat( 0.0 , 1. ) );
  
  //== Vector with direct access via CellId.
  CaloVector<MCCaloDigit*> mcDigitPtr( 0 );
  CaloVector<MCCaloDigit*> mcPrevDigitPtr( 0 );
  CaloVector<MCCaloDigit*>::iterator vi;

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

  MCCaloHits::const_iterator hitIt;
  MCCaloDigit* myDig;
  MCCaloHit* hit;
  int    timeBin;
  double storedE;
  int    storeType;

  for( hitIt = hits->begin(); hits->end() != hitIt ; ++hitIt ) {
    hit = *hitIt;
    CaloCellID id = hit->cellID() ;
    if ( !m_calo->valid( id ) ) { continue; }

    // Set the timeBin (careful of bad rounding), If in time, store.
    // if late, get a selected fraction of it, and assign it at a
    // symmetric location (-x,-y) to decorelate from the current tracks.
    
    storeType = 0;
    storedE   = hit->activeE();
    timeBin   = int( floor( hit->time() + .5 ) );//== Now in time bin number...

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
      if ( validBX[ timeBin%validBX.size() ] ) {
        HepPoint3D center = m_calo->cellCenter( id );
        center.setX( -center.x() );
        center.setY( -center.y() );
        CaloCellID id = m_calo->Cell( center );
        storeType = 3;
      }
    }
    //== Need to store ?
    if ( 0 != storeType ) {
      myDig = mcDigitPtr[id];
      if ( 0 == myDig ) {
        myDig = new MCCaloDigit( );
        myDig->setCellID( id );
        mcDigitPtr.addEntry( myDig, id );
      }
      myDig->addActiveE( storedE );
      if ( 1 == storeType ) myDig->addToHits( *hitIt );

      if ( verbose ) {
        std::string text;
        if ( 1 == storeType ) text = " stored";
        if ( 2 == storeType ) text = "  added";
        if ( 3 == storeType ) text = "    bkg";
        MCParticle* part = hit->particle();
        msg << id << text << format( 
    " %8.2f MeV, timeBin=%8d, MCPart %4d ID%8d (e= %9.3f GeV)",
    storedE/MeV, timeBin, (int) part->index(), part->particleID().pid(),
    part->momentum().e()/GeV ) 
            << endreq;
      }
    } // store ?
  } // hit loop

  //--------------------------------
  // == Process the spill-over data, but only if container specified
  //--------------------------------
  if( !m_previousData.empty() ) {
    SmartDataPtr<MCCaloHits> spill( eventSvc(), m_previousData );
    MCCaloHits* spillOver = spill;
    if ( 0 != spillOver ) {
      for( hitIt = spillOver->begin(); spillOver->end() != hitIt ; ++hitIt ) {
        hit = *hitIt;
        const CaloCellID id = hit->cellID() ;
        if ( ! m_calo->valid( id ) ) { continue; }
        
        storeType = 0;
        storedE   = hit->activeE();
        timeBin   = int( floor( hit->time() + .5 ) );//== time bin number...

        if ( m_ignoreTimeInfo ) timeBin = 0;

        if ( 1 == timeBin ) {
          storeType = 4;
          myDig = mcDigitPtr[id];
          if ( 0 == myDig ) {
            myDig = new MCCaloDigit( );
            myDig->setCellID( id );
            mcDigitPtr.addEntry( myDig, id );
          }
        } else if ( m_storePrevious && 0 == timeBin ) {
          myDig = mcPrevDigitPtr[id];
          if ( 0 == myDig ) {
            myDig = new MCCaloDigit( );
            myDig->setCellID( id );
            mcPrevDigitPtr.addEntry( myDig, id );
          }
          storeType = 5;
        }
        
        if ( 0 != storeType ) {
          myDig->addActiveE( storedE );
          if ( verbose ) {
            std::string text;
            if ( 4 == storeType ) text = "  spill";
            if ( 5 == storeType ) text = " PREV  ";
            
            MCParticle* part = hit->particle();
            msg << id << text << format(
  " %8.2f MeV, timeBin=%8d, MCPart %4d ID%8d (e= %9.3f GeV)",
  storedE/MeV, timeBin, (int) part->index(),  part->particleID().pid(),
  part->momentum().e()/GeV ) 
                << endreq;
          }
        }
      }
    }
  }
  //--------------------------------
  // == Process the LHC background data 
  //--------------------------------
  if( !m_backgroundData.empty() ) {
    SmartDataPtr<MCCaloHits> lhc( eventSvc(), m_backgroundData );
    MCCaloHits* lhcBackground = lhc;
    if ( 0 != lhcBackground ) {
      for( hitIt = lhcBackground->begin(); lhcBackground->end() != hitIt ; 
           ++hitIt ) {
        hit = *hitIt;
        const CaloCellID id = hit->cellID() ;
        if ( ! m_calo->valid( id ) ) { continue; }
        
        storeType = 0;
        storedE   = hit->activeE();
        timeBin   = int( floor( hit->time() + .5 ) );//== time bin number...

        if ( m_ignoreTimeInfo ) timeBin = 0;

        if ( 0 == timeBin ) {
          myDig = mcDigitPtr[id];
          if ( 0 == myDig ) {
            myDig = new MCCaloDigit( );
            myDig->setCellID( id );
            mcDigitPtr.addEntry( myDig, id );
          }

          myDig->addActiveE( storedE );
          if ( verbose ) {
            MCParticle* part = hit->particle();
            msg << MSG::INFO << id << " LHCbk " << format(
  " %8.2f MeV, timeBin=%8d, MCPart %4d ID%8d (e= %9.3f GeV) zv%9.1f rv%9.1f",
  storedE/MeV, timeBin, (int) part->index(), part->particleID().pid(),
  part->momentum().e()/GeV, part->originVertex()->position().z(),
  part->originVertex()->position().perp() ) 
                << endreq;
          }  
        }
      }
    }
  }
  
  /// copy data into output container 
  for( vi = mcDigitPtr.begin(); mcDigitPtr.end() != vi; ++vi ){
    MCCaloDigit* digit = *vi ;
    if( 0 == digit ) { continue ; }
    mcDigits->insert( digit );
  }

  msg << MSG::DEBUG << "-- Stored " << mcDigits->size() << " MCDigits";

  if ( m_storePrevious ) {
    for( vi = mcPrevDigitPtr.begin(); mcPrevDigitPtr.end() != vi; ++vi ){
      MCCaloDigit* digit = *vi ;
      if( 0 == digit ) { continue ; }
      mcPrevDigits->insert( digit );
    }
    msg << " (plus " << mcPrevDigits->size() << " in Prev container)";
  }
  msg << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloSignalAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;
  if( 0 != m_rndmSvc ) { m_rndmSvc->release() ; m_rndmSvc = 0 ; }
  
  return CaloAlgorithm::finalize();
};

// ============================================================================
// The End 
// ============================================================================
