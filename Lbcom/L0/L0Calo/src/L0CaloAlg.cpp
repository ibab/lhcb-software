// $Id: L0CaloAlg.cpp,v 1.36 2006-08-31 14:31:34 ocallot Exp $

/// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "DetDesc/Condition.h"


/// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloCollection.h"

/// local
#include "L0CaloAlg.h"

//
//  Level-0 calorimeter trigger
//
//

static const AlgFactory<L0CaloAlg>          Factory ;
const       IAlgFactory& L0CaloAlgFactory = Factory ;

//=============================================================================
// Creator
//=============================================================================

L0CaloAlg::L0CaloAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm              ( name , pSvcLocator            )
  , m_nameOfOutputDataContainer ( LHCb::L0ProcessorDataLocation::Calo  )
{
  declareProperty("OutputData"      , m_nameOfOutputDataContainer) ;
  declareProperty("StoreInBuffer"   , m_storeFlag      = true );
};


//=============================================================================
// Destructor
//=============================================================================

L0CaloAlg::~L0CaloAlg() {};

//=============================================================================
// Initialization: Check parameters, get DetectorElement pointers, build
//                 the trigger cards and their connections (HCAL-ECAL)
//=============================================================================
StatusCode L0CaloAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // check for the valid names of the input/output data containers

  if( m_nameOfOutputDataContainer.empty() ) {
    return Error( "The name of the Output data container is empty!" );
  }

  // Retrieve the ECAL detector element, build cards

  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  int eCard;
  int hCard;
  m_nbValidation = 0;
  
  for  ( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    ecalFe.push_back( TriggerCard( eCard, m_ecal ) );
    int validationNb =  m_ecal->validationNumber( eCard );
    ecalFe[eCard].setValidationNumber( validationNb );
    if ( m_nbValidation <= validationNb ) m_nbValidation = validationNb + 1;
  }
  
  // Retrieve the HCAL detector element, build cards

  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  
  for  ( hCard = 0; m_hcal->nCards() > hCard; ++hCard ) {
    hcalFe.push_back( TriggerCard( hCard, m_hcal ) );
  }

  // Link the ECAL cards to the HCAL cards for the trigger.
  // Method: An ECAL card is connected to a single HCAL card.
  // Get the bottom left cell, get is coordinates, extrapolate to HCAL and
  // get the HCAL cell, get its card, store the information...
  // In case this fails, try the opposite cell !

  // The zRatio allows to compute which cell is in front of which.
  // There is some hardcoding: Outer cells of HCAL are about twice the size
  // of the ECAL one.

  //=== WARNING: the first cell of a card doesn't always exist. 
  //=== Use card 1 instead of 0, 0 is incomplete in HCAL -> 0 cell size!

  double zRatio = m_hcal->cellSize( m_hcal->firstCellID( 1 ) ) /
    m_ecal->cellSize( m_ecal->firstCellID( 0 ) ) / 2. ;
  for ( eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
    LHCb::CaloCellID ecalID  = m_ecal->firstCellID( eCard );
    Gaudi::XYZPoint  center  = m_ecal->cellCenter( ecalID ) * zRatio;
    LHCb::CaloCellID hcalID  = m_hcal->Cell( center );
    int hCard          = m_hcal->cardNumber( hcalID );

    // Use last cell if first doesn't work

    if ( 0 > hCard ) {
      ecalID = m_ecal->lastCellID( eCard );
      center = m_ecal->cellCenter( ecalID ) * zRatio;
      hcalID = m_hcal->Cell( center );
      hCard  = m_hcal->cardNumber( hcalID );
    }

    // Compute the ratio of cell sizes (integer) and the offset
    // so that one can compute (relative in the card)
    //     ecalCellRow = hcalCellRow * mag + OffsetRow
    //     ecalCellCol = hcalCellCol * mag + OffsetCol

    if ( 0 <= hCard ) {
      int mag = (int) (m_hcal->cellSize(hcalID) / m_ecal->cellSize(ecalID) );
      int offsetCol = ( m_hcal->cardFirstColumn(hCard) - 16 ) * mag +
        32 - m_ecal->cardFirstColumn(eCard)  ;
      int offsetRow = ( m_hcal->cardFirstRow(hCard)    - 16 ) * mag +
        32 - m_ecal->cardFirstRow(eCard)     ;
      ecalFe[eCard].setHcalParams( hCard, mag, offsetCol, offsetRow );
      hcalFe[hCard].addEcalConnectedCard( eCard );
    } else {
      warning() << "Ecal card " << eCard << " not connected to HCAL " << endreq;
    }
  }

  // Debug the cards

  for ( eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
    debug() << "Ecal card " 
            << format( "Ecal card%4d Area%2d Row%3d Col%3d Validation%3d", 
                       eCard,
                       m_ecal->cardArea(eCard),
                       m_ecal->cardFirstRow(eCard),
                       m_ecal->cardFirstColumn(eCard),
                       ecalFe[eCard].validationNumber() );
    
    debug() << " left, corner, down, prev " 
            << format( "%3d %3d %3d %3d HCAL card %3d Mag. %1d Offset row %2d col %2d",
                       m_ecal->leftCardNumber(eCard)   ,
                       m_ecal->cornerCardNumber(eCard) ,
                       m_ecal->downCardNumber(eCard)   ,
                       m_ecal->previousCardNumber(eCard),
                       ecalFe[eCard].hcalCard(),
                       ecalFe[eCard].hcalMag(),
                       ecalFe[eCard].hcalOffsetRow(),
                       ecalFe[eCard].hcalOffsetCol() )
            << endreq;
  }
  for ( hCard=0 ;  m_hcal->nCards() > hCard; ++hCard ) {
    debug() << format( "Hcal card%4d Area%2d Row%3d Col%3d ",
                       hCard,
                       m_hcal->cardArea(hCard),
                       m_hcal->cardFirstRow(hCard),
                       m_hcal->cardFirstColumn(hCard) );
    debug() << " left, corner, down " 
            << format( "%3d %3d %3d to %2d ECAL cards: ",
                       m_hcal->leftCardNumber(hCard)    ,
                       m_hcal->cornerCardNumber(hCard)  ,
                       m_hcal->downCardNumber(hCard)    ,
                       hcalFe[hCard].numberOfEcalCards() );
    for ( eCard = 0; hcalFe[hCard].numberOfEcalCards() > eCard; ++eCard ) {
      debug() << format( "%4d", hcalFe[hCard].ecalCardNumber( eCard ) );
    }
    debug() << endreq;
  }

  // Initialize the PreShower validation mask: 1 or 2 bits, no more

  m_validPrs[0]  = 0;
  m_validPrs[1]  = 1;
  m_validPrs[2]  = 1;
  m_validPrs[3]  = 1;
  m_validPrs[4]  = 1;
  m_validPrs[5]  = 1;
  m_validPrs[6]  = 1;
  m_validPrs[7]  = 0;
  m_validPrs[8]  = 1;
  m_validPrs[9]  = 1;
  m_validPrs[10] = 1;
  m_validPrs[11] = 0;
  m_validPrs[12] = 1;
  m_validPrs[13] = 0;
  m_validPrs[14] = 0;
  m_validPrs[15] = 0;

  // Initialise the cuts

  info() << m_ecal->nCards() << " Ecal and "
         << m_hcal->nCards() << " front end cards." << endreq;

  Condition* gain = m_ecal->condition( "Gain" );
  if ( 0 == gain ) {
    return Error( "Condition 'Gain' not found in Ecal" );
  }
  if ( gain->exists( "L0EtBin" ) ) {
    m_etScale = gain->paramAsDouble( "L0EtBin" );
  } else {
    return Error( "Parameter 'L0EtBin' not found in Ecal 'Gain'" );
  }
       
  m_totRawSize = 0.;
  m_nbEvents   = 0 ;

  m_adcsEcal = tool<ICaloTriggerAdcsFromRaw>( "CaloTriggerAdcsFromRaw/EcalTriggerAdcTool" );
  m_adcsHcal = tool<ICaloTriggerAdcsFromRaw>( "CaloTriggerAdcsFromRaw/HcalTriggerAdcTool" );
  m_bitsFromRaw = tool<ICaloTriggerBitsFromRaw>( "CaloTriggerBitsFromRaw" );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Execute: Compute the calo trigger information
//=============================================================================
StatusCode L0CaloAlg::execute() {

    // Get the ECAL data, store them in the Front-End card

  sumEcalData( );

  // Get the Prs information. Adds it to the ecalFe[] objects

  addPrsData( );
  
  // Get the Spd information. Adds it to the ecalFe[] objects

  addSpdData( );

  // Loop on ECAL cards. Get the candidates, select the highest

  L0Candidate electron  ( m_ecal );
  L0Candidate photon    ( m_ecal );
  L0Candidate pi0Local  ( m_ecal );
  L0Candidate pi0Global ( m_ecal );

  std::vector<L0Candidate> allElectrons;
  std::vector<L0Candidate> allPhotons;
  std::vector<L0Candidate> allPi0Local;
  std::vector<L0Candidate> allPi0Global;

  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    allElectrons.push_back( L0Candidate( m_ecal ) );
    allPhotons.push_back(   L0Candidate( m_ecal ) );
    allPi0Local.push_back(  L0Candidate( m_ecal ) );
    allPi0Global.push_back( L0Candidate( m_ecal ) );
  }

  int eCard;
  for( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    int etMax   = ecalFe[eCard].etMax()  ;
    int etTot   = ecalFe[eCard].etTot()  ;
    LHCb::CaloCellID ID = ecalFe[eCard].cellIdMax() ;
    std::string particle = "";

    int numVal = ecalFe[eCard].validationNumber();

    // Validate ECAL by Prs/Spd, and select the highest electron and photon
    // Decision on Prs => electron/photon
    // SPD matching Prs => electron
    
    int prsMask = ecalFe[eCard].prsMask();
    int okPrs   = m_validPrs[prsMask];
    int spdMask = prsMask & ecalFe[eCard].spdMask() ;
    
    if (0 < okPrs) {
      if (0 < spdMask) {
        if( electron.et() < etMax ) {
          particle += " electron";
          electron.setCandidate( etMax, ID );
        }
        if ( allElectrons[numVal].et() < etMax ) {
          allElectrons[numVal].setCandidate( etMax, ID );
        }
      } else {
        if(  photon.et() < etMax ) {
          particle += " photon";
          photon.setCandidate( etMax, ID );
        }
        if ( allPhotons[numVal].et() < etMax ) {
          allPhotons[numVal].setCandidate( etMax, ID );
        }
      }
    }
    
    // Produces also the 'single card' pi0
    
    if ( 0 != prsMask ) {  //== Request some preshower activity
      if ( pi0Local.et() < etTot ) {
        particle += " pi0L";
        pi0Local.setCandidate( etTot, ID );
      }
      if ( allPi0Local[numVal].et() < etTot ) {
        allPi0Local[numVal].setCandidate( etTot, ID );
      }
    }
    
    // Global pi0: the 'previous' card is in the same validation
    
    int pCard = m_ecal->previousCardNumber(eCard);
    if ( 0 <= pCard ) {
      int etMaxTot  = etMax + ecalFe[pCard].etMax()  ;
      LHCb::CaloCellID ID2 = ecalFe[pCard].cellIdMax();
      int diffRow = ID2.row() - ID.row();
      int diffCol = ID2.col() - ID.col();
      //== Check that the ID are at least 2 rows or 2 columns apart so
      //== that they don't share a cell.
      if ( ( ID2.area() != ID.area() ) ||
           ( 1 < abs( diffRow )      ) ||
           ( 1 < abs( diffCol )      )   ) {
        if ( pi0Global.et() < etMaxTot ) {
          particle += " pi0G";
          pi0Global.setCandidate( etMaxTot, ID );
        }
        if ( allPi0Global[numVal].et() < etMaxTot ) {
          allPi0Global[numVal].setCandidate( etMaxTot, ID );
        }
      }          
    }
    
    if ( !particle.empty() ) {
      verbose() << "ECAL Card "
                << format( "%3d etMax %3d col %2d row %2d.",
                           ecalFe[eCard].number(), 
                           etMax, 
                           ecalFe[eCard].rowMax(),
                           ecalFe[eCard].colMax() );
      verbose() << format( " Cells: %4d%4d%4d%4d EtTot%4d Prs%3d->%d Spd%3d->%d",
                           ecalFe[eCard].et1(), ecalFe[eCard].et2(),
                           ecalFe[eCard].et3(), ecalFe[eCard].et4(), 
                           ecalFe[eCard].etTot(),
                           prsMask, okPrs, ecalFe[eCard].spdMask(), spdMask )
                << " " << particle << endreq;
    }
  } // eCard

  //===========================================================================
  //  Now do a similar processing for HCAL !
  //===========================================================================

  sumHcalData( );

  // Now add the highest ECAL energy in matching cards

  L0Candidate hadron ( m_hcal );
  int sumEt = 0;
  int cardMax = -1;

  int hCard;
  for ( hCard = 0; hCard < m_hcal->nCards(); ++hCard ) {
    if ( !hcalFe[hCard].empty() ) {
      int maxEcalEt = 0;
      int hCol = hcalFe[hCard].colMax();
      int hRow = hcalFe[hCard].rowMax();
      int eLink;
      for ( eLink=0; eLink < hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
        eCard = hcalFe[hCard].ecalCardNumber( eLink );
        if ( ecalFe[eCard].match_included( hCol, hRow ) ) {
          if ( ecalFe[eCard].etMax() > maxEcalEt ) {
            maxEcalEt = ecalFe[eCard].etMax();
          }
        }
      }
      int etMax = hcalFe[hCard].addEcalEt( maxEcalEt );   // Add ECAL to HCAL
      sumEt  += etMax ;
      if(  hadron.et() < etMax ) {
        cardMax = hCard;
        hadron.setCandidate( etMax, hcalFe[hCard].cellIdMax() );
      }
    } // card not empty
  } // hCard

  //===========================================================================
  // Prepare the output container, register it and then fill it.
  //===========================================================================

  m_rawOutput[0].clear();
  m_rawOutput[1].clear();
    
  LHCb::L0ProcessorDatas* L0Calo = new LHCb::L0ProcessorDatas();
  put( L0Calo, m_nameOfOutputDataContainer );
  
  // Store the various candidates

  electron.saveCandidate(  L0DUBase::Fiber::CaloElectron,  L0Calo );
  photon.saveCandidate(    L0DUBase::Fiber::CaloPhoton,    L0Calo );
  hadron.saveCandidate(    L0DUBase::Fiber::CaloHadron,    L0Calo );
  pi0Local.saveCandidate(  L0DUBase::Fiber::CaloPi0Local,  L0Calo );
  pi0Global.saveCandidate( L0DUBase::Fiber::CaloPi0Global, L0Calo );

  unsigned int code = 0x10000 + (sumEt << L0DUBase::Calo::Sum::Shift);    
  LHCb::L0ProcessorData* hsum = new LHCb::L0ProcessorData ( L0DUBase::Fiber::CaloSumEt, code );
  L0Calo->add( hsum );

  // Spd multiplicity counter

  code = 0x10000 + (m_spdMult << L0DUBase::Calo::Sum::Shift);
  LHCb::L0ProcessorData* spdMult = new LHCb::L0ProcessorData ( L0DUBase::Fiber::CaloSpdMult, code );
  L0Calo->add( spdMult);

  // Debug now the L0 candidates

  LHCb::L0ProcessorDatas::const_iterator item;

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "== L0ProcessorData Summary: "
            << L0Calo->size() << " entries." << endreq;
    for( item = L0Calo->begin() ; L0Calo->end() != item ; ++item ) {
      LHCb::L0ProcessorData* cand = (*item);
      debug() << format( "Key %2d Word %8x", cand->key(), cand->word() ) << endreq;
    }
  }
  
  //=== Store the perValidation candidates, only in Raw Event

  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(  L0DUBase::Fiber::CaloElectron, allElectrons[kk], 1 );
  }
  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(  L0DUBase::Fiber::CaloPhoton, allPhotons[kk], 1 );
  }
  for ( hCard = 0; hCard < m_hcal->nCards(); ++hCard ) {
    hadron.setCandidate( hcalFe[hCard].etMax(), hcalFe[hCard].cellIdMax() );
    saveInRawEvent(  L0DUBase::Fiber::CaloHadron, hadron, 0 );
  }
  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(  L0DUBase::Fiber::CaloPi0Local, allPi0Local[kk], 1 );
  }
  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(  L0DUBase::Fiber::CaloPi0Global, allPi0Global[kk], 1 );
  }

  if ( 0 < sumEt ) {
    int word = ( L0DUBase::Fiber::CaloSumEt << 24 ) + sumEt;
    m_rawOutput[0].push_back( word );
  }
  int word = ( L0DUBase::Fiber::CaloSpdMult << 24 ) + m_spdMult;
  m_rawOutput[1].push_back( word );

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() <<"== L0CaloFullCandidate  Hcal: "
            << m_rawOutput[0].size() 
            << " Ecal " << m_rawOutput[1].size()
            << " entries." << endreq;
  }  

  //== Store in the  RAW buffer if required.

  if ( m_storeFlag ) {
    m_nbEvents++;
    m_totRawSize = m_totRawSize + m_rawOutput[0].size() + m_rawOutput[1].size();
    LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    raw->addBank( 0, LHCb::RawBank::L0Calo, 0, m_rawOutput[0] );
    raw->addBank( 1, LHCb::RawBank::L0Calo, 0, m_rawOutput[1] );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize.
//=============================================================================
StatusCode L0CaloAlg::finalize() {

  if ( 0 != m_nbEvents ) {
    info() << format( "Average bank size : %7.1f words RAW.",
                      m_totRawSize/m_nbEvents )
           << endreq;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Sum the Ecal digits into the Fe cards
//=============================================================================
void L0CaloAlg::sumEcalData(  ) {

  // Reset the cards collection

  for( int eCard = 0; m_ecal->nCards() > eCard;  ++eCard ) {
    ecalFe[eCard].reset( );
  }

  std::vector<LHCb::L0CaloAdc>& adcs = m_adcsEcal->adcs( );

  debug() << "Found " << adcs.size() << " ECAL adcs" << endreq;

  for ( std::vector<LHCb::L0CaloAdc>::const_iterator itAdc = adcs.begin();
        adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    int adc = (*itAdc).adc();

    // Get digits. Sum in front-end cards.
    // Adds to the (possible) neighboring cards if at the border (row/col = 0)
    // and if the card has neighboring cards
    int card, row,  col  ;
    int down, left, corner  ;

    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << id << format( " adc %3d", adc ) << endreq;
    }

    m_ecal->cardAddress(id, card, row, col );          // Get the card #
    m_ecal->cardNeighbors( card, down, left, corner ); // neighbor.
    
    ecalFe[card].addEt( col, row, adc);
    if ( (0 == row) && (0 <= down)    ) {
      ecalFe[down].addEt  ( col,          nRowCaloCard, adc);
    }
    if ( (0 == col) && (0 <= left)    ) {
      ecalFe[left].addEt  ( nColCaloCard, row,          adc);
    }
    if ( (0 == col) && (0 == row) && (0 <= corner)  ) {
      ecalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc); 
    }
  }
}
//=============================================================================
// Sum the Hcal digits into the Fe cards
//=============================================================================

void L0CaloAlg::sumHcalData( ) {

  for( int hCard = 0; m_hcal->nCards() > hCard;  ++hCard ) {
    hcalFe[hCard].reset( );
  }

  std::vector<LHCb::L0CaloAdc>& adcs = m_adcsHcal->adcs( );

  debug() << "Found " << adcs.size() << " HCAL adcs" << endreq;

  for ( std::vector<LHCb::L0CaloAdc>::const_iterator itAdc = adcs.begin();
        adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    int adc = (*itAdc).adc();

    // Get digits. Sum in front-end cards.
    // Adds to the (possible) neighboring cards if at the border (row/col = 0)
    // and if the card has neighboring cards
    int card, row,  col  ;
    int down, left, corner  ;

    m_hcal->cardAddress( id, card, row, col ); 
    m_hcal->cardNeighbors( card, down, left, corner );

    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << id << format( " adc %3d card%3d row%3d col%3d, down%3d left%3d corner%3d", 
                                 adc, card, row, col, down, left, corner ) << endreq;
    }
    
    
    hcalFe[card].addEt( col, row, adc);
    if ( (0 == row) && (0 <= down) ) {
      hcalFe[down].addEt  ( col,          nRowCaloCard, adc);
    }
    if ( (0 == col) && (0 <= left) ) {
      hcalFe[left].addEt  ( nColCaloCard, row,          adc);
    }
    if ( (0 == col) && (0 == row) && (0 <= corner)) {
      hcalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc);
    }
  }
}

//=============================================================================
// Add the Prs information to the ECAL Front-end card
//=============================================================================
void L0CaloAlg::addPrsData( ) {

  LHCb::CaloCellID id;

  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;

  std::vector<LHCb::CaloCellID>& ids = m_bitsFromRaw->firedCells( true );

  debug() << "Found " << ids.size() << " PRS bits" << endreq;

  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;
    
    if ( MSG::VERBOSE >= msgLevel() ) verbose() << id << endreq;
    
    m_ecal->cardAddress(id, card, row, col );
    m_ecal->cardNeighbors( card, down, left, corner );
    
    ecalFe[card].setPrs( col, row );
    if ( (0 == row) && (0 <= down) ) {
      ecalFe[down].setPrs(   col,          nRowCaloCard );
    }
    if ( (0 == col) && (0 <= left) ) {
      ecalFe[left].setPrs(   nColCaloCard, row          );
    }
    if ( (0 == col) && (0 == row) && (0 <= corner) ) {
      ecalFe[corner].setPrs( nColCaloCard, nRowCaloCard );
    }
  }
}
//=============================================================================
// Add the Spd information to the ECAL Front-end card
//=============================================================================
void L0CaloAlg::addSpdData( ) {

  m_spdMult = 0;

  LHCb::CaloCellID id;

  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;

  std::vector<LHCb::CaloCellID>& ids = m_bitsFromRaw->firedCells( false );

  debug() << "Found " << ids.size() << " SPD bits" << endreq;

  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;

    if ( MSG::VERBOSE >= msgLevel() ) verbose() << id << endreq;
    
    m_spdMult++;

    m_ecal->cardAddress(id, card, row, col );
    m_ecal->cardNeighbors( card, down, left, corner );
    
    ecalFe[card].setSpd( col, row );
    if ( (0 == row) && (0 <= down) ) {
      ecalFe[down].setSpd( col, nRowCaloCard );
    }
    if ( (0 == col) && (0 <= left) ) {
      ecalFe[left].setSpd( nColCaloCard, row );
    }
    if ( (0 == col) && (0 == row) && (0 <= corner) ) {
      ecalFe[corner].setSpd( nColCaloCard, nRowCaloCard );
    }
  }
}

//=========================================================================
//  Save a candidate in the Raw buffer
//=========================================================================
void L0CaloAlg::saveInRawEvent ( int type, L0Candidate& cand, unsigned int bank ) {

  //== Coding for Raw: type (upper 8 bits), id (next 16) and et (low 8 bits). 
  if ( 0 == cand.et() ) return;

  int word = ( type << 24 ) + (cand.ID().index() << 8 ) + cand.et();
  m_rawOutput[bank].push_back( word );
}

//=============================================================================
// Functions of the auxilliary class L0Candidate
//=============================================================================
void  L0Candidate::setCandidate( int et, LHCb::CaloCellID ID ) {
  m_ID     = ID;
  if ( 255 >= et ) { m_et = et; } else { m_et = 255; }

  // If the cluster is identified by a non-existent cell, one uses the other
  // corner of the cluster (+1,+1) to compute the position. This should exist.

  if ( !m_det->valid( ID ) ) {
    LHCb::CaloCellID tmp( ID.calo(), ID.area(), ID.row()+1, ID.col()+1);
  }
};

//=========================================================================
