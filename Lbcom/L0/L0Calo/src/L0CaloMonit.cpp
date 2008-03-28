// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloMonit.cpp,v 1.17 2008-03-28 17:09:54 robbep Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "AIDA/IHistogram1D.h"
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

  
  m_histElectron  = GaudiHistoAlg::book( 1001, "Et electron "   , xMinEt, xMaxEt, nBinEt );
  m_histPhoton    = GaudiHistoAlg::book( 1002, "Et photon "     , xMinEt, xMaxEt, nBinEt );
  m_histHadron    = GaudiHistoAlg::book( 1003, "Et hadron "     , xMinEt, xMaxEt, nBinEt );
  m_histPi0Local  = GaudiHistoAlg::book( 1004, "Et Pi0 Local "  , xMinEt, xMaxEt, nBinEt );
  m_histPi0Global = GaudiHistoAlg::book( 1005, "Et Pi0 Global " , xMinEt, xMaxEt, nBinEt );
  m_histSumEt     = GaudiHistoAlg::book( 1006, "Sum Et "        , xMinSumEt, xMaxSumEt, nBinSumEt );
  m_histSpdMult   = GaudiHistoAlg::book( 1007, "Spd Mult "      , xMinSpdMult, xMaxSpdMult , nBinSpdMult );

  // Electron
  bookCalo2D(10001,"Electron Ecal map" ,"Ecal") ; 

  if ( m_fullMonitoring ) {
    m_histElectronCrate8  = GaudiHistoAlg::book  (801001," Electron crate 8", -.5, 16.5,17) ; 
    m_histElectronCrate9  = GaudiHistoAlg::book  (901001," Electron crate 9", -.5, 16.5,17) ; 
    m_histElectronCrate10 = GaudiHistoAlg::book (101001," Electron crate 10", -.5, 16.5,17) ; 
    m_histElectronCrate11 = GaudiHistoAlg::book (111001," Electron crate 11", -.5, 16.5,17) ; 
    m_histElectronCrate12 = GaudiHistoAlg::book (121001," Electron crate 12", -.5, 16.5,17) ; 
    m_histElectronCrate13 = GaudiHistoAlg::book (131001," Electron crate 13", -.5, 16.5,17) ; 
    m_histElectronCrate14 = GaudiHistoAlg::book (141001," Electron crate 14", -.5, 16.5,17) ; 
    m_histElectronCrate15 = GaudiHistoAlg::book (151001," Electron crate 15", -.5, 16.5,17) ; 
    m_histElectronCrate16 = GaudiHistoAlg::book (161001," Electron crate 16", -.5, 16.5,17) ; 
    m_histElectronCrate17 = GaudiHistoAlg::book (171001," Electron crate 17", -.5, 16.5,17) ; 
    m_histElectronCrate18 = GaudiHistoAlg::book (181001," Electron crate 18", -.5, 16.5,17) ; 
    m_histElectronCrate19 = GaudiHistoAlg::book (191001," Electron crate 19", -.5, 16.5,17) ; 
    m_histElectronCrate20 = GaudiHistoAlg::book (201001," Electron crate 20", -.5, 16.5,17) ; 
    m_histElectronCrate21 = GaudiHistoAlg::book (211001," Electron crate 21", -.5, 16.5,17) ; 
  }

  // Photon 
  bookCalo2D(10002,"Photon Ecal map" ,"Ecal") ; 
  
  if ( m_fullMonitoring ) {
    m_histPhotonCrate8  = GaudiHistoAlg::book  (801002," Photon crate 8",-.5, 16.5,17) ; 
    m_histPhotonCrate9  = GaudiHistoAlg::book  (901002," Photon crate 9",-.5, 16.5,17) ; 
    m_histPhotonCrate10 = GaudiHistoAlg::book (101002," Photon crate 10",-.5, 16.5,17) ; 
    m_histPhotonCrate11 = GaudiHistoAlg::book (111002," Photon crate 11",-.5, 16.5,17) ; 
    m_histPhotonCrate12 = GaudiHistoAlg::book (121002," Photon crate 12",-.5, 16.5,17) ; 
    m_histPhotonCrate13 = GaudiHistoAlg::book (131002," Photon crate 13",-.5, 16.5,17) ; 
    m_histPhotonCrate14 = GaudiHistoAlg::book (141002," Photon crate 14",-.5, 16.5,17) ; 
    m_histPhotonCrate15 = GaudiHistoAlg::book (151002," Photon crate 15",-.5, 16.5,17) ; 
    m_histPhotonCrate16 = GaudiHistoAlg::book (161002," Photon crate 16",-.5, 16.5,17) ; 
    m_histPhotonCrate17 = GaudiHistoAlg::book (171002," Photon crate 17",-.5, 16.5,17) ; 
    m_histPhotonCrate18 = GaudiHistoAlg::book (181002," Photon crate 18",-.5, 16.5,17) ; 
    m_histPhotonCrate19 = GaudiHistoAlg::book (191002," Photon crate 19",-.5, 16.5,17) ; 
    m_histPhotonCrate20 = GaudiHistoAlg::book (201002," Photon crate 20",-.5, 16.5,17) ; 
    m_histPhotonCrate21 = GaudiHistoAlg::book (211002," Photon crate 21",-.5, 16.5,17) ; 
  }

  // Pi0Local 
  bookCalo2D(10004,"Pi0Local Ecal map" ,"Ecal") ; 

  if ( m_fullMonitoring ) {
    m_histPi0LocalCrate8  = GaudiHistoAlg::book  (801004," Pi0Local crate 8",-.5, 16.5,17) ; 
    m_histPi0LocalCrate9  = GaudiHistoAlg::book  (901004," Pi0Local crate 9",-.5, 16.5,17) ; 
    m_histPi0LocalCrate10 = GaudiHistoAlg::book (101004," Pi0Local crate 10",-.5, 16.5,17) ; 
    m_histPi0LocalCrate11 = GaudiHistoAlg::book (111004," Pi0Local crate 11",-.5, 16.5,17) ; 
    m_histPi0LocalCrate12 = GaudiHistoAlg::book (121004," Pi0Local crate 12",-.5, 16.5,17) ; 
    m_histPi0LocalCrate13 = GaudiHistoAlg::book (131004," Pi0Local crate 13",-.5, 16.5,17) ; 
    m_histPi0LocalCrate14 = GaudiHistoAlg::book (141004," Pi0Local crate 14",-.5, 16.5,17) ; 
    m_histPi0LocalCrate15 = GaudiHistoAlg::book (151004," Pi0Local crate 15",-.5, 16.5,17) ; 
    m_histPi0LocalCrate16 = GaudiHistoAlg::book (161004," Pi0Local crate 16",-.5, 16.5,17) ; 
    m_histPi0LocalCrate17 = GaudiHistoAlg::book (171004," Pi0Local crate 17",-.5, 16.5,17) ; 
    m_histPi0LocalCrate18 = GaudiHistoAlg::book (181004," Pi0Local crate 18",-.5, 16.5,17) ; 
    m_histPi0LocalCrate19 = GaudiHistoAlg::book (191004," Pi0Local crate 19",-.5, 16.5,17) ; 
    m_histPi0LocalCrate20 = GaudiHistoAlg::book (201004," Pi0Local crate 20",-.5, 16.5,17) ; 
    m_histPi0LocalCrate21 = GaudiHistoAlg::book (211004," Pi0Local crate 21",-.5, 16.5,17) ; 
  }

  // Pi0Global 
  bookCalo2D(10005,"Pi0Global Ecal map" ,"Ecal") ; 
    
  if ( m_fullMonitoring ) {
    m_histPi0GlobalCrate8  = GaudiHistoAlg::book  (801005," Pi0Global crate 8",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate9  = GaudiHistoAlg::book  (901005," Pi0Global crate 9",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate10 = GaudiHistoAlg::book (101005," Pi0Global crate 10",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate11 = GaudiHistoAlg::book (111005," Pi0Global crate 11",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate12 = GaudiHistoAlg::book (121005," Pi0Global crate 12",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate13 = GaudiHistoAlg::book (131005," Pi0Global crate 13",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate14 = GaudiHistoAlg::book (141005," Pi0Global crate 14",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate15 = GaudiHistoAlg::book (151005," Pi0Global crate 15",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate16 = GaudiHistoAlg::book (161005," Pi0Global crate 16",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate17 = GaudiHistoAlg::book (171005," Pi0Global crate 17",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate18 = GaudiHistoAlg::book (181005," Pi0Global crate 18",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate19 = GaudiHistoAlg::book (191005," Pi0Global crate 19",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate20 = GaudiHistoAlg::book (201005," Pi0Global crate 20",-.5, 16.5,17) ; 
    m_histPi0GlobalCrate21 = GaudiHistoAlg::book (211005," Pi0Global crate 21",-.5, 16.5,17) ; 
  }

  //Hadron 
  bookCalo2D(10003,"Hadron Hcal map" ,"Hcal") ; 

  if ( m_fullMonitoring ) {
    m_histHadronCrate22 = GaudiHistoAlg::book (221003," Et hadron crate 22",-.5, 16.5,17) ; 
    m_histHadronCrate23 = GaudiHistoAlg::book (231003," Et hadron crate 23",-.5, 16.5,17) ; 
    m_histHadronCrate24 = GaudiHistoAlg::book (241003," Et hadron crate 24",-.5, 16.5,17) ; 
    m_histHadronCrate25 = GaudiHistoAlg::book (251003," Et hadron crate 25",-.5, 16.5,17) ; 
  }

  if (m_fullMonitoring) { 
    debug() << "==> Full Monitoring histograms GaudiHistoAlg::booking " << endmsg;   
    m_histElectronFull        = GaudiHistoAlg::book( 2001, "Et electron Full Monitoring"   , xMinEt, xMaxEt, nBinEt );
    m_histPhotonFull          = GaudiHistoAlg::book( 2002, "Et photon Full Monitoring"     , xMinEt, xMaxEt, nBinEt );
    m_histHadronFull          = GaudiHistoAlg::book( 2003, "Et hadron Full Monitoring"     , xMinEt, xMaxEt, nBinEt );
    m_histPi0LocalFull        = GaudiHistoAlg::book( 2004, "Et Pi0 Local Full Monitoring"  , xMinEt, xMaxEt, nBinEt );
    m_histPi0GlobalFull       = GaudiHistoAlg::book( 2005, "Et Pi0 Global Full Monitoring" , xMinEt, xMaxEt, nBinEt );
    m_histSumEtFull           = GaudiHistoAlg::book( 2006, "Sum Et Full Monitoring"        , xMinSumEt, xMaxSumEt, nBinSumEt );
    m_histSpdMultFull         = GaudiHistoAlg::book( 2007, "Spd Mult Full Monitoring"      , 
                                                     xMinSpdMult, xMaxSpdMult , nBinSpdMult );
    m_histHadronSlave1OutFull = GaudiHistoAlg::book(2008,"Et Hadron Slave1 Out Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histHadronSlave2OutFull = GaudiHistoAlg::book(2009,"Et Hadron Slave2 Out Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histHadronSlave1InFull  = GaudiHistoAlg::book(2010,"Et Hadron Slave1 In Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histHadronSlave2InFull  = GaudiHistoAlg::book(2011,"Et Hadron Slave2 In Full Monitoring" , xMinEt,xMaxEt,nBinEt) ; 
    m_histSumEtSlave1OutFull  = GaudiHistoAlg::book(2012,"Sum Et Hadron Slave1 Out Full Monitoring" , 
                                                    xMinSumEt,xMaxSumEt,nBinSumEt) ; 
    m_histSumEtSlave2OutFull  = GaudiHistoAlg::book(2013,"Sum Et Hadron Slave2 Out Full Monitoring" , 
                                                    xMinSumEt,xMaxSumEt,nBinSumEt) ; 
    m_histSumEtSlave1InFull   = GaudiHistoAlg::book(2014,"Sum Et Hadron Slave1 In Full Monitoring" , xMinSumEt,xMaxSumEt,nBinSumEt) ; 
    m_histSumEtSlave2InFull   = GaudiHistoAlg::book(2015,"Sum Et Hadron Slave2 In Full Monitoring" , xMinSumEt,xMaxSumEt,nBinSumEt) ; 
  }
 
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloMonit::execute() {

  debug() << "Execute will read " << LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix << endreq;

  LHCb::L0CaloCandidates* candidates = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Default + m_inputDataSuffix );
  LHCb::L0CaloCandidates::const_iterator cand;
  int nPhotonL0Cand = 0 ; 
  int nHadronL0Cand = 0 ; 
  m_nEvents++ ; 

  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    debug() << "Type  = " << (*cand)->type() << " Et  = " << (*cand)->etCode() << endmsg ; 
    LHCb::CaloCellID caloCell = (*cand)->id() ; 
    debug() << "Candidat x  = " << (*cand)->position().x() << " y=" << (*cand)->position().y() << endmsg ;
    debug() << "CellID  = "     << (*cand)->id() << endmsg ;
    if ( L0DUBase::CaloType::Photon == (*cand)->type()  ) nPhotonL0Cand++ ; 
    if ( L0DUBase::CaloType::Hadron == (*cand)->type()  ) nHadronL0Cand++ ; 
    if ( L0DUBase::CaloType::Electron == (*cand)->type()  ) {
      m_histElectron  -> fill( (*cand)->etCode(), 1. );
      if ( m_fullMonitoring ) {
        int card  = m_ecal->cardNumber( caloCell) ; 
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

      fillCalo2D(10001,caloCell,1.,"Electron Ecal map") ; 

    } else if ( L0DUBase::CaloType::Photon == (*cand)->type()  ) {

      m_histPhoton -> fill( (*cand)->etCode() , 1. ) ;
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
      
      fillCalo2D( 10002 , caloCell , 1. , "Electron Ecal map" ) ; 
       
    } else if ( L0DUBase::CaloType::Hadron == (*cand)->type()  ) {
      
      m_histHadron -> fill( (*cand)->etCode(), 1. );
      if ( m_fullMonitoring ) {
        int card  = m_hcal->cardNumber( caloCell) ; 
        int crate = m_hcal->cardCrate(card);
        int cardSlot = m_hcal->cardSlot(card) ;       
        if( m_hcal->isPinCard(card) ) 
          info() << " !!! Pin Diode ... " << caloCell.area() << " ID=" << caloCell << " crate = "
                 << crate << " card= " << card << " cardSlot = " << cardSlot << endmsg ;  
        if ( ( crate < 22 ) || ( crate > 25 ) ) 
          info() << " !!! area = " << caloCell.area() << " ID=" << caloCell << " crate = " << crate 
                 << " card= " << card << " cardSlot= " << cardSlot << endmsg ; 
        debug() << " area = " << caloCell.area() << " ID=" << caloCell << " crate = " << crate << " card= " << card << endmsg ; 
        if (crate == 22) m_histHadronCrate22 -> fill(cardSlot,1.) ; 
        if (crate == 23) m_histHadronCrate23 -> fill(cardSlot,1.) ; 
        if (crate == 24) m_histHadronCrate24 -> fill(cardSlot,1.) ; 
        if (crate == 25) m_histHadronCrate25 -> fill(cardSlot,1.) ; 
      }
      
      fillCalo2D(10003,caloCell,1.,"Hadron Hcal map") ; 

    } else if ( L0DUBase::CaloType::Pi0Local == (*cand)->type()  ) {
    
      m_histPi0Local  -> fill( (*cand)->etCode(), 1. );
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
      
      fillCalo2D(10004,caloCell,1.,"Pi0Local Ecal map") ;  

    } else if ( L0DUBase::CaloType::Pi0Global == (*cand)->type()  ) {
    
      m_histPi0Global -> fill( (*cand)->etCode(), 1. );
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
      
      fillCalo2D(10005,caloCell,1.,"Pi0Global Ecal map") ;  

    } else if ( L0DUBase::CaloType::SumEt == (*cand)->type()  ) {
    
      m_histSumEt     -> fill( (*cand)->etCode(), 1. );
    
    } else if ( L0DUBase::CaloType::SpdMult == (*cand)->type()  ) {
      
      m_histSpdMult   -> fill( (*cand)->etCode(), 1. );
    
    }
  }
    

  double event = -999 ; 
  double BCId  = -999 ; 

  if(exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin=get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    event = odin->eventNumber() ; 
    BCId = odin->bunchId() ; 
    m_nUsefulEvents++ ; 
  } 

  if (m_fullMonitoring) {
   
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

  //  return sc ; 
  return StatusCode::SUCCESS; 
}
//============================================================================
