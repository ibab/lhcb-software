
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local

#include "GaussRICH/RichG4HistoDefineSet1.h"
#include "GaussRICH/RichG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "globals.hh"
                                                                                                                  

//
//   Author SE 21-8-2002 
//
// Declaration of the Algorithm Factory
// static const  AlgFactory<RichG4Histo>          r_factory ;
// const        IAlgFactory& RichG4HistoFactory = r_factory ;
// RichG4Histo::RichG4Histo( const std::string& name,
//                           ISvcLocator* pSvcLocator) {}

RichG4HistoDefineSet1::RichG4HistoDefineSet1() {
  // Declare job options
  m_RichG4HistoPathSet1="RICHG4HISTOSET1/";
  //  declareProperty( "RichG4HistoPath", m_RichG4HistoPath = "/RICHG4HISTOSET1/" );

  // Book histograms
  bookRichG4HistogramsSet1() ;
  
  
}
RichG4HistoDefineSet1::~RichG4HistoDefineSet1() { ; }

void RichG4HistoDefineSet1::bookRichG4HistogramsSet1() {

  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), "RichG4HistoSet1" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set1" << endreq;
  std::string title;

  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  title="Total number of Rich1 Hits per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path for Set1 Histograms is    "
                               <<m_RichG4HistoPathSet1<<endreq;

  m_hNumTotHitRich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"1",
                             title,100,0.0,200.0);
  if(! m_hNumTotHitRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

    title="GlobalPhotoelectron Origin Y vs X for Rich1 ";
  
    //    m_hGlobalRich1PEOriginXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"2",
    //  title,300,-2000.0,2000.0,300,-2000.0, 2000.0);
    // if(!m_hGlobalRich1PEOriginXY ) {
    //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

    //    }

    title="Global PE hit coordinate positive Y vs X for Rich1 ";

    m_hGlobalRich1PeOriginPosHitXY = 
      CurHistoSvc->book(m_RichG4HistoPathSet1+"4",title,
     280,-700.0,700.0,80,800.0, 2400.0);


    title="Global hit coordinate from Agel positive Y vs X for Rich1 ";

    m_hGlobalPEOriginAgelPosXY= 
         CurHistoSvc->book(m_RichG4HistoPathSet1+"122",title,
     280,-700.0,700.0,80,800.0, 2400.0);


    //  title="GlobalPhotoelectron Origin from Agel Y vs X for Rich1 ";

    //  m_hGlobalPEOriginAgelXY = CurHistoSvc
    //  ->book(m_RichG4HistoPathSet1+"120",title,300,-2000.0,
    //    2000.0,300,-2000.0, 2000.0);
    // if(!m_hGlobalPEOriginAgelXY ) {
    // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

    //  }

  //  title="GlobalPE Origin from Agel Top Proj Y vs X for Rich1 ";

  //  m_hGlobalPEOriginAgelTopXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"122",
  //  title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginAgelTopXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }
  //  title="GlobalPE Origin from Agel Bot Proj Y vs X for Rich1 ";

  // m_hGlobalPEOriginAgelBotXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"124",
  //  title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginAgelBotXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }
  
    title="Global hit coordinate from C4F10 postive Y vs X for Rich1 ";

    m_hGlobalPEOriginC4F10PosXY= 
         CurHistoSvc->book(m_RichG4HistoPathSet1+"152",title,
     280,-700.0,700.0,80,800.0, 2400.0);


    //   title="GlobalPhotoelectron Origin from C4F10 Y vs X for Rich1 ";

    //  m_hGlobalPEOriginC4F10XY = CurHistoSvc
    //    ->book(m_RichG4HistoPathSet1+"150",title,300,-2000.0,
    //                2000.0,300,-2000.0, 2000.0);
    //  if(!m_hGlobalPEOriginC4F10XY ) {
    //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

    //  }
  title="GlobalPhotoelectron Origin from CF4 Y vs X for Rich2 ";

  //  m_hGlobalPEOriginCF4XY = CurHistoSvc->
  //  book(m_RichG4HistoPathSet1+"170",title,
  //            300,-5000.0,5000.0,300,-5000.0, 5000.0);
  //  if(!m_hGlobalPEOriginCF4XY ) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }
  title="GlobalPhotoelectron Origin from CF4 Y vs positive X for Rich2 ";

  m_hGlobalPEOriginCF4PosXY = CurHistoSvc
         ->book(m_RichG4HistoPathSet1+"172",title,
                200,2500.0,4500.0,200,-1000.0, 1000.0);
  if(!m_hGlobalPEOriginCF4PosXY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  // title="GlobalPE Origin from C4F10 Top Proj Y vs X for Rich1 ";

  // m_hGlobalPEOriginC4F10TopXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"152",
  //          title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginC4F10TopXY ) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }
  // title="GlobalPE Origin from C4F10 Bot Proj Y vs X for Rich1 ";

  //  m_hGlobalPEOriginC4F10BotXY = CurHistoSvc->book(m_RichG4HistoPathSet1+"154",
  //            title,300,-750.0,750.0,100,-250.0, 250.0);
  // if(!m_hGlobalPEOriginC4F10BotXY ) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  // }

  title="Photon Wavelength before incident on  Rich1 Mirror1 ";

  m_hWaveLenBeforeRich1Mirror1 = CurHistoSvc->book(m_RichG4HistoPathSet1+"40",
                       title,200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1Mirror1 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Photon Wavelength after incident on  Rich1 Mirror1 ";

  m_hWaveLenAfterRich1Mirror1 = CurHistoSvc->book(m_RichG4HistoPathSet1+"42",
                   title,200,150.0,950.0);
  if(!m_hWaveLenAfterRich1Mirror1 ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Photon Wavelength before incident on  Rich1 Gas QW ";

  m_hWaveLenBeforeRich1GasQW = CurHistoSvc->book(m_RichG4HistoPathSet1+"50",
               title,200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1GasQW ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Photon Wavelength after exiting from  Rich1 Gas QW ";

  m_hWaveLenAfterRich1GasQW = CurHistoSvc
             ->book(m_RichG4HistoPathSet1+"52",title,200,150.0,950.0);
  if(!m_hWaveLenAfterRich1GasQW ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Photon Wavelength before  Rich1 QE ";
  m_hWaveLenBeforeRich1QE = CurHistoSvc->book(m_RichG4HistoPathSet1+"60",title,
					      200,150.0,950.0);
  if(!m_hWaveLenBeforeRich1QE){
    RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq; 

  } 

  title="Photon Wavelength After  Rich1 QE ";     
  m_hWaveLenAfterRich1QE = CurHistoSvc->book(m_RichG4HistoPathSet1+"62",title,
					     200,150.0,950.0);
  if(!m_hWaveLenAfterRich1QE){
    RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;
   
  }


    //  title="Global PE hit coordinate Y vs X for Rich1 ";

    //    m_hGlobalHitXY= CurHistoSvc->book(m_RichG4HistoPathSet1+"3",title,
    // 300,-2000.0,2000.0,300,-2000.0, 2000.0);

    //    if(!m_hGlobalHitXY ) {
    //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

    //    }
  

  title="Cherenkov Angle in Aerogel from G4 ";


  m_hCkvAgelRich1= 
         CurHistoSvc->book(m_RichG4HistoPathSet1+"10",title,1200,0.1,0.40);
  if(! m_hCkvAgelRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Cherenkov Angle in C4F10 from G4 ";


  m_hCkvC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"20",title,
                                                    1200,0.03,0.07);
  if(! m_hCkvC4F10Rich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
  title="Cherenkov Angle in CF4 from G4 ";


  m_hCkvCF4Rich2= CurHistoSvc->book(m_RichG4HistoPathSet1+"70",
             title,1200,0.0,0.05);
  if(! m_hCkvCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Cherenkov Photon Production Z Coord from G4 ";

  //  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hCkvZEmissionPtRich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"25",
                 title,1300,900,2200);
  if(! m_hCkvZEmissionPtRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  //    title="Ref Index-1 vs wavelength for C4F10 from G4 ";

  //  m_hRefIndC4F10Rich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"22",
  //    title,200,150.0,950.0,200,0.001,0.002);
  // if(! m_hRefIndC4F10Rich1) {
  // RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }

  title="Ref Index-1 vs wavelength for CF4 from G4 ";

  m_hRefIndCF4Rich2= CurHistoSvc->book(m_RichG4HistoPathSet1+"72",title,
              200,150.0,950.0,200,0.0001,0.001);
  if(! m_hRefIndCF4Rich2) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  //  title="Ref Index-1 vs wavelength for Agel from G4 ";

  //  m_hRefIndAgelRich1= CurHistoSvc->book(m_RichG4HistoPathSet1+"12",title,
  //  200,150.0,950.0,200,0.01,0.08);
  //  if(! m_hRefIndAgelRich1) {
  //  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  //  }

  //  title = "Hpd Qw Hits Rich1 : Track prod X vs Z ";
  //  M_hHpdQwOrigXZR1= CurHistoSvc->book(m_RichG4HistoPathSet1+"1001", title,
  //				      100, 0.0, 2500.0, 100, -1000., 1000. );

  //  title = "Hpd Qw Hits Rich2 : Track prod X vs Z ";
  // M_hHpdQwOrigXZR2= CurHistoSvc->book(m_RichG4HistoPathSet1+"1002", title,
  //				      100, 5000., 13000.0, 100, -5000., 5000. );



  //  title = "Hpd Qw Hits Rich1 : Track prod Y vs Z ";

  //  M_hHpdQwOrigYZR1= CurHistoSvc->book(m_RichG4HistoPathSet1+"1010", title,
  //				      100, 0.0, 2500.0, 100, -3000., 3000. );


  //  title = "Hpd Qw Hits Rich2 : Track prod Y vs Z ";

  //  M_hHpdQwOrigYZR2= CurHistoSvc->book(m_RichG4HistoPathSet1+"1011", title,
	//			      100, 5000., 13000.0, 100, -3000., 3000. );

  

  //    title="Energy Loss in CF4 in 1 meter ";
  //  m_hEnergyLossInCF4 = CurHistoSvc->book(m_RichG4HistoPathSet1+"901",
  //                                                      title,100,0.0, 10.0);
  //
  //   title = "Number of Scintillating Photons produced in 1 meter in CF4";
  //
  //    m_hNumScintPhotProdInCF4 =CurHistoSvc->book( m_RichG4HistoPathSet1+"910",
  //                                                            title, 100,0.0, 3000.0);
  //

}







