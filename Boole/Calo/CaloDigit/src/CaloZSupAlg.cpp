// $Id: CaloZSupAlg.cpp,v 1.6 2003-11-26 13:17:02 cattanem Exp $
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
#include "CaloKernel/CaloException.h"

// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

// RawEvent
#include "Event/RawBuffer.h"

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
  , m_zsupMethod        ( "1D"  )
  , m_zsup2D            ( false )
  , m_zsupThreshold     ( 6     )

{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("InputMCData"     , m_inputMCData     ) ;
  declareProperty("ZsupMethod"      , m_zsupMethod      ) ;
  declareProperty("ZsupThreshold"   , m_zsupThreshold   ) ;
  declareProperty("BankType"        , m_bankType        ) ;
  declareProperty("EnergyScale"     , m_energyScale     ) ;
  declareProperty("NumberOfBanks"   , m_numberOfBanks   ) ;
  declareProperty("SpdInputData"    , m_spdInputData     = ""        ) ;
  declareProperty("TriggerEtScale"  , m_triggerEtScale   = 20. * MeV );
  declareProperty("TriggerThreshold", m_triggerThreshold = 10. * MeV );
  declareProperty("TriggerBankType" , m_triggerBankType        ) ;

  //=== Default values according to the name of the algorithm !
  if ( "SpdZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Spd"   );
    setInputData(      CaloDigitLocation::FullSpd );
    setOutputData(     CaloDigitLocation::Spd     );
    m_inputMCData    = MCCaloDigitLocation::Spd;
    m_zsupThreshold  = 1;
    m_numberOfBanks  = 0;
    m_triggerEtScale   = 0.;

  } else if ( "PrsZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Prs"   );
    setInputData(      CaloDigitLocation::FullPrs );
    setOutputData(     CaloDigitLocation::Prs     );
    m_inputMCData      = MCCaloDigitLocation::Prs;
    m_zsupThreshold    = 15;
    m_energyScale      = 0.1 * MeV;
    m_bankType         = RawBuffer::PrsE;
    m_triggerBankType  = RawBuffer::PrsTrig;
    m_numberOfBanks    = 8;
    m_spdInputData     = CaloDigitLocation::FullSpd ;
    m_triggerEtScale   = 0.;
    m_detNum           = CaloCellID( 1, 0, 0, 0 ).all();

  } else if ( "EcalZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Ecal"   );
    setInputData(      CaloDigitLocation::FullEcal );
    setOutputData(     CaloDigitLocation::Ecal     );
    m_inputMCData      = MCCaloDigitLocation::Ecal;
    m_zsupMethod       = "2D";
    m_zsupThreshold    = 20;
    m_energyScale      = 1.0 * MeV;
    m_bankType         = RawBuffer::EcalE;
    m_triggerBankType  = RawBuffer::EcalTrig;
    m_numberOfBanks    = 10;
    m_triggerThreshold = 0.;
    m_detNum           = CaloCellID( 2, 0, 0, 0).all();

    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.04 );
    m_corrArea.push_back( 1.08 );
    
  } else if ( "HcalZSup" == name ) {
    setDetData(        "/dd/Structure/LHCb/Hcal"   );
    setInputData(      CaloDigitLocation::FullHcal );
    setOutputData(     CaloDigitLocation::Hcal     );
    m_inputMCData      = MCCaloDigitLocation::Hcal;
    m_zsupThreshold    = 4;
    m_energyScale      = 1.0 * MeV;
    m_bankType         = RawBuffer::HcalE;
    m_triggerBankType  = RawBuffer::HcalTrig;
    m_numberOfBanks    = 4;
    m_triggerThreshold = 0.;
    m_detNum           = CaloCellID(3, 0, 0, 0).all();

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
  if ( 0 < m_numberOfBanks ) {
    msg  << " bankType " << m_bankType
         << " NumberOfBanks " << m_numberOfBanks;
  }
  msg << " TriggerEtScale " << m_triggerEtScale
      << " threshold " << m_triggerThreshold;
  if ( 0 < m_corrArea.size() ) {
    msg << " Correction factor: ";
    for ( unsigned int kk = 0 ; m_corrArea.size() > kk ; kk++ ) {
      msg << m_corrArea[kk] << " ";
    }
  }
  msg      << endreq;

  m_totDataSize = 0.;
  m_totTrigSize = 0.;
  m_nbEvents    = 0;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloZSupAlg::execute() {

  MsgStream msg(  msgSvc(), name() );
  bool debug   = msg.level() <= MSG::DEBUG;
  bool verbose = msg.level() <= MSG::VERBOSE;
  
  //*** some trivial printout

  if ( debug ) msg << MSG::DEBUG << "Perform digitization from "
                   << inputData() << " to " << outputData() << endreq;

  //*** get the input data

  CaloDigits* allDigits = get( eventSvc(), inputData(), allDigits );
  CaloDigits* spdDigits = NULL;
  if ( "" != m_spdInputData ) {
    spdDigits = get( eventSvc(), m_spdInputData, spdDigits );
  }
  
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

  //== Get the RawBuffer
  RawBuffer* rawBuffer = get( eventSvc(), RawBufferLocation::Default, 
                              rawBuffer );
  std::vector< std::vector<raw_int> > banks;
  std::vector< std::vector<raw_int> > trigBanks;
  if ( 0 < m_numberOfBanks ) {
    std::vector<raw_int> a;
    a.reserve(500);
    for ( int kk = 0 ; m_numberOfBanks > kk ; kk++ ) {
      banks.push_back( a );
      trigBanks.push_back( a );
    }
  }

  if ( debug ) msg << MSG::DEBUG << "Processing " << allDigits->size() 
                   << " Digits." << endreq;

  enum {
    DefaultFlag   ,
    NeighborFlag  ,
    SeedFlag       };

  CaloDigits::const_iterator aDig;
  std::vector<int> caloFlags    ( m_numberOfCells, DefaultFlag ) ;
  std::vector<int> trigValue    ( m_numberOfCells, 0           ) ;
  int index;

  //== Process the SPD container for PrsSpd trigger bits

  if ( 0 != spdDigits ) {
    for( aDig = spdDigits->begin(); spdDigits->end() != aDig ; ++aDig ) {
      if( 0 != *aDig ) {
        CaloCellID id = (*aDig)->cellID();
        if ( 0.1 < (*aDig)->e() ) {
          index            = m_calo->cellIndex( id );
          trigValue[index] = 2;
          if( debug ) msg << MSG::DEBUG << id << " fired" << endreq;
        }
      }
    }
  }
  
  // == Apply the threshold. If 2DZsup, tag also the neighbours

  for( aDig = allDigits->begin(); allDigits->end() != aDig ; ++aDig ) {
    if( 0 != *aDig ) {
      CaloCellID id = (*aDig)->cellID();
      index         = m_calo->cellIndex( id );
      double gain   = m_calo->cellGain( id ) ;
      double energy = (*aDig)->e();
      int    digAdc = int ( floor( energy/gain + .9) );
      int trigVal   = 0;
      if ( 0 < m_triggerEtScale ) {
        energy = energy * m_corrArea[ id.area() ] * m_calo->cellSine( id );
        trigVal = (int)floor( energy / m_triggerEtScale + .5 );
        if ( 255 < trigVal ) trigVal = 255;
        if ( 0   > trigVal ) trigVal = 0;
      } else {
        trigVal =  trigValue[index];
        if ( m_triggerThreshold < energy ) trigVal += 1;   // Prs
      }
      if ( 0 != trigVal ) trigValue[index] = trigVal;
      
      if( debug ) {
        if( m_zsupThreshold <= digAdc || 0 !=  trigVal) {
          msg << MSG::DEBUG << id 
              << format( " Energy %10.2f MeV gain %6.2f adc %4d Trig %3d",
                         (*aDig)->e()/MeV, gain, digAdc, trigVal );
          if (  m_zsupThreshold <= digAdc ) msg << " seed";
          msg << endreq;
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

  //** write tagged data as MCCaloDigit

  int nextIndex   = -1;
  int headIndex   = -1;
  int clusLength  = 0;
  int bufIndx     = -1;
  int prevIndx    = -1;
  int cellContent;
  int cellIndex;
  
  for( aDig = allDigits->begin(); allDigits->end() != aDig ; ++aDig ) {
    CaloCellID id = (*aDig)->cellID();
    index         = m_calo->cellIndex( id );
    if( DefaultFlag == caloFlags[index] ) { continue; }

    CaloDigit* digit = new CaloDigit( id, (*aDig)->e() );
    digits->add( digit ) ;
    
    if( verbose ) {
      if ( NeighborFlag == caloFlags[index] ) {
        msg << MSG::VERBOSE << id << " added as Neighbor.";
      } else {
        msg << MSG::VERBOSE << id << " added as Seed.    ";
      }
      msg << format( " Energy %9.2f MeV", (*aDig)->e()/MeV ) << endreq;
    }
    
    if ( 0 < m_numberOfBanks ) {
      //== Make clusters...
      cellContent = int ( floor( (*aDig)->e() / m_energyScale + .5 ) );
      cellIndex   = id.raw();
      
      bufIndx = bufferNumber( id );
      
      if ( bufIndx != prevIndx ) { nextIndex = -1; }
      
      if ( cellIndex != nextIndex ) {
        if ( 0 <= headIndex ) {
          banks[prevIndx][headIndex] += clusLength;
        }
        clusLength = 0;
        headIndex  = banks[bufIndx].size();
        banks[bufIndx].push_back( cellIndex << 16 );
      }
      banks[bufIndx].push_back( cellContent );
      nextIndex = cellIndex+1;
      prevIndx = bufIndx;
      clusLength++;
    }
  }
  if ( 0 <= headIndex ) {
    banks[bufIndx][headIndex] += clusLength;
  }

  msg << MSG::DEBUG << format( "Have stored %5d digits.", digits->size() ) 
      << endreq;

  //== Build the trigger banks

  prevIndx  = -1;
  nextIndex = -1;
  int word = 0;
  
  if ( 0 < m_numberOfBanks ) {
    if ( 0 < m_triggerEtScale ) {  //=== ECAL and HCAL
      for ( index = 0; m_numberOfCells > index; index++ ) {
        if ( 0 == trigValue[index] ) continue;

        CaloCellID id = m_calo->cellIdByIndex( index );

        bufIndx = bufferNumber( id );
        
        if ( bufIndx != prevIndx ) { nextIndex = -1; }
        
        cellIndex = id.raw();
        if ( cellIndex != nextIndex ) {
          if ( 0 <= prevIndx ) trigBanks[prevIndx].push_back( word );
          word = (cellIndex << 16) + (trigValue[index] << 8);
          nextIndex = cellIndex+1;
        } else {
          word += trigValue[index];
        }

        if ( verbose ) {
          msg << MSG::VERBOSE << id << " index " << index 
              << " trigValue " << trigValue[index]
              << format( "word %8x ",word) << endreq;
        }

        prevIndx = bufIndx;
      }
      if ( 0 <= prevIndx ) trigBanks[prevIndx].push_back( word );

    } else {                       //=== Preshower + SPD

      for ( index = 0; m_numberOfCells > index; index++ ) {
        if ( 0 == trigValue[index] ) continue;

        CaloCellID id = m_calo->cellIdByIndex( index );

        bufIndx = bufferNumber( id );
        
        if ( bufIndx != prevIndx ) { nextIndex = -1; }

        cellIndex = id.raw() & 0xFFF8;
        if ( cellIndex != nextIndex ) {
          if ( 0 <= prevIndx ) trigBanks[prevIndx].push_back( word );
          word = cellIndex << 16;
        }
        if ( 0 != (trigValue[index]&1) ) word += ( 1 << (index&7) );     // Prs
        if ( 0 != (trigValue[index]&2) ) word += ( 1 << ((index&7)+8) ); // SPD

        if ( verbose ) {
          msg << MSG::VERBOSE << id << " index " << index 
              << " trigValue " << trigValue[index]
              << format( " word %8x ",word) << endreq;
        }

        prevIndx = bufIndx;
        nextIndex = cellIndex;
      }
      if ( 0 <= prevIndx ) trigBanks[prevIndx].push_back( word );
    }
  }

  //== Store the banks

  if ( 0 < m_numberOfBanks ) {
    int totDataSize = 0;
    int totTrigSize = 0;

    raw_int board = 0;
    for ( unsigned int kk = 0; banks.size() > kk; kk++ ) {
      rawBuffer->addBank( board, m_bankType, banks[kk] );
      totDataSize += banks[kk].size();
      rawBuffer->addBank( board, m_triggerBankType, trigBanks[kk] );
      totTrigSize += trigBanks[kk].size();
      board++;
    }
    m_totDataSize += totDataSize;
    m_totTrigSize += totTrigSize;
    m_nbEvents++;

    if ( debug ) {
      board = 0;
      msg << MSG::DEBUG << "Bank sizes: ";
      for ( unsigned int kk = 0; banks.size() > kk; kk++ ) {
        msg << format( "%2d:%4d+%4d ", board, banks[kk].size(),
                       trigBanks[kk].size() );
        board++;
      }
      msg << endreq << "Total Data bank size " << totDataSize
          << " + trigger " << totTrigSize << endreq;
    }

    if ( verbose ) {
      board = 0;
      for ( unsigned int kk = 0; banks.size() > kk; kk++ ) {
        msg << MSG::VERBOSE << "DATA bank : " << board << endreq;
        int kl = 0;
        std::vector<raw_int>::const_iterator itW;
        
        for ( itW = banks[kk].begin(); banks[kk].end() != itW; itW++ ){
          msg << MSG::VERBOSE << format ( " %8x %8d   ", (*itW), (*itW) );
          kl++;
          if ( 0 == kl%4 ) msg << endreq;
        }
        
        msg << endreq <<  "TRIGGER bank size=" << trigBanks[kk].size() << "  "
            << endreq;
        kl = 0;
        for ( itW = trigBanks[kk].begin(); trigBanks[kk].end() != itW; itW++ ){
          msg << MSG::VERBOSE << format ( " %8x ", (*itW) );
          kl++;
          if ( 0 == kl%8 ) msg << endreq;
        }
        msg << endreq;
      }
      board++;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloZSupAlg::finalize() {

  MsgStream msg( msgSvc(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;

  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    m_totTrigSize /= m_nbEvents;
    msg << MSG::INFO << "Average bank size : " 
        << format( "%7.1f words for energy + %7.1f words for trigger.", 
                   m_totDataSize, m_totTrigSize )
        << endreq;
  }  
  
  return CaloAlgorithm::finalize();
}
