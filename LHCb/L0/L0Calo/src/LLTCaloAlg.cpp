/// local
#include "LLTCaloAlg.h"

// From Event
#include "Event/L0CaloCandidate.h"
#include "Event/L0CaloAdc.h"

// From Calo
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"

// From Kernel
#include "Kernel/CaloCellID.h"

//
//  LLT calorimeter trigger simulation algorithm
//  Compute LLT quantities from Calorimeter full information
//

DECLARE_ALGORITHM_FACTORY( LLTCaloAlg )

//=============================================================================
// Constructor
//=============================================================================
LLTCaloAlg::LLTCaloAlg( const std::string & name , ISvcLocator * pSvcLocator) 
: GaudiAlgorithm( name , pSvcLocator )
  , m_ecal(NULL)
  , m_hcal(NULL)
  , m_adcsEcal(NULL)
  , m_adcsHcal(NULL) {
  declareProperty( "ECALMultiplicityThreshold" , m_ECALThreshold = 2   ) ;
  declareProperty( "HCALMultiplicityThreshold" , m_HCALThreshold = 2   ) ;  
  declareProperty( "CaloToolName" , m_caloToolName = "CaloTriggerAdcsFromRaw" ) ; 
  declareProperty( "DoubleScale" , m_doubleScale = false ) ;
}

//=============================================================================
// Destructor
//=============================================================================
LLTCaloAlg::~LLTCaloAlg() { }

//=============================================================================
// Initialization: Check parameters, get DetectorElement pointers, build
//                 the trigger cards and their connections (HCAL-ECAL)
//=============================================================================
StatusCode LLTCaloAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; 

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Warning m_ecalFE & m_hcalFE  HAVE to contain PIN-diode FEs
  // in order to ensure the card numbering is correct afterward.
  // Pin-diode FE are anyway excluded from the L0 processing in the code

  // Retrieve the ECAL detector element, and build cards
  m_ecal = getDet< DeCalorimeter >( DeCalorimeterLocation::Ecal ) ;  
  int eCard , hCard ;  
  m_ecalFe.reserve( m_ecal -> nCards() ) ;
  
  for ( eCard = 0 ; eCard < m_ecal -> nCards() ; ++eCard ) {
    m_ecalFe.push_back( LLTTriggerCard( eCard , m_ecal ) ) ;
  }  
  
  // Retrieve the HCAL detector element, and build cards
  m_hcal = getDet< DeCalorimeter >( DeCalorimeterLocation::Hcal ) ;  
  m_hcalFe.reserve( m_hcal -> nCards() ) ;
  
  for ( hCard = 0 ; hCard < m_hcal->nCards() ; ++hCard )
    m_hcalFe.push_back( LLTTriggerCard( hCard , m_hcal ) ) ;

  info() << m_ecal->nCards() << " Ecal and "
         << m_hcal->nCards() << " Hcal front end cards." << endmsg;

  // Initialize needed tools
  m_adcsEcal = 
    tool<ICaloTriggerAdcsFromRaw>( m_caloToolName , 
                                   "EcalTriggerAdcTool", this );
  m_adcsHcal = 
    tool<ICaloTriggerAdcsFromRaw>( m_caloToolName , 
                                   "HcalTriggerAdcTool", this );

  // Gain value to convert ADC into energy
  Condition * gain = m_ecal -> condition( "Gain" ) ;
  if ( 0 == gain ) return Error( "Condition Gain not found in Ecal" ) ;
  
  if ( gain -> exists( "L0EtBin" ) )
    m_etScale = gain -> paramAsDouble( "L0EtBin" ) ;
  else
    return Error( "Parameter L0EtBin not found in Ecal Gain" ) ;

  if ( m_doubleScale ) m_etScale = 2. * m_etScale ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute: Compute the calo trigger information
//=============================================================================
StatusCode LLTCaloAlg::execute() {
  // Get the ECAL data, store them in the Front-End card
  unsigned int ecalMult = sumEcalData( );

  //  Do a similar processing for HCAL Data
  unsigned int hcalMult = sumHcalData( ) ;

  // now save in TES
  std::vector< LLTTriggerCard >::iterator it ;
  LHCb::L0CaloCandidates * out = new LHCb::L0CaloCandidates() ;

  put( out , "Trigger/LLTCalo" , true ) ;
  LHCb::L0CaloCandidate * cand( 0 ) ;
  double tol ;
  Gaudi::XYZPoint center( 0. , 0. , 0. ) , dummy( 0. , 0. , 0. ) ;
  for ( it = m_ecalFe.begin() ; it != m_ecalFe.end() ; ++it ) 
  {
    center = m_ecal -> cellCenter( it -> cellIdMax() ) ;
    tol    = m_ecal -> cellSize( it -> cellIdMax() ) * 0.5 ;
    center.SetX( center.x() + tol ) ;
    center.SetY( center.y() + tol ) ;
    cand = new LHCb::L0CaloCandidate( L0DUBase::CaloType::Electron , 
                                      it -> cellIdMax() , 
                                      it -> etMax() , 
                                      it -> etMax() * m_etScale , 
                                      center , 
                                      tol ) ;
    out -> add( cand ) ;
  }

  for ( it = m_hcalFe.begin() ; it != m_hcalFe.end() ; ++it ) 
  {
    center = m_hcal -> cellCenter( it -> cellIdMax() ) ;
    tol    = m_hcal -> cellSize( it -> cellIdMax() ) * 0.5 ;
    center.SetX( center.x() + tol ) ;
    center.SetY( center.y() + tol ) ;
    cand = new LHCb::L0CaloCandidate( L0DUBase::CaloType::Hadron , 
                                      it -> cellIdMax() , 
                                      it -> etMax() , 
                                      it -> etMax() * m_etScale , 
                                      center , 
                                      tol ) ;
    out -> add( cand ) ;
  }

  cand = new LHCb::L0CaloCandidate( L0DUBase::CaloType::SpdMult , 
                                    LHCb::CaloCellID() , 
                                    ( ecalMult + hcalMult ) , 
                                    0. , 
                                    dummy , 
                                    0. ) ;
  
  out -> add( cand ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize.
//=============================================================================
StatusCode LLTCaloAlg::finalize() {
  return GaudiAlgorithm::finalize(); 
}

//=============================================================================
// Sum the Ecal digits into the Fe cards and return multiplicity
//=============================================================================
unsigned int LLTCaloAlg::sumEcalData(  ) {
  // Reset the cards collection
  for( int eCard = 0 ; m_ecal -> nCards() > eCard;  ++eCard ) {
    // reject pin readout FE-cards
    if ( m_ecal -> isPinCard( eCard ) || 
         m_ecal -> isParasiticCard( eCard ) ) continue ; 
    m_ecalFe[eCard].reset( );
  }

  const std::vector< LHCb::L0CaloAdc >& adcs = m_adcsEcal -> adcs( ) ;

  if ( msgLevel( MSG::DEBUG ) )
    debug() << "Found " << adcs.size() << " ECAL adcs" << endmsg ;
	  
  unsigned int multiplicity = 0 ;
  
  for ( std::vector< LHCb::L0CaloAdc >::const_iterator itAdc = adcs.begin();
        adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    unsigned int adc = (*itAdc).adc() ;
    // compute multiplicity
    if ( adc > m_ECALThreshold ) ++multiplicity ;
    
    // Get digits. Sum in front-end cards.
    // Adds to the (possible) neighboring cards if at the border (row/col = 0)
    // and if the card has neighboring cards
    int card, row,  col  ;
    int down, left, corner  ;

    if ( MSG::VERBOSE >= msgLevel() )
      verbose() << id << format( " adc %3d", adc ) << endmsg;

    m_ecal -> cardAddress(id, card, row, col );          // Get the card #
    m_ecal -> cardNeighbors( card, down, left, corner ); // neighbor.
    
    m_ecalFe[ card ].addEt( col , row , adc , m_ECALThreshold ) ;
    if ( (0 == row) && (0 <= down)    ) 
      m_ecalFe[down].addEt  ( col,          nRowCaloCard, adc , m_ECALThreshold );
    if ( (0 == col) && (0 <= left)    ) 
      m_ecalFe[left].addEt  ( nColCaloCard, row,          adc , m_ECALThreshold );
    if ( (0 == col) && (0 == row) && (0 <= corner)  ) 
      m_ecalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc , m_ECALThreshold );
  }
  return multiplicity ;
}

//=============================================================================
// Sum the Hcal digits into the Fe cards and return multiplicity
//=============================================================================
unsigned int LLTCaloAlg::sumHcalData( ) {
  for( int hCard = 0; m_hcal -> nCards() > hCard;  ++hCard ) {
    // reject pin readout FE-cards
    if( m_hcal -> isPinCard( hCard ) || m_hcal -> isParasiticCard( hCard ) ) continue ;
    m_hcalFe[ hCard ].reset() ;
  }

  const std::vector< LHCb::L0CaloAdc >& adcs = m_adcsHcal -> adcs() ;

  if ( msgLevel( MSG::DEBUG ) ) 
    debug() << "Found " << adcs.size() << " HCAL adcs" << endmsg ;

  unsigned int multiplicity = 0 ;

  for ( std::vector<LHCb::L0CaloAdc>::const_iterator itAdc = adcs.begin();
        adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    unsigned int adc = (*itAdc).adc() ;
    
    if ( adc > m_HCALThreshold ) ++multiplicity ;

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

    m_hcalFe[card].addEt( col , row , adc , m_HCALThreshold ) ;    
    if ( (0 == row) && (0 <= down) ) 
      if ( ( ( m_hcal -> cardFirstColumn( card ) + col ) >=
             m_hcal -> cardFirstValidColumn( down ) ) &&
           ( ( m_hcal -> cardFirstColumn( card ) + col ) <=
             m_hcal -> cardLastValidColumn( down ) ) ) 
        m_hcalFe[down].addEt  ( col,          nRowCaloCard, adc , m_HCALThreshold );
    if ( (0 == col) && (0 <= left) ) 
      if ( ( ( m_hcal -> cardFirstRow( card ) + row ) >=
             m_hcal -> cardFirstValidRow( left ) ) && 
           ( ( m_hcal -> cardFirstRow( card ) + row ) <=
             m_hcal -> cardLastValidRow( left ) ) ) 
        m_hcalFe[left].addEt  ( nColCaloCard, row,          adc , m_HCALThreshold ) ;
    if ( (0 == col) && (0 == row) && (0 <= corner)) 
      m_hcalFe[corner].addEt( nColCaloCard, nRowCaloCard, adc , m_HCALThreshold ) ;
  }
  return multiplicity ;
}

