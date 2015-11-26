// local
#include "L0CaloMonit.h"

// AIDA
#include "AIDA/IAxis.h"

// Event
#include "Event/L0CaloCandidate.h"
#include "Event/RawEvent.h"
#include "Event/L0DUBase.h"
#include "Event/ODIN.h"

// Declare Algorithm
DECLARE_ALGORITHM_FACTORY( L0CaloMonit )

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloMonit
//
// 31/05/2001 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor
//=============================================================================
  L0CaloMonit::L0CaloMonit( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
    Calo2Dview ( name , pSvcLocator ),
    m_ecal(NULL),
    m_hcal(NULL),
    m_nEvents(0),
    m_bcidHist(NULL)
{
  declareProperty( "FullMonitoring"      , m_fullMonitoring  = false ) ;
  declareProperty( "InputDataSuffix"     , m_inputDataSuffix = ""    ) ;
  declareProperty( "UpdateFrequency"     , m_updateFrequency = -1    ) ;
  declareProperty( "LookForHotCells"     , m_lookForHotCells = false ) ;
  declareProperty( "AlarmThresholdRatio" , m_alarmThresholdRatio = 5 ) ;

  // Initialize names of histograms
  // Names of Et histogrames
  m_histName.reserve( 15 ) ; m_histName.resize( 15 , "" ) ;

  m_histName[ L0DUBase::CaloType::Electron        ] = "EtEle" ;
  m_histName[ L0DUBase::CaloType::Photon          ] = "EtPho" ;
  m_histName[ L0DUBase::CaloType::Hadron          ] = "EtHad" ;
  m_histName[ L0DUBase::CaloType::Pi0Local        ] = "EtPil" ;
  m_histName[ L0DUBase::CaloType::Pi0Global       ] = "EtPig" ;
  m_histName[ L0DUBase::CaloType::SumEt           ] = "SumEt" ;
  m_histName[ L0DUBase::CaloType::SpdMult         ] = "SpdMult" ;
  m_histName[ L0DUBase::CaloType::HadronSlave1Out ] = "EtHadSlave1Out" ;
  m_histName[ L0DUBase::CaloType::HadronSlave2Out ] = "EtHadSlave2Out" ;
  m_histName[ L0DUBase::CaloType::HadronSlave1In  ] = "EtHadSlave1In" ;
  m_histName[ L0DUBase::CaloType::HadronSlave2In  ] = "EtHadSlave2In" ;
  m_histName[ L0DUBase::CaloType::SumEtSlave1Out  ] = "SumEtHadSlave1Out" ;
  m_histName[ L0DUBase::CaloType::SumEtSlave2Out  ] = "SumEtHadSlave2Out" ;
  m_histName[ L0DUBase::CaloType::SumEtSlave1In   ] = "SumEtHadSlave1In" ;
  m_histName[ L0DUBase::CaloType::SumEtSlave2In   ] = "SumEtHadSlave2In" ;

  m_histTitle.reserve( 15 ) ; m_histTitle.resize( 15 , "" ) ;
  m_histTitle[ L0DUBase::CaloType::Electron        ] = "Et Electron" ;
  m_histTitle[ L0DUBase::CaloType::Photon          ] = "Et Photon" ;
  m_histTitle[ L0DUBase::CaloType::Hadron          ] = "Et Hadron" ;
  m_histTitle[ L0DUBase::CaloType::Pi0Local        ] = "Et Pi0 Local" ;
  m_histTitle[ L0DUBase::CaloType::Pi0Global       ] = "Et Pi0 Global" ;
  m_histTitle[ L0DUBase::CaloType::SumEt           ] = "Sum Et" ;
  m_histTitle[ L0DUBase::CaloType::SpdMult         ] = "Spd Mult" ;
  m_histTitle[ L0DUBase::CaloType::HadronSlave1Out ] = "Et Hadron Slave1 Out" ;
  m_histTitle[ L0DUBase::CaloType::HadronSlave2Out ] = "Et Hadron Slave2 Out" ;
  m_histTitle[ L0DUBase::CaloType::HadronSlave1In  ] = "Et Hadron Slave1 In" ;
  m_histTitle[ L0DUBase::CaloType::HadronSlave2In  ] = "Et Hadron Slave2 In" ;
  m_histTitle[ L0DUBase::CaloType::SumEtSlave1Out] = "SumEt Hadron Slave1 Out";
  m_histTitle[ L0DUBase::CaloType::SumEtSlave2Out] = "SumEt Hadron Slave2 Out";
  m_histTitle[ L0DUBase::CaloType::SumEtSlave1In ] = "SumEt Hadron Slave1 In" ;
  m_histTitle[ L0DUBase::CaloType::SumEtSlave2In ] = "SumEt Hadron Slave2 In" ;

  m_maxScale.reserve( 15 ) ; m_maxScale.resize( 15 , 0 ) ;
  m_maxScale[ L0DUBase::CaloType::Electron        ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::Photon          ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::Hadron          ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::Pi0Local        ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::Pi0Global       ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::SumEt           ] = 16384 ;
  m_maxScale[ L0DUBase::CaloType::SpdMult         ] = 6016 ;
  m_maxScale[ L0DUBase::CaloType::HadronSlave1Out ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::HadronSlave2Out ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::HadronSlave1In  ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::HadronSlave2In  ] = 256 ;
  m_maxScale[ L0DUBase::CaloType::SumEtSlave1Out  ] = 16384 ;
  m_maxScale[ L0DUBase::CaloType::SumEtSlave2Out  ] = 16384 ;
  m_maxScale[ L0DUBase::CaloType::SumEtSlave1In   ] = 16384 ;
  m_maxScale[ L0DUBase::CaloType::SumEtSlave2In   ] = 16384 ;
}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloMonit::~L0CaloMonit() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode L0CaloMonit::initialize() {

  StatusCode sc = Calo2Dview::initialize();

  if ( sc.isFailure() ) return sc;  // error printed already by Calo2Dview

  if( msgLevel(MSG::DEBUG) )  debug() << "==> Initialize" << endmsg;

  // Initialize event counters
  m_nEvents       = 0 ;

  // Retrieve the ECAL detector element, build cards
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  // Retrieve the HCAL detector element, build cards
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );

  // Book all histograms created by monitoring algorithm
  if( msgLevel(MSG::DEBUG) )
    debug() << "==> Default Monitoring histograms booking " << endmsg;

  // Et and multiplicity spectra histogram first
  m_etHist.reserve( 7 ) ; m_etHist.resize( 7 , 0 ) ;
  int i = 0 ;
  for ( i = 0 ; i <= L0DUBase::CaloType::SpdMult ; ++i )
    bookEtHist( i , m_etHist , "" ) ;

  // Then histograms to count number of times a cell is hit
  // only is "LookForHotCells" is activated
  if ( m_lookForHotCells ) {
    AIDA::IHistogram1D * h( 0 ) ;
    std::vector< AIDA::IHistogram1D * > v( 3 , h ) ;
    m_freqHist.reserve( 5 ) ; m_freqHist.resize( 5 , v ) ;
    for ( i = 0 ; i <= L0DUBase::CaloType::Pi0Global ; ++i )
      bookFreqHist( i , m_freqHist ) ;
  }

  // Then 2D maps
  m_mapName.reserve( 5 ) ; m_mapTitle.reserve( 5 ) ;
  m_mapName.resize( 5 , "" ) ; m_mapTitle.resize( 5 , "" ) ;
  for ( i = 0 ; i <= L0DUBase::CaloType::Pi0Global ; ++i ) {
    std::string det = "Ecal" ;
    if ( L0DUBase::CaloType::Hadron == i ) det = "Hcal" ;
    m_mapName[ i ]  = det + "Map" + abbrev( i ) ;
    m_mapTitle[ i ] = fullName( i ) + " " + det + " map" ;
    bookCalo2D( m_mapName[ i ] , m_mapTitle[ i ] , det ) ;
  }

  // Now book hits per board, only activated when Full monitoring is set
  // to true
  // Also "Et" spectra histograms for all TVB candidates are booked
  if ( m_fullMonitoring ) {
    if( msgLevel(MSG::DEBUG) )
      debug() << "==> Full Monitoring histograms booking" << endmsg ;
    AIDA::IHistogram1D * h( 0 ) ;
    std::vector< AIDA::IHistogram1D* > v( 14 , h ) ;
    m_crateHist.reserve( 7 ) ; m_crateHist.resize( 7 , v ) ;
    for ( i = 0 ; i <= L0DUBase::CaloType::Pi0Global ; ++i )
      bookCrateHist( i , m_crateHist ) ;
    m_etFullHist.reserve( 15 ) ; m_etFullHist.resize( 15 , 0 ) ;
    for ( i = 0 ; i <= L0DUBase::CaloType::SumEtSlave2In ; ++i )
      bookEtHist( i , m_etFullHist , "Full" ) ;
  }

  m_bcidHist = GaudiHistoAlg::book( "BCId" , "BCId" , 0 , 3564 , 3564 ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloMonit::execute() {

  // Read input data in default L0Calo container, ie what is sent to
  // L0DU.
  // Input data suffix is used to select if we read the L0Calo data
  // (from the L0Calo TELL1)
  // or the result of the simulation/emulation
  if( msgLevel(MSG::DEBUG) ) debug() << "Execute will read "
                                     << LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix
                                     << endmsg ;

  LHCb::L0CaloCandidates * candidates = getIfExists<LHCb::L0CaloCandidates>
    ( LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix );
  if( NULL == candidates ) return Warning( "No data at " + LHCb::L0CaloCandidateLocation::Default +
                                           m_inputDataSuffix, StatusCode::SUCCESS );

  LHCb::L0CaloCandidates::iterator cand ;

  m_nEvents++ ;

  // Read ODIN bank to obtain BCId and event number
  unsigned long long event( 0 ) ;
  unsigned int BCId( 4000 ) ;

  LHCb::ODIN * odin = getIfExists< LHCb::ODIN >( LHCb::ODINLocation::Default ) ;
  if ( NULL != odin ) {
    event = odin->eventNumber() ;
    BCId = odin->bunchId() ;
  }

  // Fill BCId histogram
  m_bcidHist -> fill( BCId , 1. ) ;

  // Loop over default candidates
  for ( cand = candidates -> begin() ; candidates -> end() != cand ;
        ++cand ) {
    LHCb::L0CaloCandidate * theCand = (*cand) ;

    if( msgLevel(MSG::DEBUG) )
      debug() << " Event " << event << " Type  = " << theCand -> type()
              << " Et  = " << theCand -> etCode() << endmsg ;

    defaultMonitoring( theCand ) ;
  }

  // Check regularly for hot cells
  if ( ( m_updateFrequency > 0 ) && ( m_lookForHotCells ) ) {
    int goForCheck = m_nEvents % m_updateFrequency ;
    if ( 0 == goForCheck ) {
      if( msgLevel(MSG::DEBUG) )
        debug() << "m_nEvents = " << m_nEvents << " go for check ..." << endmsg ;
      printHotCellSummary( ) ;
    }
  }

  // Read Full container (ie containing the input of the Selection
  // Boards)
  if ( m_fullMonitoring ) {
    if( msgLevel(MSG::DEBUG) ) debug() << "Full Monitoring" << endmsg ;

    LHCb::L0CaloCandidates* candidatesF = getIfExists<LHCb::L0CaloCandidates>
      ( LHCb::L0CaloCandidateLocation::Full + m_inputDataSuffix ) ;
    if ( NULL == candidatesF ) return Warning( "No data at " + LHCb::L0CaloCandidateLocation::Full +
                                               m_inputDataSuffix, StatusCode::SUCCESS ) ;

    LHCb::L0CaloCandidates::const_iterator candF ;

    for ( candF = candidatesF->begin() ; candidatesF->end() != candF ;
          ++candF )
      m_etFullHist[ (*candF) -> type() ] -> fill( (*candF) -> etCode() , 1. ) ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize.
//=============================================================================
StatusCode L0CaloMonit::finalize() {
  info() << "Number of events processed: " << m_nEvents << endmsg ;

  // print summary of hot cells found
  if ( ( m_updateFrequency < 0 ) && ( m_lookForHotCells ) )
    printHotCellSummary() ;

  std::vector< std::vector< AIDA::IHistogram1D * > >::iterator it ;
  for ( it = m_crateHist.begin() ; m_crateHist.end() != it ; ++it )
    (*it).clear() ;
  m_crateHist.clear() ;
  m_etHist.clear() ;

  return Calo2Dview::finalize() ;
}

//=============================================================================
// Fill histograms for the default monitoring
//=============================================================================
void L0CaloMonit::defaultMonitoring( const LHCb::L0CaloCandidate * cand ) {
  // Cell ID of the candidate
  LHCb::CaloCellID caloCell = cand -> id() ;

  // Type of the candidate (L0DUBase::CaloType)
  int type = cand -> type() ;
  int card = -1 ;

  switch ( type ) {

  case L0DUBase::CaloType::Electron:
  case L0DUBase::CaloType::Photon:
  case L0DUBase::CaloType::Hadron:
  case L0DUBase::CaloType::Pi0Local:
  case L0DUBase::CaloType::Pi0Global:

    m_etHist[ type ] -> fill( cand -> etCode() , 1. ) ;
    card = detector( type ) -> cardNumber( caloCell ) ;

    if ( m_lookForHotCells )
      m_freqHist[ type ][ caloCell.area() ] -> fill( caloCell.all() , 1. ) ;

    if ( m_fullMonitoring ) {
      int crate = detector( type ) -> cardCrate( card ) ;
      int cardSlot = detector( type ) -> cardSlot( card ) ;
      int crateIndex = crateOrder( type , crate ) ;
      m_crateHist[ type ][ crateIndex ] -> fill( cardSlot , 1. ) ;
    }
    fillCalo2D( m_mapName[ type ] , caloCell , 1. , m_mapTitle[ type ] ) ;
    break ;

  case L0DUBase::CaloType::SumEt:
  case L0DUBase::CaloType::SpdMult:
    m_etHist[ type ] -> fill( cand -> etCode() , 1. ) ;
    break ;

  default:
    break ;
  }
}

//=============================================================================
// Prints a summary of the hot cells found
//=============================================================================
void L0CaloMonit::printHotCellSummary( ) {
  info() << "=========================Hot cells ========================="
         << endmsg ;
  info() << "============================================================"
         << endmsg ;

  for ( int i = 0 ; i <= L0DUBase::CaloType::Pi0Global ; ++i ) {
    int caloType = 1 ;
    int nArea = 3 ;
    if ( L0DUBase::CaloType::Hadron == i ) {
      nArea = 2 ;
      caloType = 2 ;
    }

    for ( int j = 0 ; j < nArea ; ++j ) {
      info() << "===        " << fullName( i )
             << " Candidates " << area( j ) << " region              ==="
             << endmsg ;
      SearchForHotCellsAndReset( m_freqHist[ i ][ j ] , caloType ) ;
    }
    info() << "============================================================"
           << endmsg ;
  }
}


//=============================================================================
// Search for hot cells in the frequency histograms
// and then reset the histogram
//=============================================================================
void L0CaloMonit::SearchForHotCellsAndReset( AIDA::IHistogram1D * hist ,
                                             const int type ) {
  // Number of entries in the histogram
  int nIn = hist -> entries() ;

  const IAxis& xAxis = hist->axis() ;

  // Number of bins in the histogram
  int nBin = xAxis.bins() ;

  // Transfer histogram in vector
  std::vector< int > data ;
  data.reserve( nBin ) ;

  // Count number of cells hit
  int nUsedCells = 0 ;
  int i = 0 ;
  for ( i = 0 ; i < nBin ; i++) {
    data.push_back( hist -> binEntries( i ) ) ;
    if ( 0 != data[ i ] ) nUsedCells++ ;
  }

  if ( nUsedCells == 0 ) {
    info() << " ===> No hot channels for this region " << endmsg ;
    return ;
  }

  // Compute the mean occupancy of the are
  double meanOcc = ( (double) nIn ) / ( (double) nUsedCells ) ;

  // Find hot channels
  std::vector< int > hotChannels ;
  for ( i = 0 ; i < nBin ; i++ )
    // Declare a hot cell when its number of hits is larger than
    // N times the mean occupancy
    // where N is set by options (AlarmThresholdRatio)
    if ( data[i] > meanOcc * m_alarmThresholdRatio )
      hotChannels.push_back( i ) ;

  if ( ! hotChannels.empty() ) {
    // Print the hot cells on screen
    info() << "Number of hit cells : " << nUsedCells
           << " mean occupancy : "     << meanOcc
           << " alarm threshold : "    << m_alarmThresholdRatio*meanOcc
           << endmsg ;
    info() << "---------------------------------------------------------------"
           << endmsg ;
    info()
      << "|   Data     |    Hot(?) Cell    |    Crate    | Slot     |"
      << "   Channel   |" << endmsg ;
    info() << "---------------------------------------------------------------"
           << endmsg ;

    int card( -999 ), crate( -999 ) , cardSlot( -999 ) , channelNum( 0 ) ,
      cellChannel( 0 ) ;
    double idAll( -1. ) ;
    std::vector< LHCb::CaloCellID >::const_iterator itc ;

    for ( std::vector< int >::iterator it = hotChannels.begin() ;
          it != hotChannels.end() ; ++it ) {

      // Find the Cell ID from the x coordinate of the bin
      idAll = xAxis.binLowerEdge( (*it) ) ;
      LHCb::CaloCellID caloCell( (int)idAll ) ;

      // Find useful information to print to allow locating the hot cell
      // crate, card slot and channel number in the card
      card  = detector( type ) -> cardNumber( caloCell) ;
      crate = detector( type ) -> cardCrate(card) ;
      cardSlot = detector( type ) -> cardSlot(card) ;

      for ( itc = detector( type ) -> cardChannels( card ).begin() ;
            itc != detector( type ) -> cardChannels( card ).end() ;
            ++itc ) {
        if ( (*itc) == caloCell) {
          cellChannel = channelNum ;
          break ;
        }
        channelNum++ ;
      }

      info() << "|     " << data[ (*it) ] << "     |   " << caloCell
             << "    |     " <<crate << "     |     "
             << cardSlot << "      |  " << "   |   "
             << cellChannel << "  |  " << endmsg ;
      info() << "-----------------------------------------------------------"
             << endmsg ;
    }
  } else {
    info() << " ===> No hot channels for this region " << endmsg ;
  }
  hist->reset() ;
}

//=============================================================================
// Book a "Et" histogram
//=============================================================================
void L0CaloMonit::bookEtHist( const unsigned int i ,
                              std::vector< AIDA::IHistogram1D *> & hist ,
                              const std::string & suffix ) {
  hist[ i ] = GaudiHistoAlg::book( m_histName[ i ] + suffix ,
                                   m_histTitle[ i ] + suffix ,
                                   0 , m_maxScale[ i ] , m_maxScale[ i ] ) ;
}

//=============================================================================
// Book a "Frequency" histogram
//=============================================================================
void L0CaloMonit::bookFreqHist( const unsigned int i ,
                                std::vector< std::vector< AIDA::IHistogram1D *> >&
                                hist ) {
  unsigned int nZone( 0 ) ;
  if ( i != L0DUBase::CaloType::Hadron ) nZone = 3 ;
  else nZone = 2 ;

  hist[ i ].reserve( nZone ) ;

  int cellMin( 0 ) , cellMax( 0 ) ;

  // Inner = 2 , Middle = 1 , Outer = 0
  for ( unsigned int j = 0 ; j < nZone ; ++j ) {
    if ( L0DUBase::CaloType::Hadron == i ) {
      switch( j ) {
      case 0: cellMin = 49200 ; cellMax = 51000 ; break ;
      case 1: cellMin = 53300 ; cellMax = 55300 ; break ;
      default: break ;
      }
    } else {
      switch( j ) {
      case 0: cellMin = 32500 ; cellMax = 37000 ; break ;
      case 1: cellMin = 37000 ; cellMax = 41500 ; break ;
      case 2: cellMin = 41500 ; cellMax = 44500 ; break ;
      default: break ;
      }
    }

    hist[ i ][ j ] =
      GaudiHistoAlg::book( "Freq" + abbrev( i ) + area( j ) ,
                           fullName( i ) + " Candidate occurence " + area( j ) ,
                           cellMin , cellMax , cellMax - cellMin ) ;
  }
}

//=============================================================================
// Book a "Crate" histogram
//=============================================================================
void L0CaloMonit::bookCrateHist( const unsigned int i ,
                                 std::vector< std::vector< AIDA::IHistogram1D *> >&
                                 hist ) {
  unsigned int nCrates( 0 ) ;
  if ( i != L0DUBase::CaloType::Hadron ) nCrates = 14 ;
  else nCrates = 4 ;

  hist[ i ].reserve( nCrates ) ;

  for ( unsigned int j = 0 ; j < nCrates ; ++j )
    hist[ i ][ j ] =
      GaudiHistoAlg::book( abbrev( i ) + "Crate" + crateNumber( i , j ) ,
                           fullName( i ) + " crate " + crateNumber( i , j ) ,
                           0 , 16 , 16 ) ;
}
