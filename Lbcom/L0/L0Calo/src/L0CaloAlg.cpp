// $Id: L0CaloAlg.cpp,v 1.23 2004-05-19 12:14:18 ocallot Exp $

/// STL
#include <stdio.h>
#include <string>
#include <algorithm>

/// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"

/// L0Event
#include "Event/L0CaloCandidate.h"

/// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloCollection.h"

/// local
#include "L0CaloAlg.h"

#include "Event/L1Event.h"
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
  : Algorithm                   ( name , pSvcLocator            )
  , m_nameOfOutputDataContainer ( L0CaloCandidateLocation::Default  )
  , m_nameOfGeometryRoot        ( "/dd/Structure/LHCb/" )
  , m_pi0Strategy               ( 1 )
  , m_minEtPi0Local             ( 1.06 * GeV )
  , m_minEtGammaGlobal          ( 0.15 * GeV )
  , m_minPi0Mass                (  10. * MeV )
  , m_maxPi0Mass                ( 220. * MeV )
{

  // Declare the algorithm's properties which can be set at run time and their
  // default values

  declareProperty("OutputData"      , m_nameOfOutputDataContainer) ;
  declareProperty("GeometryRoot"    , m_nameOfGeometryRoot       ) ;
  declareProperty("Pi0Strategy"     , m_pi0Strategy              ) ;
  declareProperty("MinEtPi0Local"   , m_minEtPi0Local            ) ;
  declareProperty("MinEtGammaGlobal", m_minEtGammaGlobal         ) ;
  declareProperty("MinPi0Mass"      , m_minPi0Mass               ) ;
  declareProperty("MaxPi0Mass"      , m_maxPi0Mass               ) ;

  declareProperty("L1ElectronThr"   , m_l1ElectronThr  = 50 );
  declareProperty("L1PhotonThr"     , m_l1PhotonThr    = 50 );
  declareProperty("L1HadronThr"     , m_l1HadronThr    = 100 );
  declareProperty("L1Pi0LocalThr"   , m_l1Pi0LocalThr  = 75 );
  declareProperty("L1Pi0GlobalThr"  , m_l1Pi0GlobalThr = 75 );

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

  MsgStream msg(msgSvc(), name());

  msg << MSG::DEBUG << "Initializing" << endreq;

  // check for the valid names of the input/output data containers

  if( m_nameOfOutputDataContainer.empty() ) {
    msg <<  MSG::ERROR << "The name of the Output data container is empty!"
        << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve the ECAL detector element, build cards

  SmartDataPtr<DeCalorimeter> detEcal( detSvc() ,
                                       m_nameOfGeometryRoot + "Ecal" ) ;
  if( 0 == detEcal ) {
    msg << MSG::ERROR << "Unable to retrieve Detector Element = "
        << m_nameOfGeometryRoot+"Ecal" << endreq;
    return StatusCode::FAILURE ;
  }

  m_ecal = (DeCalorimeter*) detEcal;
  int eCard;
  int hCard;
  m_nbValidation = 0;
  
  for  ( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    ecalFe.push_back( TriggerCard( eCard, m_ecal ) );
    int validationNb =  m_ecal->validationNumber( eCard );
    ecalFe[eCard].setValidationNumber( validationNb );
    if ( m_nbValidation <= validationNb ) m_nbValidation = validationNb + 1;
  }
  
  msg << MSG::VERBOSE << m_ecal << endreq;

  // Retrieve the HCAL detector element, build cards

  SmartDataPtr<DeCalorimeter> detHcal( detSvc() ,
                                       m_nameOfGeometryRoot + "Hcal" ) ;
  if( 0 == detHcal ) {
    msg << MSG::ERROR << "Unable to retrieve Detector Element = "
        << m_nameOfGeometryRoot + "Hcal" << endreq;
    return StatusCode::FAILURE ;
  }
  m_hcal = (DeCalorimeter*) detHcal;
  for  ( hCard = 0; m_hcal->nCards() > hCard; ++hCard ) {
    hcalFe.push_back( TriggerCard( hCard, m_hcal ) );
  }
  msg << MSG::VERBOSE << m_hcal << endreq;

  // Link the ECAL cards to the HCAL cards for the trigger.
  // Method: An ECAL card is connected to a single HCAL card.
  // Get the bottom left cell, get is coordinates, extrapolate to HCAL and
  // get the HCAL cell, get its card, store the information...
  // In case this fails, try the opposite cell !

  // The zRatio allows to compute which cell is in front of which.
  // There is some hardcoding: Outer cells of HCAL are about twice the size
  // of the ECAL one.

  double zRatio = m_hcal->cellSize( m_hcal->firstCellID( 0 ) ) /
    m_ecal->cellSize( m_ecal->firstCellID( 0 ) ) / 2. ;

  for ( eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
    CaloCellID ecalID  = m_ecal->firstCellID( eCard );
    HepPoint3D center  = m_ecal->cellCenter( ecalID ) * zRatio;
    CaloCellID hcalID  = m_hcal->Cell( center );
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
    }
  }

  // Debug the cards

  for ( eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
    msg << MSG::DEBUG << "Ecal card " 
        << format( "Ecal card%4d Area%2d Row%3d Col%3d Validation%3d", 
                   eCard,
                   m_ecal->cardArea(eCard),
                   m_ecal->cardFirstRow(eCard),
                   m_ecal->cardFirstColumn(eCard),
                   ecalFe[eCard].validationNumber() );
    
    msg << " left, corner, down, prev " 
        << format( 
           "%3d %3d %3d %3d HCAL card %3d Mag. %1d Offset row %2d col %2d",
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
    msg << MSG::DEBUG 
        << format( "Hcal card%4d Area%2d Row%3d Col%3d ",
             hCard,
             m_hcal->cardArea(hCard),
             m_hcal->cardFirstRow(hCard),
             m_hcal->cardFirstColumn(hCard) );
    msg << " left, corner, down " 
        << format( "%3d %3d %3d to %2d ECAL cards: ",
                   m_hcal->leftCardNumber(hCard)    ,
                   m_hcal->cornerCardNumber(hCard)  ,
                   m_hcal->downCardNumber(hCard)    ,
                   hcalFe[hCard].numberOfEcalCards() );
    for ( eCard = 0; hcalFe[hCard].numberOfEcalCards() > eCard; ++eCard ) {
      msg << format( "%4d", hcalFe[hCard].ecalCardNumber( eCard ) );
    }
    msg << endreq;
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

  m_minPi0Mass2 = m_minPi0Mass * m_minPi0Mass ;
  m_maxPi0Mass2 = m_maxPi0Mass * m_maxPi0Mass ;

  msg << MSG::INFO << m_ecal->nCards() << " Ecal and "
      << m_hcal->nCards() << " front end cards." << endreq;
  if ( 1 == m_pi0Strategy ) {
    msg << MSG::INFO << "Simple Pi0 strategy. Local = Sum Et, Global= Et1+Et2"
        << endreq;
  } else {
    msg << MSG::INFO << "Refined Pi0 strategy. Local = Sum Et AND Et1 over "
        << m_minEtPi0Local/GeV << " GeV." << endreq;
    msg << MSG::INFO << "        Global = Two cards over "
        << m_minEtGammaGlobal/GeV << " GeV AND pi0 mass between "
        << m_minPi0Mass << " and " << m_maxPi0Mass << " MeV"<< endreq;
  }

  m_etScale = 20. * MeV;

  m_totL1Size  = 0.;
  m_totRawSize = 0.;
  m_nbEvents   = 0 ;

  msg << MSG::INFO << "L1 thresholds: "
      << " electron " << m_l1ElectronThr
      << " photon "   << m_l1PhotonThr
      << " hadron "   << m_l1HadronThr
      << " pi0L "     << m_l1Pi0LocalThr
      << " pi0G "     << m_l1Pi0GlobalThr
      << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Execute: Compute the calo trigger information
//=============================================================================

StatusCode L0CaloAlg::execute() {

  MsgStream          msg( msgSvc(), name() );

  msg << MSG::VERBOSE << "Entering L0 trigger " << endreq;

  // Get the RawEvent
  SmartDataPtr<RawBuffer> rawBuf( eventSvc(), RawBufferLocation::Default );
  if( 0 == rawBuf ) {
    msg << MSG::ERROR << "Could not retrieve raw buffer" << endmsg;
    return StatusCode::FAILURE;
  }
  
  RawEvent rawEvt( rawBuf );

  // Get the ECAL data, store them in the Front-End card

  sumEcalData( msg, rawEvt );

  // Get the Prs information. Adds it to the ecalFe[] objects

  addPrsData( msg, rawEvt );

  // Get the Spd information. Adds it to the ecalFe[] objects

  addSpdData( msg, rawEvt );

  // Loop on ECAL cards. Get the candidates, select the highest

  L0Candidate electron  ( m_ecal, m_etScale );
  L0Candidate photon    ( m_ecal, m_etScale );
  L0Candidate pi0Local  ( m_ecal, m_etScale );
  L0Candidate pi0Global ( m_ecal, m_etScale );

  std::vector<L0Candidate> allElectrons;
  std::vector<L0Candidate> allPhotons;
  std::vector<L0Candidate> allPi0Local;
  std::vector<L0Candidate> allPi0Global;

  {for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    allElectrons.push_back( L0Candidate( m_ecal, m_etScale ) );
    allPhotons.push_back(   L0Candidate( m_ecal, m_etScale ) );
    allPi0Local.push_back(  L0Candidate( m_ecal, m_etScale ) );
    allPi0Global.push_back( L0Candidate( m_ecal, m_etScale ) );
  }}

  int eCard;
  for( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    int etMax   = ecalFe[eCard].etMax()  ;
    int etTot   = ecalFe[eCard].etTot()  ;
    CaloCellID ID = ecalFe[eCard].cellIdMax() ;
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
    
    // Produces also the 'single card' pi0, with two options...
    
    if ( 1 == m_pi0Strategy ) {
      if ( 0 != prsMask ) {  //== Request some preshower activity
        if ( pi0Local.et() < etTot ) {
          particle += " pi0L";
          pi0Local.setCandidate( etTot, ID );
        }
        if ( allPi0Local[numVal].et() < etTot ) {
          allPi0Local[numVal].setCandidate( etTot, ID );
        }
      }
    } else {
      if ( (m_minEtPi0Local < etMax * m_etScale ) &&
           (0 < prsMask )     &&
           (0 == ecalFe[eCard].spdMask() ) ) {
        if ( pi0Local.et() < etTot ) {
          particle += " pi0L";
          pi0Local.setCandidate( etTot, ID );
        }
      }
    }
    
    // Global pi0: the 'previous' card is in the same validation
    
    int pCard = m_ecal->previousCardNumber(eCard);
    if ( 0 <= pCard ) {
      if ( 1 == m_pi0Strategy ) {
        int etMaxTot  = etMax + ecalFe[pCard].etMax()  ;
        CaloCellID ID2 = ecalFe[pCard].cellIdMax();
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
      } else {
        int etMaxPrev = ecalFe[pCard].etMax() ;
        int row       = ecalFe[eCard].rowMax() ;
        int col       = ecalFe[eCard].colMax() ;
        int rowPrev   = ecalFe[pCard].rowMax() - nRowCaloCard ;
        int colPrev   = ecalFe[pCard].colMax() ;
        bool phot     = (0 <  ecalFe[eCard].prsMask() ) &&
          (0 == ecalFe[eCard].spdMask() );
        bool photPrev = (0 <  ecalFe[pCard].prsMask() ) &&
          (0 == ecalFe[pCard].spdMask() );
        if ( (m_minEtGammaGlobal <= etMax     * m_etScale)  &&
             (m_minEtGammaGlobal <= etMaxPrev * m_etScale)  &&
             ( (row > rowPrev+1) ||                     // No ghost test
               (col > colPrev+1) ||
               (col < colPrev-1)    )   &&
             ( phot || photPrev )  ) {                  // One is photonic
          
          // Compute the distance and radius in half-cell units.
          
          CaloCellID IDprev = m_ecal->cardCellID( eCard, rowPrev, colPrev );
          double halfSize = .5 * m_ecal->cellSize( ID );
          int x1 = (int) floor( .5 + m_ecal->cellX( ID )     / halfSize);
          int x2 = (int) floor( .5 + m_ecal->cellX( IDprev ) / halfSize);
          int y1 = (int) floor( .5 + m_ecal->cellY( ID )     / halfSize);
          int y2 = (int) floor( .5 + m_ecal->cellY( IDprev ) / halfSize);
          int dist2   = (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
          int radius2 = (x1+x2) * (x1+x2) + (y1+y2) * (y1+y2);
          int mass2   = 2 * etMax * etMaxPrev *
            (int) (m_etScale * m_etScale) * dist2 / radius2 ;
          int etMaxTot  = etMax + ecalFe[pCard].etMax()  ;
          
          msg << MSG::VERBOSE << "Pi0Glo: cards " 
              << format( "%3d %3d etMax %3d %3d col %2d %2d row %2d %2d",
                         eCard, pCard, etMax, etMaxPrev,
                         col, colPrev, row, rowPrev );
          msg << format( " dist2 %5d radius2 %5d mass2 %5d etSum %4d",
                         dist2, radius2, mass2, etMaxTot ) 
              << endreq;
          
          if ( (m_minPi0Mass2 < mass2) && (m_maxPi0Mass2 > mass2) ) {
            if ( pi0Global.et() < etMaxTot ) {
              particle += " pi0G";
              pi0Global.setCandidate( etMaxTot, ID );
            }
          }
        }
      }
    }
    
    if ( !particle.empty() ) {
      msg << MSG::VERBOSE << "ECAL Card "
          << format( "%3d etMax %3d col %2d row %2d.",
                     ecalFe[eCard].number(), 
                     etMax, 
                     ecalFe[eCard].rowMax(),
                     ecalFe[eCard].colMax() );
      msg << format( " Cells: %4d%4d%4d%4d EtTot%4d Prs%3d->%d Spd%3d->%d",
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

  sumHcalData( msg, rawEvt );

  // Now add the highest ECAL energy in matching cards

  L0Candidate hadron ( m_hcal, m_etScale );
  L0Candidate hadron2( m_hcal, m_etScale );
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

  if ( 0 <= cardMax ) {
    int maxArea = m_hcal->cardArea(cardMax);
    int maxCol  = m_hcal->cardFirstColumn(cardMax) + hcalFe[cardMax].colMax();
    int maxRow  = m_hcal->cardFirstRow(cardMax) + hcalFe[cardMax].rowMax();

    for ( hCard = 0; hCard < m_hcal->nCards(); ++hCard ) {
      if ( !hcalFe[hCard].empty() &&
           hCard != cardMax          ) {
        int hCol = m_hcal->cardFirstColumn(hCard) + hcalFe[hCard].colMax();
        int hRow = m_hcal->cardFirstRow(hCard) + hcalFe[hCard].rowMax();
        // ** Ghost cleaning
        if ( (maxArea != m_hcal->cardArea(hCard))   ||
             (1 < hCol-maxCol) || (1 < maxCol-hCol) ||
             (1 < hRow-maxRow) || (1 < maxRow-hRow)   ) {

          int etMax = hcalFe[hCard].etMax();
          if( hadron2.et() < etMax ) {
            hadron2.setCandidate( etMax, hcalFe[hCard].cellIdMax() );
          }
        } // not a ghost
      } // card not empty
    } // loop hCard
  } // 0 < cardMax

  //===========================================================================
  // Prepare the output container, register it and then fill it.
  //===========================================================================

  m_rawOutput.clear();
  m_l1Output.clear();
    
  L0CaloCandidates* L0Calo = new L0CaloCandidates();
  StatusCode sc = eventSvc()->registerObject( m_nameOfOutputDataContainer ,  
                                              L0Calo ) ;
  if( sc.isFailure() ) {
    if( 0 != L0Calo ) { delete L0Calo ; L0Calo = 0 ; }
    msg << MSG::ERROR << "Unable to register the output container="
        << m_nameOfOutputDataContainer << endreq;
    msg << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  // Store the various candidates

  electron.saveCandidate(  L0Calo::Electron,  L0Calo );
  photon.saveCandidate(    L0Calo::Photon,    L0Calo );
  hadron.saveCandidate(    L0Calo::Hadron,    L0Calo );
  hadron2.saveCandidate(   L0Calo::Hadron2,   L0Calo );
  pi0Local.saveCandidate(  L0Calo::Pi0Local,  L0Calo );
  pi0Global.saveCandidate( L0Calo::Pi0Global, L0Calo );

  HepPoint3D dummy( 0., 0., 0.);
  if ( 0 < sumEt ) {
    L0CaloCandidate* hsum = new L0CaloCandidate ( L0Calo::SumEt, CaloCellID(),
                                                  sumEt,
                                                  sumEt * m_etScale,
                                                  dummy,
                                                  0. );
    L0Calo->add( hsum );
    int word = ( L0Calo::SumEt << 24 ) + sumEt;
    m_l1Output.push_back( (word >> 16) & 0xFFFF );
    m_l1Output.push_back( word & 0xFFFF );

  }

  // Spd multiplicity counter

  L0CaloCandidate* spdMult = new L0CaloCandidate ( L0Calo::SpdMult, 
                                                   CaloCellID(),
                                                   m_spdMult, 0.,
                                                   dummy,
                                                   0. );
  L0Calo->add( spdMult);
  int word = ( L0Calo::SpdMult << 24 ) + m_spdMult;
  m_l1Output.push_back( (word >> 16) & 0xFFFF );
  m_l1Output.push_back( word & 0xFFFF );

  // Debug now the L0 candidates

  L0CaloCandidates::const_iterator item;

  msg << MSG::DEBUG << "== L0CaloCandidate Summary: "
      << L0Calo->size() << " entries." << endreq;
  for( item = L0Calo->begin() ; L0Calo->end() != item ; ++item ) {
    L0CaloCandidate* cand = (*item);
    msg << MSG::DEBUG << cand << endreq;
  }

  //=== Store the perValidation candidates

  L0CaloCandidates* L0FullCalo = new L0CaloCandidates();
  sc = eventSvc()->registerObject( L0CaloCandidateLocation::Full, L0FullCalo );
  if( sc.isFailure() ) {
    if( 0 != L0FullCalo ) { delete L0FullCalo ; L0FullCalo = 0 ; }
    msg << MSG::ERROR << "Unable to register the output container="
        << "/Event/Trig/L0/FullCalo" << endreq;
    msg << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }
  int kk;

  for ( kk=0 ; m_nbValidation > kk ; kk++ ) {
    allElectrons[kk].saveCandidate( L0Calo::Electron,  L0FullCalo );
    saveCandidate(  L0Calo::Electron, allElectrons[kk], m_l1ElectronThr );
  }
  for ( kk=0 ; m_nbValidation > kk ; kk++ ) {
    allPhotons[kk].saveCandidate(   L0Calo::Photon,    L0FullCalo );
    saveCandidate(  L0Calo::Photon, allPhotons[kk], m_l1PhotonThr );
  }
  for ( hCard = 0; hCard < m_hcal->nCards(); ++hCard ) {
    hadron.setCandidate( hcalFe[hCard].etMax(), hcalFe[hCard].cellIdMax() );
    hadron.saveCandidate( L0Calo::Hadron, L0FullCalo );
    saveCandidate(  L0Calo::Hadron, hadron, m_l1HadronThr );
  }
  for ( kk=0 ; m_nbValidation > kk ; kk++ ) {
    allPi0Local[kk].saveCandidate(  L0Calo::Pi0Local,  L0FullCalo );
    saveCandidate(  L0Calo::Pi0Local, allPi0Local[kk], m_l1Pi0LocalThr );
  }
  for ( kk=0 ; m_nbValidation > kk ; kk++ ) {
    allPi0Global[kk].saveCandidate( L0Calo::Pi0Global, L0FullCalo );
    saveCandidate(  L0Calo::Pi0Global, allPi0Global[kk], m_l1Pi0GlobalThr );
  }

  if ( 0 < sumEt ) {
    int word = ( L0Calo::SumEt << 24 ) + sumEt;
    m_rawOutput.push_back( word );
  }
  word = ( L0Calo::SpdMult << 24 ) + m_spdMult;
  m_rawOutput.push_back( word );

  msg << MSG::DEBUG <<"== L0CaloFullCandidate Summary: "
      << L0FullCalo->size() << " entries." << endreq;

  for( item = L0FullCalo->begin() ; L0FullCalo->end() != item ; ++item ) {
    L0CaloCandidate* cand = (*item);
    msg << MSG::DEBUG << cand << endreq;
  }

  msg << MSG::DEBUG
      << "L1 output size = " << m_l1Output.size()
      << " Raw Output size = " << m_rawOutput.size()
      << endreq;


  //== Store in the L1 and RAW buffer if required.

  if ( m_storeFlag ) {
    m_nbEvents++;
    m_totL1Size  += m_l1Output.size();
    m_totRawSize += m_rawOutput.size();
    SmartDataPtr<L1Buffer> l1Buf( eventSvc(), L1BufferLocation::Default );
    if ( 0 != l1Buf ) {
      l1Buf->addBank( 0, L1Buffer::L0Calo, m_l1Output );
    } else {
      msg << MSG::INFO 
          << "No L1 buffer available. No storage of L1 information."
          << endreq;
    }
    rawBuf->addBank( 0, RawBuffer::L0Calo, m_rawOutput );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize. No action...
//=============================================================================

StatusCode L0CaloAlg::finalize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;   // End of finalization step
  if ( 0 != m_nbEvents ) {
    msg << MSG::INFO 
        << format( "Average bank size : %7.1f words L1, %7.1f words RAW.",
                   m_totL1Size/m_nbEvents,
                   m_totRawSize/m_nbEvents )
        << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Sum the Ecal digits into the Fe cards
//=============================================================================

void L0CaloAlg::sumEcalData( MsgStream& msg, RawEvent& rawEvt ) {

  // Reset the cards collection

  for( int eCard = 0; m_ecal->nCards() > eCard;  ++eCard ) {
    ecalFe[eCard].reset( );
  }

  // Get the ECAL input container

  const std::vector<RawBank>& banks = rawEvt.banks( RawBuffer::EcalTrig );
  std::vector<RawBank>::const_iterator itB;
  
  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;

  for ( itB = banks.begin(); banks.end() != itB; itB++ ) {
    const raw_int* data = (*itB).data();
    int dataSize = (*itB).dataSize();
    while ( 0 < dataSize ) {
      short rawID = (*data)>>16;
      int content = (*data) & 0xFFFF;
      int shift = 8;
      while ( 0 <= shift ) {
        int digEt = ( content >> shift ) & 0xFF;
        if ( 0 != digEt ) {
          CaloCellID ID( rawID );

          msg << MSG::VERBOSE << ID << format( " adc %3d", digEt ) << endreq;

          m_ecal->cardAddress(ID, card, row, col );          // Get the card #
          m_ecal->cardNeighbors( card, down, left, corner ); // neighbor.
          
          ecalFe[card].addEt( col, row, digEt);
          if ( (0 == row) && (0 <= down)    ) {
            ecalFe[down].addEt  ( col,          nRowCaloCard, digEt);
          }
          if ( (0 == col) && (0 <= left)    ) {
            ecalFe[left].addEt  ( nColCaloCard, row,          digEt);
          }
          if ( (0 == col) && (0 == row) && (0 <= corner)  ) {
            ecalFe[corner].addEt( nColCaloCard, nRowCaloCard, digEt); 
          }
        }
        shift -= 8;
        rawID++;
      }
      data++;
      dataSize--;
    }
  }
}

//=============================================================================
// Sum the Hcal digits into the Fe cards
//=============================================================================

void L0CaloAlg::sumHcalData( MsgStream& msg, RawEvent& rawEvt ) {

  for( int hCard = 0; m_hcal->nCards() > hCard;  ++hCard ) {
    hcalFe[hCard].reset( );
  }

  // Get the HCAL input container
  const std::vector<RawBank>& banks = rawEvt.banks( RawBuffer::HcalTrig );
  std::vector<RawBank>::const_iterator itB;
  
  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;

  for ( itB = banks.begin(); banks.end() != itB; itB++ ) {
    const raw_int* data = (*itB).data();
    int dataSize = (*itB).dataSize();
    while ( 0 < dataSize ) {
      short rawID = (*data)>>16;
      int content = (*data) & 0xFFFF;
      int shift = 8;
      while ( 0 <= shift ) {
        int digEt = ( content >> shift ) & 0xFF;
        if ( 0 != digEt ) {
          CaloCellID ID( rawID );

          msg << MSG::VERBOSE << ID << format( " adc %3d", digEt ) << endreq;

          m_hcal->cardAddress(ID, card, row, col ); 
          m_hcal->cardNeighbors( card, down, left, corner );
    
          hcalFe[card].addEt( col, row, digEt);
          if ( (0 == row) && (0 <= down) ) {
            hcalFe[down].addEt  ( col,          nRowCaloCard, digEt);
          }
          if ( (0 == col) && (0 <= left) ) {
            hcalFe[left].addEt  ( nColCaloCard, row,          digEt);
          }
          if ( (0 == col) && (0 == row) && (0 <= corner)) {
            hcalFe[corner].addEt( nColCaloCard, nRowCaloCard, digEt);
          }
        }
        shift -= 8;
        rawID++;
      }
      data++;
      dataSize--;
    }
  }
}

//=============================================================================
// Add the Prs information to the ECAL Front-end card
//=============================================================================

void L0CaloAlg::addPrsData( MsgStream& msg, RawEvent& rawEvt ) {

  const std::vector<RawBank>& banks = rawEvt.banks( RawBuffer::PrsTrig );
  std::vector<RawBank>::const_iterator itB;
  
  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;

  for ( itB = banks.begin(); banks.end() != itB; itB++ ) {
    const raw_int* data = (*itB).data();
    int dataSize = (*itB).dataSize();
    while ( 0 < dataSize ) {
      short rawID = (*data)>>16;
      int content = (*data) & 0xFF;
      msg << MSG::VERBOSE << format( "Prs : %8x -> ", (*data) );
      while ( 0 != content ) {
        if ( 0 != (content & 1) ) {
          CaloCellID ID( rawID );

          msg << " " << ID ;

          m_ecal->cardAddress(ID, card, row, col );
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
        content = content >> 1;
        rawID++;
      }
      msg << endreq;
      
      data++;
      dataSize--;
    }
  }
}
//=============================================================================
// Add the Spd information to the ECAL Front-end card
//=============================================================================

void L0CaloAlg::addSpdData( MsgStream& msg, RawEvent& rawEvt ) {

  m_spdMult = 0;

  const std::vector<RawBank>& banks = rawEvt.banks( RawBuffer::PrsTrig );
  std::vector<RawBank>::const_iterator itB;
  
  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;

  for ( itB = banks.begin(); banks.end() != itB; itB++ ) {
    const raw_int* data = (*itB).data();
    int dataSize = (*itB).dataSize();
    while ( 0 < dataSize ) {
      short rawID = ( (*data)>>16 ) & 0x3FFF;  // the ID is the Prs one.
      int content = ( (*data)>>8  ) & 0xFF ;
      msg << MSG::VERBOSE << format( "Spd : %8x -> ", (*data) );
      while ( 0 != content ) {
        if ( 0 != (content & 1) ) {
          m_spdMult++;
          CaloCellID ID( rawID );

          msg << " " << ID ;

          m_ecal->cardAddress(ID, card, row, col );
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
        content = content >> 1;
        rawID++;
      }
      msg << endreq;
      data++;
      dataSize--;
    }
  }
}

//=========================================================================
//  
//=========================================================================
void L0CaloAlg::saveCandidate ( int type, L0Candidate& cand, int thr ) {

  //== Coding for L1-RAW. 
  if ( 0 == cand.et() ) return;

  int word = ( type << 24 ) + (cand.ID().raw() << 8 ) + cand.et();
  m_rawOutput.push_back( word );
  if ( thr < cand.et() && 122 > m_l1Output.size() ) {
    m_l1Output.push_back( (word >> 16) & 0xFFFF );
    m_l1Output.push_back( word & 0xFFFF );
  }
}

//=============================================================================
// Functions of the auxilliary class L0Candidate
//=============================================================================

void  L0Candidate::setCandidate( int et, CaloCellID ID ) {
  m_ID     = ID;
  if ( 255 >= et ) { m_et = et; } else { m_et = 255; }

  // If the cluster is identified by a non-existent cell, one uses the other
  // corner of the cluster (+1,+1) to compute the position. This should exist.

  if ( m_det->valid( ID ) ) {
    m_center = m_det->cellCenter( ID );
    m_tol    = m_det->cellSize( ID ) * .5;
    m_center.setX( m_center.x() + m_tol );
    m_center.setY( m_center.y() + m_tol );
  } else {
    CaloCellID tmp( ID.calo(), ID.area(), ID.row()+1, ID.col()+1);
    m_center = m_det->cellCenter( tmp );
    m_tol    = m_det->cellSize( tmp ) * .5;
    m_center.setX( m_center.x() - m_tol );
    m_center.setY( m_center.y() - m_tol );
  }
};

//=========================================================================
//  
//=========================================================================
void  L0Candidate::saveCandidate( int type, L0CaloCandidates* L0Calo ) {
  if ( 0 < m_et ) {
    L0CaloCandidate* temp = new L0CaloCandidate ( type,
                                                  m_ID,
                                                  m_et,
                                                  m_et * m_etScale,
                                                  m_center,
                                                  m_tol );
    L0Calo->add( temp );
  }
};
