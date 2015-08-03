// local
#include "L0CaloPRSResetMonitor.h"

// Calo
#include "CaloDet/DeCalorimeter.h"

// Event
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUBase.h"

// Declare Algorithm
DECLARE_ALGORITHM_FACTORY( L0CaloPRSResetMonitor )

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloPRSResetMonirot
//
// 10/08/2011 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor
//=============================================================================
L0CaloPRSResetMonitor::L0CaloPRSResetMonitor( const std::string& name, 
                                              ISvcLocator* pSvcLocator ) : 
  GaudiHistoAlg ( name , pSvcLocator ),
  m_nEvents( 0 ) , 
  m_alarm  ( 0 ) { 
  declareProperty( "InputData"       , 
                   m_inputData  = LHCb::L0CaloCandidateLocation::Full ) ;  
  declareProperty( "UpdateFrequency" , m_updateFrequency = 80 )  ;

  // define alarm published service
  declareInfo( "L0CaloPRSReset" , m_alarm ,
               "Empty board in L0 Electron, PRS Reset needed." ) ;
}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloPRSResetMonitor::~L0CaloPRSResetMonitor() { }

//=============================================================================
// Initialisation. 
//=============================================================================
StatusCode L0CaloPRSResetMonitor::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize();

  if ( sc.isFailure() ) return sc;

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Initialize event counters
  m_nEvents = 0 ;

  // Reset alarm
  m_alarm = 0 ;

  // Retrieve the ECAL detector element, build cards
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );

  // Initialize the counter
  int eCard( 0 );
  for ( eCard = 0 ; eCard < m_ecal -> nCards() ; ++eCard ) {
    if ( ( ! m_ecal -> isPinCard( eCard ) ) && 
	 ( ! m_ecal -> isParasiticCard( eCard ) ) )
      m_electronCounter[ eCard ] = 0 ;
  }
 
  return StatusCode::SUCCESS; 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloPRSResetMonitor::execute() {

  // Read input data specified by options
  if( msgLevel(MSG::DEBUG) ) 
    debug() << "Execute will read " << m_inputData << endmsg ;

  LHCb::L0CaloCandidates * candidates = 
    getIfExists<LHCb::L0CaloCandidates>( m_inputData );
  if ( NULL == candidates ) return Warning( "No data at " + m_inputData, StatusCode::SUCCESS ) ;

  LHCb::L0CaloCandidates::iterator cand ;

  int card( 0 ) , crate( 0 ) , slot( 0 ) ;

  bool oneElectronFound = false ;

  // Loop over the candidates
  for ( cand = candidates -> begin() ; candidates -> end() != cand ; 
        ++cand ) { 
    LHCb::L0CaloCandidate * theCand = (*cand) ;

    // Use only electron candidates
    if ( theCand -> type() != L0DUBase::CaloType::Electron ) 
      continue ;
    
    oneElectronFound = true ;

    // Find the candidate board number
    card = m_ecal -> cardNumber( theCand -> id() ) ;
    if ( 0 != m_electronCounter.count( card ) ) 
      m_electronCounter[ card ] = m_electronCounter[ card ] + 1 ;

    // Count the candidates in the extreme boards
    crate = m_ecal -> cardCrate( card ) ;
    slot  = m_ecal -> cardSlot( card ) ;
    if ( ( ( crate == 8 ) && ( slot == 1 ) ) ||
         ( ( crate == 8 ) && ( slot == 13 ) ) || 
         ( ( crate == 21 ) && ( slot == 1 ) ) || 
         ( ( crate == 21 ) && ( slot == 13 ) ) ) 
      m_nEvents++ ;
  }

  if ( ! oneElectronFound ) return StatusCode::SUCCESS ;

  // Analyse counters at the requested frequency
  if ( m_nEvents > m_updateFrequency ) {
    bool found = false ;
    m_nEvents = 0 ;
    std::map< int , int >::iterator it = m_electronCounter.begin() ;
    while ( it != m_electronCounter.end() ) {
      found = ( it -> second == 0 ) ;
      if ( found ) break ;
      ++it ;
    }
    if ( found ) { 
      m_alarm = 1 ;
      Error( "No L0Electron candidates, reset of PRS will be done automatically" , 
             StatusCode::SUCCESS , 20 ).ignore() ;
    } else m_alarm = 0 ;

    // Reset the counters
    int eCard( 0 );
    for ( eCard = 0 ; eCard < m_ecal -> nCards() ; ++eCard ) {
      if ( ( ! m_ecal -> isPinCard( eCard ) ) && 
	   ( ! m_ecal -> isParasiticCard( eCard ) ) ) 
        m_electronCounter[ eCard ] = 0 ;
    }
  }

  return StatusCode::SUCCESS; 
}
