/// local
#include "L0CaloAlg.h"

/// STL
#include <fstream>
#include <numeric>

// From Event
#include "Event/L0CaloCandidate.h"
#include "Event/L0CaloAdc.h"

// From Kernel
#include "Kernel/CaloCellID.h"

// From Calo
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"

// local
#include "L0CaloCandidatesFromRawBank.h"

//
//  Level-0 calorimeter trigger simulation algorithm
//  Compute L0Calo quantities from Calorimeter full information
//

DECLARE_ALGORITHM_FACTORY( L0CaloAlg )

// Temporary, to be moved to confDB
int L0CaloAlg::s_ecalLUT[ 14 ][ 2 ] = {
  { 1 , 7 } , 
  { 2 , 8 } , 
  { 25 , 26 } , 
  { 9 , 11 } , 
  { 10 , 12 } , 
  { 3 , 5 } , 
  { 4 , 6 } , 
  { 15 , 17 } , 
  { 16 , 18 } , 
  { 23 , 21 } , 
  { 24 , 22 } , 
  { 27 , 28 } , 
  { 13 , 19 } , 
  { 14 , 20 }
} ;
 
int L0CaloAlg::s_hcalLUT[ 4 ][ 16 ] = {
  { 0 , 0  , 1  , 3  , 5  , 7  , 9  , 11 , 
    13 , 5  , 9  , 14 , 18 , 0  , 0  , 0 } , 
  { 0 , 13 , 15 , 17 , 19 , 21 , 23 , 6  , 
    17 , 3  , 25 , 1  , 27 , 4  , 10 , 0 } ,
  { 0 , 15 , 21 , 19 , 23 , 21 , 22 , 8  , 
    19 , 25 , 27 , 13 , 15 , 17 , 12 , 0 } ,
  { 0 , 0  , 7  , 11 , 16 , 20 , 1  , 3  , 
    5  , 7  , 9  , 11 , 23 , 0  , 0  , 0 }
} ; 

//=============================================================================
// Constructor
//=============================================================================
L0CaloAlg::L0CaloAlg( const std::string & name , ISvcLocator * pSvcLocator)
  : L0AlgBase( name , pSvcLocator )
  , m_usePsSpd( true )
  , m_addEcalToHcal( true )
  , m_ecal(NULL)
  , m_hcal(NULL)
  , m_prs(NULL)
  , m_validPrs()
  , m_validPrsInner()
  , m_adcsEcal(NULL)
  , m_adcsHcal(NULL)
  , m_bitsFromRaw(NULL)
  , m_bankToTES(NULL)
  , m_rawOutput  ( 2 )
  , m_totRawSize(0)
  , m_nbEvents(0)
  , m_bankVersion( 1 ) 
  , m_nbValidation( 0 )
  , m_l0Cond( 0 ) {
  declareProperty( "CreateHCALLut"   , m_createHCALLut      = false    ) ;
  declareProperty( "UsePSSPD"        , m_usePsSpdOpts       = true     ) ;
  declareProperty( "AddECALToHCAL"   , m_addEcalToHcalOpts  = true     ) ;
  declareProperty( "UseNewElectron"  , m_newElectron        = true     ) ;
  declareProperty( "HcalThreshold"   , m_hcalThreshold      = 8        ) ;
  declareProperty( "EcalThreshold"   , m_ecalThreshold      = 0        ) ;
  declareProperty( "MaskHotCells"    , m_maskHotCells       = false    ) ;
  declareProperty( "IgnoreDatabase"  , m_ignoreDatabase     = false    ) ;
  m_spdMult = std::vector< int >( 16 , 0 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloAlg::~L0CaloAlg() { }

//=============================================================================
// Initialization: Check parameters, get DetectorElement pointers, build
//                 the trigger cards and their connections (HCAL-ECAL)
//=============================================================================
StatusCode L0CaloAlg::initialize() {
  StatusCode sc = L0AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; 

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Warning m_ecalFE & m_hcalFE  HAVE to contain PIN-diode FEs
  // in order to ensure the card numbering is correct afterward.
  // Pin-diode FE are anyway excluded from the L0 processing in the code

  // Retrieve the ECAL detector element, and build cards
  m_ecal = getDet< DeCalorimeter >( DeCalorimeterLocation::Ecal ) ;  
  int eCard , hCard , validationNb ;  
  m_ecalFe.reserve( m_ecal -> nCards() ) ;
  
  for ( eCard = 0 ; eCard < m_ecal -> nCards() ; ++eCard ) {
    m_ecalFe.push_back( TriggerCard( eCard , m_ecal ) ) ;
    validationNb = m_ecal -> validationNumber( eCard ) ;    
    if ( m_nbValidation <= validationNb ) m_nbValidation = validationNb + 1 ;
    if( !( m_ecal -> isPinCard( eCard ) ) && !( m_ecal -> isParasiticCard( eCard ) ) )
      m_ecalFe[ eCard ].setValidationNumber( validationNb ) ;
  }  
  
  // Retrieve the HCAL detector element, and build cards
  m_hcal = getDet< DeCalorimeter >( DeCalorimeterLocation::Hcal ) ;  
  m_hcalFe.reserve( m_hcal -> nCards() ) ;
  
  for ( hCard = 0 ; hCard < m_hcal->nCards() ; ++hCard )
    m_hcalFe.push_back( TriggerCard( hCard , m_hcal ) ) ;

  // Get PRS Detector Element
  m_prs = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );  

  // Get Condition AddEcalToHcal to set correct processing in the
  // L0Calo emulation
  try {
    registerCondition( "Conditions/Calibration/Ecal/L0Calibration" ,
                       m_l0Cond ,
                       &L0CaloAlg::updateL0Calibration ) ;
  } catch ( GaudiException &e ) {
    error() << e << endmsg ;
    error() << "Cannot access Ecal/Calibration/L0Calibration condition" 
            << endmsg ;
    error() << "Run with options configuration: " << endmsg ;
    m_usePsSpd = m_usePsSpdOpts ;
    m_addEcalToHcal = m_addEcalToHcalOpts ;
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

  double zRatio = 
    m_hcal->cellSize( m_hcal->firstCellID( 1 ) ) /
    m_ecal->cellSize( m_ecal->firstCellID( 0 ) ) / 2. ;

  for ( eCard = 0 ; eCard < m_ecal -> nCards() ; ++eCard ) {
    // Reject pin cards
    if( m_ecal -> isPinCard( eCard ) || m_ecal -> isParasiticCard( eCard ) ) continue ;
    
    LHCb::CaloCellID ecalID  = m_ecal -> firstCellID( eCard ) ;
    Gaudi::XYZPoint  center  = m_ecal -> cellCenter( ecalID ) * zRatio ;
    LHCb::CaloCellID hcalID  = m_hcal -> Cell( center ) ;
    int hCard = m_hcal -> cardNumber( hcalID ) ;

    // Use last cell if first doesn't work
    if ( 0 > hCard ) {
      ecalID = m_ecal -> lastCellID( eCard ) ;
      center = m_ecal -> cellCenter( ecalID ) * zRatio ;
      hcalID = m_hcal -> Cell( center ) ;
      hCard  = m_hcal -> cardNumber( hcalID ) ;
    }

    // Compute the ratio of cell sizes (integer) and the offset
    // so that one can compute (relative in the card)
    //     ecalCellRow = hcalCellRow * mag + OffsetRow
    //     ecalCellCol = hcalCellCol * mag + OffsetCol
    if ( 0 <= hCard ) {
      int mag = (int) ( m_hcal -> cellSize( hcalID ) / 
                        m_ecal -> cellSize( ecalID ) ) ;
      int offsetCol = 
        ( m_hcal -> cardFirstColumn( hCard ) - 16 ) * mag +
        32 - m_ecal -> cardFirstColumn( eCard ) ;
      int offsetRow = 
        ( m_hcal -> cardFirstRow( hCard ) - 16 ) * mag +
        32 - m_ecal -> cardFirstRow( eCard ) ;
      m_ecalFe[ eCard ].setHcalParams( hCard , mag , offsetCol , offsetRow ) ;
      m_hcalFe[ hCard ].addEcalConnectedCard( eCard ) ;
    } else {
      warning() << "Ecal card " << eCard << " not connected to HCAL " 
                << endmsg ;
    }
  }

  // Debug the cards
  if ( msgLevel( MSG::DEBUG ) ) {      
    for ( eCard = 0 ;  eCard < m_ecal -> nCards() ; ++eCard ) {
      // Reject pin cards
      if( m_ecal -> isPinCard( eCard ) || m_ecal -> isParasiticCard( eCard ) ) continue ;

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
              << endmsg ;
    }
  
    for ( hCard=0 ;  m_hcal->nCards() > hCard; ++hCard ) {
      // reject pin cards
      if( m_hcal -> isPinCard( hCard ) || m_hcal -> isParasiticCard( hCard ) ) continue ;
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
      for ( eCard = 0; m_hcalFe[hCard].numberOfEcalCards() > eCard; 
            ++eCard ) {
        debug() << format( "%4d", m_hcalFe[hCard].ecalCardNumber( eCard ) );
      }
     debug() << endmsg;
   }
  }

  // Create LUT for the TVB at the pit in case it is needed  
  if ( m_createHCALLut ) createHCALLut( ) ;

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

  // PRS validation mask for the Inner part
  if ( ! m_newElectron ) {
    m_validPrsInner[0]  = 0;
    m_validPrsInner[1]  = 1;
    m_validPrsInner[2]  = 1;
    m_validPrsInner[3]  = 1;
    m_validPrsInner[4]  = 1;
    m_validPrsInner[5]  = 1;
    m_validPrsInner[6]  = 1;
    m_validPrsInner[7]  = 0;
    m_validPrsInner[8]  = 1;
    m_validPrsInner[9]  = 1;
    m_validPrsInner[10] = 1;
    m_validPrsInner[11] = 0;
    m_validPrsInner[12] = 1;
    m_validPrsInner[13] = 0;
    m_validPrsInner[14] = 0;
    m_validPrsInner[15] = 0;
  } else {
    m_validPrsInner[0]  = 0;
    m_validPrsInner[1]  = 1;
    m_validPrsInner[2]  = 1;
    m_validPrsInner[3]  = 1;
    m_validPrsInner[4]  = 1;
    m_validPrsInner[5]  = 1;
    m_validPrsInner[6]  = 1;
    m_validPrsInner[7]  = 1;
    m_validPrsInner[8]  = 1;
    m_validPrsInner[9]  = 1;
    m_validPrsInner[10] = 1;
    m_validPrsInner[11] = 1;
    m_validPrsInner[12] = 1;
    m_validPrsInner[13] = 1;
    m_validPrsInner[14] = 1;
    m_validPrsInner[15] = 1;
  }

  info() << m_ecal->nCards() << " Ecal and "
         << m_hcal->nCards() << " Hcal front end cards." << endmsg;

  // Numbers for bank size monitoring
  m_totRawSize = 0.;
  m_nbEvents   = 0 ;

  // Initialize needed tools
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
}

//=============================================================================
// Execute: Compute the calo trigger information
//=============================================================================
StatusCode L0CaloAlg::execute() {
  // Get the ECAL data, store them in the Front-End card
  sumEcalData( );

  // Get Spd+Prs data
  m_PrsSpdIds = m_bitsFromRaw -> prsSpdCells( ) ;

  if ( m_usePsSpd ) {
    // Get the Prs information. Adds it to the m_ecalFe[] objects
    addPrsData( ) ;
  }
  // Get the Spd information. Adds it to the m_ecalFe[] objects
  addSpdData( ) ;
  
  // Vector to store the per-TVB candidates
  std::vector< L0Candidate > 
    allElectrons( m_nbValidation , L0Candidate( m_ecal ) ) ,
    allPhotons  ( m_nbValidation , L0Candidate( m_ecal ) ) ,
    allPi0Local ( m_nbValidation , L0Candidate( m_ecal ) ) ,
    allPi0Global( m_nbValidation , L0Candidate( m_ecal ) ) ;

  // Loop on ECAL cards. Get the candidates, select the highest candidates 
  // per validation board
  int eCard , etMax( 0 ) , etTot( 0 ) , numVal( 0 ) , etMaxTot( 0 ) ;
  int prsMask( 0 ) , okPrs( 0 ) , spdMask( 0 ) ;
  int pCard( 0 ), diffRow( 0 ) , diffCol( 0 ) ;
  LHCb::CaloCellID ID , ID2 ;
  
  for( eCard = 0 ; m_ecal -> nCards() > eCard ; ++eCard ) {
    // reject pin readout FE-cards
    if( m_ecal->isPinCard(eCard) || m_ecal -> isParasiticCard( eCard ) ) continue ;
    
    etMax = m_ecalFe[ eCard ].etMax() ;
    etTot = m_ecalFe[ eCard ].etTot()  ;
    ID    = m_ecalFe[ eCard ].cellIdMax() ;
    numVal = m_ecalFe[eCard].validationNumber();

    // Validate ECAL by Prs/Spd, and select the highest electron and photon 
    // per TVB
    // Decision on Prs => electron/photon
    // SPD matching Prs => electron
    
    prsMask = m_ecalFe[ eCard ].prsMask() ;
    
    if ( ID.area() == 2 ) okPrs = m_validPrsInner[ prsMask ] ;
    else okPrs = m_validPrs[ prsMask ] ;
    
    spdMask = prsMask & m_ecalFe[ eCard ].spdMask() ;

    if ( m_usePsSpd ) {
      if (0 < okPrs) {
        if (0 < spdMask) {
          if ( allElectrons[ numVal ].et() < etMax ) 
            allElectrons[ numVal ].setCandidate( etMax , ID ) ;
        } else {
          if ( allPhotons[ numVal ].et() < etMax ) 
            allPhotons[numVal].setCandidate( etMax, ID );
        }
      }
      // Produces also the 'single card' pi0
      if ( 0 != prsMask ) {  //== Request some preshower activity
        if ( allPi0Local[ numVal ].et() < etTot ) 
          allPi0Local[ numVal ].setCandidate( etTot , ID ) ;
      }  
    } else {
      if ( allElectrons[ numVal ].et() < etMax ) 
        allElectrons[numVal].setCandidate( etMax , ID ) ;
      if ( allPhotons[ numVal ].et() < etMax ) 
        allPhotons[ numVal ].setCandidate( etMax , ID ) ;
      // Produces also the 'single card' pi0
      if ( allPi0Local[ numVal ].et() < etTot ) 
        allPi0Local[ numVal ].setCandidate( etTot, ID );
    }
    
    // Global pi0: the 'previous' card is in the same validation
    pCard = m_ecal -> previousCardNumber( eCard ) ;
    if ( 0 <= pCard ) {
      etMaxTot = etMax + m_ecalFe[ pCard ].etMax()  ;
      if ( etMaxTot > 255 ) etMaxTot = 255 ;
      ID2 = m_ecalFe[pCard].cellIdMax();
      diffRow = ID2.row() - ID.row();
      diffCol = ID2.col() - ID.col();
      //== Check that the ID are at least 2 rows or 2 columns apart so that 
      // they don't share a cell.
      if ( ( ID2.area() != ID.area() ) || ( 1 < abs( diffRow ) ) || 
           ( 1 < abs( diffCol ) ) ) 
        if ( allPi0Global[ numVal ].et() < etMaxTot ) 
          allPi0Global[numVal].setCandidate( etMaxTot, ID2 );
    }
  } // eCard

  //  Do a similar processing for HCAL Data
  sumHcalData( );
  
  // Add the highest ECAL energy in matching cards and store
  // the candidates (one per Selection Board input, in 3 vectors
  // one vector per SB: (0 - master, 1 - slave1, 2 - slave2)
  std::vector< std::vector< L0Candidate > > sbHcalInput( 3 ) ;
  // And the compute the sum per Selection Board (sum of the
  // inputs, removing the duplicates)
  std::vector< int > sbHcalOutputSum( 3 , 0 ) ;

  int hCard , eLink( 0 ) ;
  int maxHcalEt( 0 ) , ecalEt( 0 ) , hCol( 0 ) , hRow( 0 ) ;
  int firstColumn( 0 ) , firstValidRow( 0 ) , firstRow( 0 ) , 
    firstValidCol( 0 ) ;
  // tvbMap contains the candidate per TVB, since one HCAL board can talk
  // to two TVB ("duplicates")
  std::map< int , int >::const_iterator itTvb ;
  std::map< int , int > tvbMap ;
  std::vector< int > forSumEt( 2 , 0 ) ;
  int type ( 0 ) , tvbNumber( 0 ) ;
  
  for ( hCard = 0 ; hCard < m_hcal -> nCards() ; ++hCard ) {
    // reject pin readout FE-cards
    if ( m_hcal -> isPinCard( hCard ) || m_hcal -> isParasiticCard( hCard ) ) continue ;
    
    maxHcalEt = m_hcalFe[hCard].etMax() ;
    ID = m_hcalFe[ hCard ].cellIdMax() ;
    type = m_hcal -> selectionType( hCard ) ;
    
    tvbMap.clear() ;
    // Do not add if the HCAL energy is 0 and the address of the
    // maximum is not on row 0, column 0 (ie do not treat half boards
    // of the HCAL)
    if ( 0 == maxHcalEt ) {
      firstColumn   = m_hcal -> cardFirstColumn( hCard ) ;
      firstValidCol = m_hcal -> cardFirstValidColumn( hCard ) ;
      firstRow      = m_hcal -> cardFirstRow( hCard ) ;
      firstValidRow = m_hcal -> cardFirstValidRow( hCard ) ;
      if ( ( firstRow != firstValidRow ) || ( firstColumn != firstValidCol ) ) 
        continue ;
    }
    
    hCol = m_hcalFe[ hCard ].colMax();
    hRow = m_hcalFe[ hCard ].rowMax();
    for ( eLink = 0 ; eLink < m_hcalFe[ hCard ].numberOfEcalCards() ; 
          ++eLink ) {
      eCard = m_hcalFe[ hCard ].ecalCardNumber( eLink ) ;
      if ( m_ecalFe[ eCard ].match_included( hCol , hRow ) ) {
        if ( m_addEcalToHcal)
          ecalEt = m_ecalFe[ eCard ].etMax() ;
      } else ecalEt = 0 ;
      tvbNumber = m_ecalFe[ eCard ].validationNumber() ;
      // Now save the max Ecal candidate per TVB (there can be up to 2 
      // TVB linked to the HCAL board
      if ( tvbMap.find( tvbNumber ) == tvbMap.end() ) 
        tvbMap[ tvbNumber ] = ecalEt ;
      else
        if ( tvbMap[ tvbNumber ] < ecalEt ) 
          tvbMap[ tvbNumber ] = ecalEt ;
    }
    
    // Now save the per-SB candidates, and compute the per-SB sum, taking 
    // only the highest of the 2 duplicates
    forSumEt[ 0 ] = 0 ; forSumEt[ 1 ] = 0 ; 
    int i = 0 ;
    for ( itTvb = tvbMap.begin() ; itTvb != tvbMap.end() ; ++itTvb ) {
      L0Candidate cand( m_hcal ) ;
      cand.setCandidate( maxHcalEt + (*itTvb).second , ID ) ;
      if ( 0 != cand.et() ) sbHcalInput[ type ].push_back( cand ) ;
      forSumEt[ i ] = cand.et() ;
      ++i ;
    }
    sbHcalOutputSum[ type ] += std::max( forSumEt[ 0 ] , forSumEt[ 1 ] ) ; 
    if ( sbHcalOutputSum[ type ] > 16383 ) sbHcalOutputSum[ type ] = 16383 ;
  } // hCard
  
  // Save the candidates in CaloProcessor data location (for L0DU) 
  LHCb::L0ProcessorDatas* L0Calo = new LHCb::L0ProcessorDatas() ;
  std::string procName =  LHCb::L0ProcessorDataLocation::Calo; // NO L0-context location for procData
  put( L0Calo, procName ) ;
  
  // Candidate is maximum of all Selection Boards inputs. In case of equality, 
  // take the one with smallest input number (done in compareL0Candidate)
  L0Candidate electron( m_ecal ) , photon( m_ecal ) , 
    pi0Local( m_ecal ) , pi0Global ( m_ecal ) ;

  // Now remove the candidates with 0 energy
  std::vector< L0Candidate >::iterator itElectron , itPhoton , itPi0Local ,
     itPi0Global ;
  itElectron = std::remove_if( allElectrons.begin() , allElectrons.end() , 
                               L0Calo::zeroEt );
  itPhoton   = std::remove_if( allPhotons.begin()   , allPhotons.end()   , 
                               L0Calo::zeroEt );
  itPi0Local = std::remove_if( allPi0Local.begin()  , allPi0Local.end()  , 
                               L0Calo::zeroEt );
  itPi0Global = std::remove_if( allPi0Global.begin() , allPi0Global.end() , 
                                L0Calo::zeroEt );

  if ( ! allElectrons.empty() ) electron  =
    (*std::max_element( allElectrons.begin() , itElectron , 
                        L0Calo::compareL0Candidate ) ) ;
  if ( ! allPhotons.empty() ) photon    = 
    (*std::max_element( allPhotons.begin() , itPhoton , 
                        L0Calo::compareL0Candidate ) ) ;
  if ( ! allPi0Local.empty() ) pi0Local  = 
    (*std::max_element( allPi0Local.begin() , itPi0Local , 
                        L0Calo::compareL0Candidate ) ) ;
  if ( ! allPi0Global.empty() ) pi0Global = 
    (*std::max_element( allPi0Global.begin() , itPi0Global , 
                        L0Calo::compareL0Candidate ) ) ;
  
  // Store the various candidates
  electron.saveCandidate ( L0DUBase::Fiber::CaloElectron  , L0Calo ) ;
  photon.saveCandidate   ( L0DUBase::Fiber::CaloPhoton    , L0Calo ) ;
  pi0Local.saveCandidate ( L0DUBase::Fiber::CaloPi0Local  , L0Calo ) ;
  pi0Global.saveCandidate( L0DUBase::Fiber::CaloPi0Global , L0Calo ) ;

  // now compute the highest per SB
  L0Candidate hadronMaster( m_hcal ) , hadronSlave1( m_hcal ) ,
    hadronSlave2( m_hcal ) ;
  if ( ! sbHcalInput[ 0 ].empty() ) hadronMaster = 
    (*std::max_element( sbHcalInput[ 0 ].begin() , sbHcalInput[ 0 ].end() , 
                        L0Calo::compareL0Candidate ) ) ;
  if ( ! sbHcalInput[ 1 ].empty() ) hadronSlave1 = 
    (*std::max_element( sbHcalInput[ 1 ].begin() , sbHcalInput[ 1 ].end() , 
                        L0Calo::compareL0Candidate ) ) ;
  if ( ! sbHcalInput[ 2 ].empty() ) hadronSlave2 = 
    (*std::max_element( sbHcalInput[ 2 ].begin() , sbHcalInput[ 2 ].end() , 
                        L0Calo::compareL0Candidate ) ) ;

  // find the highest hadron
  // with priority: Master, Slave1 , Slave2
  L0Candidate hadron( m_hcal )  ;
  if ( ( hadronMaster.et() >= hadronSlave1.et() ) && 
       ( hadronMaster.et() >= hadronSlave2.et() ) ) 
    hadron = hadronMaster ;
  else if ( hadronSlave1.et() >= hadronSlave2.et() ) 
    hadron = hadronSlave1 ;
  else
    hadron = hadronSlave2 ;
  
  // save HCAL candidate
  hadron.saveCandidate( L0DUBase::Fiber::CaloHadron , L0Calo ) ;

  // save sum Et
  int sumEt = 
    std::accumulate( sbHcalOutputSum.begin() , sbHcalOutputSum.end() , 0 ) ;
  if ( sumEt > 16383 ) sumEt = 16383 ;
  unsigned int code = 0x10000 + ( sumEt << L0DUBase::Calo::Sum::Shift ) ;
  LHCb::L0ProcessorData * hsum = 
    new LHCb::L0ProcessorData( L0DUBase::Fiber::CaloSumEt , code ) ;
  L0Calo -> add( hsum ) ;

  // Spd multiplicity counter
  int totSpdMult = std::accumulate( m_spdMult.begin() , m_spdMult.end() , 0 ) ;
  code = 0x10000 + ( totSpdMult << L0DUBase::Calo::Sum::Shift ) ;
  LHCb::L0ProcessorData* spdMult = 
    new LHCb::L0ProcessorData ( L0DUBase::Fiber::CaloSpdMult, code ) ;
  L0Calo -> add( spdMult ) ;

  // Debug now the L0 candidates
  if ( msgLevel( MSG::DEBUG ) ) {
    LHCb::L0ProcessorDatas::const_iterator item;
    debug() << "== L0ProcessorData Summary: " << L0Calo->size() 
            << " entries." << endmsg ;
    for( item = L0Calo->begin() ; L0Calo->end() != item ; ++item ) {
      LHCb::L0ProcessorData* cand = (*item);
      debug() << format( "Key %2d Word %8x", cand->key(), cand->word() ) 
              << endmsg ;
    }
  }
  
  // Now save the L0CaloCandidates: create a RAW bank and then decode it
  // Prepare the raw banks, and save them.
  m_rawOutput[ 0 ].clear() ;
  m_rawOutput[ 1 ].clear() ;

  // saveInRawEvent( IO , SLAVE , MASK ) 
  // IO    --- 0 for input link, 1 for output of Selection board 
  // SLAVE --- 0 for all selection board except hadron ones 
  // MASK  --- always 0 on MC, used only in real data 

  // Save all Selection boards inputs
  std::vector< L0Candidate >::iterator it ;
  for ( it = allElectrons.begin() ; it != itElectron ; ++it )
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Electron , *it , 1 ) ;
  for ( it = allPhotons.begin() ; it != itPhoton ; ++it )
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Photon , *it , 1 ) ;
  for ( it = allPi0Local.begin() ; it != itPi0Local ; ++it )
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Pi0Local , *it , 1 ) ;
  for ( it = allPi0Global.begin() ; it != itPi0Global ; ++it )
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Pi0Global , *it , 1 ) ;

  // Save the output candidates (maximum)
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::Electron  , electron  , 1 ) ;
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::Photon    , photon    , 1 ) ;
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::Pi0Local  , pi0Local  , 1 ) ;
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::Pi0Global , pi0Global , 1 ) ;

  // The HCAL part (Slave 1)
  for ( it = sbHcalInput[ 1 ].begin() ; it != sbHcalInput[ 1 ].end() ; ++it ) 
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Hadron , *it , 0 ) ;
  //Save the Slave1 highest candidate 
  saveInRawEvent( 1 , 1 , 0 , L0DUBase::CaloType::Hadron, hadronSlave1 , 0 ) ;
  //Save the local sum of Slave1
  saveInRawEvent( 1 , 1 , 0 , L0DUBase::CaloType::SumEt , 
                  sbHcalOutputSum[ 1 ] , 0 ) ;
  // Slave 2
  for ( it = sbHcalInput[ 2 ].begin() ; it != sbHcalInput[ 2 ].end() ; ++it ) 
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Hadron , *it , 0 ) ;
  // Save the local highest slave2 candidate
  saveInRawEvent( 1 , 2 , 0 , L0DUBase::CaloType::Hadron , hadronSlave2 , 0 ) ;
  // Save the local sum of slave2 
  saveInRawEvent( 1 , 2 , 0 , L0DUBase::CaloType::SumEt , 
                  sbHcalOutputSum[ 2 ] , 0 ) ;
  // Master 
  for ( it = sbHcalInput[ 0 ].begin() ; it != sbHcalInput[ 0 ].end() ; ++it ) 
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::Hadron , *it , 0 ) ;
  // Highest from Slave 1 and 2
  saveInRawEvent( 0 , 1 , 0 , L0DUBase::CaloType::Hadron , hadronSlave1 , 0 ) ;
  saveInRawEvent( 0 , 2 , 0 , L0DUBase::CaloType::Hadron , hadronSlave2 , 0 ) ;
  // Sum from Slave 1 and 2 
  saveInRawEvent( 0 , 1 , 0 , L0DUBase::CaloType::SumEt , 
                  sbHcalOutputSum[ 1 ] , 0 ) ;
  saveInRawEvent( 0 , 2 , 0 , L0DUBase::CaloType::SumEt , 
                  sbHcalOutputSum[ 2 ] , 0 ) ;
  // Global info on Master 
  // Save the Highest of all boards
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::Hadron , hadron , 0 ) ;
  // Save the sum 
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::SumEt , sumEt , 0 ) ;

  // Rhe SPD part 
  std::vector< int >::iterator itSpd ;
  int kk = 0 ;
  for ( itSpd = m_spdMult.begin() ; itSpd != m_spdMult.end() ; ++itSpd )
    saveInRawEvent( 0 , 0 , 0 , L0DUBase::CaloType::SpdMult , kk++ , 
                    (*itSpd) , 1 ) ;
  saveInRawEvent( 1 , 0 , 0 , L0DUBase::CaloType::SpdMult , 0 , 
                  totSpdMult , 1 ) ;

  // Store the RAW bank if required.
  if ( writeBanks() ) {
    m_nbEvents++;
    m_totRawSize = m_totRawSize + m_rawOutput[0].size() + 
      m_rawOutput[1].size();
    LHCb::RawEvent* raw = 
      getOrCreate<LHCb::RawEvent,LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    raw->addBank( 0, LHCb::RawBank::L0Calo, m_bankVersion , m_rawOutput[0] );
    raw->addBank( 1, LHCb::RawBank::L0Calo, m_bankVersion , m_rawOutput[1] );
  }

  if( writeOnTES() ) {
    std::string name = dataLocation( LHCb::L0CaloCandidateLocation::Default ) ;
    std::string nameFull = dataLocation( LHCb::L0CaloCandidateLocation::Full ) ;
    LHCb::L0CaloCandidates * outFull = new LHCb::L0CaloCandidates( ) ;
    LHCb::L0CaloCandidates * out = new LHCb::L0CaloCandidates() ;
    put( outFull , nameFull , IgnoreRootInTES ) ;
    put( out, name , IgnoreRootInTES ) ;
    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << "L0CaloCandidatesFromRawBank Registered output in TES" 
              << endmsg ;

    LHCb::RawBankReadoutStatus readoutStatus( LHCb::RawBank::L0Calo ) ;
    m_bankToTES -> convertRawBankToTES( m_rawOutput, outFull, out , 
                                        m_bankVersion , readoutStatus );
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
           << endmsg ;
  }
  return L0AlgBase::finalize(); 
}

//=============================================================================
// Sum the Ecal digits into the Fe cards
//=============================================================================
void L0CaloAlg::sumEcalData(  ) {
  // Reset the cards collection
  for( int eCard = 0 ; m_ecal -> nCards() > eCard;  ++eCard ) {
    // reject pin readout FE-cards
    if ( m_ecal -> isPinCard( eCard ) || m_ecal -> isParasiticCard( eCard ) ) continue ; 
    m_ecalFe[eCard].reset( );
  }

  const std::vector< LHCb::L0CaloAdc >& adcs = m_adcsEcal -> adcs() ;

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "Found " << adcs.size() << " ECAL adcs" << endmsg ;
	  
  for ( std::vector<LHCb::L0CaloAdc>::const_iterator itAdc = adcs.begin();
        adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    int adc = (*itAdc).adc();
    // Get digits. Sum in front-end cards.
    // Adds to the (possible) neighboring cards if at the border (row/col = 0)
    // and if the card has neighboring cards
    int card, row,  col  ;
    int down, left, corner  ;

    if ( MSG::VERBOSE >= msgLevel() )
      verbose() << id << format( " adc %3d", adc ) << endmsg;

    m_ecal -> cardAddress(id, card, row, col );          // Get the card #
    m_ecal -> cardNeighbors( card, down, left, corner ); // neighbor.
    
    m_ecalFe[ card ].addEt( col , row , adc ) ;
    if ( (0 == row) && (0 <= down)    ) 
      m_ecalFe[down].addEt  ( col,          nRowCaloCard, adc);
    if ( (0 == col) && (0 <= left)    ) 
      m_ecalFe[left].addEt  ( nColCaloCard, row,          adc);
    if ( (0 == col) && (0 == row) && (0 <= corner)  ) 
      m_ecalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc);
  }
}

//=============================================================================
// Sum the Hcal digits into the Fe cards
//=============================================================================
void L0CaloAlg::sumHcalData( ) {
  for( int hCard = 0; m_hcal -> nCards() > hCard;  ++hCard ) {
    // reject pin readout FE-cards
    if( m_hcal -> isPinCard( hCard ) || m_hcal -> isParasiticCard( hCard ) ) continue ;
    m_hcalFe[ hCard ].reset() ;
  }

  const std::vector< LHCb::L0CaloAdc >& adcs = m_adcsHcal -> adcs() ;

  if ( msgLevel( MSG::DEBUG ) ) 
    debug() << "Found " << adcs.size() << " HCAL adcs" << endmsg ;

  for ( std::vector<LHCb::L0CaloAdc>::const_iterator itAdc = adcs.begin();
        adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();

    if ( m_maskHotCells ) {
      // Mask Hot HCAL cells (4 per side, close to the beam pipe)
      int areaw = id.area() ;
      if ( 1 == areaw ) {
        int roww = id.row() ;
        int colw = id.col() ;
        if ( ( roww == 15 ) && ( colw == 13 ) ) continue ;
        if ( ( roww == 16 ) && ( colw == 13 ) ) continue ;
        if ( ( roww == 15 ) && ( colw == 18 ) ) continue ;
        if ( ( roww == 16 ) && ( colw == 18 ) ) continue ;
        if ( ( roww == 15 ) && ( colw == 12 ) ) continue ;
        if ( ( roww == 16 ) && ( colw == 12 ) ) continue ;
        if ( ( roww == 15 ) && ( colw == 19 ) ) continue ;
        if ( ( roww == 16 ) && ( colw == 19 ) ) continue ; 
      }
    }
    
    int adc = (*itAdc).adc();
    
    // Get digits. Sum in front-end cards.
    // Adds to the (possible) neighboring cards if at the border (row/col = 0)
    // and if the card has neighboring cards
    int card, row,  col  ;
    int down, left, corner  ;

    m_hcal->cardAddress( id, card, row, col ); 
    m_hcal->cardNeighbors( card, down, left, corner );

    if ( MSG::VERBOSE >= msgLevel() ) 
      verbose() << id 
                << format( " adc %3d card%3d row%3d col%3d, down%3d left%3d corner%3d", 
                           adc, card, row, col, down, left, corner ) 
                << endmsg ;

    m_hcalFe[card].addEt( col , row , adc ) ;    
    if ( (0 == row) && (0 <= down) ) 
      if ( ( ( m_hcal -> cardFirstColumn( card ) + col ) >=
             m_hcal -> cardFirstValidColumn( down ) ) &&
           ( ( m_hcal -> cardFirstColumn( card ) + col ) <=
             m_hcal -> cardLastValidColumn( down ) ) ) 
          m_hcalFe[down].addEt  ( col,          nRowCaloCard, adc);
    if ( (0 == col) && (0 <= left) ) 
      if ( ( ( m_hcal -> cardFirstRow( card ) + row ) >=
             m_hcal -> cardFirstValidRow( left ) ) && 
           ( ( m_hcal -> cardFirstRow( card ) + row ) <=
             m_hcal -> cardLastValidRow( left ) ) ) 
          m_hcalFe[left].addEt  ( nColCaloCard, row,          adc) ;
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

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "Found " << ids.size() << " PRS bits" << endmsg;

  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;

    if ( MSG::VERBOSE >= msgLevel() ) verbose() << id << endmsg;
    
    m_ecal->cardAddress(id, card, row, col );
    m_ecal->cardNeighbors( card, down, left, corner );
    
    m_ecalFe[card].setPrs( col, row );
    if ( (0 == row) && (0 <= down) )
      m_ecalFe[down].setPrs(   col,          nRowCaloCard );
    if ( (0 == col) && (0 <= left) ) 
      m_ecalFe[left].setPrs(   nColCaloCard, row          );
    if ( (0 == col) && (0 == row) && (0 <= corner) ) 
      m_ecalFe[corner].setPrs( nColCaloCard, nRowCaloCard );
  }
}
//=============================================================================
// Add the Spd information to the ECAL Front-end card
//=============================================================================
void L0CaloAlg::addSpdData( ) {

  for ( unsigned int i = 0 ; i < m_spdMult.size() ; ++i )  
    m_spdMult[ i ] = 0 ;

  LHCb::CaloCellID id;

  // Get digits. Sum in front-end cards.
  // Adds to the (possible) neighboring cards if at the border (row/col = 0)
  // and if the card has neighboring cards
  int card, row,  col  ;
  int down, left, corner  ;
  int spdCard( 0 ) , slot( 0 ) , crate( 0 ) , identif( 0 ) ;

  std::vector<LHCb::CaloCellID>& ids = m_PrsSpdIds.second;
  if ( msgLevel( MSG::DEBUG ) ) 
    debug() << "Found " << ids.size() << " SPD bits" << endmsg;

  for ( std::vector<LHCb::CaloCellID>::const_iterator itID = ids.begin();
        ids.end() != itID; ++itID ) {
    id = *itID;

    if ( msgLevel( MSG::VERBOSE ) ) verbose() << id << endmsg;

    m_ecal->cardAddress(id, card, row, col );
    m_ecal->cardNeighbors( card, down, left, corner );    
 
    m_ecalFe[card].setSpd( col, row );
    if ( (0 == row) && (0 <= down) ) 
      m_ecalFe[down].setSpd( col, nRowCaloCard );
    if ( (0 == col) && (0 <= left) ) 
      m_ecalFe[left].setSpd( nColCaloCard, row );
    if ( (0 == col) && (0 == row) && (0 <= corner) ) 
      m_ecalFe[corner].setSpd( nColCaloCard, nRowCaloCard );
    
    spdCard  = m_prs -> cardNumber( id ) ;
    slot     = m_prs -> cardSlot( spdCard ) ;
    crate    = m_prs -> cardCrate( spdCard ) ;
    identif = 2 * crate ; 
    if ( slot >= 8 ) identif += 1 ;
    m_spdMult[ identif ] += 1 ;
    if ( m_spdMult[ identif ] > 1023 ) m_spdMult[ identif ] = 1023 ;
  }
}

//=============================================================================
//  Save a candidate in the Raw bank for SPD
//=============================================================================
void L0CaloAlg::saveInRawEvent ( int io , int slave , int mask , int type, 
                                 int cardNumber , int spdMult ,
                                 unsigned int bank ) {
  //== Coding for Raw: IO (msb 1 bit) , Slave (2 bits) ,
  // Mask (1 bit) , type (4 bits), id (4 bits) and
  // multiplicity (lsb 10 bits). 
  if ( 0 == spdMult ) return;

  if (type != L0DUBase::CaloType::SpdMult ) {
    warning()<<"Should be of type CaloSpdMult and is of type " 
             << type << endmsg ;
    return ;
  }  

  int word = ( io << 31 ) | ( slave << 29 ) | ( mask << 28 ) | 
    ( type << 24 ) | ( cardNumber << 10 ) | spdMult ;
  m_rawOutput[ bank ].push_back( word );
}

//=============================================================================
// Save a candidate in the RAW bank for most common cases
//=============================================================================
void L0CaloAlg::saveInRawEvent ( int io, int slave, int mask, int type, 
                                 L0Candidate& cand, unsigned int bank ) {
  //== Coding for Raw: IO (lsb 1 bit) , Slave (2 bits) , 
  // Mask (1 bit) , type (4 bits), id (16 bits) and et (low 8 bits). 
  if ( 0 == cand.et() ) return;
  if ( L0DUBase::CaloType::Hadron == type ) 
    if ( cand.et() <= m_hcalThreshold ) return ;
  if ( L0DUBase::CaloType::Hadron != type ) 
    if ( cand.et() <= m_ecalThreshold ) return ;

  if (L0DUBase::CaloType::SumEt == type ) {
    warning()<<"Should NOT be of type CaloSumEt .... " << endmsg ;
    return ;
  }

  int word = ( io << 31 ) | ( slave << 29 ) | ( mask << 28 ) |
    ( type << 24 ) | (cand.ID().index() << 8 ) | cand.et() ;
  m_rawOutput[ bank ].push_back( word ) ;
}

//=============================================================================
// Save a candidate in the RAW bank for SumEt
//=============================================================================
void L0CaloAlg::saveInRawEvent ( int io, int slave, int mask, int type, 
                                 int sumEt, unsigned int bank ) {
  if ( 0 == sumEt ) return ;
  
  if ( L0DUBase::CaloType::SumEt == type || 
       L0DUBase::CaloType::SpdMult == type) {
    int word = ( io << 31 ) | ( slave << 29 ) | ( mask << 28 ) | 
      ( type << 24 ) | sumEt ;
    m_rawOutput[ bank ].push_back( word ) ;
  } else 
    warning() << "Should be of type CaloSumEt or CaloSpdMult.... " 
              << endmsg ;
}

//=========================================================================
// Create HCAL LUT to configure TVB at the pit
//=========================================================================
void L0CaloAlg::createHCALLut( ) {  
  std::map< int , std::vector< int > > validMap ;
  validMap[ 0 ].push_back(0) ; validMap[ 0 ].push_back(1) ; 
  validMap[ 0 ].push_back(2) ; validMap[ 0 ].push_back(3) ;
  validMap[ 1 ].push_back(4) ; validMap[ 1 ].push_back(5) ; 
  validMap[ 1 ].push_back(6) ; validMap[ 1 ].push_back(-1) ;
  validMap[ 2 ].push_back(0) ; validMap[ 2 ].push_back(1) ; 
  validMap[ 2 ].push_back(2) ; validMap[ 2 ].push_back(3) ;
  validMap[ 3 ].push_back(4) ; validMap[ 3 ].push_back(5) ; 
  validMap[ 3 ].push_back(6) ; validMap[ 3 ].push_back(-1) ;
  validMap[ 4 ].push_back(7) ; validMap[ 4 ].push_back(8) ; 
  validMap[ 4 ].push_back(11) ; validMap[ 4 ].push_back(18) ;
  validMap[ 5 ].push_back(17) ; validMap[ 5 ].push_back(24) ; 
  validMap[ 5 ].push_back(9) ; validMap[ 5 ].push_back(10) ;
  validMap[ 6 ].push_back(12) ; validMap[ 6 ].push_back(13) ; 
  validMap[ 6 ].push_back(14) ; validMap[ 6 ].push_back(-1) ;
  validMap[ 7 ].push_back(15) ; validMap[ 7 ].push_back(16) ; 
  validMap[ 7 ].push_back(19) ; validMap[ 7 ].push_back(-1) ;
  validMap[ 8 ].push_back(12) ; validMap[ 8 ].push_back(13) ; 
  validMap[ 8 ].push_back(14) ; validMap[ 8 ].push_back(-1) ;
  validMap[ 9 ].push_back(15) ; validMap[ 9 ].push_back(16) ; 
  validMap[ 9 ].push_back(-1) ; validMap[ 9 ].push_back(23) ;
  validMap[ 10 ].push_back(20) ; validMap[ 10 ].push_back(-1) ; 
  validMap[ 10 ].push_back(-1) ; validMap[ 10 ].push_back(-1) ;
  validMap[ 11 ].push_back(22) ; validMap[ 11 ].push_back(21) ; 
  validMap[ 11 ].push_back(-1) ; validMap[ 11 ].push_back(-1) ;
  validMap[ 12 ].push_back(20) ; validMap[ 12 ].push_back(21) ; 
  validMap[ 12 ].push_back(-1) ; validMap[ 12 ].push_back(-1) ;
  validMap[ 13 ].push_back(22) ; validMap[ 13 ].push_back(-1) ; 
  validMap[ 13 ].push_back(-1) ; validMap[ 13 ].push_back(-1) ;
  validMap[ 14 ].push_back(27) ; validMap[ 14 ].push_back(-1) ; 
  validMap[ 14 ].push_back(-1) ; validMap[ 14 ].push_back(-1) ;
  validMap[ 15 ].push_back(29) ; validMap[ 15 ].push_back(28) ; 
  validMap[ 15 ].push_back(-1) ; validMap[ 15 ].push_back(-1) ;
  validMap[ 16 ].push_back(27) ; validMap[ 16 ].push_back(28) ; 
  validMap[ 16 ].push_back(-1) ; validMap[ 16 ].push_back(-1) ;
  validMap[ 17 ].push_back(29) ; validMap[ 17 ].push_back(-1) ; 
  validMap[ 17 ].push_back(-1) ; validMap[ 17 ].push_back(-1) ;
  validMap[ 18 ].push_back(26) ; validMap[ 18 ].push_back(33) ; 
  validMap[ 18 ].push_back(34) ; validMap[ 18 ].push_back(-1) ;
  validMap[ 19 ].push_back(35) ; validMap[ 19 ].push_back(36) ; 
  validMap[ 19 ].push_back(37) ; validMap[ 19 ].push_back(-1) ;
  validMap[ 20 ].push_back(-1) ; validMap[ 20 ].push_back(33) ; 
  validMap[ 20 ].push_back(34) ; validMap[ 20 ].push_back(30) ;
  validMap[ 21 ].push_back(35) ; validMap[ 21 ].push_back(36) ; 
  validMap[ 21 ].push_back(37) ; validMap[ 21 ].push_back(-1) ;
  validMap[ 22 ].push_back(39) ; validMap[ 22 ].push_back(40) ; 
  validMap[ 22 ].push_back(25) ; validMap[ 22 ].push_back(32) ;
  validMap[ 23 ].push_back(31) ; validMap[ 23 ].push_back(38) ; 
  validMap[ 23 ].push_back(41) ; validMap[ 23 ].push_back(42) ;
  validMap[ 24 ].push_back(43) ; validMap[ 24 ].push_back(44) ; 
  validMap[ 24 ].push_back(45) ; validMap[ 24 ].push_back(46) ;
  validMap[ 25 ].push_back(47) ; validMap[ 25 ].push_back(48) ; 
  validMap[ 25 ].push_back(49) ; validMap[ 25 ].push_back(-1) ;
  validMap[ 26 ].push_back(43) ; validMap[ 26 ].push_back(44) ; 
  validMap[ 26 ].push_back(45) ; validMap[ 26 ].push_back(46) ;
  validMap[ 27 ].push_back(47) ; validMap[ 27 ].push_back(48) ; 
  validMap[ 27 ].push_back(49) ; validMap[ 27 ].push_back(-1) ;    
        
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
          theFile << "-------------------------------------------------------" 
                  << std::endl ;
        }
      } else { 
        for ( int ecalCard = 0 ; ecalCard < 8 ; ++ecalCard ) {
          // Find the corresponding ECAL card number:
          int theCard = -1 ;
          for ( int eCard=0 ;  m_ecal->nCards() > eCard; ++eCard ) {
            if ( ( m_ecalFe[eCard].validationNumber() == valid ) && 
                 ( m_ecal->cardSlot( eCard ) % 8 == ecalCard ) ) {
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
                  m_ecalFe[theCard].addEt( 1 + ecalAdd % 8 , 
                                           1 + (int) ecalAdd / 8 , 1 ) ;
                  theFile << m_ecalFe[theCard].match_included( hcalAdd % 8 , 
                                                               (int)hcalAdd/8 ) 
                          << " " ;                
                  
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
          theFile << "--------------------------------------------------------" 
                  << std::endl ;         
        }
      } 
      theFile.close() ;
    }
  }
}  

//=============================================================================
// Callback function to check L0Calo conditions
//=============================================================================
StatusCode L0CaloAlg::updateL0Calibration( ) {
  if( msgLevel(MSG::DEBUG) ) debug() << "Updating L0Calibration" << endmsg ;

  if ( ! m_l0Cond -> exists( "AddECALToHCAL" ) ) {
    Warning("AddECALToHCAL parameter does not exist in DB").ignore() ;
    Warning("Use default AddECALToHCAL = true").ignore() ;
    m_addEcalToHcal = m_addEcalToHcalOpts ;
  } else {
    if ( m_ignoreDatabase ) {
      Warning( "Database content will be ignored" ).ignore() ;
      m_addEcalToHcal = m_addEcalToHcalOpts ;
    } else 
      m_addEcalToHcal = (m_l0Cond -> param< int >( "AddECALToHCAL" ))!=0 ;
  }

  if ( ! m_l0Cond -> exists( "UsePSSPD" ) ) {
    Warning("UsePSSPD parameter does not exist in DB").ignore() ;
    Warning("Use default UsePSSPD = true").ignore() ;
    m_usePsSpd = m_usePsSpdOpts ;
  } else {
    if ( m_ignoreDatabase ) {
      Warning( "Database content will be ignored" ).ignore() ;
      m_usePsSpd = m_usePsSpdOpts ;
    } else
      m_usePsSpd      = (m_l0Cond -> param< int >( "UsePSSPD" ))!=0 ;
  }
  
  if( msgLevel(MSG::DEBUG) ) {
    debug() << "Add ECAL to HCAL = " << m_addEcalToHcal << endmsg ;
    debug() << "Use PS and SPD = " << m_usePsSpd << endmsg ;
  }
  
  return StatusCode::SUCCESS ;
}
