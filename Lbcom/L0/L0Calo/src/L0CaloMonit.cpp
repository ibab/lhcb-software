// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloMonit.cpp,v 1.22 2008-07-25 09:04:34 robbep Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram2D.h"
// Event/L0Event
#include "Event/L0CaloCandidate.h"
#include "Event/RawEvent.h"
#include "Event/L0DUBase.h"
#include "Event/ODIN.h"

// Det
#include "CaloDet/DeCalorimeter.h"

// local
#include "L0CaloMonit.h"

DECLARE_ALGORITHM_FACTORY( L0CaloMonit );

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloMonit
//
// 31/05/2001 : Olivier Callot
//-----------------------------------------------------------------------------

// Standard creator
L0CaloMonit::L0CaloMonit( const std::string& name, 
                          ISvcLocator* pSvcLocator )  
  : Calo2Dview ( name , pSvcLocator ) 
{ 
  declareProperty( "FullMonitoring"  , m_fullMonitoring  = false ) ;  
  declareProperty( "InputDataSuffix" , m_inputDataSuffix = ""   ) ;  
  declareProperty( "UpdateFrequency" , m_updateFrequency = -1   ) ;  
  declareProperty( "LookForHotCells" , m_lookForHotCells = false  ) ;  
  declareProperty( "AlarmThresholdRatio", m_alarmThresholdRatio = 5 ) ; 
}

//=============================================================================
// Standard destructor
//=============================================================================
L0CaloMonit::~L0CaloMonit() {};

//=============================================================================
// Finalize.
//=============================================================================
StatusCode L0CaloMonit::finalize() {
  info() << "Number of events " << m_nEvents << endmsg ;
  info() << "Number of useful events (Odin bank decoded) " << m_nUsefulEvents 
         << endmsg;

  if ( ( m_updateFrequency < 0 ) && ( m_lookForHotCells ) ) { 
    int caloType = 1 ; 
    info()<<" =========================Hot cells ========================="<<endreq ; 
    info()<<" ============================================================"<<endreq ; 
    info()<<" ===        Electron Candidates Inner region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histEleFreqInn , caloType ) ; 
    info()<<" ===        Electron Candidates Middle region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histEleFreqMid , caloType ) ; 
    info()<<" ===        Electron Candidates Outer region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histEleFreqOut , caloType ) ; 
    info()<<" ============================================================"<<endreq ; 
    info()<<" ===        Photon Candidates Inner region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPhoFreqInn , caloType ) ; 
    info()<<" ===        Photon Candidates Middle region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPhoFreqMid , caloType ) ; 
    info()<<" ===        Photon Candidates Outer region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPhoFreqOut , caloType ) ; 
    info()<<" ============================================================"<<endreq ; 
    info()<<" ===        Pi0Local Candidates Inner region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPilFreqInn , caloType ) ; 
    info()<<" ===        Pi0Local Candidates Middle region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPilFreqMid , caloType ) ; 
    info()<<" ===        Pi0Local Candidates Outer region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPilFreqOut , caloType ) ; 
    info()<<" ============================================================"<<endreq ; 
    info()<<" ===        Pi0Global Candidates Inner region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPigFreqInn , caloType ) ; 
    info()<<" ===        Pi0Global Candidates Middle region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPigFreqMid , caloType ) ; 
    info()<<" ===        Pi0Global Candidates Outer region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histPigFreqOut , caloType ) ; 
    caloType = 2 ; 
    info()<<" ============================================================"<<endreq ; 
    info()<<" ===        Hadron Candidates Inner region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histHadFreqInn , caloType ) ; 
    info()<<" ===        Hadron Candidates Outer region              ===" <<endreq ; 
    SearchForHotCellsAndReset(m_histHadFreqOut , caloType ) ; 
    info()<<" ============================================================"<<endreq ; 
  }
    
  return Calo2Dview::finalize();  // must be called after all other actions
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloMonit::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_nUsefulEvents = 0 ; 
  m_nEvents = 0 ; 

  // Retrieve the ECAL detector element, build cards
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  // Retrieve the HCAL detector element, build cards
  m_hcal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  

  debug() << "==> Default Monitoring histograms booking " << endmsg;
  
  float delta  ; 

  int   nBinEt = 255 ; 
  float xMinEt = 0   ; 
  float xMaxEt = 255. ; 
  delta = (xMaxEt - xMinEt)/nBinEt ; 
  xMinEt = xMinEt - delta*.5 ; 
  xMaxEt = xMaxEt - delta*.5 ; 

  int   nBinSumEt = 16383 ; 
  float xMinSumEt = 0 ; 
  float xMaxSumEt = 16383. ; 
  delta = (xMaxSumEt - xMinSumEt)/nBinSumEt ; 
  xMinSumEt = xMinSumEt - delta*.5 ; 
  xMaxSumEt = xMaxSumEt - delta*.5 ; 

  int   nBinSpdMult = 1023 ; 
  float xMinSpdMult = 0 ; 
  float xMaxSpdMult = 1023. ; 
  delta = (xMaxSpdMult - xMinSpdMult)/nBinSpdMult ; 
  xMinSpdMult = xMinSpdMult - delta*.5 ; 
  xMaxSpdMult = xMaxSpdMult - delta*.5 ; 

  
  m_histElectron  = GaudiHistoAlg::book( "EtEle", "Et electron "   , xMinEt, xMaxEt, nBinEt );
  m_histPhoton    = GaudiHistoAlg::book( "EtPho", "Et photon "     , xMinEt, xMaxEt, nBinEt );
  m_histHadron    = GaudiHistoAlg::book( "EtHad", "Et hadron "     , xMinEt, xMaxEt, nBinEt );
  m_histPi0Local  = GaudiHistoAlg::book( "EtPil", "Et Pi0 Local "  , xMinEt, xMaxEt, nBinEt );
  m_histPi0Global = GaudiHistoAlg::book( "EtPig", "Et Pi0 Global " , xMinEt, xMaxEt, nBinEt );
  m_histSumEt     = GaudiHistoAlg::book( "SumEt", "Sum Et "        , xMinSumEt, xMaxSumEt, nBinSumEt );
  m_histSpdMult   = GaudiHistoAlg::book( "SpdMult", "Spd Mult "      , xMinSpdMult, xMaxSpdMult , nBinSpdMult );

  if ( m_lookForHotCells ) {
    m_histEleFreqInn  = GaudiHistoAlg::book( "FreqEleInn", 
                                             "Electron Candidate occurence Inner",
                                             41500., 44500., 3000 );
    m_histEleFreqMid  = GaudiHistoAlg::book( "FreqEleMid", 
                                             "Electron Candidate occurence Middle",
                                             37000., 41500., 4500 );
    m_histEleFreqOut  = GaudiHistoAlg::book( "FreqEleOut", 
                                             "Electron Candidate occurence Outer",
                                             32500., 37000., 4500 );

    m_histPhoFreqInn  = GaudiHistoAlg::book( "FreqPhoInn", 
                                             "Photon Candidate occurence Inner",
                                             41500., 44500., 3000 );
    m_histPhoFreqMid  = GaudiHistoAlg::book( "FreqPhoMid", 
                                             "Photon Candidate occurence Middle",
                                             37000., 41500., 4500 );
    m_histPhoFreqOut  = GaudiHistoAlg::book( "FreqPhoOut", 
                                             "Photon Candidate occurence Outer",
                                             32500., 37000., 4500 );

    m_histPilFreqInn  = GaudiHistoAlg::book( "FreqPilInn", 
                                             "Pi0Local Candidate occurence Inner",
                                             41500., 44500., 3000 );
    m_histPilFreqMid  = GaudiHistoAlg::book( "FreqPilMid", 
                                             "Pi0Local Candidate occurence Middle",
                                             37000., 41500., 4500 );
    m_histPilFreqOut  = GaudiHistoAlg::book( "FreqPilOut", 
                                             "Pi0Local Candidate occurence Outer",
                                             32500., 37000., 4500 );

    m_histPigFreqInn  = GaudiHistoAlg::book( "FreqPigInn", 
                                             "Pi0Global Candidate occurence Inner",
                                             41500., 44500., 3000 );
    m_histPigFreqMid  = GaudiHistoAlg::book( "FreqPigMid", 
                                             "Pi0Global Candidate occurence Middle",
                                             37000., 41500., 4500 );
    m_histPigFreqOut  = GaudiHistoAlg::book( "FreqPigOut", 
                                             "Pi0Global Candidate occurence Outer",
                                             32500., 37000., 4500 );
  
    m_histHadFreqInn  = GaudiHistoAlg::book( "FreqHadInn", 
                                             "Hadron Candidate occurence Inner",
                                             53300., 55300. , 2000) ; 
    m_histHadFreqOut  = GaudiHistoAlg::book( "FreqHadOut", 
                                             "Hadron Candidate occurence Outer",
                                             49200, 51000. , 1800) ; 
  }
    
  // Electron
  bookCalo2D("EcalMapEle","Electron Ecal map" ,"Ecal") ; 

  if ( m_fullMonitoring ) {
    m_histElectronCrate8  = GaudiHistoAlg::book  ("EleCrate8"," Electron crate 8", -.5, 16.5,17) ; 
    m_histElectronCrate9  = GaudiHistoAlg::book  ("EleCrate9"," Electron crate 9", -.5, 16.5,17) ; 
    m_histElectronCrate10 = GaudiHistoAlg::book ("EleCrate10"," Electron crate 10", -.5, 16.5,17) ; 
    m_histElectronCrate11 = GaudiHistoAlg::book ("EleCrate11"," Electron crate 11", -.5, 16.5,17) ; 
    m_histElectronCrate12 = GaudiHistoAlg::book ("EleCrate12"," Electron crate 12", -.5, 16.5,17) ; 
    m_histElectronCrate13 = GaudiHistoAlg::book ("EleCrate13"," Electron crate 13", -.5, 16.5,17) ; 
    m_histElectronCrate14 = GaudiHistoAlg::book ("EleCrate14"," Electron crate 14", -.5, 16.5,17) ; 
    m_histElectronCrate15 = GaudiHistoAlg::book ("EleCrate15"," Electron crate 15", -.5, 16.5,17) ; 
    m_histElectronCrate16 = GaudiHistoAlg::book ("EleCrate16"," Electron crate 16", -.5, 16.5,17) ; 
    m_histElectronCrate17 = GaudiHistoAlg::book ("EleCrate17"," Electron crate 17", -.5, 16.5,17) ; 
    m_histElectronCrate18 = GaudiHistoAlg::book ("EleCrate18"," Electron crate 18", -.5, 16.5,17) ; 
    m_histElectronCrate19 = GaudiHistoAlg::book ("EleCrate19"," Electron crate 19", -.5, 16.5,17) ; 
    m_histElectronCrate20 = GaudiHistoAlg::book ("EleCrate20"," Electron crate 20", -.5, 16.5,17) ; 
    m_histElectronCrate21 = GaudiHistoAlg::book ("EleCrate21"," Electron crate 21", -.5, 16.5,17) ; 
  }

  // Photon 
  bookCalo2D("EcalMapPho","Photon Ecal map" ,"Ecal") ; 
  
  if ( m_fullMonitoring ) {
    m_histPhotonCrate8  = GaudiHistoAlg::book  ("PhoCrate8"," Photon crate 8",-.5, 16.5,17) ; 
    m_histPhotonCrate9  = GaudiHistoAlg::book  ("PhoCrate9"," Photon crate 9",-.5, 16.5,17) ; 
    m_histPhotonCrate10 = GaudiHistoAlg::book ("PhoCrate10"," Photon crate 10",-.5, 16.5,17) ; 
    m_histPhotonCrate11 = GaudiHistoAlg::book ("PhoCrate11"," Photon crate 11",-.5, 16.5,17) ; 
    m_histPhotonCrate12 = GaudiHistoAlg::book ("PhoCrate12"," Photon crate 12",-.5, 16.5,17) ; 
    m_histPhotonCrate13 = GaudiHistoAlg::book ("PhoCrate13"," Photon crate 13",-.5, 16.5,17) ; 
    m_histPhotonCrate14 = GaudiHistoAlg::book ("PhoCrate14"," Photon crate 14",-.5, 16.5,17) ; 
    m_histPhotonCrate15 = GaudiHistoAlg::book ("PhoCrate15"," Photon crate 15",-.5, 16.5,17) ; 
    m_histPhotonCrate16 = GaudiHistoAlg::book ("PhoCrate16"," Photon crate 16",-.5, 16.5,17) ; 
    m_histPhotonCrate17 = GaudiHistoAlg::book ("PhoCrate17"," Photon crate 17",-.5, 16.5,17) ; 
    m_histPhotonCrate18 = GaudiHistoAlg::book ("PhoCrate18"," Photon crate 18",-.5, 16.5,17) ; 
    m_histPhotonCrate19 = GaudiHistoAlg::book ("PhoCrate19"," Photon crate 19",-.5, 16.5,17) ; 
    m_histPhotonCrate20 = GaudiHistoAlg::book ("PhoCrate20"," Photon crate 20",-.5, 16.5,17) ; 
    m_histPhotonCrate21 = GaudiHistoAlg::book ("PhoCrate21"," Photon crate 21",-.5, 16.5,17) ; 
  }

  // Pi0Local 
  bookCalo2D("EcalMapPil","Pi0Local Ecal map" ,"Ecal") ; 

  if ( m_fullMonitoring ) {
    m_histPi0LocalCrate8  = GaudiHistoAlg::book  ("PilCrate8"," Pi0Local crate 8",-.5, 16.5,17) ; 
    m_histPi0LocalCrate9  = GaudiHistoAlg::book  ("PilCrate9"," Pi0Local crate 9",-.5, 16.5,17) ; 
    m_histPi0LocalCrate10 = GaudiHistoAlg::book ("PilCrate10"," Pi0Local crate 10",-.5, 16.5,17) ; 
    m_histPi0LocalCrate11 = GaudiHistoAlg::book ("PilCrate11"," Pi0Local crate 11",-.5, 16.5,17) ; 
    m_histPi0LocalCrate12 = GaudiHistoAlg::book ("PilCrate12"," Pi0Local crate 12",-.5, 16.5,17) ; 
    m_histPi0LocalCrate13 = GaudiHistoAlg::book ("PilCrate13"," Pi0Local crate 13",-.5, 16.5,17) ; 
    m_histPi0LocalCrate14 = GaudiHistoAlg::book ("PilCrate14"," Pi0Local crate 14",-.5, 16.5,17) ; 
    m_histPi0LocalCrate15 = GaudiHistoAlg::book ("PilCrate15"," Pi0Local crate 15",-.5, 16.5,17) ; 
    m_histPi0LocalCrate16 = GaudiHistoAlg::book ("PilCrate16"," Pi0Local crate 16",-.5, 16.5,17) ; 
    m_histPi0LocalCrate17 = GaudiHistoAlg::book ("PilCrate17"," Pi0Local crate 17",-.5, 16.5,17) ; 
    m_histPi0LocalCrate18 = GaudiHistoAlg::book ("PilCrate18"," Pi0Local crate 18",-.5, 16.5,17) ; 
    m_histPi0LocalCrate19 = GaudiHistoAlg::book ("PilCrate19"," Pi0Local crate 19",-.5, 16.5,17) ; 
    m_histPi0LocalCrate20 = GaudiHistoAlg::book ("PilCrate20"," Pi0Local crate 20",-.5, 16.5,17) ; 
    m_histPi0LocalCrate21 = GaudiHistoAlg::book ("PilCrate21"," Pi0Local crate 21",-.5, 16.5,17) ; 
  }

  // Pi0Global 
  bookCalo2D("EcalMapPig","Pi0Global Ecal map" ,"Ecal") ; 
    
  if ( m_fullMonitoring ) {
    m_histPi0GlobalCrate8  = GaudiHistoAlg::book  ("PigCrate8"," Pi0Global crate 8",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate9  = GaudiHistoAlg::book  ("PigCrate9"," Pi0Global crate 9",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate10 = GaudiHistoAlg::book ("PigCrate10"," Pi0Global crate 10",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate11 = GaudiHistoAlg::book ("PigCrate11"," Pi0Global crate 11",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate12 = GaudiHistoAlg::book ("PigCrate12"," Pi0Global crate 12",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate13 = GaudiHistoAlg::book ("PigCrate13"," Pi0Global crate 13",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate14 = GaudiHistoAlg::book ("PigCrate14"," Pi0Global crate 14",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate15 = GaudiHistoAlg::book ("PigCrate15"," Pi0Global crate 15",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate16 = GaudiHistoAlg::book ("PigCrate16"," Pi0Global crate 16",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate17 = GaudiHistoAlg::book ("PigCrate17"," Pi0Global crate 17",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate18 = GaudiHistoAlg::book ("PigCrate18"," Pi0Global crate 18",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate19 = GaudiHistoAlg::book ("PigCrate19"," Pi0Global crate 19",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate20 = GaudiHistoAlg::book ("PigCrate20"," Pi0Global crate 20",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate21 = GaudiHistoAlg::book ("PigCrate21"," Pi0Global crate 21",-.5, 16.5,17) ; 
  }

  //Hadron 
  bookCalo2D("HcalMapHad","Hadron Hcal map" ,"Hcal") ; 

  if ( m_fullMonitoring ) {
    m_histHadronCrate22 = GaudiHistoAlg::book ("HadCrate22"," Et hadron crate 22",-.5, 16.5,17) ; 
    m_histHadronCrate23 = GaudiHistoAlg::book ("HadCrate23"," Et hadron crate 23",-.5, 16.5,17) ; 
    m_histHadronCrate24 = GaudiHistoAlg::book ("HadCrate24"," Et hadron crate 24",-.5, 16.5,17) ; 
    m_histHadronCrate25 = GaudiHistoAlg::book ("HadCrate25"," Et hadron crate 25",-.5, 16.5,17) ; 
  }

  if (m_fullMonitoring) { 
    debug() << "==> Full Monitoring histograms GaudiHistoAlg::booking " << endmsg;   

    m_histElectronFull        = GaudiHistoAlg::book( "EtEleFull", "Et electron Full Monitoring"   , xMinEt, xMaxEt, nBinEt );
    m_histPhotonFull          = GaudiHistoAlg::book( "EtPhoFull", "Et photon Full Monitoring"     , xMinEt, xMaxEt, nBinEt );
    m_histHadronFull          = GaudiHistoAlg::book( "EtHadFull", "Et hadron Full Monitoring"     , xMinEt, xMaxEt, nBinEt );
    m_histPi0LocalFull        = GaudiHistoAlg::book( "EtPilFull", "Et Pi0 Local Full Monitoring"  , xMinEt, xMaxEt, nBinEt );
    m_histPi0GlobalFull       = GaudiHistoAlg::book( "EtPigFull", "Et Pi0 Global Full Monitoring" , xMinEt, xMaxEt, nBinEt );
    m_histSumEtFull           = GaudiHistoAlg::book( "SumEtFull", "Sum Et Full Monitoring"        , xMinSumEt, xMaxSumEt, nBinSumEt );
    m_histSpdMultFull         = GaudiHistoAlg::book( "SpdMultFull", "Spd Mult Full Monitoring"      , xMinSpdMult, xMaxSpdMult , nBinSpdMult );
                                                     
    m_histHadronSlave1OutFull = GaudiHistoAlg::book("EtHadSlave1OutFull","Et Hadron Slave1 Out Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histHadronSlave2OutFull = GaudiHistoAlg::book("EtHadSlave2OutFull","Et Hadron Slave2 Out Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histHadronSlave1InFull  = GaudiHistoAlg::book("EtHadSlave1InFull","Et Hadron Slave1 In Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histHadronSlave2InFull  = GaudiHistoAlg::book("EtHadSlave2InFull","Et Hadron Slave2 In Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histSumEtSlave1OutFull  = GaudiHistoAlg::book("SumEtHadSlave1OutFull","Sum Et Hadron Slave1 Out Full Monitoring" , 
                                                    xMinSumEt,xMaxSumEt,nBinSumEt) ; 
    m_histSumEtSlave2OutFull  = GaudiHistoAlg::book("SumEtHadSlave2OutFull","Sum Et Hadron Slave2 Out Full Monitoring" , 
                                                    xMinSumEt,xMaxSumEt,nBinSumEt) ; 
    m_histSumEtSlave1InFull   = GaudiHistoAlg::book("SumEtHadSlave1InFull","Sum Et Hadron Slave1 In Full Monitoring" , 
						    xMinSumEt,xMaxSumEt,nBinSumEt) ; 
    m_histSumEtSlave2InFull   = GaudiHistoAlg::book("SumEtHadSlave2InFull","Sum Et Hadron Slave2 In Full Monitoring" , 
						    xMinSumEt,xMaxSumEt,nBinSumEt) ; 
  }
 
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloMonit::execute() {

  debug() << "Execute will read " << LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix << endreq;

  if ( ! exist< LHCb::L0CaloCandidates >( LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix ) ) {
    debug() << "No data at " <<  LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix << endreq ;
    return StatusCode::SUCCESS ;
  }

  LHCb::L0CaloCandidates* candidates = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix );
  LHCb::L0CaloCandidates::const_iterator cand;
  int nPhotonL0Cand = 0 ; 
  int nHadronL0Cand = 0 ; 
  m_nEvents++ ; 

  double event = -999 ; 
  double BCId  = -999 ; 

  if(exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin=get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    event = odin->eventNumber() ; 
    BCId = odin->bunchId() ; 
    m_nUsefulEvents++ ; 
  } 


  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    debug() << "Event "<<event<<" Type  = " << (*cand)->type() << " Et  = " << (*cand)->etCode() << endmsg ; 
    LHCb::CaloCellID caloCell = (*cand)->id() ; 
    //    debug() << "Candidat x  = " << (*cand)->position().x() << " y=" << (*cand)->position().y() << endmsg ;
    //    debug() << "CellID  = "     << (*cand)->id() << " CellID.all = "<<(*cand)->id().all()<<endmsg ;
    if ( L0DUBase::CaloType::Photon == (*cand)->type()  ) nPhotonL0Cand++ ; 
    if ( L0DUBase::CaloType::Hadron == (*cand)->type()  ) nHadronL0Cand++ ; 
    if ( L0DUBase::CaloType::Electron == (*cand)->type()  ) {
      m_histElectron  -> fill( (*cand)->etCode(), 1. );
      int card  = m_ecal->cardNumber( caloCell) ; 
      if( ! m_ecal->isPinCard(card) ) { 
        if ( m_lookForHotCells ) {
          if (caloCell.area() == 2 ) m_histEleFreqInn  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 1 ) m_histEleFreqMid  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 0 ) m_histEleFreqOut  -> fill(caloCell.all(),1.) ; 
        }
      } 
      if ( m_fullMonitoring ) {
        int crate = m_ecal->cardCrate(card); 
        int cardSlot = m_ecal->cardSlot(card) ; 
        
        if( m_ecal->isPinCard(card) ) 
          info() << " !!! Pin Diode ... " << caloCell.area() << " ID=" << caloCell << " crate = "
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        if ( ( crate < 8 ) || ( crate > 21 ) ) 
          info() << " !!! Bad crate number area = " << caloCell.area() << " ID= " << caloCell << " crate = " 
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        if (crate == 8)  m_histElectronCrate8  -> fill(cardSlot,1.) ; 
        if (crate == 9)  m_histElectronCrate9  -> fill(cardSlot,1.) ; 
        if (crate == 10) m_histElectronCrate10 -> fill(cardSlot,1.) ; 
        if (crate == 11) m_histElectronCrate11 -> fill(cardSlot,1.) ; 
        if (crate == 12) m_histElectronCrate12 -> fill(cardSlot,1.) ; 
        if (crate == 13) m_histElectronCrate13 -> fill(cardSlot,1.) ; 
        if (crate == 14) m_histElectronCrate14 -> fill(cardSlot,1.) ; 
        if (crate == 15) m_histElectronCrate15 -> fill(cardSlot,1.) ; 
        if (crate == 16) m_histElectronCrate16 -> fill(cardSlot,1.) ; 
        if (crate == 17) m_histElectronCrate17 -> fill(cardSlot,1.) ; 
        if (crate == 18) m_histElectronCrate18 -> fill(cardSlot,1.) ; 
        if (crate == 19) m_histElectronCrate19 -> fill(cardSlot,1.) ; 
        if (crate == 20) m_histElectronCrate20 -> fill(cardSlot,1.) ; 
        if (crate == 21) m_histElectronCrate21 -> fill(cardSlot,1.) ; 
      }

      fillCalo2D("EcalMapEle",caloCell,1.,"Electron Ecal map") ; 

    } else if ( L0DUBase::CaloType::Photon == (*cand)->type()  ) {

      m_histPhoton -> fill( (*cand)->etCode() , 1. ) ;
      int card  = m_ecal->cardNumber( caloCell) ; 
      if( ! m_ecal->isPinCard(card) ) { 
        if ( m_lookForHotCells ) {
          if (caloCell.area() == 2 ) m_histPhoFreqInn  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 1 ) m_histPhoFreqMid  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 0 ) m_histPhoFreqOut  -> fill(caloCell.all(),1.) ; 
        }
      } 
      if ( m_fullMonitoring ) {
        int card  = m_ecal->cardNumber( caloCell) ; 
        int crate = m_ecal->cardCrate(card); 
        int cardSlot = m_ecal->cardSlot(card) ; 
        if ( ( crate < 8 ) || ( crate > 21 ) )  
          info() << " !!! area = " << caloCell.area() << " ID=" << caloCell << " crate = " << crate
                 << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        if( m_ecal->isPinCard(card) ) 
          info() << " !!! Pin Diode ... " << caloCell.area() << " ID=" << caloCell << " crate = " << crate
                 << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        
        debug() << " Photon candidate area = " << caloCell.area() << " ID= " << caloCell << " crate = " 
                << crate << " card= " << card << endmsg ; 
        
        if (crate == 8)  m_histPhotonCrate8  -> fill(cardSlot,1.) ; 
        if (crate == 9)  m_histPhotonCrate9  -> fill(cardSlot,1.) ; 
        if (crate == 10) m_histPhotonCrate10 -> fill(cardSlot,1.) ; 
        if (crate == 11) m_histPhotonCrate11 -> fill(cardSlot,1.) ; 
        if (crate == 12) m_histPhotonCrate12 -> fill(cardSlot,1.) ; 
        if (crate == 13) m_histPhotonCrate13 -> fill(cardSlot,1.) ; 
        if (crate == 14) m_histPhotonCrate14 -> fill(cardSlot,1.) ; 
        if (crate == 15) m_histPhotonCrate15 -> fill(cardSlot,1.) ; 
        if (crate == 16) m_histPhotonCrate16 -> fill(cardSlot,1.) ; 
        if (crate == 17) m_histPhotonCrate17 -> fill(cardSlot,1.) ; 
        if (crate == 18) m_histPhotonCrate18 -> fill(cardSlot,1.) ; 
        if (crate == 19) m_histPhotonCrate19 -> fill(cardSlot,1.) ; 
        if (crate == 20) m_histPhotonCrate20 -> fill(cardSlot,1.) ; 
        if (crate == 21) m_histPhotonCrate21 -> fill(cardSlot,1.) ;
      }
      
      fillCalo2D("EcalMapPho" , caloCell , 1. , "Photon Ecal map" ) ; 
       
    } else if ( L0DUBase::CaloType::Hadron == (*cand)->type()  ) {
      
      m_histHadron -> fill( (*cand)->etCode(), 1. );
      int card  = m_hcal->cardNumber( caloCell) ;
      if( ! m_hcal->isPinCard(card) ) {
        if ( m_lookForHotCells ) {
          if (caloCell.area() == 1 ) m_histHadFreqInn  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 0 ) m_histHadFreqOut  -> fill(caloCell.all(),1.) ; 
        }
      }
      if ( m_fullMonitoring ) {
        
        int crate = m_hcal->cardCrate(card);
        int cardSlot = m_hcal->cardSlot(card) ;       
        if( m_hcal->isPinCard(card) ) 
          info() << " !!! Pin Diode ... " << caloCell.area() << " ID=" << caloCell << " crate = "
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ;  
        if ( ( crate < 22 ) || ( crate > 25 ) ) 
          info() << " !!! area = " << caloCell.area() << " ID=" << caloCell << " crate = " << crate 
                 << " card= " << card << " cardSlot= " << cardSlot << endmsg ; 
        debug() << " area = " << caloCell.area() << " ID=" << caloCell << " crate = " << crate << " card= " << card << endmsg ; 
        if ( m_lookForHotCells ) {
          if (crate == 22) m_histHadronCrate22 -> fill(cardSlot,1.) ; 
          if (crate == 23) m_histHadronCrate23 -> fill(cardSlot,1.) ; 
          if (crate == 24) m_histHadronCrate24 -> fill(cardSlot,1.) ; 
          if (crate == 25) m_histHadronCrate25 -> fill(cardSlot,1.) ; 
        }
      }
      
      fillCalo2D("HcalMapHad",caloCell,1.,"Hadron Hcal map") ; 

    } else if ( L0DUBase::CaloType::Pi0Local == (*cand)->type()  ) {
    
      m_histPi0Local  -> fill( (*cand)->etCode(), 1. );
      int card  = m_ecal->cardNumber( caloCell) ; 
      if( ! m_ecal->isPinCard(card) ) { 
        if ( m_lookForHotCells ) {
          if (caloCell.area() == 2 ) m_histPilFreqInn  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 1 ) m_histPilFreqMid  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 0 ) m_histPilFreqOut  -> fill(caloCell.all(),1.) ; 
        }
      } 
      if ( m_fullMonitoring ) {
        int card  = m_ecal->cardNumber( caloCell) ; 
        int crate = m_ecal->cardCrate(card); 
        int cardSlot = m_ecal->cardSlot(card) ; 
        if( m_ecal->isPinCard(card) ) 
          info() << " !!! Pin Diode ... " << caloCell.area() << " ID=" << caloCell << " crate = " 
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        if ( ( crate < 8 ) || ( crate > 21 ) ) 
          info() << " !!! area = " << caloCell.area() << " ID=" << caloCell << " crate = " 
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        
        if (crate == 8)  m_histPi0LocalCrate8  -> fill(cardSlot,1.) ; 
        if (crate == 9)  m_histPi0LocalCrate9  -> fill(cardSlot,1.) ; 
        if (crate == 10) m_histPi0LocalCrate10 -> fill(cardSlot,1.) ; 
        if (crate == 11) m_histPi0LocalCrate11 -> fill(cardSlot,1.) ; 
        if (crate == 12) m_histPi0LocalCrate12 -> fill(cardSlot,1.) ; 
        if (crate == 13) m_histPi0LocalCrate13 -> fill(cardSlot,1.) ; 
        if (crate == 14) m_histPi0LocalCrate14 -> fill(cardSlot,1.) ; 
        if (crate == 15) m_histPi0LocalCrate15 -> fill(cardSlot,1.) ; 
        if (crate == 16) m_histPi0LocalCrate16 -> fill(cardSlot,1.) ; 
        if (crate == 17) m_histPi0LocalCrate17 -> fill(cardSlot,1.) ; 
        if (crate == 18) m_histPi0LocalCrate18 -> fill(cardSlot,1.) ; 
        if (crate == 19) m_histPi0LocalCrate19 -> fill(cardSlot,1.) ; 
        if (crate == 20) m_histPi0LocalCrate20 -> fill(cardSlot,1.) ; 
        if (crate == 21) m_histPi0LocalCrate21 -> fill(cardSlot,1.) ; 
      }
      
      fillCalo2D("EcalMapPil",caloCell,1.,"Pi0Local Ecal map") ;  

    } else if ( L0DUBase::CaloType::Pi0Global == (*cand)->type()  ) {
    
      m_histPi0Global -> fill( (*cand)->etCode(), 1. );
      int card  = m_ecal->cardNumber( caloCell) ; 
      if( ! m_ecal->isPinCard(card) ) { 
        if ( m_lookForHotCells ) {
          if (caloCell.area() == 2 ) m_histPigFreqInn  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 1 ) m_histPigFreqMid  -> fill(caloCell.all(),1.) ; 
          if (caloCell.area() == 0 ) m_histPigFreqOut  -> fill(caloCell.all(),1.) ; 
        }
      } 
      if ( m_fullMonitoring ) {
        int card  = m_ecal->cardNumber( caloCell) ; 
        int crate = m_ecal->cardCrate(card); 
        int cardSlot = m_ecal->cardSlot(card) ; 
        if( m_ecal->isPinCard(card) ) 
          info() << " !!! Pin Diode ... " << caloCell.area() << " ID=" << caloCell << " crate = " 
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        if ( ( crate < 8 ) || ( crate > 21 ) ) 
          info() << " !!! area = " << caloCell.area() << " ID=" << caloCell << " crate = " 
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ; 
        
        if (crate == 8)  m_histPi0GlobalCrate8  -> fill(cardSlot,1.) ; 
        if (crate == 9)  m_histPi0GlobalCrate9  -> fill(cardSlot,1.) ; 
        if (crate == 10) m_histPi0GlobalCrate10 -> fill(cardSlot,1.) ; 
        if (crate == 11) m_histPi0GlobalCrate11 -> fill(cardSlot,1.) ; 
        if (crate == 12) m_histPi0GlobalCrate12 -> fill(cardSlot,1.) ; 
        if (crate == 13) m_histPi0GlobalCrate13 -> fill(cardSlot,1.) ; 
        if (crate == 14) m_histPi0GlobalCrate14 -> fill(cardSlot,1.) ; 
        if (crate == 15) m_histPi0GlobalCrate15 -> fill(cardSlot,1.) ; 
        if (crate == 16) m_histPi0GlobalCrate16 -> fill(cardSlot,1.) ; 
        if (crate == 17) m_histPi0GlobalCrate17 -> fill(cardSlot,1.) ; 
        if (crate == 18) m_histPi0GlobalCrate18 -> fill(cardSlot,1.) ; 
        if (crate == 19) m_histPi0GlobalCrate19 -> fill(cardSlot,1.) ; 
        if (crate == 20) m_histPi0GlobalCrate20 -> fill(cardSlot,1.) ; 
        if (crate == 21) m_histPi0GlobalCrate21 -> fill(cardSlot,1.) ; 
      }
      
      fillCalo2D("EcalMapPig",caloCell,1.,"Pi0Global Ecal map") ;  

    } else if ( L0DUBase::CaloType::SumEt == (*cand)->type()  ) {
    
      m_histSumEt     -> fill( (*cand)->etCode(), 1. );
    
    } else if ( L0DUBase::CaloType::SpdMult == (*cand)->type()  ) {
      
      m_histSpdMult   -> fill( (*cand)->etCode(), 1. );
    
    }
  }
    


  if (m_fullMonitoring) {
    if ( ! exist< LHCb::L0CaloCandidates >( LHCb::L0CaloCandidateLocation::Full+m_inputDataSuffix ) ) {
      debug() << "No data at " << LHCb::L0CaloCandidateLocation::Full+m_inputDataSuffix << endreq ;
    } else {
      LHCb::L0CaloCandidates* candidatesF = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full+m_inputDataSuffix );
      LHCb::L0CaloCandidates::const_iterator candF;
      
      for ( candF = candidatesF->begin() ; candidatesF->end() != candF ; ++candF ) {
        if ( L0DUBase::CaloType::Electron == (*candF)->type()  ) {
          m_histElectronFull  -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::Photon == (*candF)->type()  ) {
          m_histPhotonFull    -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::Hadron == (*candF)->type()  ) {
          m_histHadronFull    -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::Pi0Local == (*candF)->type()  ) {
          m_histPi0LocalFull  -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::Pi0Global == (*candF)->type()  ) {
          m_histPi0GlobalFull-> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::SumEt == (*candF)->type()  ) {
          m_histSumEtFull     -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::HadronSlave1Out == (*candF)->type()  ) {
          m_histHadronSlave1OutFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::HadronSlave2Out == (*candF)->type()  ) {
          m_histHadronSlave2OutFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::HadronSlave1In == (*candF)->type()  ) {
          m_histHadronSlave1InFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::HadronSlave2In == (*candF)->type()  ) {
          m_histHadronSlave2InFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::SumEtSlave1Out == (*candF)->type()  ) {
          m_histSumEtSlave1OutFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::SumEtSlave2Out == (*candF)->type()  ) {
          m_histSumEtSlave2OutFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::SumEtSlave1In == (*candF)->type()  ) {
          m_histSumEtSlave1InFull   -> fill( (*candF)->etCode(), 1. );
        } else if ( L0DUBase::CaloType::SumEtSlave2In == (*candF)->type()  ) {
          m_histSumEtSlave2InFull   -> fill( (*candF)->etCode(), 1. );
        }
      }
    }
  }

  if ( ( m_updateFrequency > 0 ) && ( m_lookForHotCells ) ) { 
    int goForCheck = m_nEvents%m_updateFrequency ; 
    if (goForCheck == 0 ) { 
      info()<<"m_nEvents == "<<m_nEvents<<" go for check ... "<<endreq ; 
      if (m_lookForHotCells ) { 
        int caloType = 1 ; 
        info()<<" =========================Hot cells ========================="<<endreq ; 
        info()<<" ============================================================"<<endreq ; 
        info()<<" ===        Electron Candidates Inner region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histEleFreqInn , caloType ) ; 
        info()<<" ===        Electron Candidates Middle region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histEleFreqMid , caloType ) ; 
        info()<<" ===        Electron Candidates Outer region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histEleFreqOut , caloType ) ; 
        info()<<" ============================================================"<<endreq ; 
        info()<<" ===        Photon Candidates Inner region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPhoFreqInn , caloType ) ; 
        info()<<" ===        Photon Candidates Middle region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPhoFreqMid , caloType ) ; 
        info()<<" ===        Photon Candidates Outer region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPhoFreqOut , caloType ) ; 
        info()<<" ============================================================"<<endreq ; 
        info()<<" ===        Pi0Local Candidates Inner region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPilFreqInn , caloType ) ; 
        info()<<" ===        Pi0Local Candidates Middle region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPilFreqMid , caloType ) ; 
        info()<<" ===        Pi0Local Candidates Outer region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPilFreqOut , caloType ) ; 
        info()<<" ============================================================"<<endreq ; 
        info()<<" ===        Pi0Global Candidates Inner region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPigFreqInn , caloType ) ; 
        info()<<" ===        Pi0Global Candidates Middle region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPigFreqMid , caloType ) ; 
        info()<<" ===        Pi0Global Candidates Outer region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histPigFreqOut , caloType ) ; 
        caloType = 2 ; 
        info()<<" ============================================================"<<endreq ; 
        info()<<" ===        Hadron Candidates Inner region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histHadFreqInn , caloType ) ; 
        info()<<" ===        Hadron Candidates Outer region              ===" <<endreq ; 
        SearchForHotCellsAndReset(m_histHadFreqOut , caloType ) ; 
        info()<<" ============================================================"<<endreq ; 
      }
    }
  }

  //  return sc ; 
  return StatusCode::SUCCESS; 
}

//============================================================================
void L0CaloMonit::SearchForHotCellsAndReset(IHistogram1D* hist , int caloType ) { 
  float nIn = hist->entries() ;
  const IAxis& xAxis = hist->axis() ; 
  int nBin = xAxis.bins() ; 
  float data[nBin] ; 
  float nUsedCells = 0 ; 
  for ( int i = 0 ; i<nBin ; i++) {
    data[i] = hist->binEntries(i) ; 
    if (data[i] !=0) nUsedCells++ ; 
  } 
  bool hotChannels = false ; 
  float meanOcc = nIn/nUsedCells ; 
  for ( int i = 0 ; i<nBin ; i++) { 
    data[i] = hist->binEntries(i) ; 
    if (data[i] > meanOcc*m_alarmThresholdRatio ) { 
      hotChannels = true ; 
      break ; 
    }
  }

  if (hotChannels) { 
    info()<<"Number of hit cells : "<<nUsedCells<<" mean occupancy : "<<meanOcc<<" alarm threshold : "<<m_alarmThresholdRatio*meanOcc<<endreq ; 
    info()<<"-------------------------------------------------------------------------"<<endreq ; 
    info()<<"|   Data     |    Hot(?) Cell    |    Crate    | Slot     |   Channel   |"<<endreq ; 
    info()<<"-------------------------------------------------------------------------"<<endreq ; 
    for ( int i = 0 ; i<nBin ; i++) { 
      data[i] = hist->binEntries(i) ; 
      if (data[i] > meanOcc*m_alarmThresholdRatio ) { 
	double idAll = xAxis.binLowerEdge(i) ; 
	LHCb::CaloCellID caloCell ((int)idAll)  ; 
	int card = -999 ; 
	int crate = -999 ; 
	int cardSlot = -999 ; 
	int channelNum = 0 ; 
	int cellChannel = 0 ; 
	
	if (caloType == 1 ) { 
	  card  = m_ecal->cardNumber( caloCell) ; 
	  crate = m_ecal->cardCrate(card); 
	  cardSlot = m_ecal->cardSlot(card) ; 
	  std::vector<LHCb::CaloCellID>&  myCardChannels = m_ecal->cardChannels( card ) ;
	  std::vector<LHCb::CaloCellID>::iterator it ;
	  for ( it = myCardChannels.begin() ; it != myCardChannels.end() ;  it++ ) {
	    if (*it == caloCell) cellChannel = channelNum ; 
	    channelNum++ ; 
	  } 
	}
	if (caloType == 2 ) { 
	  card  = m_hcal->cardNumber( caloCell) ; 
	  crate = m_hcal->cardCrate(card); 
	  cardSlot = m_hcal->cardSlot(card) ; 
	  std::vector<LHCb::CaloCellID>&  myCardChannels = m_hcal->cardChannels( card ) ;
	  std::vector<LHCb::CaloCellID>::iterator it ;
	  for ( it = myCardChannels.begin() ; it != myCardChannels.end() ;  it++ ) {
	    if (*it == caloCell) cellChannel = channelNum ; 
	    channelNum++ ; 
	  }
	}
	
	info()<<"|     "<<data[i]<<"     |   "<<caloCell<<"    |     "<<crate<<"     |     " <<cardSlot<<"      |  "<<"   |   "<<cellChannel << "  |  "<<endreq ; 
	info()<<"-------------------------------------------------------------------------"<<endreq ; 
      } 
    } 
  } else {
    info()<<" ===> No hot channels for this region "<<endreq ;
  }
  hist->reset() ; 
} 
