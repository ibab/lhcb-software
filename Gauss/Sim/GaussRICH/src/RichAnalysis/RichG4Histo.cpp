
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
//local

#include "RichG4Histo.h"
#include "RichG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//
//   Author SE 21-8-2002 
//
// Declaration of the Algorithm Factory
// static const  AlgFactory<RichG4Histo>          r_factory ;
// const        IAlgFactory& RichG4HistoFactory = r_factory ;
// RichG4Histo::RichG4Histo( const std::string& name,
//                           ISvcLocator* pSvcLocator) {}

RichG4Histo::RichG4Histo() {
  // Declare job options
  m_RichG4HistoPath="RICHG4HISTO/";
  //  declareProperty( "RichG4HistoPath", m_RichG4HistoPath = "/RICHG4HISTO/" );

  // Book histograms
  bookRichG4Histograms() ;
  
  
}
RichG4Histo::~RichG4Histo() { ; }

// StatusCode RichG4Histo::initialize() {
  // MsgStream log(msgSvc(), name());
  //  log << MSG::DEBUG << "RichG4 Histo Initialize" << endreq;

//  return StatusCode::SUCCESS;
// }
// Main execution
// StatusCode RichG4Histo::execute() {

  // MsgStream log( msgSvc(), name() );
  // log << MSG::DEBUG << "Rich G4 Histo Execute" << endreq;

//  return StatusCode::SUCCESS;

// }

//  Finalize
// StatusCode RichG4Histo::finalize() {
  // MsgStream log(msgSvc(), name());
  // log << MSG::DEBUG << "Rich G4 Histo Finalize" << endreq;

//  return StatusCode::SUCCESS;

// }
void RichG4Histo::bookRichG4Histograms() {

  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), "RichG4Histo" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo" << endreq;
  std::string title;

  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  title="Total number of Rich1 Hits per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "
                               <<m_RichG4HistoPath<<endreq;

  m_hNumTotHitRich1= CurHistoSvc->book(m_RichG4HistoPath+"1",
                             title,100,0.0,200.0);
  if(! m_hNumTotHitRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Total number of Rich1 Hits in Aerogel per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "
                              <<m_RichG4HistoPath<<endreq;

  m_hNumTotHitAgelRich1= CurHistoSvc->book(m_RichG4HistoPath+"125",
                              title,100,0.0,200.0);
  if(! m_hNumTotHitAgelRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Total number of Rich1 Hits in C4F10 per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hNumTotHitC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPath+"155",
                          title,100,0.0,200.0);
  if(! m_hNumTotHitC4F10Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Total number of Rich1 Hits in CF4 per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hNumTotHitCF4Rich1= CurHistoSvc->book(m_RichG4HistoPath+"175",
                     title,100,0.0,200.0);
  if(! m_hNumTotHitCF4Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  //  title="GlobalPhotoelectron Origin Y vs X for Rich1 ";
  //
  //  m_hGlobalPEOriginXY = CurHistoSvc->book(m_RichG4HistoPath+"2",title,300,-2000.0,2000.0,300,-2000.0, 2000.0);
  //  if(!m_hGlobalPEOriginXY ) {
  //   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }
  title="GlobalPhotoelectron Origin from Agel Y vs X for Rich1 ";

  m_hGlobalPEOriginAgelXY = CurHistoSvc
      ->book(m_RichG4HistoPath+"120",title,300,-2000.0,
        2000.0,300,-2000.0, 2000.0);
  if(!m_hGlobalPEOriginAgelXY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  //  title="GlobalPE Origin from Agel Top Proj Y vs X for Rich1 ";

  //  m_hGlobalPEOriginAgelTopXY = CurHistoSvc->book(m_RichG4HistoPath+"122",title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginAgelTopXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }
  //  title="GlobalPE Origin from Agel Bot Proj Y vs X for Rich1 ";

  // m_hGlobalPEOriginAgelBotXY = CurHistoSvc->book(m_RichG4HistoPath+"124",title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginAgelBotXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }
  
   title="GlobalPhotoelectron Origin from C4F10 Y vs X for Rich1 ";

  m_hGlobalPEOriginC4F10XY = CurHistoSvc
        ->book(m_RichG4HistoPath+"150",title,300,-2000.0,
                     2000.0,300,-2000.0, 2000.0);
  if(!m_hGlobalPEOriginC4F10XY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="GlobalPhotoelectron Origin from CF4 Y vs X for Rich2 ";

  m_hGlobalPEOriginCF4XY = CurHistoSvc->
    book(m_RichG4HistoPath+"170",title,300,-5000.0,5000.0,300,-5000.0, 5000.0);
  if(!m_hGlobalPEOriginCF4XY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="GlobalPhotoelectron Origin from CF4 Y vs positive X for Rich2 ";

  m_hGlobalPEOriginCF4PosXY = CurHistoSvc
         ->book(m_RichG4HistoPath+"171",title,200,2500.0,4500.0,200,-1000.0, 1000.0);
  if(!m_hGlobalPEOriginCF4PosXY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  // title="GlobalPE Origin from C4F10 Top Proj Y vs X for Rich1 ";

  // m_hGlobalPEOriginC4F10TopXY = CurHistoSvc->book(m_RichG4HistoPath+"152",title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginC4F10TopXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }
  // title="GlobalPE Origin from C4F10 Bot Proj Y vs X for Rich1 ";

  //  m_hGlobalPEOriginC4F10BotXY = CurHistoSvc->book(m_RichG4HistoPath+"154",title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginC4F10BotXY ) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }

  title="Photon Wavelength before incident on  Rich1 Mirror1 ";

  m_hWaveLenBeforeRich1Mirror1 = CurHistoSvc->book(m_RichG4HistoPath+"40",
                       title,200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1Mirror1 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Photon Wavelength after incident on  Rich1 Mirror1 ";

  m_hWaveLenAfterRich1Mirror1 = CurHistoSvc->book(m_RichG4HistoPath+"42",
                   title,200,150.0,950.0);
  if(!m_hWaveLenAfterRich1Mirror1 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Photon Wavelength before incident on  Rich1 Gas QW ";

  m_hWaveLenBeforeRich1GasQW = CurHistoSvc->book(m_RichG4HistoPath+"50",
               title,200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1GasQW ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Photon Wavelength after exiting from  Rich1 Gas QW ";

  m_hWaveLenAfterRich1GasQW = CurHistoSvc
             ->book(m_RichG4HistoPath+"52",title,200,150.0,950.0);
  if(!m_hWaveLenAfterRich1GasQW ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Photon Wavelength before  Rich1 QE ";
  m_hWaveLenBeforeRich1QE = CurHistoSvc->book(m_RichG4HistoPath+"60",title,
					      200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1QE){
    RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq; 

  } 

  title="Photon Wavelength After  Rich1 QE ";     
  m_hWaveLenAfterRich1QE = CurHistoSvc->book(m_RichG4HistoPath+"62",title,
					     200,150.0,950.0);
  if(!m_hWaveLenAfterRich1QE){
    RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;
   
  }

  //  title="Global hit coordinate Y vs X for Rich1 ";

  //  m_hGlobalHitXY= CurHistoSvc->book(m_RichG4HistoPath+"3",title,300,-2000.0,2000.0,300,-2000.0, 2000.0);

  //  if(!m_hGlobalHitXY ) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }
  

  title="Cherenkov Angle in Aerogel from G4 ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "
                 <<m_RichG4HistoPath<<endreq;

  m_hCkvAgelRich1= 
         CurHistoSvc->book(m_RichG4HistoPath+"10",title,1200,0.1,0.40);
  if(! m_hCkvAgelRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Cherenkov Angle in C4F10 from G4 ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hCkvC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPath+"20",title,
                                                    1200,0.03,0.07);
  if(! m_hCkvC4F10Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Cherenkov Angle in CF4 from G4 ";


  RichG4Histolog << MSG::INFO
          <<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hCkvCF4Rich2= CurHistoSvc->book(m_RichG4HistoPath+"70",title,1200,0.0,0.05);
  if(! m_hCkvCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Cherenkov Photon Production Z Coord from G4 ";

  //  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hCkvZEmissionPtRich1= CurHistoSvc->book(m_RichG4HistoPath+"25",
                 title,1300,900,2200);
  if(! m_hCkvZEmissionPtRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  //    title="Ref Index-1 vs wavelength for C4F10 from G4 ";

  //  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  //  m_hRefIndC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPath+"22",title,200,150.0,950.0,200,0.001,0.002);
  // if(! m_hRefIndC4F10Rich1) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }

  title="Ref Index-1 vs wavelength for CF4 from G4 ";

  //  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hRefIndCF4Rich2= CurHistoSvc->book(m_RichG4HistoPath+"72",title,
              200,150.0,950.0,200,0.0001,0.001);
  if(! m_hRefIndCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  //  title="Ref Index-1 vs wavelength for Agel from G4 ";

  //  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  //  m_hRefIndAgelRich1= CurHistoSvc->book(m_RichG4HistoPath+"12",title,200,150.0,950.0,200,0.01,0.08);
  //  if(! m_hRefIndAgelRich1) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }

  
   //  return StatusCode::SUCCESS;

}







