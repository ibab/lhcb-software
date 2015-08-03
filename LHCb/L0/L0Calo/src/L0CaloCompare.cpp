// local
#include "L0CaloCompare.h"

#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/L0CaloCandidate.h"
#include "Event/RawEvent.h"
#include "Event/L0DUBase.h"
#include "Event/ODIN.h"

DECLARE_ALGORITHM_FACTORY( L0CaloCompare )

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCompare
//
//-----------------------------------------------------------------------------

// Standard creator
L0CaloCompare::L0CaloCompare( const std::string& name, 
                              ISvcLocator* pSvcLocator )  
  : Calo2Dview ( name , pSvcLocator ) 
  , m_histSpdMult_Comp(NULL) 
  , m_histSumEt_Comp(NULL)
  , m_histTotalCount(NULL)
  , m_histErrorCount(NULL)
{ 
  m_idleBCIdVector.push_back( 3561 ) ;
  m_idleBCIdVector.push_back( 3562 ) ;
  m_idleBCIdVector.push_back( 3563 ) ;
  declareProperty( "FullMonitoring"      , m_fullMonitoring    = true ) ;  
  declareProperty( "ReferenceDataSuffix" , m_referenceDataSuffix = "" ) ;  
  declareProperty( "CheckDataSuffix"     , m_checkDataSuffix = "RAW"  ) ;  
  declareProperty( "IdleBCIdList"        , m_idleBCIdVector ) ;

  m_errorCounterHisto.reserve( 5 ) ;

  m_mapCompareName.reserve( 5 ) ; m_mapCompareName.resize( 15 , "" ) ;
  m_mapCompareTitle.reserve( 5 ) ; m_mapCompareTitle.resize( 15 , "" ) ;
  m_mapAllName.reserve( 5 ) ; m_mapAllName.resize( 15 , "" ) ;
  m_mapAllTitle.reserve( 5 ) ; m_mapAllTitle.resize( 15 , "" ) ;

  m_errorCounterName.reserve( 5 ) ; m_errorCounterName.resize( 15 , "" ) ;
  m_errorCounterTitle.reserve( 5 ) ; m_errorCounterTitle.resize( 15 , "" ) ;

  m_mapCompareName [ L0DUBase::CaloType::Electron  ] = "EcalMapEleCompare" ;  
  m_mapCompareTitle[ L0DUBase::CaloType::Electron  ] = "Electron Ecal map" ;
  m_mapAllName     [ L0DUBase::CaloType::Electron  ] = "EcalMapEleAll" ;  
  m_mapAllTitle    [ L0DUBase::CaloType::Electron  ] = "Electron Ecal map all" ;
  m_errorCounterName [ L0DUBase::CaloType::Electron  ] = "EleErrorCounter" ;  
  m_errorCounterTitle[ L0DUBase::CaloType::Electron  ] = "Electron error counter" ;

  m_mapCompareName [ L0DUBase::CaloType::Photon    ] = "EcalMapPhoCompare" ;  
  m_mapCompareTitle[ L0DUBase::CaloType::Photon    ] = "Photon Ecal map" ;
  m_mapAllName     [ L0DUBase::CaloType::Photon    ] = "EcalMapPhoAll" ;  
  m_mapAllTitle    [ L0DUBase::CaloType::Photon    ] = "Photon Ecal map all" ;
  m_errorCounterName [ L0DUBase::CaloType::Photon  ] = "PhoErrorCounter" ;  
  m_errorCounterTitle[ L0DUBase::CaloType::Photon  ] = "Photon error counter" ;


  m_mapCompareName [ L0DUBase::CaloType::Hadron    ] = "HcalMapHadCompare" ;  
  m_mapCompareTitle[ L0DUBase::CaloType::Hadron    ] = "Hadron Hcal map" ;
  m_mapAllName     [ L0DUBase::CaloType::Hadron    ] = "HcalMapHadAll" ;  
  m_mapAllTitle    [ L0DUBase::CaloType::Hadron    ] = "Hadron Hcal map all" ;  
  m_errorCounterName [ L0DUBase::CaloType::Hadron  ] = "HadErrorCounter" ;  
  m_errorCounterTitle[ L0DUBase::CaloType::Hadron  ] = "Hadron error counter" ;


  m_mapCompareName [ L0DUBase::CaloType::Pi0Local  ] = "EcalMapPilCompare" ;  
  m_mapCompareTitle[ L0DUBase::CaloType::Pi0Local  ] = "Pi0Local Ecal map" ;
  m_mapAllName     [ L0DUBase::CaloType::Pi0Local  ] = "EcalMapPilAll" ;  
  m_mapAllTitle    [ L0DUBase::CaloType::Pi0Local  ] = "Pi0Local Ecal map all" ;
  m_errorCounterName [ L0DUBase::CaloType::Pi0Local  ] = "PilErrorCounter" ;  
  m_errorCounterTitle[ L0DUBase::CaloType::Pi0Local  ] = "Pi0Local error counter" ;


  m_mapCompareName [ L0DUBase::CaloType::Pi0Global ] = "EcalMapPigCompare" ;  
  m_mapCompareTitle[ L0DUBase::CaloType::Pi0Global ] = "Pi0Global Ecal map" ;
  m_mapAllName     [ L0DUBase::CaloType::Pi0Global ] = "EcalMapPigAll" ;  
  m_mapAllTitle    [ L0DUBase::CaloType::Pi0Global ] = "Pi0Global Ecal map all";
  m_errorCounterName [ L0DUBase::CaloType::Pi0Global  ] = "PigErrorCounter" ;  
  m_errorCounterTitle[ L0DUBase::CaloType::Pi0Global  ] = "Pi0Global error counter" ;

}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloCompare::~L0CaloCompare() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloCompare::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) {
    debug() << "==> Initialize" << endmsg;
    debug() << "==> Monitoring histograms booking " << endmsg;
  }
  
  std::string det( "" ) ;
  for ( int i = 0 ; i <= L0DUBase::CaloType::Pi0Global ; ++i ) {
    det = "Ecal" ;
    if ( L0DUBase::CaloType::Hadron == i ) det = "Hcal" ;
    bookCalo2D( m_mapCompareName[ i ] , m_mapCompareTitle[ i ]  , det ) ; 
    bookCalo2D( m_mapAllName[ i ]     , m_mapAllTitle[ i ]      , det ) ;
    m_errorCounterHisto[i] = GaudiHistoAlg::book(m_errorCounterName[ i ] , m_errorCounterTitle[ i ] ,0.,2.,2) ;

  }

  m_histSpdMult_Comp  = GaudiHistoAlg::book( "SpdMult_Comp" , 
                                             "SpdMult comparison" ,
                                             -101. , 101. , 202 ) ;
  m_histSumEt_Comp    = GaudiHistoAlg::book( "SumEt_Comp", 
                                             "SumEt comparison " , 
                                             -101. , 101. , 202 ) ;

  m_histTotalCount = GaudiHistoAlg::book( "TotalCounter_Comp" , 
                                          "Counter of compared candidates" , 
                                          0 , 4 , 4 ) ;

  m_histErrorCount = GaudiHistoAlg::book( "ErrorCounter_Comp" , 
                                          "Counter of compared candidates in error" , 
                                          0 , 4 , 4 ) ;

  m_idles.insert( m_idleBCIdVector.begin() , m_idleBCIdVector.end() ) ;

  return StatusCode::SUCCESS; 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCompare::execute() {

  // Remove events during IDLEs
  // Read ODIN bank to obtain BCId and event number
  unsigned int bcId( 4000 ) ; 

  LHCb::ODIN * odin = getIfExists< LHCb::ODIN >( LHCb::ODINLocation::Default ) ;
  if ( NULL != odin ) bcId = odin -> bunchId() ;
  
  if ( std::binary_search( m_idles.begin() , m_idles.end() , bcId ) ) 
    return StatusCode::SUCCESS ;

  LHCb::L0CaloCandidates::const_iterator candRef;

  LHCb::L0CaloCandidates::const_iterator candCheck;

  std::string inputLocationReference( "" ) ;
  std::string inputLocationCheck    ( "" ) ;
  
  if ( m_fullMonitoring ) {
    inputLocationReference = 
      LHCb::L0CaloCandidateLocation::Full + m_referenceDataSuffix ;
    inputLocationCheck =
      LHCb::L0CaloCandidateLocation::Full + m_checkDataSuffix ;
  } else {
    inputLocationReference = 
      LHCb::L0CaloCandidateLocation::Default + m_referenceDataSuffix ;
    inputLocationCheck = 
      LHCb::L0CaloCandidateLocation::Default + m_checkDataSuffix ;
  }

  if( msgLevel(MSG::DEBUG) ) {
    debug() << "Execute will read " << inputLocationReference 
            << " as a reference" << endmsg ;
    debug() << "Execute will read " << inputLocationCheck
            << " to be checked " << endmsg;
  }
    
  LHCb::L0CaloCandidates* candidatesRef  = getIfExists<LHCb::L0CaloCandidates>( inputLocationReference );
  if ( NULL == candidatesRef ) return Warning( "REF Not found", StatusCode::SUCCESS );

  LHCb::L0CaloCandidates* candidatesCheck = getIfExists<LHCb::L0CaloCandidates>( inputLocationCheck );
  if ( NULL == candidatesCheck ) return Warning( "CHECK Not found", StatusCode::SUCCESS );

  LHCb::L0CaloCandidates* candidatesDefaultRef(0);
  LHCb::L0CaloCandidates* candidatesDefaultCheck(0);
  
  if ( m_fullMonitoring ) {
    inputLocationReference = 
      LHCb::L0CaloCandidateLocation::Default + m_referenceDataSuffix ;
    inputLocationCheck = 
      LHCb::L0CaloCandidateLocation::Default + m_checkDataSuffix ;    

    candidatesDefaultRef = getIfExists<LHCb::L0CaloCandidates>( inputLocationReference );
    if( NULL == candidatesDefaultRef ) return Warning( "Default REF Not found", StatusCode::SUCCESS );
    
    candidatesDefaultCheck = getIfExists<LHCb::L0CaloCandidates>( inputLocationCheck );
    if( NULL == candidatesDefaultCheck ) return Warning( "Default CHECK Not found", StatusCode::SUCCESS );
  }

  // First fill a map for each type ... for the reference L0 candidates
  typedef std::multimap< int , LHCb::L0CaloCandidate * > l0cmap ;
  std::vector< l0cmap > mapRef ;
  mapRef.reserve( 5 ) ;
  mapRef.resize( 5 , l0cmap() ) ;

  LHCb::L0CaloCandidate * SumEtRef = 0 ;  
  LHCb::L0CaloCandidate * SpdMultRef = 0 ; 

  LHCb::CaloCellID caloCell ;
  int rawId( 0 ) ;
  int type( 0 ) ;

  for ( candRef = candidatesRef->begin() ; candidatesRef->end() != candRef ; 
        ++candRef ) {
    caloCell = (*candRef) -> id() ; 
    rawId    = (*candRef) -> id().all() ; 
    type     = (*candRef) -> type() ;
    
    switch ( type ) {
    case L0DUBase::CaloType::Electron:
    case L0DUBase::CaloType::Photon:
    case L0DUBase::CaloType::Pi0Local:
    case L0DUBase::CaloType::Pi0Global:
    case L0DUBase::CaloType::Hadron:
      if( msgLevel(MSG::DEBUG) )
        debug() << "Type= " << type << " cellID = " << caloCell 
                << " etCode = " << (*candRef)->etCode() 
                << " rawId= " << rawId << endmsg;
      mapRef[type].insert(std::pair<int,LHCb::L0CaloCandidate *>( rawId , 
                                                                  *candRef ) ) ;
      m_errorCounterHisto[type]->fill(0.5) ;
      break ;
    case L0DUBase::CaloType::SpdMult:
      // in full monitoring mode, ignore SpdMult candidates which are 
      // partial multiplicities
      if ( m_fullMonitoring ) break ;
      if( msgLevel(MSG::DEBUG) ) 
        debug() << " SpdMult (1) : etCode = " << (*candRef)->etCode() 
                << " rawId= " << rawId << endmsg ;
      SpdMultRef = *candRef ;
      break ; 
    case L0DUBase::CaloType::SumEt:
      // in full monitoring mode, ignore SumEt candidates which are 
      // partial multiplicities
      if ( m_fullMonitoring ) break ;
      if( msgLevel(MSG::DEBUG) ) 
        debug() << " SumEt : etCode = " << (*candRef)->etCode() 
                << " rawId= " << rawId << endmsg;
      SumEtRef = *candRef ;
      break ; 
    default:
      break ;
    }
  }

  // If full monitoring, look also at the default container to obtain the
  // values of SPD Mult and SumEt which are only there
  if ( m_fullMonitoring ) {
    for ( candRef = candidatesDefaultRef->begin() ; 
          candidatesDefaultRef->end() != candRef ; ++candRef ) {
      type = (*candRef) -> type() ;
      
      switch ( type ) {
      case L0DUBase::CaloType::SpdMult:
        if( msgLevel(MSG::DEBUG) ) 
          debug() << " SpdMult (2) : etCode = " << (*candRef)->etCode() 
                  << " rawId= " << rawId << endmsg ;
        SpdMultRef = *candRef ;
        break ; 
      case L0DUBase::CaloType::SumEt:
        if( msgLevel(MSG::DEBUG) ) 
          debug() << " SumEt : etCode = " << (*candRef)->etCode() 
                  << " rawId= " << rawId << endmsg;
        SumEtRef = *candRef ;
        break ; 
      default:
        break ;
      }
    }  
  }

  l0cmap::iterator iterMap ;  
  int etCodeCheck( 0 ) , etCodeRef( 0 ) ;

  for ( candCheck = candidatesCheck->begin() ; 
        candidatesCheck->end() != candCheck ; ++candCheck ) {
    caloCell    = (*candCheck) -> id() ; 
    rawId       = (*candCheck) -> id().all() ; 
    etCodeCheck = (*candCheck) -> etCode() ; 
    type        = (*candCheck) -> type() ;
    
    switch ( type ) {
    case L0DUBase::CaloType::Electron:
    case L0DUBase::CaloType::Photon:
    case L0DUBase::CaloType::Pi0Local:
    case L0DUBase::CaloType::Pi0Global:
    case L0DUBase::CaloType::Hadron:

      if( msgLevel(MSG::DEBUG) ) 
        debug() << "Type= " << type << " cellID to check = " << caloCell 
                << " etCode = " << etCodeCheck << " rawId= " << rawId << endmsg ;

      fillCalo2D( m_mapAllName[ type ] , caloCell , 1. , 
                  m_mapAllTitle[ type ] ) ;

      if ( type == L0DUBase::CaloType::Electron ) 
        m_histTotalCount -> fill( 0.5 ) ;
      else if ( type == L0DUBase::CaloType::Photon ) 
        m_histTotalCount -> fill( 1.5 ) ;
      else if ( type == L0DUBase::CaloType::Hadron ) 
        m_histTotalCount -> fill( 2.5 ) ;
      
      iterMap = mapRef[ type ].find( rawId ) ; 
      if ( iterMap == mapRef[ type ].end() ) {
        if( msgLevel(MSG::DEBUG) ) 
          debug() << "          " << candidateTypeName( type ) 
                  << " L0cand not found ! " << endmsg ; 
        fillCalo2D( m_mapCompareName[ type ] , caloCell , 1. , 
                    m_mapCompareTitle[ type ] ) ;
        m_errorCounterHisto[type]->fill(1.5) ;

        if ( type == L0DUBase::CaloType::Electron ) 
          m_histErrorCount -> fill( 0.5 ) ;
        else if ( type == L0DUBase::CaloType::Photon ) 
          m_histErrorCount -> fill( 1.5 ) ;
        else if ( type == L0DUBase::CaloType::Hadron ) 
          m_histErrorCount -> fill( 2.5 ) ;

      } else {
        //        LHCb::L0CaloCandidate * theCand = (*iterMap).second ; 
        std::pair< l0cmap::iterator , l0cmap::iterator > 
          res = mapRef[ type ].equal_range( rawId ) ;
        bool found = false ;
        for ( iterMap = res.first ; iterMap != res.second ; ++iterMap ) {
          LHCb::L0CaloCandidate * theCand = (*iterMap).second ;
          etCodeRef = theCand -> etCode() ;
          if ( etCodeCheck == etCodeRef ) found = true ;
        }
        if ( ! found ) {
          if( msgLevel(MSG::DEBUG) ) 
            debug() << " Same cell but different etCode : ref = " << etCodeRef  
                    << " check = " << etCodeCheck << endmsg ;
          fillCalo2D( m_mapCompareName[ type ] , caloCell , 1. , 
                      m_mapCompareTitle[ type ] ) ; 
          m_errorCounterHisto[type]->fill(1.5) ; 

          if ( type == L0DUBase::CaloType::Electron ) 
            m_histErrorCount -> fill( 0.5 ) ;
          else if ( type == L0DUBase::CaloType::Photon ) 
            m_histErrorCount -> fill( 1.5 ) ;
          else if ( type == L0DUBase::CaloType::Hadron ) 
            m_histErrorCount -> fill( 2.5 ) ;

        }
      }
      
      break ;
    case L0DUBase::CaloType::SumEt:
      // in full monitoring mode, ignore SumET candidates which are 
      // partial multiplicities
      if ( m_fullMonitoring ) break ;
      {
        if( msgLevel(MSG::DEBUG) ) 
          debug() << " SumEt : etCode = " << etCodeCheck << endmsg ;

        // m_histTotalCount -> fill( 3.5 ) ;

        int sumRef( 0 ) ;
        int diff_SumEt( 0 ) ;
        if ( 0 != SumEtRef ) {
          sumRef = SumEtRef -> etCode() ;
          if( msgLevel(MSG::DEBUG) ) 
            debug() << "SumEtRef etCode = " << sumRef  << endmsg ;
        }
        diff_SumEt = etCodeCheck - sumRef ;
        if ( diff_SumEt > 100.  ) diff_SumEt = 100 ; 
        else if ( diff_SumEt < -100. ) diff_SumEt = -100 ; 
        m_histSumEt_Comp -> fill( diff_SumEt ) ; 
        if ( etCodeCheck != sumRef ) { 
          if( msgLevel(MSG::DEBUG) ) debug() << " SumEt ... Pb " <<endmsg;  
          // m_histErrorCount -> fill( 3.5 ) ;
        }
      }
    break ; 
    case L0DUBase::CaloType::SpdMult:
      // in full monitoring mode, ignore SpdMult candidates which are 
      // partial multiplicities
      if ( m_fullMonitoring ) break ;
      { 
        if( msgLevel(MSG::DEBUG) ) 
          debug() << "SpdMult (3) : etCode = " << etCodeCheck << endmsg ;
        m_histTotalCount -> fill( 3.5 ) ;

        int spdRef( 0 ) ;
        int diff_SpdMult( 0 ) ;
        if ( 0 != SpdMultRef ) spdRef = SpdMultRef -> etCode() ;
        diff_SpdMult = etCodeCheck - spdRef ;
        if (diff_SpdMult > 100.) diff_SpdMult = 100 ; 
        else if (diff_SpdMult < -100.) diff_SpdMult = -100 ; 
        m_histSpdMult_Comp -> fill( diff_SpdMult ) ; 
        if ( etCodeCheck != spdRef ) { 
          if( msgLevel(MSG::DEBUG) ) debug() << " SpdMult ... Pb (1) " <<endmsg;
          m_histErrorCount -> fill( 3.5 ) ;
        }
      }
    break ; 
    default:
      break ;
    }
  }

  if ( m_fullMonitoring ) {
    for ( candCheck = candidatesDefaultCheck->begin() ; 
          candidatesDefaultCheck->end() != candCheck ; ++candCheck ) {
      etCodeCheck = (*candCheck) -> etCode() ; 
      type        = (*candCheck) -> type() ;
      
      switch ( type ) {
      case L0DUBase::CaloType::SumEt:
        {
          if( msgLevel(MSG::DEBUG) ) 
            debug() << " SumEt : etCode = " << etCodeCheck << endmsg ;

          // m_histTotalCount -> fill( 3.5 ) ;

          int sumRef( 0 ) ;
          int diff_SumEt( 0 ) ;
          if ( 0 != SumEtRef ) {
            sumRef = SumEtRef -> etCode() ;
            if( msgLevel(MSG::DEBUG) ) 
              debug() << "SumEtRef etCode = " << sumRef  << endmsg ;
          }
          diff_SumEt = etCodeCheck - sumRef ;
          if ( diff_SumEt > 100.  ) diff_SumEt = 100 ; 
          else if ( diff_SumEt < -100. ) diff_SumEt = -100 ; 
          m_histSumEt_Comp -> fill(diff_SumEt) ; 
          if ( etCodeCheck != sumRef ) { 
            if( msgLevel(MSG::DEBUG) ) debug() << " SumEt ... Pb " <<endmsg;  
            // m_histErrorCount -> fill( 3.5 ) ;
          }  
        }
        
      break ; 
      case L0DUBase::CaloType::SpdMult:
        { 
          if( msgLevel(MSG::DEBUG) ) 
            debug() << "SpdMult (4) : etCode = " << etCodeCheck << endmsg ;
          
          m_histTotalCount -> fill( 3.5 ) ;

          int spdRef( 0 ) ;
          int diff_SpdMult( 0 ) ;
          if ( 0 != SpdMultRef ) spdRef = SpdMultRef -> etCode() ;
          diff_SpdMult = etCodeCheck - spdRef ;
          if (diff_SpdMult > 100.) diff_SpdMult = 100 ; 
          else if (diff_SpdMult < -100.) diff_SpdMult = -100 ; 
          m_histSpdMult_Comp -> fill( diff_SpdMult ) ; 
          if ( etCodeCheck != spdRef ) {
            if( msgLevel(MSG::DEBUG) ) debug() << " SpdMult ... Pb (2) " <<endmsg;
            m_histErrorCount -> fill( 3.5 ) ;
          } 
        }
      break ;
      default: 
        break ;
      }
    }
  }
  
  return StatusCode::SUCCESS; 
}
//============================================================================
// Prints string to describe type
//============================================================================
std::string L0CaloCompare::candidateTypeName( int type ) const {
  switch( type ) {
  case L0DUBase::CaloType::Electron: return "Ele" ; break ;
  case L0DUBase::CaloType::Photon: return "Pho" ; break ;
  case L0DUBase::CaloType::Pi0Local: return "Pil" ; break ;
  case L0DUBase::CaloType::Pi0Global: return "Pig" ; break ;
  case L0DUBase::CaloType::Hadron: return "Had" ; break ;
  default: break ;
  }
  return "Unknown" ;
}
