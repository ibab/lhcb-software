// Include files

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUBase.h"

//From LHCb
#include "Event/ODIN.h" 

// From CaloDet 
#include "CaloDet/DeCalorimeter.h"

// local
#include "L0CaloNtpMonitor.h"


//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloNtpMonitor
//
// 2008-02-22 : Marie-Helene Schune
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0CaloNtpMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloNtpMonitor::L0CaloNtpMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_ecal(0)
  , m_hcal(0)
{
  declareProperty( "InputDataSuffix" , m_inputDataSuffix ="" ) ;  
 }

//=============================================================================
// Destructor
//=============================================================================
L0CaloNtpMonitor::~L0CaloNtpMonitor() { } 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0CaloNtpMonitor::initialize( ) {
  StatusCode sc = GaudiTupleAlg::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg ;
  // Retrieve the ECAL detector element, build cards
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  // Retrieve the HCAL detector element, build cards
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Initialization
//=============================================================================  
StatusCode L0CaloNtpMonitor::execute( ) {
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg ;  

  Tuple ntp = nTuple( 800 , name() , CLID_ColumnWiseTuple ) ;
  StatusCode sc;
  // Get ODIN 
  LHCb::ODIN* odin = getIfExists< LHCb::ODIN >( LHCb::ODINLocation::Default ) ;
  if( NULL != odin ){
    sc = ntp -> column( "run"   , odin->runNumber()            );
    sc = ntp -> column( "event" , (double) odin->eventNumber() );
    sc = ntp -> column( "bunch" , odin->bunchId()              );
    sc = ntp -> column( "orbit" , odin->orbitNumber()          );
    sc = ntp -> column( "time"  , (double) odin->gpsTime()     );
    sc = ntp -> column( "ttype" , odin->triggerType()          );
    sc = ntp -> column( "btype" , odin->bunchCrossingType()    );
    if( msgLevel(MSG::DEBUG) ) 
      debug() <<" ODIN event "<<  odin->eventNumber()  << endmsg;
  }
  else{
    info() << " Emtpy location for ODIN " << endmsg;
  }

  std::vector< int > etCodeEle, etCodePho, etCodePil, etCodePig, etCodeHad ;
  int etCodeSumH = -999 ;
  std::vector< double > etEle, etPho, etPil, etPig, etHad ;
  double etSumH = -999. ;  
  
  std::vector< int > rowEle, rowPho, rowPil, rowPig, rowHad ;
  std::vector< int > colEle, colPho, colPil, colPig, colHad ;
  std::vector< int > cellIdEle, cellIdPho, cellIdPil, cellIdPig, cellIdHad ;
  std::vector< int > areaEle, areaPho, areaPil, areaPig, areaHad ;
  std::vector< int > crateEle, cratePho, cratePil, cratePig, crateHad ;
  std::vector< int > slotEle, slotPho, slotPil, slotPig, slotHad ;
  
  LHCb::L0CaloCandidates* candidates = 
    getIfExists<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full+m_inputDataSuffix );
  if ( NULL != candidates ) {
    LHCb::L0CaloCandidates::const_iterator cand;
    for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
      int card  = -1 ; 
      int crate = -1 ; 
      int slot = -1 ; 
      switch ( (*cand)->type() ) {
      case L0DUBase::CaloType::Electron:
        etCodeEle.push_back( (*cand) -> etCode() ) ;
        etEle.push_back( (*cand) -> et() / Gaudi::Units::GeV ) ;
        colEle.push_back((*cand) -> id().col()) ; 
        rowEle.push_back((*cand) -> id().row()) ; 
        areaEle.push_back((*cand) -> id().area()) ; 
        cellIdEle.push_back((*cand) -> id().all()) ; 
        card  = m_ecal->cardNumber((*cand) -> id()) ; 
        crate = m_ecal->cardCrate(card); 
        slot = m_ecal->cardSlot(card); 
        slotEle.push_back(slot) ; 
        crateEle.push_back(crate) ; 
        if( msgLevel(MSG::DEBUG) ) 
          debug() << " etCode electron " << (*cand) -> etCode() << " slot = "<<slot<<" crate= "<<crate<<endmsg;
        break ;
      case L0DUBase::CaloType::Photon:
        etCodePho.push_back( (*cand) -> etCode() ) ;
        etPho.push_back( (*cand) -> et() / Gaudi::Units::GeV ) ;
        colPho.push_back((*cand) -> id().col()) ; 
        rowPho.push_back((*cand) -> id().row()) ; 
        areaPho.push_back((*cand) -> id().area()) ; 
        cellIdPho.push_back((*cand) -> id().all()) ; 
        card  = m_ecal->cardNumber((*cand) -> id()) ; 
        crate = m_ecal->cardCrate(card); 
        slot = m_ecal->cardSlot(card); 
        slotPho.push_back(slot) ; 
        cratePho.push_back(crate) ; 
        break ;
      case L0DUBase::CaloType::Hadron:
        etCodeHad.push_back( (*cand) -> etCode() ) ;
        etHad.push_back( (*cand) -> et() / Gaudi::Units::GeV ) ;
        colHad.push_back((*cand) -> id().col()) ; 
        rowHad.push_back((*cand) -> id().row()) ; 
        areaHad.push_back((*cand) -> id().area()) ; 
        cellIdHad.push_back((*cand) -> id().all()) ; 
        card  = m_hcal->cardNumber((*cand) -> id()) ; 
        crate = m_hcal->cardCrate(card); 
        slot = m_hcal->cardSlot(card); 
        slotHad.push_back(slot) ; 
        crateHad.push_back(crate) ; 
        if( msgLevel(MSG::DEBUG) ) 
          debug() << " etCode hadron " << (*cand) -> etCode() << endmsg;
        break ;
      case L0DUBase::CaloType::Pi0Local:
        etCodePil.push_back( (*cand) -> etCode() ) ;
        etPil.push_back( (*cand) -> et() / Gaudi::Units::GeV ) ;
        colPil.push_back((*cand) -> id().col()) ; 
        rowPil.push_back((*cand) -> id().row()) ; 
        areaPil.push_back((*cand) -> id().area()) ; 
        cellIdPil.push_back((*cand) -> id().all()) ; 
        card  = m_ecal->cardNumber((*cand) -> id()) ; 
        crate = m_ecal->cardCrate(card); 
        slot = m_ecal->cardSlot(card); 
        slotPil.push_back(slot) ; 
        cratePil.push_back(crate) ; 
        break ;
      case L0DUBase::CaloType::Pi0Global:
        etCodePig.push_back( (*cand) -> etCode() ) ;
        etPig.push_back( (*cand) -> et() / Gaudi::Units::GeV ) ;
        colPig.push_back((*cand) -> id().col()) ; 
        rowPig.push_back((*cand) -> id().row()) ; 
        areaPig.push_back((*cand) -> id().area()) ; 
        cellIdPig.push_back((*cand) -> id().all()) ; 
        card  = m_ecal->cardNumber((*cand) -> id()) ; 
        crate = m_ecal->cardCrate(card); 
        slot = m_ecal->cardSlot(card); 
        slotPig.push_back(slot) ; 
        cratePig.push_back(crate) ; 
        break ;
      case L0DUBase::CaloType::SumEt:
        etCodeSumH = (*cand) -> etCode() ;
        etSumH = (*cand) -> et() / Gaudi::Units::GeV ;
        break ;        
      }       
    }
  }

  if( msgLevel(MSG::DEBUG) ) debug()<<" apres decodage FULL"<<endmsg ; 

  int etCodeEleDef( -999 ) , etCodePhoDef( -999 ) , etCodePilDef( -999 ) , 
    etCodePigDef( -999 ) , etCodeHadDef( -999 ) ;
  int etCodeSumHDef = -999 ;
  double  etEleDef( -999. ) , etPhoDef( -999. ) , etPilDef( -999. ) , 
    etPigDef( -999. ) , etHadDef( -999. ) ;
  double etSumHDef = -999. ;  
  
  int areaEleDef, areaPhoDef, areaPilDef, areaPigDef, areaHadDef ;
  int rowEleDef, rowPhoDef, rowPilDef, rowPigDef, rowHadDef ;
  int  colEleDef, colPhoDef, colPilDef, colPigDef, colHadDef ;
  int cardEleDef, cardPhoDef, cardPilDef, cardPigDef, cardHadDef ;
  int slotEleDef, slotPhoDef, slotPilDef, slotPigDef, slotHadDef ;
  int crateEleDef, cratePhoDef, cratePilDef, cratePigDef, crateHadDef ;
  
  areaEleDef = -999 ; 
  areaPhoDef = -999 ; 
  areaPilDef = -999 ; 
  areaPigDef = -999 ; 
  areaHadDef = -999 ; 
  
  slotEleDef = -999 ; 
  slotPhoDef = -999 ; 
  slotPilDef = -999 ; 
  slotPigDef = -999 ; 
  slotHadDef = -999 ; 
  crateEleDef = -999 ; 
  cratePhoDef = -999 ; 
  cratePilDef = -999 ; 
  cratePigDef = -999 ; 
  crateHadDef = -999 ; 
  
  rowEleDef = -999 ; 
  rowPhoDef = -999 ; 
  rowPilDef = -999 ; 
  rowPigDef = -999 ; 
  rowHadDef = -999 ; 
  colEleDef = -999 ; 
  colPhoDef = -999 ; 
  colPilDef = -999 ; 
  colPigDef = -999 ; 
  colHadDef = -999 ;
  
  int cellIdEleDef = -999 ; 
  int cellIdPhoDef = -999 ; 
  int cellIdPilDef = -999 ; 
  int cellIdPigDef = -999 ; 
  int cellIdHadDef = -999 ;
  
  LHCb::L0CaloCandidates* candidatesDef =
    getIfExists<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Default+m_inputDataSuffix );
  if ( NULL != candidatesDef ) {
    LHCb::L0CaloCandidates::const_iterator candDef;
    for ( candDef = candidatesDef->begin() ; candidatesDef->end() != candDef ; ++candDef ) {
      switch ( (*candDef)->type() ) {
      case L0DUBase::CaloType::Electron:
        etCodeEleDef =  (*candDef) -> etCode()  ;
        etEleDef =  (*candDef) -> et() / Gaudi::Units::GeV  ;
        colEleDef = (*candDef) -> id().col() ; 
        rowEleDef = (*candDef) -> id().row() ; 
        areaEleDef = (*candDef) -> id().area() ; 
        cellIdEleDef = (*candDef) -> id().all() ; 
        cardEleDef  = m_ecal->cardNumber((*candDef) -> id()) ; 
        crateEleDef = m_ecal->cardCrate(cardEleDef); 
        slotEleDef = m_ecal->cardSlot(cardEleDef); 
        break ;
      case L0DUBase::CaloType::Photon:
        etCodePhoDef =  (*candDef) -> etCode() ;
        etPhoDef =  (*candDef) -> et() / Gaudi::Units::GeV ;
        colPhoDef = (*candDef) -> id().col(); 
        rowPhoDef = (*candDef) -> id().row(); 
        areaPhoDef = (*candDef) -> id().area(); 
        cellIdPhoDef = (*candDef) -> id().all() ; 
        cardPhoDef  = m_ecal->cardNumber((*candDef) -> id()) ; 
        cratePhoDef = m_ecal->cardCrate(cardPhoDef);
        slotPhoDef = m_ecal->cardSlot(cardPhoDef);  
        break ;
      case L0DUBase::CaloType::Hadron:
        etCodeHadDef =  (*candDef) -> etCode() ;
        etHadDef =  (*candDef) -> et() / Gaudi::Units::GeV ;
        colHadDef = (*candDef) -> id().col(); 
        rowHadDef = (*candDef) -> id().row(); 
        areaHadDef = (*candDef) -> id().area(); 
        cellIdHadDef = (*candDef) -> id().all() ; 
        cardHadDef  = m_hcal->cardNumber((*candDef) -> id()) ; 
        crateHadDef = m_hcal->cardCrate(cardHadDef);
        slotHadDef = m_ecal->cardSlot(cardHadDef);   
        break ;
      case L0DUBase::CaloType::Pi0Local:
        etCodePilDef =  (*candDef) -> etCode() ;
        etPilDef =  (*candDef) -> et() / Gaudi::Units::GeV ;
        colPilDef = (*candDef) -> id().col(); 
        rowPilDef = (*candDef) -> id().row(); 
        areaPilDef = (*candDef) -> id().area(); 
        cellIdPilDef = (*candDef) -> id().all() ; 
        cardPilDef  = m_ecal->cardNumber((*candDef) -> id()) ; 
        cratePilDef = m_ecal->cardCrate(cardPilDef);
        slotPilDef = m_ecal->cardSlot(cardPilDef);   
        break ;
      case L0DUBase::CaloType::Pi0Global:
        etCodePigDef =  (*candDef) -> etCode() ;
        etPigDef =  (*candDef) -> et() / Gaudi::Units::GeV ;
        colPigDef = (*candDef) -> id().col(); 
        rowPigDef = (*candDef) -> id().row(); 
        areaPigDef = (*candDef) -> id().area(); 
        cellIdPigDef = (*candDef) -> id().all() ; 
        cardPigDef  = m_ecal->cardNumber((*candDef) -> id()) ; 
        cratePigDef = m_ecal->cardCrate(cardPigDef);
        slotPigDef = m_ecal->cardSlot(cardPigDef);   
        break ;
      case L0DUBase::CaloType::SumEt:
        etCodeSumHDef = (*candDef) -> etCode() ;
        etSumHDef = (*candDef) -> et() / Gaudi::Units::GeV ;
        break ;        
      }       
    }
  }
  
  ntp -> farray( "etCodeEle" , etCodeEle , "NEle" , 28 ) ;
  ntp -> farray( "etEle"     , etEle     , "NEle" , 28 ) ;
  ntp -> farray( "areaEle"    , areaEle    , "NEle" , 28 ) ;
  ntp -> farray( "rowEle"    , rowEle    , "NEle" , 28 ) ;
  ntp -> farray( "colEle"    , colEle    , "NEle" , 28 ) ;
  ntp -> farray( "cellIdEle" , cellIdEle , "NEle" , 28 ) ;
  ntp -> farray( "slotEle" , slotEle , "NEle" , 28 ) ;
  ntp -> farray( "crateEle" , crateEle , "NEle" , 28 ) ;
  ntp -> farray( "etCodePho" , etCodePho , "NPho" , 28 ) ;
  ntp -> farray( "etPho"     , etPho     , "NPho" , 28 ) ;
  ntp -> farray( "areaPho"    , areaPho    , "NPho" , 28 ) ;
  ntp -> farray( "rowPho"    , rowPho    , "NPho" , 28 ) ;
  ntp -> farray( "colPho"    , colPho    , "NPho" , 28 ) ;
  ntp -> farray( "cellIdPho" , cellIdPho , "NPho" , 28 ) ;
  ntp -> farray( "slotPho" , slotPho , "NPho" , 28 ) ;
  ntp -> farray( "cratePho" , cratePho , "NPho" , 28 ) ;
  ntp -> farray( "etCodePil" , etCodePil , "NPil" , 28 ) ;
  ntp -> farray( "etPil"     , etPil     , "NPil" , 28 ) ;
  ntp -> farray( "areaPil"    , areaPil    , "NPil" , 28 ) ;
  ntp -> farray( "rowPil"    , rowPil    , "NPil" , 28 ) ;
  ntp -> farray( "colPil"    , colPil    , "NPil" , 28 ) ;
  ntp -> farray( "cellIdPil" , cellIdPil , "NPil" , 28 ) ;
  ntp -> farray( "slotPil" , slotPil , "NPil" , 28 ) ;
  ntp -> farray( "cratePil" , cratePil , "NPil" , 28 ) ;
  ntp -> farray( "etCodePig" , etCodePig , "NPig" , 28 ) ;
  ntp -> farray( "etPig"     , etPig     , "NPig" , 28 ) ;
  ntp -> farray( "areaPig"    , areaPig    , "NPig" , 28 ) ;
  ntp -> farray( "rowPig"    , rowPig    , "NPig" , 28 ) ;
  ntp -> farray( "colPig"    , colPig    , "NPig" , 28 ) ;
  ntp -> farray( "cellIdPig" , cellIdPig , "NPig" , 28 ) ;
  ntp -> farray( "slotPig" , slotPig , "NPig" , 28 ) ;
  ntp -> farray( "cratePig" , cratePig , "NPig" , 28 ) ;
  ntp -> farray( "etCodeHad" , etCodeHad , "NHad" , 80 ) ;
  ntp -> farray( "etHad"     , etHad     , "NHad" , 80 ) ;
  ntp -> farray( "areaHad"    , areaHad    , "NHad" , 80 ) ;
  ntp -> farray( "rowHad"    , rowHad    , "NHad" , 80 ) ;
  ntp -> farray( "colHad"    , colHad    , "NHad" , 80 ) ;
  ntp -> farray( "cellIdHad" , cellIdHad , "NHad" , 80 ) ;
  ntp -> farray( "slotHad" , slotHad , "NHad" , 80 ) ;
  ntp -> farray( "crateHad" , crateHad , "NHad" , 80 ) ;

  ntp -> column( "etCodeSumH" , etCodeSumH ) ;
  ntp -> column( "etSumH" , etSumH ) ;  
  
  ntp -> column( "etCodeEleDef" , etCodeEleDef ) ; 
  ntp -> column( "etEleDef"     , etEleDef    ) ; 
  ntp -> column( "areaEleDef"    , areaEleDef   ) ; 
  ntp -> column( "rowEleDef"    , rowEleDef   ) ; 
  ntp -> column( "colEleDef"    , colEleDef   ) ; 
  ntp -> column( "cellIdEleDef" , cellIdEleDef) ; 
  ntp -> column( "crateEleDef" , crateEleDef) ; 
  ntp -> column( "slotEleDef" , slotEleDef) ; 
  ntp -> column( "etCodePhoDef" , etCodePhoDef) ; 
  ntp -> column( "etPhoDef"     , etPhoDef    ) ; 
  ntp -> column( "areaPhoDef"    , areaPhoDef   ) ; 
  ntp -> column( "rowPhoDef"    , rowPhoDef   ) ; 
  ntp -> column( "colPhoDef"    , colPhoDef   ) ; 
  ntp -> column( "cellIdPhoDef" , cellIdPhoDef) ; 
  ntp -> column( "cratePhoDef" , cratePhoDef) ; 
  ntp -> column( "slotPhoDef" , slotPhoDef) ; 
  ntp -> column( "etCodePilDef" , etCodePilDef) ; 
  ntp -> column( "etPilDef"     , etPilDef    ) ; 
  ntp -> column( "areaPilDef"    , areaPilDef   ) ; 
  ntp -> column( "rowPilDef"    , rowPilDef   ) ; 
  ntp -> column( "colPilDef"    , colPilDef   ) ; 
  ntp -> column( "cellIdPilDef" , cellIdPilDef) ; 
  ntp -> column( "cratePilDef" , cratePilDef) ; 
  ntp -> column( "slotPilDef" , slotPilDef) ; 
  ntp -> column( "etCodePigDef" , etCodePigDef) ; 
  ntp -> column( "etPigDef"     , etPigDef    ) ; 
  ntp -> column( "areaPigDef"    , areaPigDef   ) ; 
  ntp -> column( "rowPigDef"    , rowPigDef   ) ; 
  ntp -> column( "colPigDef"    , colPigDef   ) ; 
  ntp -> column( "cellIdPigDef" , cellIdPigDef) ; 
  ntp -> column( "cratePigDef" , cratePigDef) ; 
  ntp -> column( "slotPigDef" , slotPigDef) ; 
  ntp -> column( "etCodeHadDef" , etCodeHadDef) ; 
  ntp -> column( "etHadDef"     , etHadDef    ) ; 
  ntp -> column( "areaHadDef"    , areaHadDef   ) ; 
  ntp -> column( "rowHadDef"    , rowHadDef   ) ; 
  ntp -> column( "colHadDef"    , colHadDef   ) ; 
  ntp -> column( "cellIdHadDef" , cellIdHadDef) ; 
  ntp -> column( "crateHadDef" , crateHadDef) ; 
  ntp -> column( "slotHadDef" , slotHadDef) ; 
  ntp -> column( "etCodeSumHDef" , etCodeSumHDef ) ;
  ntp -> column( "etSumHDef" , etSumHDef ) ;  

  ntp->write(); 
  return StatusCode::SUCCESS ;
}
