// $Id: CaloZSupAlg.cpp,v 1.2 2002-11-18 12:21:29 ocallot Exp $
// STL
#include <string>
#include <stdio.h>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/ObjectVector.h"
// Event 
#include "Event/MCTruth.h"
// Calo/CaloKernel
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloException.h"
// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloZSupAlg.h"

// ============================================================================
/** @file CaloZSupAlg.cpp
 *
 *    Calorimeter Zero suppression.
 *
 *    @author: Olivier Callot
 *    @Date:   10 October 2002
 */
// ============================================================================

// MANDATORY!!!
static const AlgFactory<CaloZSupAlg>          Factory ;
const       IAlgFactory& CaloZSupAlgFactory = Factory ;


//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloZSupAlg::CaloZSupAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : CaloAlgorithm       ( name , pSvcLocator            )
  , m_tagData           ( ""    )
  , m_zsupMethod        ( "1D"  )
  , m_zsup2D            ( false )
  , m_zsupThreshold     ( 6     )

{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("InputMCData"     , m_inputMCData     ) ;
  declareProperty("TagData"         , m_tagData         ) ;
  declareProperty("ZsupMethod"      , m_zsupMethod      ) ;
  declareProperty("ZsupThreshold"   , m_zsupThreshold   ) ;

  //=== Default values according to the name of the algorithm !
  if ( "SpdZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Spd"   );
    setInputData(      CaloDigitLocation::FullSpd );
    setOutputData(     CaloDigitLocation::Spd     );
    m_inputMCData    = MCCaloDigitLocation::Spd;
    m_zsupThreshold  = 1;
  } else if ( "PrsZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Prs"   );
    setInputData(      CaloDigitLocation::FullPrs );
    setOutputData(     CaloDigitLocation::Prs     );
    m_inputMCData    = MCCaloDigitLocation::Prs;
    m_tagData        = CaloDigitLocation::Spd;
    m_zsupThreshold  = 15;
  } else if ( "EcalZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Ecal"   );
    setInputData(      CaloDigitLocation::FullEcal );
    setOutputData(     CaloDigitLocation::Ecal     );
    m_inputMCData    = MCCaloDigitLocation::Ecal;
    m_zsupMethod     = "2D";
    m_zsupThreshold  = 20;
  } else if ( "HcalZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Hcal"   );
    setInputData(      CaloDigitLocation::FullHcal );
    setOutputData(     CaloDigitLocation::Hcal     );
    m_inputMCData    = MCCaloDigitLocation::Hcal;
    m_zsupThreshold  = 4;
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

  MsgStream msg( msgSvc(), name());

  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) { 
    return Error("Can not initialize the base class  CaloAlgorithm!", sc); 
  } 

  // Retrieve the calorimeter we are working with.
  m_calo = get( detSvc() , detData() , (DeCalorimeter*) 0 );
  m_numberOfCells = m_calo->numberOfCells();

  //*** A few check of the parameters

  if ( "NO" != m_zsupMethod && 
       "1D" != m_zsupMethod && 
       "2D" != m_zsupMethod) {
    msg << MSG::ERROR << "Unknown Z-sup mode" << m_zsupMethod
        << " (must be NO,1D or 2D)" << endreq;
    return StatusCode::FAILURE;
  }

  m_zsup2D = ( "2D" == m_zsupMethod );

  if ( "NO" == m_zsupMethod ) {
    if( -1000 != m_zsupThreshold ) {
      msg << MSG::INFO << " Threshold is reset from "<< m_zsupThreshold
          << " to " << -1000 << endreq; }
    m_zsupThreshold = -1000 ; // no threshold at all !
  }

  msg << MSG::INFO << "Calorimeter has " <<  m_numberOfCells
      << " cells. Zsup method "  << m_zsupMethod 
      << " Threshold " << m_zsupThreshold;
  if ( m_tagData != "" ) { 
    msg << " with tag from " << m_tagData; 
  }
  msg << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloZSupAlg::execute() {

  MsgStream msg(  msgSvc(), name() );

  //*** some trivial printout

  msg << MSG::DEBUG << "Perform digitization from "
      << inputData() << " to " << outputData() << endreq;

  //*** get the input data

  CaloDigits* allDigits = get( eventSvc(), inputData(), allDigits );

  //***  prepare the output container

  CaloDigits* digits = new CaloDigits();

  /// set MCtruth for the container!
  SmartDataPtr<MCCaloDigits> mcdp ( eventSvc(), m_inputMCData );
  MCCaloDigits* mcd = mcdp ;
  StatusCode sc     = setMCTruth( digits , mcd );
  if( sc.isFailure() )  {
    msg << MSG::ERROR << "Could not set MCTruth information!" << endreq ;
    return sc ;
  }

  sc = put( digits, outputData() );
  if( sc.isFailure() ) { return sc ; } 

  msg << MSG::DEBUG << "Processing " << allDigits->size() 
      << " Digits." << endreq;

  enum {
    DefaultFlag   ,
    NeighborFlag  ,
    SeedFlag      ,
    TagFlag        };

  CaloDigits::const_iterator aDig;
  std::vector<int> caloFlags    ( m_numberOfCells, DefaultFlag ) ;
  int index;

  //*** If there is a TagData (Spd data for Preshower Zsup), read this
  //*** container and set the TagFlag for those cells in the TagData 


  if ( m_tagData != "" ) {
    CaloDigits* tags = get( eventSvc() , m_tagData, tags );
    for( aDig = tags->begin() ; tags->end() != aDig; ++aDig ) {
      if( 0 != *aDig ) {
        index = m_calo->cellIndex( (*aDig)->cellID() );
        caloFlags[index] = TagFlag ;
        msg << MSG::DEBUG << "Added tag for " << (*aDig)->cellID() << endreq;
      }
    }
  }

  // == Apply the threshold. If 2DZsup, tag also the neighbours

  for( aDig = allDigits->begin(); allDigits->end() != aDig ; ++aDig ) {
    if( 0 != *aDig ) {
      CaloCellID id = (*aDig)->cellID();
      index         = m_calo->cellIndex( id );
      double gain   = m_calo->cellGain( id ) ;
      int    digAdc = int ( floor( (*aDig)->e()/gain + .9) );
      if( m_zsupThreshold <= digAdc ) {
        if( MSG::DEBUG >= msg.level() ) {
          msg << MSG::DEBUG << id 
              << format( " Energy %10.2f MeV gain %6.2f adc %4d OK",
                         (*aDig)->e()/MeV, gain, digAdc )
              << endreq;
        }
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

  //** write tagged data as MCCaloDigit

  for( aDig = allDigits->begin(); allDigits->end() != aDig ; ++aDig ) {
    CaloCellID id = (*aDig)->cellID();
    index         = m_calo->cellIndex( id );
    if( DefaultFlag == caloFlags[index] ) { continue; }
    CaloDigit* digit = new CaloDigit( id, (*aDig)->e() );
    digits->add( digit ) ;
    
    if( MSG::DEBUG >= msg.level() ) {
      if ( TagFlag == caloFlags[index] ) {
        msg << MSG::DEBUG << id << " added due to Tag. ";
      } else if ( NeighborFlag == caloFlags[index] ) {
        msg << MSG::DEBUG << id << " added as Neighbor.";
      } else {
        msg << MSG::DEBUG << id << " added as Seed.    ";
      }
      msg << format( " Energy %9.2f MeV", (*aDig)->e()/MeV ) << endreq;
    }
  }

  msg << MSG::DEBUG << format( "Have stored %5d digits.", digits->size() ) 
      << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloZSupAlg::finalize() {

  MsgStream msg( msgSvc(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;
  return CaloAlgorithm::finalize();
}
