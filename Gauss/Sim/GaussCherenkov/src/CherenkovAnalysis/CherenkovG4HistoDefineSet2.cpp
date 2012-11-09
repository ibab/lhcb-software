// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local

#include "CherenkovG4HistoDefineSet2.h"
#include "GaussRICH/RichG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// CLHEP
// #include "CLHEP/Units/PhysicalConstants.h"
//#include "Kernel/SystemOfUnits.h"
#include "globals.hh"
//
//   Author SE 21-8-2002 
//
// Declaration of the Algorithm Factory
// static const  AlgFactory<RichG4Histo>          r_factory ;
// const        IAlgFactory& RichG4HistoFactory = r_factory ;
// RichG4Histo::RichG4Histo( const std::string& name,
//                           ISvcLocator* pSvcLocator) {}

CherenkovG4HistoDefineSet2::CherenkovG4HistoDefineSet2() {
  // Declare job options
  m_RichG4HistoPathSet2="RICHG4HISTOSET2/";
  //  declareProperty( "RichG4HistoPath", m_RichG4HistoPath = "/RICHG4HISTOSET2/" );

  // Book histograms
  bookRichG4HistogramsSet2() ;
  
  
}
CherenkovG4HistoDefineSet2::~CherenkovG4HistoDefineSet2() { ; }

void CherenkovG4HistoDefineSet2::bookRichG4HistogramsSet2() {

  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                                         "RichG4HistoSet2" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set2" << endreq;
  std::string title;

  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  title=" Momentum of Charged Particles creating Hits in Rich1" ;
  
    m_hMomChPartRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet2+"501",
                                        title,100,0.0,100000.0);
  title=" Momentum of Charged Particles creating Hits in Rich2" ;
    
    m_hMomChPartRich2 =   CurHistoSvc->book(m_RichG4HistoPathSet2+"511",
                                        title,100,0.0,100000.0);



 title="Total number of Rich1 Hits in Large events per event ";

  m_hNumTotHitRich1Large= CurHistoSvc->book(m_RichG4HistoPathSet2+"106",
                              title,1000,0.0,8000.0);
  if(! m_hNumTotHitRich1Large) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Total number of Rich1 Hits in Aerogel in large events per event ";

  m_hNumTotHitAgelRich1Large= CurHistoSvc->book(m_RichG4HistoPathSet2+"123",
                              title,1000,0.0,8000.0);
  if(! m_hNumTotHitAgelRich1Large) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Total number of Rich1 Hits in Gas per event in all events ";

  m_hNumTotHitGasRich1Large= CurHistoSvc->book(m_RichG4HistoPathSet2+"153",
                              title,1000,0.0,8000.0);
  if(! m_hNumTotHitGasRich1Large) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }


  title="Total number of Rich2 Hits per event in all events ";

  m_hNumTotHitRich2Large= CurHistoSvc->book(m_RichG4HistoPathSet2+"173",
                              title,1000,0.0,8000.0);
  if(! m_hNumTotHitRich2Large) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }



  title="Total number of Rich1 Hits in Aerogel per event ";

  m_hNumTotHitAgelRich1= CurHistoSvc->book(m_RichG4HistoPathSet2+"125",
                              title,100,0.0,25.0);
  if(! m_hNumTotHitAgelRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

 
  title="Number of Rich1 Hits from Aerogel from primary particlegun per event ";

  m_hNumHitAgelPrim= CurHistoSvc->book(m_RichG4HistoPathSet2+"127",
                              title,100,0.0,25.0);
  if(! m_hNumHitAgelPrim) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of  saturated(beta gt 0.999)  Rich1 Hits inluding Rayleigh in Aerogel per track ";

  m_hNumHitAgelWithRlySat= CurHistoSvc->book(m_RichG4HistoPathSet2+"128",
                              title,100,0.0,25.0);
  if(! m_hNumHitAgelWithRlySat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of  saturated(beta gt 0.999)  Rich1 Hits in Aerogel per track ";

  m_hNumHitAgelSat= CurHistoSvc->book(m_RichG4HistoPathSet2+"129",
                              title,100,0.0,25.0);
  if(! m_hNumHitAgelSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of  Rich1 Hits in Aerogel per saturated track with no hpd refl or backscatter";

  m_hNumHitAgelSatNoRefl= CurHistoSvc->book(m_RichG4HistoPathSet2+"130",
                              title,100,0.0,25.0);
  if(! m_hNumHitAgelSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of  saturated  Rich1 Hits in Aerogel per track for All Tracks in RICH1 ";

  m_hNumTotHitAgelFullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"329",
                              title,100,0.0,25.0);
  if(! m_hNumTotHitAgelFullAcceptSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;
  }

  title="Number of  Sat Hits Vs polar Angle in Aerogel  ";

  m_hNumHitVsAngAgelFullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"322",
                              title,100,0.0,0.4,100,0.0,25.0);
  if(! m_hNumHitVsAngAgelFullAcceptSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;
  }


  title="Total number of Rich1 Hits in Rich1Gas per event ";


  m_hNumTotHitC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPathSet2+"155",
                          title,100,0.0,100.0);
  if(! m_hNumTotHitC4F10Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }


  title="Number of Rich1 Hits in Rich1Gas from primary particlegun per event";


  m_hNumHitC4F10Prim= CurHistoSvc->book(m_RichG4HistoPathSet2+"157",
                          title,100,0.0,100.0);
  if(! m_hNumHitC4F10Prim) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich1 saturated (beta gt 0.999) Hits in Rich1Gas per track";


  m_hNumHitC4F10Sat= CurHistoSvc->book(m_RichG4HistoPathSet2+"159",
                          title,100,0.0,100.0);
  if(! m_hNumHitC4F10Sat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich1 Hits from Rich1Gas per saturated track with no hpd refl or backscatter";


  m_hNumHitC4F10SatNoRefl= CurHistoSvc->book(m_RichG4HistoPathSet2+"160",
                          title,100,0.0,100.0);
  if(! m_hNumHitC4F10Sat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich1 Hits from Rich1Gas per saturated HighMom track with no hpd refl or backscatter";


  m_hNumHitC4F10SatNoReflHighMom= CurHistoSvc->book(m_RichG4HistoPathSet2+"158",
                          title,100,0.0,100.0);
  if(!m_hNumHitC4F10SatNoReflHighMom) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich1 saturated Hits in Rich1Gas per track for All Tracks in Rich1";


  m_hNumTotHitC4F10FullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"359",
                          title,100,0.0,100.0);
  if(! m_hNumTotHitC4F10FullAcceptSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich1 saturated signal Hits in Rich1Gas per track for All Tracks in Rich1";


  m_hNumTotSignalHitC4F10FullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"358",
                          title,100,0.0,100.0);
  if(! m_hNumTotHitC4F10FullAcceptSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }


  title="Number of Sat hits vs Track Polar Angle in Rich1Gas ";


  m_hNumHitVsAngC4F10FullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"352",
                          title,100, 0.0, 0.4, 100,0.0,60.0);
  if(! m_hNumHitVsAngC4F10FullAcceptSat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }




  title="Total number of Non-scint Rich2 Hits in Rich2Gas per event ";


  m_hNumTotHitCF4NonScintRich2= CurHistoSvc->book(m_RichG4HistoPathSet2+"171",
                     title,100,0.0,100.0);
  if(! m_hNumTotHitCF4NonScintRich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Total number of Scintillation Rich2 Hits in Rich2Gas per event ";


  m_hNumTotHitCF4ScintRich2= CurHistoSvc->book(m_RichG4HistoPathSet2+"172",
                     title,100,0.0,100.0);
  if(! m_hNumTotHitCF4ScintRich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }


  title="Total number of Rich2 Hits in Rich2Gas per event ";


  m_hNumTotHitCF4Rich2= CurHistoSvc->book(m_RichG4HistoPathSet2+"175",
                     title,100,0.0,100.0);
  if(! m_hNumTotHitCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich2 Hits in Rich2Gas from primary particlegun per event";


  m_hNumHitCF4Prim= CurHistoSvc->book(m_RichG4HistoPathSet2+"177",
                     title,100,0.0,100.0);
  if(! m_hNumHitCF4Prim) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich2 saturated (beta gt 0.999 ) Hits in Rich2Gas per track";


  m_hNumHitCF4Sat= CurHistoSvc->book(m_RichG4HistoPathSet2+"179",
                     title,100,0.0,100.0);
  if(! m_hNumHitCF4Sat) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich2 Hits in Rich2Gas per saturated track with no hpd refl or backscatter or Scintillation";


  m_hNumHitCF4SatNoReflNoScint= CurHistoSvc->book(m_RichG4HistoPathSet2+"180",
                     title,100,0.0,100.0);
  if(! m_hNumHitCF4SatNoReflNoScint) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Number of Rich2 Hits in Rich2Gas per saturated High Mom track with no hpd refl or backscatter or scintillation ";


  m_hNumHitCF4SatNoReflNoScintHighMom= CurHistoSvc->book(m_RichG4HistoPathSet2+"178",
                     title,100,0.0,100.0);
  if(! m_hNumHitCF4SatNoReflNoScintHighMom) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Number of Rich2 Hits in Rich2Gas per saturated track with no hpd refl or backscatter";


  m_hNumHitCF4SatNoRefl= CurHistoSvc->book(m_RichG4HistoPathSet2+"181",
                     title,100,0.0,100.0);
  if(!m_hNumHitCF4SatNoRefl ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Number of Rich2 Hits in Rich2Gas per saturated track from Scintillation ";


  m_hNumHitCF4SatScin= CurHistoSvc->book(m_RichG4HistoPathSet2+"182",
                     title,100,0.0,50.0);
  if(!m_hNumHitCF4SatScin ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }




  title="Number of Rich2 saturated Hits in Rich2Gas per track for All Tracks in Rich2";


  m_hNumTotHitCF4FullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"379",
                     title,100,0.0,100.0);
  if(!m_hNumTotHitCF4FullAcceptSat ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Number of Rich2 saturated Signal Hits in Rich2Gas per track for All Tracks in Rich2";


  m_hNumTotSignalHitCF4FullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"378",
                     title,100,0.0,100.0);
  if(!m_hNumTotHitCF4FullAcceptSat ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Number of Sat  Hits vs polar Angle in Rich2Gas ";


  m_hNumHitVsAngCF4FullAcceptSat = CurHistoSvc->book(m_RichG4HistoPathSet2+"372",
                     title,100,0.0,0.4,100,0.0,60.0);
  if(!m_hNumHitVsAngCF4FullAcceptSat ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Total number of non radiator hits in RICH1 per event ";


  m_hNumTotHitNoRadiatorRich1 = CurHistoSvc->book(m_RichG4HistoPathSet2+"193",
                     title, 20,0.0,20.0);
  if(! m_hNumTotHitNoRadiatorRich1 ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Total number of non radiator hits in RICH2 per event ";


  m_hNumTotHitNoRadiatorRich2 = CurHistoSvc->book(m_RichG4HistoPathSet2+"195",
                     title, 20,0.0,20.0);
  if(! m_hNumTotHitNoRadiatorRich2 ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Total number of non radiator hits per event ";


  m_hNumTotHitNoRadiator = CurHistoSvc->book(m_RichG4HistoPathSet2+"197",
                     title, 20,0.0,20.0);
  if(! m_hNumTotHitNoRadiator ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }


 title="Cherenkov Theta Angle at Photon Production in Aerogel vs Track Momentum";


  m_hCkvProdAgelRich1= 
         CurHistoSvc->book(m_RichG4HistoPathSet2+"600",title, 100,0.0,30000.0,60,0.1,0.40);
  if(! m_hCkvProdAgelRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Cherenkov Theta Angle at Photon Production in Rich1Gas vs Track Momemtum ";


  m_hCkvProdC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPathSet2+"610",title,
                                                   100,0.0,100000.0,60,0.03,0.07 );
  if(! m_hCkvProdC4F10Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
   

  title="Cherenkov Theta Angle at Photon Production in Rich2Gas vs Track Momemtum ";


  m_hCkvProdCF4Rich2= CurHistoSvc->book(m_RichG4HistoPathSet2+"620",
             title,100,0.0,100000.0,60,0.0,0.05 );
  if(! m_hCkvProdCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }


  
}







