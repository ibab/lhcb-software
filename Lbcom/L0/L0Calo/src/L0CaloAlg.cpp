// $Id: L0CaloAlg.cpp,v 1.52 2008-07-16 13:31:56 robbep Exp $

/// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "DetDesc/Condition.h"

#include <fstream>

/// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloCollection.h"

/// local
#include "L0CaloAlg.h"

//
//  Level-0 calorimeter trigger
//
//
DECLARE_ALGORITHM_FACTORY( L0CaloAlg );

//=============================================================================
// Creator
//=============================================================================

L0CaloAlg::L0CaloAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm              ( name , pSvcLocator            )
  , m_nameOfOutputDataContainer ( LHCb::L0ProcessorDataLocation::Calo  )
  , m_rawOutput( 2 )
  , m_bankVersion( 1 ) 
{
  declareProperty("OutputData"      , m_nameOfOutputDataContainer) ;
  declareProperty("StoreInBuffer"   , m_storeFlag      = true ) ;
  declareProperty("UsePSSPD"        , m_usePsSpd       = true ) ;
  declareProperty("AddECALToHCAL"   , m_addEcalToHcal  = true ) ;
  declareProperty("CreateHCALLut"   , m_createHCALLut  = false ) ;
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

  // Warning m_ecalFE & m_hcalFE  HAVE to contain PIN-diode FEs
  // in order to ensure the card numbering is correct afterward.
  // Pin-diode FE are anyway excluded from the L0 processing in the code


  // Retrieve the ECAL detector element, build cards
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  int eCard;
  int hCard;
  m_nbValidation = 0;
  m_ecalFe.reserve( m_ecal->nCards() );
  for  ( eCard = 0; m_ecal->nCards() > eCard; ++eCard ) {
    m_ecalFe.push_back( TriggerCard( eCard, m_ecal ) );
    int validationNb =  m_ecal->validationNumber( eCard );
    if ( m_nbValidation <= validationNb ) m_nbValidation = validationNb + 1;
    if( !m_ecal->isPinCard(eCard) )
      m_ecalFe[eCard].setValidationNumber( validationNb );
  }  
  // Retrieve the HCAL detector element, build cards
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  
  m_hcalFe.reserve( m_hcal->nCards() );
  for  ( hCard = 0; m_hcal->nCards() > hCard; ++hCard ) {
    m_hcalFe.push_back( TriggerCard( hCard, m_hcal ) );
  }


  m_prs = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );  


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
    if( m_ecal->isPinCard(eCard) )continue;// reject pin readout FE-cards
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
      m_ecalFe[eCard].setHcalParams( hCard, mag, offsetCol, offsetRow );
      m_hcalFe[hCard].addEcalConnectedCard( eCard );
    } else {
      warning() << "Ecal card " << eCard << " not connected to HCAL " << endreq;
    }
  }

  // Debug the cards

  for ( eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
    if( m_ecal->isPinCard(eCard) )continue;// reject pin readout FE-cards
    debug() << "Ecal card " 
            << format( "Ecal card%4d Area%2d Row%3d Col%3d Validation%3d", 
                       eCard,
                       m_ecal->cardArea(eCard),
                       m_ecal->cardFirstRow(eCard),
                       m_ecal->cardFirstColumn(eCard),
                       m_ecalFe[eCard].validationNumber() );
    
    debug() << " left, corner, down, prev " 
            << format( "%3d %3d %3d %3d HCAL card %3d Mag. %1d Offset row %2d col %2d",
                       m_ecal->leftCardNumber(eCard)   ,
                       m_ecal->cornerCardNumber(eCard) ,
                       m_ecal->downCardNumber(eCard)   ,
                       m_ecal->previousCardNumber(eCard),
                       m_ecalFe[eCard].hcalCard(),
                       m_ecalFe[eCard].hcalMag(),
                       m_ecalFe[eCard].hcalOffsetRow(),
                       m_ecalFe[eCard].hcalOffsetCol() )
            << endreq;
  }
  for ( hCard=0 ;  m_hcal->nCards() > hCard; ++hCard ) {
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
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
                       m_hcalFe[hCard].numberOfEcalCards() );
    for ( eCard = 0; m_hcalFe[hCard].numberOfEcalCards() > eCard; ++eCard ) {
      debug() << format( "%4d", m_hcalFe[hCard].ecalCardNumber( eCard ) );
    }
    debug() << endreq;
  }
  
  
  if ( m_createHCALLut ) createHCALLut( ) ;

  createECALLutVB() ; 
  createHCALLutVB() ; 
  
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
         << m_hcal->nCards() << " Hcal front end cards." << endreq;

  m_totRawSize = 0.;
  m_nbEvents   = 0 ;

  m_adcsEcal = 
    tool<ICaloTriggerAdcsFromRaw>( "CaloTriggerAdcsFromRaw", 
                                   "EcalTriggerAdcTool", this );
  m_adcsHcal = 
    tool<ICaloTriggerAdcsFromRaw>( "CaloTriggerAdcsFromRaw", 
                                   "HcalTriggerAdcTool", this );
  m_bitsFromRaw = 
    tool<ICaloTriggerBitsFromRaw>( "CaloTriggerBitsFromRaw", 
                                   "CaloTriggerBitsFromRaw", this );

  m_bankToTES = 
    tool<L0CaloCandidatesFromRawBank>( "L0CaloCandidatesFromRawBank" );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Execute: Compute the calo trigger information
//=============================================================================
StatusCode L0CaloAlg::execute() {
  // Get the ECAL data, store them in the Front-End card
  
  sumEcalData( );

  if ( m_usePsSpd ) {
      
    // Get Spd+Prs data
    m_PrsSpdIds = m_bitsFromRaw->prsSpdCells( );
    
    // Get the Prs information. Adds it to the m_ecalFe[] objects
    
    addPrsData( );
    
    // Get the Spd information. Adds it to the m_ecalFe[] objects
    
    addSpdData( );
  }
  
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
    if( m_ecal->isPinCard(eCard) )continue;// reject pin readout FE-cards
    int etMax   = m_ecalFe[eCard].etMax()  ;
    int etTot   = m_ecalFe[eCard].etTot()  ;
    LHCb::CaloCellID ID = m_ecalFe[eCard].cellIdMax() ;

    std::string particle = "";
    int numVal = m_ecalFe[eCard].validationNumber();

    // Validate ECAL by Prs/Spd, and select the highest electron and photon
    // Decision on Prs => electron/photon
    // SPD matching Prs => electron
    
    int prsMask = m_ecalFe[eCard].prsMask();
    int okPrs   = m_validPrs[prsMask];
    int spdMask = prsMask & m_ecalFe[eCard].spdMask() ;

    if ( m_usePsSpd ) {
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
    } else {
      if( electron.et() < etMax ) {
        particle += " electron";
        electron.setCandidate( etMax, ID );
      }
      if ( allElectrons[numVal].et() < etMax ) {
        allElectrons[numVal].setCandidate( etMax, ID );
      }
      if(  photon.et() < etMax ) {
        particle += " photon";
        photon.setCandidate( etMax, ID );
      }
      if ( allPhotons[numVal].et() < etMax ) {
        allPhotons[numVal].setCandidate( etMax, ID );
      }
      
      // Produces also the 'single card' pi0
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
      int etMaxTot  = etMax + m_ecalFe[pCard].etMax()  ;
      LHCb::CaloCellID ID2 = m_ecalFe[pCard].cellIdMax();
      int diffRow = ID2.row() - ID.row();
      int diffCol = ID2.col() - ID.col();
      //== Check that the ID are at least 2 rows or 2 columns apart so
      //== that they don't share a cell.
      if ( ( ID2.area() != ID.area() ) ||
           ( 1 < abs( diffRow )      ) ||
           ( 1 < abs( diffCol )      )   ) {
        if ( pi0Global.et() < etMaxTot ) {
          particle += " pi0G";
          pi0Global.setCandidate( etMaxTot, ID2 );
        }

        if ( allPi0Global[numVal].et() < etMaxTot ) {
          allPi0Global[numVal].setCandidate( etMaxTot, ID2 );
        }
      }          
    }
    
    if ( !particle.empty() ) {
      verbose() << "ECAL Card "
                << format( "%3d etMax %3d col %2d row %2d.",
                           m_ecalFe[eCard].number(), 
                           etMax, 
                           m_ecalFe[eCard].rowMax(),
                           m_ecalFe[eCard].colMax() );
      verbose() << format(" Cells: %4d%4d%4d%4d EtTot%4d Prs%3d->%d Spd%3d->%d",
                           m_ecalFe[eCard].et1(), m_ecalFe[eCard].et2(),
                           m_ecalFe[eCard].et3(), m_ecalFe[eCard].et4(), 
                           m_ecalFe[eCard].etTot(),
                           prsMask, okPrs, m_ecalFe[eCard].spdMask(), spdMask )
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
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
    //    Add also if HCAL energy is 0
    // if ( !m_hcalFe[hCard].empty() ) {
    //
    int maxEcalEt = 0;
    int hCol = m_hcalFe[hCard].colMax();
    int hRow = m_hcalFe[hCard].rowMax();
    int eLink;


    LHCb::CaloCellID ID = m_hcalFe[hCard].cellIdMax() ;
    std::string particle = "";
    particle += " hadron";

    if ( m_addEcalToHcal ) {
      for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
        eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
        if ( m_ecalFe[eCard].match_included( hCol, hRow ) ) {
          if ( m_ecalFe[eCard].etMax() > maxEcalEt ) {
            maxEcalEt = m_ecalFe[eCard].etMax();
          }
        }
      }
    }    
//    int etMax = m_hcalFe[hCard].addEcalEt( maxEcalEt );   // Add ECAL to HCAL
    
    int etMax = m_hcalFe[hCard].etMax() + maxEcalEt ;   // Add ECAL to HCAL 
    sumEt += etMax ;
    if(  hadron.et() < etMax ) {
      cardMax = hCard;
      hadron.setCandidate( etMax, m_hcalFe[hCard].cellIdMax() );
    }
    //
    //} // card not empty
    //
  } // hCard


  if ( msgLevel( MSG::DEBUG ) ) {      
    for ( hCard = 0; hCard < m_hcal->nCards(); ++hCard ) {
      if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
      int type = m_hcal->selectionType( hCard );
      std::string typeName ;
      switch ( type ) {
      case 0:  typeName = "Master"  ; break ;
      case 1:  typeName = "Slave1"    ; break ;
      case 2:  typeName = "Slave2"    ; break ;
      default: typeName = "Unknown" ; break ;
      }
      debug()<<" hCard# "<<hCard<<" ID= "<<m_hcalFe[hCard].cellIdMax()
            <<" type = "<<typeName<<endmsg;
    }
  }

  int allhcalFe_Slave1[50];
  int allhcalFe_Slave2[50];
  int allhcalFe_Master[50];

  int allSlave1 = 0 ; 
  int allSlave2 = 0 ; 
  int allMaster = 0 ; 

  for ( hCard = 0; hCard < m_hcal->nCards(); ++hCard ) {
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
    int type = m_hcal->selectionType( hCard );
    if (type == 0) {
      allhcalFe_Master[allMaster] = hCard ; 
      allMaster++  ; 
    }
    if (type == 1) {
      allhcalFe_Slave1[allSlave1] = hCard ; 
      allSlave1++  ; 
    }
    if (type == 2) {
      allhcalFe_Slave2[allSlave2] = hCard ; 
      allSlave2++  ; 
    }
  }

  debug()<<" #of hcal cards related to Master= "
         <<allMaster<<" related to Slave1= "
         <<allSlave1<<" related to Slave2= "<<allSlave2<<endmsg;

  // Compute the size of the array which will be saved in the SEL boads
  int nbSlave1SEL = 0 ; 
  int nbSlave2SEL = 0 ; 
  int nbMasterSEL = 0 ; 
  for ( int kk=0 ; allSlave1 > kk ; kk++ ) {
    int hCard = allhcalFe_Slave1[kk] ;
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
    std::vector<int> vValCards ;
    int eLink;
    for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
      eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
      int valNum = m_ecalFe[eCard].validationNumber() ;
      std::vector<int>::iterator it ;
      int alreadyIn = 0 ;
      for ( it = vValCards.begin() ; it != vValCards.end() ;  it++ ) {
        if (*it == valNum) alreadyIn = 1 ;
      }
      if (alreadyIn == 0 ) vValCards.push_back(valNum);
    }
    nbSlave1SEL += vValCards.size();
  }
  for ( int kk=0 ; allSlave2 > kk ; kk++ ) {
    int hCard = allhcalFe_Slave2[kk] ;
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
    std::vector<int> vValCards ;
    int eLink;
    for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
      eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
      int valNum = m_ecalFe[eCard].validationNumber() ;
      std::vector<int>::iterator it ;
      int alreadyIn = 0 ;
      for ( it = vValCards.begin() ; it != vValCards.end() ;  it++ ) {
        if (*it == valNum) alreadyIn = 1 ;
      }
      if (alreadyIn == 0 ) vValCards.push_back(valNum);
    }
    nbSlave2SEL += vValCards.size();
  }
  for ( int kk=0 ; allMaster > kk ; kk++ ) {
    int hCard = allhcalFe_Master[kk] ;
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
    std::vector<int> vValCards ;
    int eLink;
    for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
      eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
      int valNum = m_ecalFe[eCard].validationNumber() ;
      std::vector<int>::iterator it ;
      int alreadyIn = 0 ;
      for ( it = vValCards.begin() ; it != vValCards.end() ;  it++ ) {
        if (*it == valNum) alreadyIn = 1 ;
      }
      if (alreadyIn == 0 ) vValCards.push_back(valNum);
    }
    nbMasterSEL += vValCards.size();
  }

  std::vector<L0Candidate> allHadronsSlave1;
  std::vector<L0Candidate> allHadronsSlave2;
  std::vector<L0Candidate> allHadronsMaster;

  L0Candidate hadronSlave1 ( m_hcal );
  L0Candidate hadronSlave2 ( m_hcal );
  L0Candidate hadronMaster ( m_hcal );

  L0Candidate sumAllMaster ( m_hcal );

  for ( int kk=0 ; nbSlave1SEL > kk ; kk++ ) {
    allHadronsSlave1.push_back( L0Candidate( m_hcal ) );
  }
  for ( int kk=0 ; nbSlave2SEL > kk ; kk++ ) {
    allHadronsSlave2.push_back( L0Candidate( m_hcal ) );
  }
  for ( int kk=0 ; nbMasterSEL > kk ; kk++ ) {
    allHadronsMaster.push_back( L0Candidate( m_hcal ) );
  }

  // process for Slave 1
  int etMaxEcalPerValNum[30];
  int iSlave1 = 0 ; 
  int sumEtSlave1 = 0 ;
  for ( int kk=0 ; allSlave1 > kk ; kk++ ) {
    int hCard = allhcalFe_Slave1[kk] ;
    int hCol = m_hcalFe[hCard].colMax();
    int hRow = m_hcalFe[hCard].rowMax();
    int eLink;
    LHCb::CaloCellID ID = m_hcalFe[hCard].cellIdMax() ;
    std::string particle = "";
    particle += " hadron";
    for ( int ii=0 ; m_nbValidation> ii ; ii++ ){
      etMaxEcalPerValNum[ii] = -1 ; 
    }
    for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
      eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
      int valNum = m_ecalFe[eCard].validationNumber() ;
      if ( etMaxEcalPerValNum[valNum] <0) etMaxEcalPerValNum[valNum] = 0 ; 
      if ( m_ecalFe[eCard].match_included( hCol, hRow ) ) {
        if ( m_ecalFe[eCard].etMax() > etMaxEcalPerValNum[valNum] ) {
          etMaxEcalPerValNum[valNum]= m_ecalFe[eCard].etMax();
        }
      }
    }
    int sumEtMaxPerCard = 0;
    for ( int ii=0 ; m_nbValidation> ii ; ii++ ){
      if (etMaxEcalPerValNum[ii] > -1 ) {
        int et( 0 ) ;
        if ( m_addEcalToHcal ) et = m_hcalFe[hCard].etMax() + etMaxEcalPerValNum[ii] ; 
        else et = m_hcalFe[hCard].etMax() ;
        if (iSlave1 > nbSlave1SEL ) 
          info()<<" !!!! iSlave1= "<<iSlave1<<" nbSlave1SEL= "
                <<nbSlave1SEL<< endmsg;
        if (iSlave1 < nbSlave1SEL ) {
          allHadronsSlave1[iSlave1].setCandidate( et ,
                                                  m_hcalFe[hCard].cellIdMax() );
          if (et > sumEtMaxPerCard) sumEtMaxPerCard = et ; 
        }
        iSlave1++;
        if (hadronSlave1.et() < et ) {
          hadronSlave1.setCandidate(et , m_hcalFe[hCard].cellIdMax());
        }
      }
    }
    sumEtSlave1 += sumEtMaxPerCard ; 
  } // end processing for Slave 1 
  //store one candidate ... note that ID is meaningless ... taken
  // from the maximum candidate 
  // at maximum sumEtSlave should be equal to 2^14-1 = 16383
  if (sumEtSlave1 > 16383 ) sumEtSlave1 = 16383 ;
  // debug   
  debug()<<" ====================================================" <<endmsg;
  debug()<<" ============== Slave 1 =============================" <<endmsg;
  for ( int kk=0 ; nbSlave1SEL > kk ; kk++ ) {
    debug()<<"Candidat "<<kk<<" Index= "<<allHadronsSlave1[kk].ID()<<" Et= "
           << allHadronsSlave1[kk].et()<< endmsg;
  }
  debug()<<" Maximum in Slave 1 Index= "<<hadronSlave1.ID()<<" Et= "
         << hadronSlave1.et()<< endmsg;
  debug()<<" sumEt in Slave 1 ="<<sumEtSlave1<< endmsg;
  //--------------------------------------------------------------------------
  // process for Slave 2
  int iSlave2 = 0 ; 
  int sumEtSlave2 = 0 ;
  for ( int kk=0 ; allSlave2 > kk ; kk++ ) {
    int hCard = allhcalFe_Slave2[kk] ;
    int hCol = m_hcalFe[hCard].colMax();
    int hRow = m_hcalFe[hCard].rowMax();
    int eLink;
    LHCb::CaloCellID ID = m_hcalFe[hCard].cellIdMax() ;
    std::string particle = "";
    particle += " hadron";
    
    for ( int ii=0 ; m_nbValidation> ii ; ii++ ){
      etMaxEcalPerValNum[ii] = -1 ; 
    }
    for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
      eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
      int valNum = m_ecalFe[eCard].validationNumber() ;
      if ( etMaxEcalPerValNum[valNum] <0) etMaxEcalPerValNum[valNum] = 0 ; 
      if ( m_ecalFe[eCard].match_included( hCol, hRow ) ) {
        if ( m_ecalFe[eCard].etMax() > etMaxEcalPerValNum[valNum] ) {
          etMaxEcalPerValNum[valNum]= m_ecalFe[eCard].etMax();
        }
      }
    }
    int sumEtMaxPerCard = 0;
    for ( int ii=0 ; m_nbValidation> ii ; ii++ ){
      if (etMaxEcalPerValNum[ii] > -1 ) {
        int et( 0 )  ;
        if ( m_addEcalToHcal ) et = m_hcalFe[hCard].etMax() + etMaxEcalPerValNum[ii] ; 
        else et = m_hcalFe[hCard].etMax() ;
        
        if (iSlave2 > nbSlave2SEL ) 
          info()<<"!!!! iSlave2= "<<iSlave2<<" nbSlave2SEL= "
                <<nbSlave2SEL<< endmsg;
        if (iSlave2 < nbSlave2SEL ) {
          allHadronsSlave2[iSlave2].setCandidate( et ,
                                                  m_hcalFe[hCard].cellIdMax() );
          if (et > sumEtMaxPerCard) sumEtMaxPerCard = et ; 
        }
        iSlave2++;
        if (hadronSlave2.et() < et ) {
          hadronSlave2.setCandidate(et , m_hcalFe[hCard].cellIdMax());
        }
      }
    }
    sumEtSlave2 += sumEtMaxPerCard ; 
  } // end processing for Slave 2 
  // at maximum sumEtSlave should be equal to 2^14 - 1 = 16383
  if (sumEtSlave2 > 16383 ) sumEtSlave2 = 16383 ;
  // debug   
  debug()<<" ==================================================== " <<endmsg;
  debug()<<" =============== Slave 2 ============================ " <<endmsg;
  for ( int kk=0 ; nbSlave2SEL > kk ; kk++ ) {
    debug()<<" Candidat "<<kk<<" Index= "<<allHadronsSlave2[kk].ID()
           <<" Et= "<< allHadronsSlave2[kk].et()<< endmsg;
  }
  debug()<<" Maximum in Slave 2 Index= "<<hadronSlave2.ID()
         <<" Et= "<< hadronSlave2.et()<< endmsg;
  debug()<<" sumEt in Slave 2 ="<<sumEtSlave2<< endmsg;
  //-----------------------------------------------------------
  // process for Master
  int iMaster = 0 ; 
  int sumEtMaster = 0 ;
  for ( int kk=0 ; allMaster > kk ; kk++ ) {
    int hCard = allhcalFe_Master[kk] ;
    int hCol = m_hcalFe[hCard].colMax();
    int hRow = m_hcalFe[hCard].rowMax();
    int eLink;
    LHCb::CaloCellID ID = m_hcalFe[hCard].cellIdMax() ;
    std::string particle = "";
    particle += " hadron";
    for ( int ii=0 ; m_nbValidation> ii ; ii++ ){
      etMaxEcalPerValNum[ii] = -1 ; 
    }
    for ( eLink=0; eLink < m_hcalFe[hCard].numberOfEcalCards() ; ++eLink) {
      eCard = m_hcalFe[hCard].ecalCardNumber( eLink );
      int valNum = m_ecalFe[eCard].validationNumber() ;
      if ( etMaxEcalPerValNum[valNum] <0) etMaxEcalPerValNum[valNum] = 0 ; 
      if ( m_ecalFe[eCard].match_included( hCol, hRow ) ) {
        if ( m_ecalFe[eCard].etMax() > etMaxEcalPerValNum[valNum] ) {
          etMaxEcalPerValNum[valNum]= m_ecalFe[eCard].etMax();
        }
      }
    }
    int sumEtMaxPerCard = 0;
    for ( int ii=0 ; m_nbValidation> ii ; ii++ ){
      if (etMaxEcalPerValNum[ii] > -1 ) {
        int et( 0 ) ;
        if ( m_addEcalToHcal ) et = m_hcalFe[hCard].etMax() + etMaxEcalPerValNum[ii] ; 
        else et = m_hcalFe[hCard].etMax() ;
        if (iMaster > nbMasterSEL ) 
          info()<<"!!!! iMaster= "<<iMaster<<" nbMasterSEL= "
                <<nbMasterSEL<< endmsg;
        if (iMaster < nbMasterSEL ) {
          allHadronsMaster[iMaster].setCandidate( et ,
                                                  m_hcalFe[hCard].cellIdMax() );
          if (et > sumEtMaxPerCard) sumEtMaxPerCard = et ; 
        }
        iMaster++;
        if (hadronMaster.et() < et ) {
          hadronMaster.setCandidate(et , m_hcalFe[hCard].cellIdMax());
        }
      }
    }
    sumEtMaster += sumEtMaxPerCard ; 
  } // end processing for Master 
  //store one candidate ... note that ID is meaningless ... 
  // taken from the maximum candidate 
  // at maximum sumEtSlave should be equal to 2^14 - 1 = 16383
  if (sumEtMaster > 16383 ) sumEtMaster = 16383 ;

  // debug   
  debug()<<" ==================================================== " <<endmsg;
  debug()<<" ================Master ============================= " <<endmsg;
  for ( int kk=0 ; nbMasterSEL > kk ; kk++ ) {
    debug()<<"Candidat "<<kk<<" Index= "<<allHadronsMaster[kk].ID()<<" Et= "
           << allHadronsMaster[kk].et()<< endmsg;
  }
  debug()<<" Maximum in Master Index= "<<hadronMaster.ID()<<" Et= "
         << hadronMaster.et()<< endmsg;
  debug()<<" sumEt in Master ="<<sumEtMaster<< endmsg;


  //==========================================================================
  // Process for SPD now 
  //==========================================================================

  LHCb::CaloCellID id;

  std::vector<LHCb::CaloCellID>& ids = m_PrsSpdIds.second;

  int nbSpdMultCards = 16 ; 
  int valMult[16] ; 
  for ( int kk=0 ; nbSpdMultCards > kk ; kk++ ) {
    valMult[kk] = 0; 
  }
  
  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;
    int spdCard  = m_prs->cardNumber( id );
    int slot     = m_prs->cardSlot(spdCard); 
    int crate    = m_prs->cardCrate(spdCard); 
    
    int identif = 2*crate ; 
    if (slot >= 8 ) identif+= 1 ; 
    debug() <<"SPD id "<<id<<" spdCard= " << spdCard<< " slot = "
            <<slot<< " crate="<<crate<<" identif = "<<identif<< endreq;
    valMult[identif] += 1 ; 
  }


  int totSpdMult = 0 ; 
  for ( int kk=0 ; nbSpdMultCards > kk ; kk++ ) {
    // check for maximum : 2^10 -1 = 1023
    if ( valMult[kk] >  1023) valMult[kk] = 1023 ; 
    debug() <<"kk= "<<kk<<" valMult[kk]= " << valMult[kk] << endreq;
    totSpdMult +=  valMult[kk] ; 
  }
  debug() <<"Overall Spd mutiplicity : "<<totSpdMult << endreq;

  //==========================================================================
  // End of Process for SPD 
  //==========================================================================
  

  //===========================================================================
  // Prepare the output container, register it and then fill it.
  //===========================================================================

  m_rawOutput[0].clear();
  m_rawOutput[1].clear();
    
  LHCb::L0ProcessorDatas* L0Calo = new LHCb::L0ProcessorDatas();
  put( L0Calo, m_nameOfOutputDataContainer ) ;
  
  L0Candidate theCorrectElectron ( m_ecal );
  theCorrectElectron.setCandidate(electron.et(),electron.ID()) ; 

  L0Candidate theCorrectPhoton ( m_ecal );
  theCorrectPhoton.setCandidate(photon.et(),photon.ID()) ; 

  L0Candidate theCorrectPi0Local ( m_ecal );
  theCorrectPi0Local.setCandidate(pi0Local.et(),pi0Local.ID()) ; 

  L0Candidate theCorrectPi0Global ( m_ecal );
  theCorrectPi0Global.setCandidate(pi0Global.et(),pi0Global.ID()) ; 


  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    if ( allElectrons[kk].et() == electron.et() && electron.et() != 0 && allElectrons[kk].ID().all()  != electron.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      int inputNumber_all = findInputNumberForEcal(allElectrons[kk]) ; 
      int inputNumber = findInputNumberForEcal(theCorrectElectron) ; 
      if (inputNumber_all < inputNumber ) theCorrectElectron.setCandidate(allElectrons[kk].et(),allElectrons[kk].ID()) ; 
    }
  }

  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    if ( allPhotons[kk].et() == photon.et() && photon.et() != 0 && allPhotons[kk].ID().all()  != photon.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      int inputNumber_all = findInputNumberForEcal(allPhotons[kk]) ; 
      int inputNumber = findInputNumberForEcal(theCorrectPhoton) ; 
      if (inputNumber_all < inputNumber ) theCorrectPhoton.setCandidate(allPhotons[kk].et(),allPhotons[kk].ID()) ; 
    }
  }

  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    if ( allPi0Local[kk].et() == pi0Local.et() && pi0Local.et() != 0 && allPi0Local[kk].ID().all()  != pi0Local.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      int inputNumber_all = findInputNumberForEcal(allPi0Local[kk]) ; 
      int inputNumber = findInputNumberForEcal(theCorrectPi0Local) ; 
      if (inputNumber_all < inputNumber ) theCorrectPi0Local.setCandidate(allPi0Local[kk].et(),allPi0Local[kk].ID()) ; 
    }
  }

  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    if ( allPi0Global[kk].et() == pi0Global.et() && pi0Global.et() != 0 && allPi0Global[kk].ID().all()  != pi0Global.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      int inputNumber_all = findInputNumberForEcal(allPi0Global[kk]) ; 
      int inputNumber = findInputNumberForEcal(theCorrectPi0Global) ; 
      if (inputNumber_all < inputNumber ) theCorrectPi0Global.setCandidate(allPi0Global[kk].et(),allPi0Global[kk].ID()) ; 
    }
  }

  // Store the various candidates
  theCorrectElectron.saveCandidate (  L0DUBase::Fiber::CaloElectron,  L0Calo );
  theCorrectPhoton.saveCandidate   (  L0DUBase::Fiber::CaloPhoton,  L0Calo );
  theCorrectPi0Local.saveCandidate (  L0DUBase::Fiber::CaloPi0Local,  L0Calo );
  theCorrectPi0Global.saveCandidate(  L0DUBase::Fiber::CaloPi0Global,  L0Calo );

  debug()<<" electromagnetic candidates done .... " <<endreq ; 


  L0Candidate theCorrectHadronMaster ( m_hcal );
  theCorrectHadronMaster.setCandidate(hadronMaster.et(),hadronMaster.ID()) ; 

  L0Candidate theCorrectHadronSlave1 ( m_hcal );
  theCorrectHadronSlave1.setCandidate(hadronSlave1.et(),hadronSlave1.ID()) ; 

  L0Candidate theCorrectHadronSlave2 ( m_hcal );
  theCorrectHadronSlave2.setCandidate(hadronSlave2.et(),hadronSlave2.ID()) ; 

  // First for Slave1, Slave2 and Master if same Et select the one with the smaller input ... (as for electromagnetic candidates) 
  for ( int kk=0 ; nbMasterSEL > kk ; kk++ ) {
    if ( allHadronsMaster[kk].et() == hadronMaster.et() && hadronMaster.et() != 0 && allHadronsMaster[kk].ID().all()  != hadronMaster.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      LHCb::CaloCellID caloCell = allHadronsMaster[kk].ID(); 
      int inputNumber_all = findInputNumberForHcal(allHadronsMaster[kk]) ; 
      int inputNumber = findInputNumberForHcal(theCorrectHadronMaster) ; 
      if (inputNumber_all < inputNumber ) theCorrectHadronMaster.setCandidate(allHadronsMaster[kk].et(),allHadronsMaster[kk].ID()) ; 
    }
  }

  for ( int kk=0 ; nbSlave1SEL > kk ; kk++ ) {
    if ( allHadronsSlave1[kk].et() == hadronSlave1.et() && hadronSlave1.et() != 0 && allHadronsSlave1[kk].ID().all()  != hadronSlave1.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      int inputNumber_all = findInputNumberForHcal(allHadronsSlave1[kk]) ; 
      int inputNumber = findInputNumberForHcal(theCorrectHadronSlave1) ; 
      if (inputNumber_all < inputNumber ) theCorrectHadronSlave1.setCandidate(allHadronsSlave1[kk].et(),allHadronsSlave1[kk].ID()) ; 
    }
  }

  for ( int kk=0 ; nbSlave2SEL > kk ; kk++ ) {
    if ( allHadronsSlave2[kk].et() == hadronSlave2.et() && hadronSlave2.et() != 0 && allHadronsSlave2[kk].ID().all()  != hadronSlave2.ID().all() ) { 
      // an other candidate with the same energy but different cell number...=> mimic the electronic      
      // behaviour look at the validation board # and at the cardSlot and search for the corresponding input#
      int inputNumber_all = findInputNumberForHcal(allHadronsSlave2[kk]) ; 
      int inputNumber = findInputNumberForHcal(theCorrectHadronSlave2) ; 
      if (inputNumber_all < inputNumber ) theCorrectHadronSlave2.setCandidate(allHadronsSlave2[kk].et(),allHadronsSlave2[kk].ID()) ; 
    }
  }

  // if Slave1 = Slave2 => save Slave1 
  L0Candidate theCorrectHadronSlave ( m_hcal );
  theCorrectHadronSlave.setCandidate(hadronSlave1.et(),hadronSlave1.ID()) ; 
  if (theCorrectHadronSlave1.et() >  theCorrectHadronSlave2.et() ) theCorrectHadronSlave.setCandidate(theCorrectHadronSlave1.et(),theCorrectHadronSlave1.ID()) ; 
  if (theCorrectHadronSlave1.et() <  theCorrectHadronSlave2.et() ) theCorrectHadronSlave.setCandidate(theCorrectHadronSlave2.et(),theCorrectHadronSlave2.ID()) ; 
  if (theCorrectHadronSlave1.et() == theCorrectHadronSlave2.et() ) theCorrectHadronSlave.setCandidate(theCorrectHadronSlave1.et(),theCorrectHadronSlave1.ID()) ; 

  L0Candidate theCorrectHadron ( m_hcal );
  theCorrectHadron.setCandidate(hadronSlave1.et(),hadronSlave1.ID()) ; 
  if (theCorrectHadronSlave.et() > theCorrectHadronMaster.et() ) theCorrectHadron.setCandidate(theCorrectHadronSlave.et(),theCorrectHadronSlave.ID()) ; 
  if (theCorrectHadronSlave.et() < theCorrectHadronMaster.et() ) theCorrectHadron.setCandidate(theCorrectHadronMaster.et(),theCorrectHadronMaster.ID()) ; 
  if (theCorrectHadronSlave.et() == theCorrectHadronMaster.et() ) theCorrectHadron.setCandidate(theCorrectHadronMaster.et(),theCorrectHadronMaster.ID()) ; 

  theCorrectHadron.saveCandidate(    L0DUBase::Fiber::CaloHadron,    L0Calo );


  unsigned int code = 0x10000 + (sumEt << L0DUBase::Calo::Sum::Shift);    
  LHCb::L0ProcessorData* hsum = 
    new LHCb::L0ProcessorData ( L0DUBase::Fiber::CaloSumEt, code );
  L0Calo->add( hsum );

  // Spd multiplicity counter

  code = 0x10000 + (m_spdMult << L0DUBase::Calo::Sum::Shift);
  LHCb::L0ProcessorData* spdMult = 
    new LHCb::L0ProcessorData ( L0DUBase::Fiber::CaloSpdMult, code );
  L0Calo->add( spdMult);

  // Debug now the L0 candidates

  LHCb::L0ProcessorDatas::const_iterator item;

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "== L0ProcessorData Summary: "
            << L0Calo->size() << " entries." << endreq;
    for( item = L0Calo->begin() ; L0Calo->end() != item ; ++item ) {
      LHCb::L0ProcessorData* cand = (*item);
      debug() << format( "Key %2d Word %8x", cand->key(), cand->word() ) 
              << endreq;
    }
  }
  
  //=== Store the perValidation candidates, only in Raw Event

  int IO = 0; // =0 input   =1 output 
  int slave = 0 ; // 0 all selection board except hadron ones 
  int mask = 0 ; // always on MC used only in real data 

  // Save all SEL boards info. 
  IO = 0 ; 
  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Electron, 
                    allElectrons[kk], 1 );
  }
  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(IO,slave,mask,  L0DUBase::CaloType::Photon, 
                   allPhotons[kk], 1 );
  }

  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(IO,slave,mask,  L0DUBase::CaloType::Pi0Local, 
                   allPi0Local[kk], 1 );
  }
  for ( int kk=0 ; m_nbValidation > kk ; kk++ ) {
    saveInRawEvent(IO,slave,mask,  L0DUBase::CaloType::Pi0Global, 
                   allPi0Global[kk], 1 );
  }


  //Save the candidates ... 
  IO = 1 ; 
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Electron, theCorrectElectron, 1 );
  saveInRawEvent(IO,slave,mask,  L0DUBase::CaloType::Photon, theCorrectPhoton, 1 );
  saveInRawEvent(IO,slave,mask,  L0DUBase::CaloType::Pi0Local, theCorrectPi0Local, 1 );
  saveInRawEvent(IO,slave,mask,  L0DUBase::CaloType::Pi0Global, theCorrectPi0Global, 1 );

  //-----------------------------------------------------------------------------------
  //For the HCAL part 
  //-----------------------------------------------------------------------------------

  // Save all SEL boards info. 
  // Start with the 2 slaves (1 and 2) 
  //Slave 1
  IO = 0 ; 
  slave = 0 ; // 0 all selection board except hadron global info 
  mask = 0 ; // always on MC used only in real data 
  for ( int kk=0 ; nbSlave1SEL > kk ; kk++ ) {
    saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, 
                    allHadronsSlave1[kk], 1 );
  }
  //Save the local highest candidate 
  IO = 1 ; 
  slave = 1 ; // 0 all selection board except hadron global info 
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, theCorrectHadronSlave1, 1 );
  //Save the local sum 
  IO = 1 ; 
  slave = 1 ; // 0 all selection board except hadron global info 
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SumEt, sumEtSlave1, 1 );


  //Slave 2
  IO = 0 ; 
  slave = 0 ; // 0 all selection board except hadron global info 
  for ( int kk=0 ; nbSlave2SEL > kk ; kk++ ) {
    saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, 
                    allHadronsSlave2[kk], 1 );
  }
  //Save the local highest candidate 
  IO = 1 ; 
  slave = 2 ; // 0 all selection board except hadron global info 
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, theCorrectHadronSlave2, 1 );
  //Save the local sum 
  IO = 1 ; 
  slave = 2 ; // 0 all selection board except hadron global info 
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SumEt, sumEtSlave2, 1 );

  // Save now the master 
  IO = 0 ; 
  slave = 0 ; // 0 all selection board except hadron global info 
  mask = 0 ; // always on MC used only in real data 
  for ( int kk=0 ; nbMasterSEL > kk ; kk++ ) {
    saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, 
                    allHadronsMaster[kk], 1 );
  }
  // Highest from Slave 1 and 2
  IO = 0 ;
  slave = 1 ;
  mask = 0;
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, theCorrectHadronSlave1, 1 );
  slave = 2 ;
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, theCorrectHadronSlave2, 1 );
  // Sum from Slave 1 and 2 
  IO = 0 ;
  slave = 1 ;
  mask = 0;
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SumEt, sumEtSlave1, 1 );
  slave = 2 ;
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SumEt, sumEtSlave2, 1 );
  // Global info on Master 
  IO = 1 ;
  slave = 0 ;
  mask = 0;
  // Save the Highest of Master, Slave1 and Slave2 
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::Hadron, theCorrectHadron, 1 );

  // Compute the Sum of Master + Slave1 + Slave2 
  int outputSumEtMaster = sumEtSlave1 + sumEtSlave2 + sumEtMaster ;
  //store one candidate ... note that ID is meaningless ... 
  //  taken from the maximum candidate 
  // at maximum outputSumMaster should be equal to 2^14 - 1 = 16383
  if (outputSumEtMaster > 16383 ) outputSumEtMaster = 16383 ;
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SumEt,
                  outputSumEtMaster , 1 );
                  
  //---------------------------------------------------------------------------
  //End of the HCAL part 
  //---------------------------------------------------------------------------


  //---------------------------------------------------------------------------
  //For the SPD part 
  //---------------------------------------------------------------------------
  IO = 0 ; 
  slave = 0 ; // 0 all selection board except hadron global info 
  mask = 0 ; // always on MC used only in real data 

  for ( int kk=0 ; nbSpdMultCards > kk ; kk++ ) {
    saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SpdMult, 
                    kk, valMult[kk] , 1 );
  }
  IO = 1 ;
  saveInRawEvent( IO,slave,mask, L0DUBase::CaloType::SpdMult, 
                  totSpdMult , 1 );

  //---------------------------------------------------------------------------
  //End of the SPD part 
  //-------------------------------------------------6-------------------------


  //== Store in the  RAW buffer if required.
  if ( m_storeFlag ) {
    m_nbEvents++;
    m_totRawSize = m_totRawSize + m_rawOutput[0].size() + m_rawOutput[1].size();
    LHCb::RawEvent* raw = 
      get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    raw->addBank( 0, LHCb::RawBank::L0Calo, m_bankVersion , m_rawOutput[0] );
    raw->addBank( 1, LHCb::RawBank::L0Calo, m_bankVersion , m_rawOutput[1] );
  } else {
    std::string name     = rootInTES() + LHCb::L0CaloCandidateLocation::Default;
    std::string nameFull = rootInTES() + LHCb::L0CaloCandidateLocation::Full;
    m_bankToTES->convertRawBankToTES( m_rawOutput, nameFull, name , m_bankVersion );
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
    if( m_ecal->isPinCard(eCard) )continue;// reject pin readout FE-cards
    m_ecalFe[eCard].reset( );
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
    
    m_ecalFe[card].addEt( col, row, adc);
    if ( (0 == row) && (0 <= down)    ) {
      m_ecalFe[down].addEt  ( col,          nRowCaloCard, adc);
    }
    if ( (0 == col) && (0 <= left)    ) {
      m_ecalFe[left].addEt  ( nColCaloCard, row,          adc);
    }
    if ( (0 == col) && (0 == row) && (0 <= corner)  ) {
      m_ecalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc); 
    }
  }
}
//=============================================================================
// Sum the Hcal digits into the Fe cards
//=============================================================================

void L0CaloAlg::sumHcalData( ) {

  for( int hCard = 0; m_hcal->nCards() > hCard;  ++hCard ) {
    if( m_hcal->isPinCard(hCard) )continue;// reject pin readout FE-cards
    m_hcalFe[hCard].reset( );
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
      verbose() << id 
                << format( " adc %3d card%3d row%3d col%3d, down%3d left%3d corner%3d", 
                           adc, card, row, col, down, left, corner ) << endreq;
    }

    m_hcalFe[card].addEt( col , row , adc ) ;    
    if ( (0 == row) && (0 <= down) ) {
      if ( ( ( m_hcal -> cardFirstColumn( card ) + col ) >=
             m_hcal -> cardFirstValidColumn( down ) ) &&
           ( ( m_hcal -> cardFirstColumn( card ) + col ) <=
             m_hcal -> cardLastValidColumn( down ) ) ) 
          m_hcalFe[down].addEt  ( col,          nRowCaloCard, adc); 
    }
    if ( (0 == col) && (0 <= left) ) {
      if ( ( ( m_hcal -> cardFirstRow( card ) + row ) >=
             m_hcal -> cardFirstValidRow( left ) ) && 
           ( ( m_hcal -> cardFirstRow( card ) + row ) <=
             m_hcal -> cardLastValidRow( left ) ) ) 
          m_hcalFe[left].addEt  ( nColCaloCard, row,          adc) ;
    }
    if ( (0 == col) && (0 == row) && (0 <= corner)) 
          m_hcalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc) ;
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

  std::vector<LHCb::CaloCellID>& ids = m_PrsSpdIds.first;

  debug() << "Found " << ids.size() << " PRS bits" << endreq;

  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;

    if ( MSG::VERBOSE >= msgLevel() ) verbose() << id << endreq;
    
    m_ecal->cardAddress(id, card, row, col );
    m_ecal->cardNeighbors( card, down, left, corner );
    
    m_ecalFe[card].setPrs( col, row );
    if ( (0 == row) && (0 <= down) ) {
      m_ecalFe[down].setPrs(   col,          nRowCaloCard );
    }
    if ( (0 == col) && (0 <= left) ) {
      m_ecalFe[left].setPrs(   nColCaloCard, row          );
    }
    if ( (0 == col) && (0 == row) && (0 <= corner) ) {
      m_ecalFe[corner].setPrs( nColCaloCard, nRowCaloCard );
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

  std::vector<LHCb::CaloCellID>& ids = m_PrsSpdIds.second;


  debug() << "Found " << ids.size() << " SPD bits" << endreq;

  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;

    if ( MSG::VERBOSE >= msgLevel() ) verbose() << id << endreq;
    
    m_spdMult++;

    m_ecal->cardAddress(id, card, row, col );
    m_ecal->cardNeighbors( card, down, left, corner );
    
    m_ecalFe[card].setSpd( col, row );
    if ( (0 == row) && (0 <= down) ) {
      m_ecalFe[down].setSpd( col, nRowCaloCard );
    }
    if ( (0 == col) && (0 <= left) ) {
      m_ecalFe[left].setSpd( nColCaloCard, row );
    }
    if ( (0 == col) && (0 == row) && (0 <= corner) ) {
      m_ecalFe[corner].setSpd( nColCaloCard, nRowCaloCard );
    }
  }
}

//=========================================================================
//  Save a candidate in the Raw buffer
//=========================================================================
void L0CaloAlg::saveInRawEvent ( int IO, int slave, int mask, int type, 
                                 int identif,int SpdMult , 
                                 unsigned int bank ) {
  //== Coding for Raw: IO (upper 1 bit) , Slave (2 bits) ,
  // Mask (1 bit) , type (4 bits), id (4 bits) and et (low 10 bits). 
  if ( 0 == SpdMult ) return;
  int word ; 
  if (type != L0DUBase::CaloType::SpdMult ) {
    warning()<<"Should be of type CaloSpdMult and is of type " 
             << type << endmsg;
  }
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "== L0CaloAlg saveInRawEvent " << " IO= " << IO 
            << " slave= " << slave << " mask= " << mask 
            << "type= " << type << " Identif= " << identif
            << " SpdMutl= " << SpdMult << endreq ;
  }

  word = ( IO << 31 ) + ( slave << 29 ) + ( mask << 28 ) + 
    ( type << 24 ) + (identif << 10 ) + SpdMult;
  m_rawOutput[bank].push_back( word );
}

//=====================================================================
void L0CaloAlg::saveInRawEvent ( int IO, int slave, int mask, int type, 
                                 L0Candidate& cand, 
                                 unsigned int bank ) {
  //== Coding for Raw: IO (upper 1 bit) , Slave (2 bits) , 
  // Mask (1 bit) , type (4 bits), id (16 bits) and et (low 8 bits). 
  if ( 0 == cand.et() ) return;
  int word ; 
  if (L0DUBase::CaloType::SumEt == type ) {
    warning()<<"Should NOT be of type CaloSumEt .... " << endmsg ;
  }
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "== L0CaloAlg saveInRawEvent " <<" IO= "<<IO
            <<" slave= "<<slave<<" mask= "<<mask<<"type= "
            <<type<< " Index= "<<cand.ID().index()<<" cell ID " 
            <<cand.ID()<<" Et= "<<cand.et()<< endreq;
  }

  word = ( IO << 31 ) + ( slave << 29 ) + ( mask << 28 ) + 
    ( type << 24 ) + (cand.ID().index() << 8 ) + cand.et();
  m_rawOutput[bank].push_back( word );
}

//========================================================================
void L0CaloAlg::saveInRawEvent ( int IO, int slave, int mask, int type, 
                                 int et, unsigned int bank ) {
  //== Coding for Raw: IO (upper 1 bit) , Slave (2 bits) , 
  // Mask (1 bit) , type (4 bits), id (16 bits) and et (low 8 bits). 
  if ( 0 == et ) return;
  int word ; 
  if (L0DUBase::CaloType::SumEt == type || 
      L0DUBase::CaloType::SpdMult == type) {
    if ( msgLevel( MSG::DEBUG ) ) {
      if (L0DUBase::CaloType::SumEt == type ) 
        debug() << "== L0CaloAlg saveInRawEvent " <<" IO= "<<IO
                <<" slave= "<<slave<<" mask= "<<mask
                <<"type= "<<type<< " Et= "<<et << endreq;
      if (L0DUBase::CaloType::SpdMult == type ) 
        debug() << "== L0CaloAlg saveInRawEvent " <<" IO= "<<IO
                <<" slave= "<<slave<<" mask= "<<mask<<"type= "<<type<<
          " TotSpdMult= "<<et << endreq;
    }
    word = ( IO << 31 ) + ( slave << 29 ) + ( mask << 28 ) + 
      ( type << 24 ) + et;
    m_rawOutput[bank].push_back( word );
  } else {
    warning() << "Should be of type CaloSumEt or CaloSpdMult.... " 
              << endmsg ;
  }
}
//=====================================================================
int L0CaloAlg::findInputNumberForEcal ( L0Candidate& cand ) { 
  LHCb::CaloCellID caloCell =  cand.ID(); 
  int card  = m_ecal->cardNumber( caloCell) ; 
  int cardSlot = m_ecal->cardSlot(card) ; 
  int crateNumber = m_ecal->cardCrate(card); 
  int valBoardExt = 0 ; 
  if (cardSlot >= 8 ) valBoardExt = 1 ; 
  // LUT filled by hand to link  crateNum.valBoardExt and #input       
  int inputNumber = m_EcalLUT[crateNumber][valBoardExt] ; 
  //  info()<<"cellID= "<<caloCell<<" cardSlot= "<<cardSlot<<" crate= "<<crateNumber<<" valBoardExt= "<<valBoardExt<<" inputNumber= "<<inputNumber<<endreq ;       
  return inputNumber ; 
}
//=====================================================================
int L0CaloAlg::findInputNumberForHcal ( L0Candidate& cand ) { 
  LHCb::CaloCellID caloCell =  cand.ID(); 
  int card  = m_hcal->cardNumber( caloCell) ; 
  int cardSlot = m_hcal->cardSlot(card) ; 
  int crateNumber = m_hcal->cardCrate(card); 
  // LUT filled by hand to link HcalCrate and HcalSlot and #input       
  //  info()<<"cellID= "<<caloCell<<" cardSlot= "<<cardSlot<<" crate= "<<crateNumber<<" card = "<<card<<endreq ; 
  int inputNumber = m_HcalLUT[crateNumber][cardSlot] ; 
  return inputNumber ; 
}
//=====================================================================
// Functions of the auxilliary class L0Candidate
//=====================================================================
void  L0Candidate::setCandidate( int et, LHCb::CaloCellID ID ) {
  m_ID     = ID;
  if ( 255 >= et ) { m_et = et; } else { m_et = 255; }

  // If the cluster is identified by a non-existent cell, one uses the other
  // cells of the cluster in the order:
  // the top cell: (+1,0)
  // the right cell: (0,+1) if the top does not exist
  // the corner cell: (+1,+1) if the two others do not exist
  
  if ( !m_det->valid( ID ) ) {

    LHCb::CaloCellID topCell( ID.calo() , ID.area() , ID.row()+1 , 
                              ID.col() ) ;
    m_ID = topCell ;
    
    if ( ! m_det -> valid( topCell ) ) {
      LHCb::CaloCellID rightCell( ID.calo() , ID.area() , ID.row() , 
                                  ID.col()+1 );
      m_ID = rightCell ;
      
      if ( ! m_det -> valid( rightCell ) ) {
        LHCb::CaloCellID cornerCell( ID.calo() , ID.area() , ID.row() + 1 , 
                                     ID.col() + 1 ) ;
        m_ID = cornerCell ;
      }
    }
  }
};

//=========================================================================
// Utility function to create ECAL LUT from validation board number to input#
//=========================================================================
void L0CaloAlg::createECALLutVB( ) {  
  m_EcalLUT[8][0] = 1 ; 
  m_EcalLUT[9][0] = 2 ; 
  m_EcalLUT[13][0] = 3; 
  m_EcalLUT[14][0] = 4; 
  m_EcalLUT[13][1] = 5; 
  m_EcalLUT[14][1] = 6; 
  m_EcalLUT[8][1] = 7; 
  m_EcalLUT[9][1] = 8; 
  m_EcalLUT[11][0] = 9; 
  m_EcalLUT[12][0] = 10; 
  m_EcalLUT[11][1] = 11; 
  m_EcalLUT[12][1] = 12; 
  m_EcalLUT[20][0] = 13; 
  m_EcalLUT[21][0] = 14; 
  m_EcalLUT[15][0] = 15; 
  m_EcalLUT[16][0] = 16; 
  m_EcalLUT[15][1] = 17; 
  m_EcalLUT[16][1] = 18; 
  m_EcalLUT[20][1] = 19; 
  m_EcalLUT[21][1] = 20; 
  m_EcalLUT[17][1] = 21; 
  m_EcalLUT[18][1] = 22; 
  m_EcalLUT[17][0] = 23; 
  m_EcalLUT[18][0] = 24; 
  m_EcalLUT[10][0] = 25; 
  m_EcalLUT[10][1] = 26; 
  m_EcalLUT[19][0] = 27; 
  m_EcalLUT[19][1] = 28; 

}
//=========================================================================
// Utility function to create HCAL LUT from validation board number to input#
//=========================================================================
void L0CaloAlg::createHCALLutVB( ) {  
  m_HcalLUT[22][2] = 1 ; 
  m_HcalLUT[22][3] = 3 ; 
  m_HcalLUT[22][4] = 5; 
  m_HcalLUT[22][5] = 7; 
  m_HcalLUT[22][6] = 9; 
  m_HcalLUT[22][7] = 11; 
  m_HcalLUT[22][8] = 13; 
  m_HcalLUT[22][9] = 5; 
  m_HcalLUT[22][10] = 9; 
  m_HcalLUT[22][11] = 14; 
  m_HcalLUT[22][12] = 18; 

  m_HcalLUT[23][1] = 13 ; 
  m_HcalLUT[23][2] = 15 ; 
  m_HcalLUT[23][3] = 17 ; 
  m_HcalLUT[23][4] = 19; 
  m_HcalLUT[23][5] = 21; 
  m_HcalLUT[23][6] = 23; 
  m_HcalLUT[23][7] = 6; 
  m_HcalLUT[23][8] = 17; 
  m_HcalLUT[23][9] = 3; 
  m_HcalLUT[23][10] = 25; 
  m_HcalLUT[23][11] = 1; 
  m_HcalLUT[23][12] = 27; 
  m_HcalLUT[23][13] = 4; 
  m_HcalLUT[23][14] = 10; 

  m_HcalLUT[24][1] = 15 ; 
  m_HcalLUT[24][2] = 21 ; 
  m_HcalLUT[24][3] = 19 ; 
  m_HcalLUT[24][4] = 23; 
  m_HcalLUT[24][5] = 21; 
  m_HcalLUT[24][6] = 22; 
  m_HcalLUT[24][7] = 8; 
  m_HcalLUT[24][8] = 19; 
  m_HcalLUT[24][9] = 25; 
  m_HcalLUT[24][10] = 27; 
  m_HcalLUT[24][11] = 13; 
  m_HcalLUT[24][12] = 15; 
  m_HcalLUT[24][13] = 17; 
  m_HcalLUT[24][14] = 12; 


  m_HcalLUT[25][2] = 7 ; 
  m_HcalLUT[25][3] = 11 ; 
  m_HcalLUT[25][4] = 16; 
  m_HcalLUT[25][5] = 20; 
  m_HcalLUT[25][6] = 1; 
  m_HcalLUT[25][7] = 3; 
  m_HcalLUT[25][8] = 5; 
  m_HcalLUT[25][9] = 7; 
  m_HcalLUT[25][10] = 9; 
  m_HcalLUT[25][11] = 11; 
  m_HcalLUT[25][12] = 23; 


}
//=========================================================================
// Create HCAL LUT
//=========================================================================
void L0CaloAlg::createHCALLut( ) {  
  std::map< int , std::vector< int > > validMap ;
  validMap[ 0 ].push_back(0) ; validMap[ 0 ].push_back(1) ; validMap[ 0 ].push_back(2) ; validMap[ 0 ].push_back(3) ;
  validMap[ 1 ].push_back(4) ; validMap[ 1 ].push_back(5) ; validMap[ 1 ].push_back(6) ; validMap[ 1 ].push_back(-1) ;
  validMap[ 2 ].push_back(0) ; validMap[ 2 ].push_back(1) ; validMap[ 2 ].push_back(2) ; validMap[ 2 ].push_back(3) ;
  validMap[ 3 ].push_back(4) ; validMap[ 3 ].push_back(5) ; validMap[ 3 ].push_back(6) ; validMap[ 3 ].push_back(-1) ;
  validMap[ 4 ].push_back(7) ; validMap[ 4 ].push_back(8) ; validMap[ 4 ].push_back(11) ; validMap[ 4 ].push_back(18) ;
  validMap[ 5 ].push_back(17) ; validMap[ 5 ].push_back(24) ; validMap[ 5 ].push_back(9) ; validMap[ 5 ].push_back(10) ;
  validMap[ 6 ].push_back(12) ; validMap[ 6 ].push_back(13) ; validMap[ 6 ].push_back(14) ; validMap[ 6 ].push_back(-1) ;
  validMap[ 7 ].push_back(15) ; validMap[ 7 ].push_back(16) ; validMap[ 7 ].push_back(19) ; validMap[ 7 ].push_back(-1) ;
  validMap[ 8 ].push_back(12) ; validMap[ 8 ].push_back(13) ; validMap[ 8 ].push_back(14) ; validMap[ 8 ].push_back(-1) ;
  validMap[ 9 ].push_back(15) ; validMap[ 9 ].push_back(16) ; validMap[ 9 ].push_back(-1) ; validMap[ 9 ].push_back(23) ;
  validMap[ 10 ].push_back(20) ; validMap[ 10 ].push_back(-1) ; validMap[ 10 ].push_back(-1) ; validMap[ 10 ].push_back(-1) ;
  validMap[ 11 ].push_back(22) ; validMap[ 11 ].push_back(21) ; validMap[ 11 ].push_back(-1) ; validMap[ 11 ].push_back(-1) ;
  validMap[ 12 ].push_back(20) ; validMap[ 12 ].push_back(21) ; validMap[ 12 ].push_back(-1) ; validMap[ 12 ].push_back(-1) ;
  validMap[ 13 ].push_back(22) ; validMap[ 13 ].push_back(-1) ; validMap[ 13 ].push_back(-1) ; validMap[ 13 ].push_back(-1) ;
  validMap[ 14 ].push_back(27) ; validMap[ 14 ].push_back(-1) ; validMap[ 14 ].push_back(-1) ; validMap[ 14 ].push_back(-1) ;
  validMap[ 15 ].push_back(29) ; validMap[ 15 ].push_back(28) ; validMap[ 15 ].push_back(-1) ; validMap[ 15 ].push_back(-1) ;
  validMap[ 16 ].push_back(27) ; validMap[ 16 ].push_back(28) ; validMap[ 16 ].push_back(-1) ; validMap[ 16 ].push_back(-1) ;
  validMap[ 17 ].push_back(29) ; validMap[ 17 ].push_back(-1) ; validMap[ 17 ].push_back(-1) ; validMap[ 17 ].push_back(-1) ;
  validMap[ 18 ].push_back(26) ; validMap[ 18 ].push_back(33) ; validMap[ 18 ].push_back(34) ; validMap[ 18 ].push_back(-1) ;
  validMap[ 19 ].push_back(35) ; validMap[ 19 ].push_back(36) ; validMap[ 19 ].push_back(37) ; validMap[ 19 ].push_back(-1) ;
  validMap[ 20 ].push_back(-1) ; validMap[ 20 ].push_back(33) ; validMap[ 20 ].push_back(34) ; validMap[ 20 ].push_back(30) ;
  validMap[ 21 ].push_back(35) ; validMap[ 21 ].push_back(36) ; validMap[ 21 ].push_back(37) ; validMap[ 21 ].push_back(-1) ;
  validMap[ 22 ].push_back(39) ; validMap[ 22 ].push_back(40) ; validMap[ 22 ].push_back(25) ; validMap[ 22 ].push_back(32) ;
  validMap[ 23 ].push_back(31) ; validMap[ 23 ].push_back(38) ; validMap[ 23 ].push_back(41) ; validMap[ 23 ].push_back(42) ;
  validMap[ 24 ].push_back(43) ; validMap[ 24 ].push_back(44) ; validMap[ 24 ].push_back(45) ; validMap[ 24 ].push_back(46) ;
  validMap[ 25 ].push_back(47) ; validMap[ 25 ].push_back(48) ; validMap[ 25 ].push_back(49) ; validMap[ 25 ].push_back(-1) ;
  validMap[ 26 ].push_back(43) ; validMap[ 26 ].push_back(44) ; validMap[ 26 ].push_back(45) ; validMap[ 26 ].push_back(46) ;
  validMap[ 27 ].push_back(47) ; validMap[ 27 ].push_back(48) ; validMap[ 27 ].push_back(49) ; validMap[ 27 ].push_back(-1) ;    
        
  for ( int valid = 0 ; valid < 28 ; ++valid ) {
    for ( int input = 0 ; input < 4 ; ++input ) {
      std::ostringstream fileNameStr ;
      fileNameStr << "Valid_" << valid << "_HCAL" << input << ".txt" ;
      std::string fileName = fileNameStr.str() ; 
      std::ofstream theFile( fileName.c_str() ) ;
      theFile << "Valid " << valid << " LUT for HCAL" << input << std::endl ;
      if ( -1 == (validMap[ valid ])[ input ] ) {
        for ( int ecalCard = 0 ; ecalCard < 8 ; ++ecalCard ) {
          for ( int ecalAdd = 0 ; ecalAdd < 32 ; ++ecalAdd ) {
            for ( int hcalAdd = 0 ; hcalAdd < 32 ; ++hcalAdd ) {
               theFile << "0 " ;                
            }
            theFile << std::endl ;
          }
          theFile << "--------------------------------------------------------" << std::endl ;
        }
      } else { 
        for ( int ecalCard = 0 ; ecalCard < 8 ; ++ecalCard ) {
          // Find the corresponding ECAL card number:
          int theCard = -1 ;
          for ( int eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
            if ( ( m_ecalFe[eCard].validationNumber() == valid ) && ( m_ecal->cardSlot( eCard ) % 8 == ecalCard ) ) {
              theCard = eCard ;
              break ;
            }
          } 
          
          // No card connected to this input --> all zeros
          if ( theCard == -1 ) {
            for ( int ecalAdd = 0 ; ecalAdd < 32 ; ++ecalAdd ) {
              for ( int hcalAdd = 0 ; hcalAdd < 32 ; ++hcalAdd ) {
                theFile << "0 " ;                
              }
              theFile << std::endl ;
            }
          } else {
            // Check if this board is in front of the HCAL board
            if ( m_ecalFe[theCard].hcalCard() == (validMap[ valid ])[input] ) {
              for ( int ecalAdd = 0 ; ecalAdd < 32 ; ++ecalAdd ) {
                for ( int hcalAdd = 0 ; hcalAdd < 32 ; ++hcalAdd ) {
                
                  m_ecalFe[theCard].reset() ;
                  m_ecalFe[theCard].addEt( 1 + ecalAdd % 8 , 1 + (int) ecalAdd / 8 , 1 ) ;
                  theFile << m_ecalFe[theCard].match_included( hcalAdd % 8 , (int) hcalAdd / 8 ) << " " ;                
                  
                }
                theFile << std::endl ;
              }              
            } else {
              for ( int ecalAdd = 0 ; ecalAdd < 32 ; ++ecalAdd ) {
                for ( int hcalAdd = 0 ; hcalAdd < 32 ; ++hcalAdd ) {
                  theFile << "0 " ;                
                }
                theFile << std::endl ;
              }         
            }
          }
    
          // Print separator         
          theFile << "--------------------------------------------------------" << std::endl ;         
        }
      } 
      theFile.close() ;
    }
  }
}  
