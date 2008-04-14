// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloCompare.cpp,v 1.1 2008-04-14 07:46:45 robbep Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event/L0Event
#include "Event/L0CaloCandidate.h"
#include "Event/RawEvent.h"
#include "Event/L0DUBase.h"

#include "Event/ODIN.h"
// local
#include "L0CaloCompare.h"

DECLARE_ALGORITHM_FACTORY( L0CaloCompare );

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCompare
//
//-----------------------------------------------------------------------------

// Standard creator
L0CaloCompare::L0CaloCompare( const std::string& name, 
                          ISvcLocator* pSvcLocator )  
  : Calo2Dview ( name , pSvcLocator ) 
{ 
  declareProperty("FullMonitoring"   , m_fullMonitoring    = true ) ;  
  declareProperty( "ReferenceDataSuffix" , m_referenceDataSuffix = "" ) ;  
  declareProperty( "CheckDataSuffix" , m_checkDataSuffix = "RAW" ) ;  
}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloCompare::~L0CaloCompare() {};

//=============================================================================
// Finalize.
//=============================================================================
StatusCode L0CaloCompare::finalize() {

  info()<<"Number of events "<<m_nEvents <<endmsg;
  info()<<"Number of useful events (Odin bank decoded) "<<m_nUsefulEvents <<endmsg;
  return Calo2Dview::finalize();  // must be called after all other actions
}


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloCompare::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_nUsefulEvents = 0 ; 
  m_nEvents = 0 ; 

  // Retrieve the ECAL detector element, build cards
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  // Retrieve the HCAL detector element, build cards
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  

  debug() << "==> Monitoring histograms booking " << endmsg;

  bookCalo2D(10001,"Electron Ecal map" ,"Ecal") ; 
  bookCalo2D(10002,"Photon Ecal map" ,"Ecal") ; 
  bookCalo2D(10004,"Pi0Local Ecal map" ,"Ecal") ; 
  bookCalo2D(10005,"Pi0Global Ecal map" ,"Ecal") ; 
  bookCalo2D(10003,"Hadron Hcal map" ,"Hcal") ; 
 
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCompare::execute() {

  LHCb::L0CaloCandidates* candidatesRef  ; 
  LHCb::L0CaloCandidates::const_iterator candRef;

  LHCb::L0CaloCandidates* candidatesCheck  ; 
  LHCb::L0CaloCandidates::const_iterator candCheck;
  
  if (m_fullMonitoring == true) { 
    debug() << "Execute will read " <<LHCb::L0CaloCandidateLocation::Full+m_referenceDataSuffix<<" as a reference"<< endreq;
    debug() << "Execute will read " <<LHCb::L0CaloCandidateLocation::Full+m_checkDataSuffix<<" to be checked "<< endreq;
    candidatesRef = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full+m_referenceDataSuffix );
    candidatesCheck = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full+m_checkDataSuffix );

  }
  if (m_fullMonitoring != true) { 
    debug() << "Execute will read " <<LHCb::L0CaloCandidateLocation::Default+m_referenceDataSuffix<<" as a reference"<< endreq;
    debug() << "Execute will read " <<LHCb::L0CaloCandidateLocation::Default+m_checkDataSuffix<<" to be checked "<< endreq;
    candidatesRef = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Default+m_referenceDataSuffix );
    candidatesCheck = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Default+m_checkDataSuffix );
  }

  m_nEvents++ ; 

  double event = -999 ; 
  double BCId = -999 ; 

  if(exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin=get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    event = odin->eventNumber() ; 
    BCId = odin->bunchId() ; 
    m_nUsefulEvents++ ; 
  } 



  // First fill a map for each type ... for the reference L0 candidates
  std::map<int ,LHCb::L0CaloCandidate * > mapEleRef ;  
  mapEleRef.clear() ; 
  std::map<int ,LHCb::L0CaloCandidate * > mapPhoRef ;  
  mapPhoRef.clear() ; 
  std::map<int ,LHCb::L0CaloCandidate * > mapPilRef ;  
  mapPilRef.clear() ; 
  std::map<int ,LHCb::L0CaloCandidate * > mapPigRef ;  
  mapPigRef.clear() ; 
  std::map<int ,LHCb::L0CaloCandidate * > mapHadRef ;  
  mapHadRef.clear() ; 

  for ( candRef = candidatesRef->begin() ; candidatesRef->end() != candRef ; ++candRef ) {
    LHCb::CaloCellID caloCell = (*candRef)->id() ; 
    int rawId = (*candRef)->id().all() ; 
    switch ( (*candRef)->type() ) {
    case L0DUBase::CaloType::Electron:
      debug()<<"Event= "<<event<<" Ele : cellID = "<<(*candRef)->id()<<" etCode = "<<(*candRef)->etCode()<<" rawId= "<<rawId<<endreq;
      mapEleRef[rawId] = *candRef ;
      break ; 
    case L0DUBase::CaloType::Photon:
      debug()<<"Event= "<<event<<" Pho : cellID = "<<(*candRef)->id()<<" etCode = "<<(*candRef)->etCode()<<" rawId= "<<rawId<<endreq;
      mapPhoRef[rawId] = *candRef ;
      break ; 
    case L0DUBase::CaloType::Pi0Local:
      debug()<<"Event= "<<event<<" Pil : cellID = "<<(*candRef)->id()<<" etCode = "<<(*candRef)->etCode()<<" rawId= "<<rawId<<endreq;
      mapPilRef[rawId] = *candRef ;
      break ; 
    case L0DUBase::CaloType::Pi0Global:
      debug()<<"Event= "<<event<<" Pig : cellID = "<<(*candRef)->id()<<" etCode = "<<(*candRef)->etCode()<<" rawId= "<<rawId<<endreq;
      mapPigRef[rawId] = *candRef ;
      break ; 
    case L0DUBase::CaloType::Hadron:
      debug()<<"Event= "<<event<<" Had : cellID = "<<(*candRef)->id()<<"etCode = "<<(*candRef)->etCode()<<" rawId= "<<rawId<<endreq;
      mapHadRef[rawId] = *candRef ;
      break ; 
    }
  }     
  

  std::map<int ,LHCb::L0CaloCandidate * >::iterator iterMap ;  


  for ( candCheck = candidatesCheck->begin() ; candidatesCheck->end() != candCheck ; ++candCheck ) {
    LHCb::CaloCellID caloCell = (*candCheck)->id() ; 
    int rawId = (*candCheck)->id().all() ; 
    int etCodeCheck = (*candCheck)->etCode() ; 
    switch ( (*candCheck)->type() ) {
    case L0DUBase::CaloType::Electron:
      debug()<<"Event= "<<event<<" Ele : cellID to check = "<<(*candCheck)->id()<<" etCode = "<<(*candCheck)->etCode()<<" rawId= "<<rawId<<endreq;
      iterMap = mapEleRef.find(rawId) ; 
      if (iterMap == mapEleRef.end()) debug()<<"          Ele L0cand not found ! "<<endreq; 
      if (iterMap == mapEleRef.end()) fillCalo2D(10001,caloCell,1.,"Electron Ecal map") ; 
      else{
	LHCb::L0CaloCandidate* theCand = (*iterMap).second ; 
	int etCodeRef = theCand->etCode() ;
	if ( etCodeCheck =! etCodeRef) debug()<<" Ele : same cell but different etCode : ref = "<<theCand->etCode()<<" check = "<<(*candCheck)->etCode()<<endreq;
	if ( etCodeCheck =! etCodeRef) fillCalo2D(10001,caloCell,1.,"Electron Ecal map") ; 
      }
      break ; 
    case L0DUBase::CaloType::Photon:
      debug()<<"Event= "<<event<<" Pho : cellID to check = "<<(*candCheck)->id()<<" etCode = "<<(*candCheck)->etCode()<<" rawId= "<<rawId<<endreq;
      iterMap = mapPhoRef.find(rawId) ; 
      if (iterMap == mapPhoRef.end()) debug()<<"          Pho L0cand not found ! "<<endreq; 
      if (iterMap == mapPhoRef.end()) fillCalo2D(10002,caloCell,1.,"Photon Ecal map") ; 
      else{
	LHCb::L0CaloCandidate* theCand = (*iterMap).second ; 
	int etCodeRef = theCand->etCode() ;
	if ( etCodeCheck =! etCodeRef) debug()<<" Pho : same cell but different etCode : ref = "<<theCand->etCode()<<" check = "<<(*candCheck)->etCode()<<endreq;
	if ( etCodeCheck =! etCodeRef) fillCalo2D(10002,caloCell,1.,"Photon Ecal map") ; 
      }
      break ; 
    case L0DUBase::CaloType::Pi0Local:
      debug()<<"Event= "<<event<<" Pil : cellID to check = "<<(*candCheck)->id()<<" etCode = "<<(*candCheck)->etCode()<<" rawId= "<<rawId<<endreq;
      iterMap = mapPilRef.find(rawId) ; 
      if (iterMap == mapPilRef.end()) debug()<<"          Pil L0cand not found ! "<<endreq; 
      if (iterMap == mapPilRef.end()) fillCalo2D(10004,caloCell,1.,"Pi0Local Ecal map") ; 
      else{
	LHCb::L0CaloCandidate* theCand = (*iterMap).second ; 
	int etCodeRef = theCand->etCode() ;
	if ( etCodeCheck =! etCodeRef) debug()<<" Pil : same cell but different etCode : ref = "<<theCand->etCode()<<" check = "<<(*candCheck)->etCode()<<endreq;
	if ( etCodeCheck =! etCodeRef) fillCalo2D(10004,caloCell,1.,"Pi0Local Ecal map") ; 
      }
      break ; 
    case L0DUBase::CaloType::Pi0Global:
      debug()<<"Event= "<<event<<" Pig : cellID to check = "<<(*candCheck)->id()<<" etCode = "<<(*candCheck)->etCode()<<" rawId= "<<rawId<<endreq;
      iterMap = mapPigRef.find(rawId) ; 
      if (iterMap == mapPigRef.end()) debug()<<"          Pig L0cand not found ! "<<endreq; 
      if (iterMap == mapPigRef.end()) fillCalo2D(10005,caloCell,1.,"Pi0Global Ecal map") ; 
      else{
	LHCb::L0CaloCandidate* theCand = (*iterMap).second ; 
	int etCodeRef = theCand->etCode() ;
	if ( etCodeCheck =! etCodeRef) debug()<<" Pig : same cell but different etCode : ref = "<<theCand->etCode()<<" check = "<<(*candCheck)->etCode()<<endreq;
	if ( etCodeCheck =! etCodeRef) fillCalo2D(10005,caloCell,1.,"Pi0Global Ecal map") ; 
      }
      break ; 
    case L0DUBase::CaloType::Hadron:
      debug()<<"Event= "<<event<<" Had : cellID to check = "<<(*candCheck)->id()<<"etCode = "<<(*candCheck)->etCode()<<" rawId= "<<rawId<<endreq;
      iterMap = mapHadRef.find(rawId) ; 
      if (iterMap == mapHadRef.end()) debug()<<"          Had L0cand not found ! "<<endreq; 
      if (iterMap == mapHadRef.end()) fillCalo2D(10003,caloCell,1.,"Hadron Hcal map") ; 
      else{
	LHCb::L0CaloCandidate* theCand = (*iterMap).second ; 
	int etCodeRef = theCand->etCode() ;
	if ( etCodeCheck =! etCodeRef) debug()<<" Had : same cell but different etCode : ref = "<<theCand->etCode()<<" check = "<<(*candCheck)->etCode()<<endreq;
	if ( etCodeCheck =! etCodeRef) fillCalo2D(10003,caloCell,1.,"Hadron Hcal map") ; 
      }
      break ; 
    }
  }     


  //  return sc ; 
  return StatusCode::SUCCESS; 
}
//============================================================================
