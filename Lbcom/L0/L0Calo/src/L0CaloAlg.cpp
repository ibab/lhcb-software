// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloAlg.cpp,v 1.7 2001-06-12 15:05:55 ocallot Exp $

/// STL
#include <string>
#include <algorithm>

/// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"

/// CaloEvent
#include "CaloEvent/CaloDigit.h"

/// CaloGen
#include "CaloGen/CaloException.h"
#include "CaloGen/CaloCollection.h"

/// local
#include "L0Calo/L0CaloCandidate.h"
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
  : Algorithm                   ( name , pSvcLocator            )
  , m_nameOfEcalDataContainer   ( "/Event/Raw/Ecal/Digit"    )
  , m_nameOfHcalDataContainer   ( "/Event/Raw/Hcal/Digit"    )
  , m_nameOfPrsDataContainer    ( "/Event/Raw/Prs/Digit"     )
  , m_nameOfSpdDataContainer    ( "/Event/Raw/Spd/Digit"     )
  , m_nameOfOutputDataContainer ( "/Event/FE/L0/Calo"   )
  , m_nameOfGeometryRoot        ( "/dd/Structure/LHCb/" )
  , m_etScale                   ( 20. * MeV )
  , m_prsThreshold              ( 10. * MeV )
  , m_pi0Strategy               ( 1 )
  , m_minEtPi0Local             ( 1.06 * GeV )
  , m_minEtGammaGlobal          ( 0.15 * GeV )
  , m_minPi0Mass                (  10. * MeV )
  , m_maxPi0Mass                ( 220. * MeV )
{

  // Declare the algorithm's properties which can be set at run time and their
  // default values

  declareProperty("EcalData"        , m_nameOfEcalDataContainer  ) ;
  declareProperty("HcalData"        , m_nameOfHcalDataContainer  ) ;
  declareProperty("PrsData"         , m_nameOfPrsDataContainer   ) ;
  declareProperty("SpdData"         , m_nameOfSpdDataContainer   ) ;
  declareProperty("GeometryRoot"    , m_nameOfGeometryRoot       ) ;
  declareProperty("EtScale"         , m_etScale                  ) ;
  declareProperty("PrsThreshold"    , m_prsThreshold             ) ;
  declareProperty("OutputData"      , m_nameOfOutputDataContainer) ;
  declareProperty("Pi0Strategy"     , m_pi0Strategy              ) ;
  declareProperty("MinEtPi0Local"   , m_minEtPi0Local            ) ;
  declareProperty("MinEtGammaGlobal", m_minEtGammaGlobal         ) ;
  declareProperty("MinPi0Mass"      , m_minPi0Mass               ) ;
  declareProperty("MaxPi0Mass"      , m_maxPi0Mass               ) ;
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

  MsgStream log(messageService(), name());

  log << MSG::INFO << "Initializing" << endreq;

  // check for the valid names of the input/output data containers

  if( m_nameOfEcalDataContainer.empty() ) {
    log <<  MSG::ERROR << "The name of the ECAL data container is empty!"
	<< endreq;
    return StatusCode::FAILURE;
  }
  if( m_nameOfHcalDataContainer.empty() ) {
    log <<  MSG::ERROR << "The name of the HCAL data container is empty!"
	<< endreq;
    return StatusCode::FAILURE;
  }
  if( m_nameOfOutputDataContainer.empty() ) {
    log <<  MSG::ERROR << "The name of the Output data container is empty!"
	<< endreq;
    return StatusCode::FAILURE;
  }
  {
    m_nameOfOutputDirectory = m_nameOfOutputDataContainer ;
    std::string::size_type pos = m_nameOfOutputDirectory.find_last_of('/') ;
    if( std::string::npos != pos ){ m_nameOfOutputDirectory.erase(pos); }
  }

  // Retrieve the ECAL detector element, build cards

  SmartDataPtr<DeCalorimeter> detEcal( detDataService() ,
				       m_nameOfGeometryRoot + "Ecal" ) ;
  if( 0 == detEcal ) {
    log << MSG::ERROR << "Unable to retrieve Detector Element = "
	<< m_nameOfGeometryRoot+"Ecal" << endreq;
    return StatusCode::FAILURE ;
  }

  m_ecal = (DeCalorimeter*) detEcal;
  m_ecal->buildCells();
  m_ecal->buildCards();
  int eCard;
  int hCard;
  for  ( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    ecalFe.push_back( TriggerCard( eCard, m_ecal ) );
  }
  log << MSG::VERBOSE << m_ecal << endreq;

  // Retrieve the HCAL detector element, build cards

  SmartDataPtr<DeCalorimeter> detHcal( detDataService() ,
				       m_nameOfGeometryRoot + "Hcal" ) ;
  if( 0 == detHcal ) {
    log << MSG::ERROR << "Unable to retrieve Detector Element = "
	<< m_nameOfGeometryRoot + "Hcal" << endreq;
    return StatusCode::FAILURE ;
  }
  m_hcal = (DeCalorimeter*) detHcal;
  m_hcal->buildCells();
  m_hcal->buildCards();
  for  ( hCard = 0; m_hcal->nCards() > hCard; ++hCard ) {
    hcalFe.push_back( TriggerCard( hCard, m_hcal ) );
  }
  log << MSG::VERBOSE << m_hcal << endreq;

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
    char line[80];
    log << MSG::VERBOSE << "Ecal card " ;
    sprintf( line, "%3d Area %1d Row %2d Col %2d ", eCard,
             m_ecal->cardArea(eCard),
             m_ecal->cardFirstRow(eCard),
             m_ecal->cardFirstColumn(eCard)  );
    log << line << " left, corner, down, prev" ;
    sprintf( line,
             "%3d %3d %3d %3d HCAL card %3d Mag. %1d Offset row %2d col %2d",
             m_ecal->leftCardNumber(eCard)   ,
             m_ecal->cornerCardNumber(eCard) ,
             m_ecal->downCardNumber(eCard)   ,
             m_ecal->previousCardNumber(eCard),
             ecalFe[eCard].hcalCard(),
             ecalFe[eCard].hcalMag(),
             ecalFe[eCard].hcalOffsetRow(),
             ecalFe[eCard].hcalOffsetCol());
    log << line << endreq;
  }
  for ( hCard=0 ;  m_hcal->nCards() > hCard; ++hCard ) {
    char line[80];
    log << MSG::VERBOSE << "Hcal card " ;
    sprintf( line, "%3d Area %1d Row %2d Col %2d ",
             hCard,
             m_hcal->cardArea(hCard),
             m_hcal->cardFirstRow(hCard),
             m_hcal->cardFirstColumn(hCard) );
    log << line << " left, corner, down" ;
    sprintf( line,
             "%3d %3d %3d to %2d ECAL cards: ",
             m_hcal->leftCardNumber(hCard)    ,
             m_hcal->cornerCardNumber(hCard)  ,
             m_hcal->downCardNumber(hCard)    ,
             hcalFe[hCard].numberOfEcalCards() );
    log << line ;
    for ( eCard = 0; hcalFe[hCard].numberOfEcalCards() > eCard; ++eCard ) {
      log.width(4); log << hcalFe[hCard].ecalCardNumber( eCard ) ;
    }
    log << endreq;
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

  // Initialize the gain correction for leakage in the 2x2 area

  m_gainCorrEcal[0] = 1.00 ;
  m_gainCorrEcal[1] = 1.04 ;
  m_gainCorrEcal[2] = 1.08 ;

  m_gainCorrHcal[0] = 1.00 ;
  m_gainCorrHcal[1] = 1.05 ;

  // Initialise the cuts

  m_minPi0Mass2 = m_minPi0Mass * m_minPi0Mass ;
  m_maxPi0Mass2 = m_maxPi0Mass * m_maxPi0Mass ;

  log << MSG::INFO << m_ecal->nCards() << " Ecal and "
      << m_hcal->nCards() << " front end cards."
      << " Preshower threshold " << m_prsThreshold/MeV << " MeV." << endreq;
  if ( 1 == m_pi0Strategy ) {
    log << MSG::INFO << "Simple Pi0 strategy. Local = Sum Et, Global= Et1+Et2"
	<< endreq;
  } else {
    log << MSG::INFO << "Refined Pi0 strategy. Local = Sum Et AND Et1 over "
	<< m_minEtPi0Local/GeV << " GeV." << endreq;
    log << MSG::INFO << "        Global = Two cards over "
	<< m_minEtGammaGlobal/GeV << " GeV AND pi0 mass between "
	<< m_minPi0Mass << " and " << m_maxPi0Mass << " MeV"<< endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Execute: Compute the calo trigger information
//=============================================================================

StatusCode L0CaloAlg::execute() {

  MsgStream          log( messageService(), name() );

  log << MSG::VERBOSE << "Entering L0 trigger " << endreq;

  // Get the ECAL data, store them in the Front-End card

  sumEcalData( log );

  // Get the Prs information. Adds it to the ecalFe[] objects

  addPrsData( log );

  // Get the Spd information. Adds it to the ecalFe[] objects

  addSpdData( log );

  // Loop on ECAL cards. Get the candidates, select the highest

  L0Candidate electron  ( m_ecal, m_etScale );
  L0Candidate photon    ( m_ecal, m_etScale );
  L0Candidate pi0Local  ( m_ecal, m_etScale );
  L0Candidate pi0Global ( m_ecal, m_etScale );

  int eCard;
  for( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    if ( !ecalFe[eCard].empty() ) {
      int etMax   = ecalFe[eCard].etMax()  ;
      int etTot   = ecalFe[eCard].etTot()  ;
      CaloCellID ID = ecalFe[eCard].cellIdMax() ;
      std::string particle = "";

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
	    electron.setCandidate( etMax, eCard, ID );
	  }
	} else {
	  if(  photon.et() < etMax ) {
	    particle += " photon";
	    photon.setCandidate( etMax, eCard, ID );
	  }
	}
      }

      // Produces also the 'single card' pi0, with two options...

      if ( 1 == m_pi0Strategy ) {
	if ( pi0Local.et() < etTot ) {
	  particle += " pi0L";
	  pi0Local.setCandidate( etTot, eCard, ID );
	}
      } else {
	if ( (m_minEtPi0Local < etMax * m_etScale ) &&
	     (0 < prsMask )     &&
	     (0 == ecalFe[eCard].spdMask() ) ) {
	  if ( pi0Local.et() < etTot ) {
	    particle += " pi0L";
	    pi0Local.setCandidate( etTot, eCard, ID );
	  }
	}
      }

      // Global pi0: the 'previous' card is in the same validation

      int pCard = m_ecal->previousCardNumber(eCard);
      if ( 0 <= pCard ) {
	if ( 1 == m_pi0Strategy ) {
	  int etMaxTot  = etMax + ecalFe[pCard].etMax()  ;
	  if ( pi0Global.et() < etMaxTot ) {
	    particle += " pi0G";
	    pi0Global.setCandidate( etMaxTot, pCard, ID );
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

	    log << MSG::VERBOSE << "Pi0Glo: cards " ;
	    log.width(3); log << eCard << " "  ;
	    log.width(3); log << pCard << " etMax ";
	    log.width(4); log << etMax;
	    log.width(4); log << etMaxPrev << " col, row ";
	    log.width(3); log << col;
	    log.width(3); log << colPrev;
	    log.width(3); log << row;
	    log.width(3); log << rowPrev << " dist2 ";
	    log.width(6); log << dist2   << " radius2 ";
	    log.width(6); log << radius2 << " mass2 ";
	    log.width(6); log << mass2   << " etSum " << etMaxTot << endreq;

	    if ( (m_minPi0Mass2 < mass2) && (m_maxPi0Mass2 > mass2) ) {
	      if ( pi0Global.et() < etMaxTot ) {
		particle += " pi0G";
		pi0Global.setCandidate( etMaxTot, pCard, ID );
	      }
	    }
	  }
	}
      }

      if ( !particle.empty() ) {
	log << MSG::VERBOSE << "ECAL Card " ;
	log.width(4); log << ecalFe[eCard].number() << " EtMax ";
	log.width(4); log << etMax << " at row, col" ;
	log.width(3); log << ecalFe[eCard].rowMax() ;
	log.width(3); log << ecalFe[eCard].colMax() << " 4 cells: ";
	log.width(4); log << ecalFe[eCard].et1();
	log.width(4); log << ecalFe[eCard].et2();
	log.width(4); log << ecalFe[eCard].et3();
	log.width(4); log << ecalFe[eCard].et4() << "  EtTot ";
	log.width(4); log << ecalFe[eCard].etTot()  << " Prs ";
	log.width(2); log << prsMask << " -> " << okPrs << " Spd ";
	log.width(2); log << ecalFe[eCard].spdMask() << " -> " << spdMask
			  << " " << particle << endreq;
      }
    } // card not empty
  } // eCard

//=============================================================================
//  Now do a similar processing for HCAL !
//=============================================================================

  sumHcalData( log );

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
	hadron.setCandidate( etMax, hCard, hcalFe[hCard].cellIdMax() );
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
	    hadron2.setCandidate( etMax, hCard, hcalFe[hCard].cellIdMax() );
	  }
	} // not a ghost
      } // card not empty
    } // loop hCard
  } // 0 < cardMax

//=============================================================================
// Prepare the output container, register it and then fill it.
//=============================================================================

  L0CaloCandidateVector* L0Calo = new L0CaloCandidateVector();

  // register the output container it into the Transient Store!
  // Search first if the directory exists

  {
    SmartDataPtr<DataObject> outDir( eventDataService() ,
				     m_nameOfOutputDirectory );
    if( 0 == outDir )                         // touch the output directory
      { log << MSG::ERROR << " OutputDirectory="
	    << m_nameOfOutputDirectory << "\tdoes not exist" << endreq ; }
  }
  StatusCode sc = eventDataService()->registerObject(
                  m_nameOfOutputDataContainer ,  L0Calo ) ;
  if( sc.isFailure() ) {
    if( 0 != L0Calo ) { delete L0Calo ; L0Calo = 0 ; }
    log << MSG::ERROR << "Unable to register the output container="
	<< m_nameOfOutputDataContainer << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  // Store the various candidates

  electron.saveCandidate(  L0::Electron,  L0Calo );
  photon.saveCandidate(    L0::Photon,    L0Calo );
  hadron.saveCandidate(    L0::Hadron,    L0Calo );
  hadron2.saveCandidate(   L0::Hadron2,   L0Calo );
  pi0Local.saveCandidate(  L0::Pi0Local,  L0Calo );
  pi0Global.saveCandidate( L0::Pi0Global, L0Calo );

  if ( 0 < sumEt ) {
    HepPoint3D dummy( 0., 0., 0.);
    L0CaloCandidate* hsum = new L0CaloCandidate ( L0::SumEt, CaloCellID(),
						  sumEt,
						  sumEt * m_etScale,
						  dummy,
						  0. );
    L0Calo->push_back( hsum );
  }

  // Debug now the L0 candidates

  log << MSG::DEBUG << "L0CaloCandidate Summary: "
      << L0Calo->size() << " entries." << endreq;
  for( L0CaloCandidateVector::const_iterator item = L0Calo->begin() ;
       L0Calo->end() != item ; ++item ) {
    L0CaloCandidate* cand = (*item);
    log << MSG::DEBUG << cand << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize. No action...
//=============================================================================

StatusCode L0CaloAlg::finalize() {
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Finalize" << endreq;   // End of finalization step
  return StatusCode::SUCCESS;
}

//=============================================================================
// Sum the Ecal digits into the Fe cards
//=============================================================================

void L0CaloAlg::sumEcalData( MsgStream& log ) {

  // Reset the cards collection

  for( int eCard = 0; m_ecal->nCards() > eCard;  ++eCard ) {
    ecalFe[eCard].reset( );
  }

  // Get the ECAL input container

  SmartDataPtr< ObjectVector<CaloDigit> > ecalDigit ( eventDataService(),
                                          m_nameOfEcalDataContainer );
  if( 0 == ecalDigit ) {
    log << MSG::ERROR << "Unable to retrieve Ecal data container="
	<< m_nameOfEcalDataContainer << endreq;
    return;
  }

  // Get digits. Convert to Transverse Energy, Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards

  for( ObjectVector<CaloDigit>::const_iterator digit = ecalDigit->begin() ;
       ecalDigit->end() != digit ; ++digit ) {
    CaloCellID ID  = (*digit)->cellID();
    double  energy = (*digit)->e() * m_gainCorrEcal[ID.area()];
    double  eT     = energy * m_ecal->cellSine(ID) ;
    int     digEt  = (int) floor( eT/m_etScale + .5 );  // Round to nearest int


    if ( 0 < digEt ) {
      int card, row,  col  ;
      int down, left, corner  ;
      m_ecal->cardAddress(ID, card, row, col );          // Get the card #
      m_ecal->cardNeighbors( card, down, left, corner ); // Get the neighbor.

      ecalFe[card].addEt( col, row, digEt);
      if ( (0 == row) && (0 <= down)    ) {
	ecalFe[down].addEt  ( col,          nRowCaloCard, digEt);
      }
      if ( (0 == col) && (0 <= left)    ) {
	ecalFe[left].addEt  ( nColCaloCard, row,          digEt);
      }
      if ( (0 == col) && (0 == row) && (0 <= corner)  ) {
	ecalFe[corner].addEt( nColCaloCard, nRowCaloCard, digEt); }
    }
  }
}

//=============================================================================
// Sum the Hcal digits into the Fe cards
//=============================================================================

void L0CaloAlg::sumHcalData( MsgStream& log ) {

  for( int hCard = 0; m_hcal->nCards() > hCard;  ++hCard ) {
    hcalFe[hCard].reset( );
  }

  // Get the HCAL input container

  SmartDataPtr<ObjectVector<CaloDigit> > hcalDigit ( eventDataService() ,
                                         m_nameOfHcalDataContainer );
  if( 0 == hcalDigit ) {
    log << MSG::ERROR << "Unable to retrieve Hcal data container="
	<< m_nameOfHcalDataContainer << endreq;
    return;
  }

  // Get HCAL digits. Convert to Transverse Energy, Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighbors down/left/corner.

  for( ObjectVector<CaloDigit>::const_iterator digit = hcalDigit->begin() ;
       hcalDigit->end() != digit ; ++digit ) {
    CaloCellID ID = (*digit)->cellID();
    double energy = (*digit)->e() * m_gainCorrHcal[ID.area()];
    double eT     = energy * m_hcal->cellSine(ID) ;
    int    digEt  = (int) floor( eT/m_etScale + .5 ) ; // Round to nearest int.

    if ( 0 < digEt ) {
      int card, row,  col  ;
      int down, left, corner  ;
      m_hcal->cardAddress(ID, card, row, col );    // Card and internal address
      m_hcal->cardNeighbors( card, down, left, corner );   // Neighboring cards

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
  }
}

//=============================================================================
// Add the Prs information to the ECAL Front-end card
//=============================================================================

void L0CaloAlg::addPrsData( MsgStream& log ) {

  SmartDataPtr<ObjectVector<CaloDigit> > prsDigit ( eventDataService() ,
                                                    m_nameOfPrsDataContainer );
  if( 0 == prsDigit ) {
    log << MSG::ERROR << "Unable to retrieve Prs data container="
	<< m_nameOfPrsDataContainer << endreq;
    return ;
  }

  for( ObjectVector<CaloDigit>::const_iterator digit = prsDigit->begin() ;
       prsDigit->end() != digit ; ++digit ) {
    CaloCellID ID     = (*digit)->cellID();
    double     energy = (*digit)->e();

    if ( m_prsThreshold < energy ) {
      int card, row,  col  ;
      int down, left, corner  ;
      m_ecal->cardAddress(ID, card, row, col );    // Card and internal address
      m_ecal->cardNeighbors( card, down, left, corner );   // Neighboring cards

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
}
//=============================================================================
// Add the Spd information to the ECAL Front-end card
//=============================================================================

void L0CaloAlg::addSpdData( MsgStream& log ) {

  SmartDataPtr<ObjectVector<CaloDigit> > spdDigit ( eventDataService() ,
                                                    m_nameOfSpdDataContainer );
  if( 0 == spdDigit ) {
    log << MSG::ERROR << "Unable to retrieve Spd data container="
	<< m_nameOfSpdDataContainer << endreq;
    return;
  }

  for( ObjectVector<CaloDigit>::const_iterator digit = spdDigit->begin() ;
       spdDigit->end() != digit ; ++digit ) {
    CaloCellID ID     = (*digit)->cellID();
    double     energy = (*digit)->e();

    if ( 0.1 < energy ) {        // Threshold necessary when reading back DST !
      int card, row,  col  ;
      int down, left, corner  ;
      m_ecal->cardAddress(ID, card, row, col );    // Card and internal address
      m_ecal->cardNeighbors( card, down, left, corner );   // Neighboring cards
      
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
}

//=============================================================================
// Functions of the auxilliary class L0Candidate
//=============================================================================

void  L0Candidate::setCandidate( int et, int card, CaloCellID ID ) {
  m_ID     = ID;
  m_card   = card;
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

void  L0Candidate::saveCandidate( int type, L0CaloCandidateVector* L0Calo ) {
  if ( 0 < m_et ) {
    L0CaloCandidate* temp = new L0CaloCandidate ( type,
						  m_ID,
						  m_et,
						  m_et * m_etScale,
						  m_center,
						  m_tol );
    L0Calo->push_back( temp );
  }
};
