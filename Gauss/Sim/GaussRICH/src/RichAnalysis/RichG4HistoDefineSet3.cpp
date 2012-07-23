// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local

#include "RichG4HistoDefineSet3.h"
#include "RichG4SvcLocator.h"

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

RichG4HistoDefineSet3::RichG4HistoDefineSet3() {
  // Declare job options
  m_RichG4HistoPathSet3="RICHG4HISTOSET3/";
  //  declareProperty( "RichG4HistoPath", m_RichG4HistoPath = "/RICHG4HISTOSET3/" );

  // Book histograms
  bookRichG4HistogramsSet3() ;
  
  
}
RichG4HistoDefineSet3::~RichG4HistoDefineSet3() { ; }

void RichG4HistoDefineSet3::bookRichG4HistogramsSet3() {

  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                                         "RichG4HistoSet3" );

  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set3" << endreq;
  std::string title;

  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  title=" Num Phot produced in Gas of  Rich1" ;
  
    m_hNumPhotGasProdRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"300",
                                        title,100,0.0,10000.0);
 
  title=" Num Phot from Gas On Mirror1 of  Rich1" ;

   m_hNumPhotGasMirror1Rich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"320",
                                        title,100,0.0,10000.0);

  title=" Num Phot from Gas On Mirror2 of  Rich1" ;

 
   m_hNumPhotGasMirror2Rich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"340",
                                        title,100,0.0,10000.0);

  title=" Num Phot from Gas On Qwindow of  Rich1" ;

   m_hNumPhotGasQWRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"360",
                                        title,100,0.0,10000.0);

  title=" Num Phot from Gas On HpdQw of  Rich1" ;

   m_hNumPhotGasHpdQWRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"380",
                                        title,100,0.0,10000.0);

  title=" Num Phot from Gas On Before QE of  Rich1" ;

   m_hNumPhotGasBeforeQERich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"400",
                                        title,100,0.0,5000.0);

  title=" Num Phot from Gas On After QE of  Rich1" ;

    m_hNumPhotGasAfterQERich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"420",
                                        title,100,0.0,3000.0);
  
  title=" Num PhotoElectrons from Gas On SiDet of  Rich1" ;

    m_hNumPeGasSiDetRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"440",
                                        title,100,0.0,3000.0);



  title=" Num Phot produced in Aerogel of  Rich1" ;
  
    m_hNumPhotAgelProdRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"500",
                                        title,100,0.0,10000.0);

   title=" Num Phot from Aerogel On Downstr face of Agel in  Rich1" ;

   m_hNumPhotAgelDownstrAgelRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"510",
                                        title,100,0.0,5000.0);

  title=" Num Phot from Aerogel On Mirror1 of  Rich1" ;

   m_hNumPhotAgelMirror1Rich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"520",
                                        title,100,0.0,5000.0);

  title=" Num Phot from Aerogel On Mirror2 of  Rich1" ;

 
   m_hNumPhotAgelMirror2Rich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"540",
                                        title,100,0.0,5000.0);

  title=" Num Phot from Aerogel On Qwindow of  Rich1" ;

   m_hNumPhotAgelQWRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"560",
                                        title,100,0.0,5000.0);

  title=" Num Phot from Aerogel On HpdQw of  Rich1" ;

   m_hNumPhotAgelHpdQWRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"580",
                                        title,100,0.0,5000.0);

  title=" Num Phot from Aerogel On Before QE of  Rich1" ;

   m_hNumPhotAgelBeforeQERich1 =   CurHistoSvc->
                                        book(m_RichG4HistoPathSet3+"600",
                                        title,100,0.0,3000.0);

  title=" Num Phot from Aerogel On After QE of  Rich1" ;

    m_hNumPhotAgelAfterQERich1 =   CurHistoSvc->
                                        book(m_RichG4HistoPathSet3+"620",
                                        title,100,0.0,2000.0);
  
  title=" Num PhotoElectrons from Aerogel On SiDet of  Rich1" ;

    m_hNumPeAgelSiDetRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet3+"640",
                                        title,100,0.0,1000.0);

    title = "Number of hits from Agel Radiators ";
    m_hNumAgelRadiator = CurHistoSvc->book(m_RichG4HistoPathSet3+"710",
					      title, 50, 0.0, 500.0);
    title = "Number of hits from c4f10 radiator ";

    m_hNumc4f10Radiator=  CurHistoSvc->book(m_RichG4HistoPathSet3+"711",
					    title, 50, 0.0, 4000.0 );

    title = "Number of hits from cf4 radiator ";

    m_hNumcf4Radiator=  CurHistoSvc->book(m_RichG4HistoPathSet3+"712",
					    title, 50, 0.0, 4000.0 );

    title = "Number of hits from filter generic radiator ";

    m_hNumfiltergenericRadiator = CurHistoSvc->book(m_RichG4HistoPathSet3+"714",
						   title,50, 0.0, 500.0 );
    title = "Number of hits from filter d263 radiator ";

    m_hNumfilterd263Radiator =  CurHistoSvc->book(m_RichG4HistoPathSet3+"715",
					    title, 50, 0.0, 500.0 );

    title = "Number of hits from rich1 gas qw radiators ";
    m_hNumRich1GasqwRadiator=  CurHistoSvc->book(m_RichG4HistoPathSet3+"716",
						 title,50, 0.0, 500.0 );
    title = "Number of hits from rich2 gas qw radiators ";
    m_hNumRich2GasqwRadiator=  CurHistoSvc->book(m_RichG4HistoPathSet3+"717",
						 title,50, 0.0, 500.0 );
   title = "Number of hits from rich1 hpdqw radiators ";
   m_hNumRich1HpdqwRadiator = CurHistoSvc->book(m_RichG4HistoPathSet3+"718",
                      title,50, 0.0, 1000.0 );
   title = "Number of hits from rich2 hpdqw radiators ";
   m_hNumRich2HpdqwRadiator = CurHistoSvc->book(m_RichG4HistoPathSet3+"719",
                      title,50, 0.0, 1000.0 );
                        
  
   title = "Number of hits from Rich1 without Hpd Reflections ";
   m_hNumRich1HpdNoRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"808",
                      title,100, 0.0, 1000.0 );

   title = "Number of hits from Rich1 without Hpd Reflections SmallSize upto50";
   m_hNumRich1HpdNoReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"809",
                      title,100, 0.0, 50.0 );

   title = "Number of hits from Rich2 without Hpd Reflections ";
   m_hNumRich2HpdNoRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"848",
                      title,100, 0.0, 1000.0 );

   title = "Number of hits from Rich2 without Hpd Reflections SmallSize upto 50";
   m_hNumRich2HpdNoReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"849",
                      title,100, 0.0, 50.0 );


   title = "Number of hits from Rich1 Hpd Reflections ";
   m_hNumRich1HpdRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"810",
                      title,100, 0.0, 1000.0 );

   title = "Number of hits from Rich1 Hpd Reflections SmallSize upto50";
   m_hNumRich1HpdReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"811",
                      title,100, 0.0, 50.0 );

   title = "Number of hits from Rich2 Hpd Reflections ";
   m_hNumRich2HpdRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"850",
                      title,100, 0.0, 1000.0 );

   title = "Number of hits from Rich2 Hpd Reflections SmallSize upto 50";
   m_hNumRich2HpdReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"851",
                      title,100, 0.0, 50.0 );


                        
  
   title = "Number of hits from Rich1 Gas HpdReflections ";
   m_hNumRich1GasHpdRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"815",
                      title,100, 0.0, 1000.0 );
   title = "Number of hits from Rich1 Gas HpdReflections small sizeupto 50";
   m_hNumRich1GasHpdReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"816",
                      title,100, 0.0, 50.0 );

   title = "Number of hits from Rich1 Gas Hpd BackScattering ";
   m_hNumRich1GasHpdBackScat = CurHistoSvc->book(m_RichG4HistoPathSet3+"820",
                      title,100, 0.0, 100.0 );

   title = "Number of hits from Rich1 Gas Hpd QWPC reflections  ";
   m_hNumRich1GasHpdQWPCRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"830",
                      title,100, 0.0, 200.0 );
   title = "Number of hits from Rich1 Gas Hpd QWPC reflections smallupto 50 ";
   m_hNumRich1GasHpdQWPCReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"831",
                      title,100, 0.0, 50.0 );

   title = "Number of hits from Rich1 Gas Hpd Chromium reflections ";
   m_hNumRich1GasHpdChromiumRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"840",
                      title,100, 0.0, 100.0 );
   title = "Number of hits from Rich1 Gas Hpd Silicon reflections ";
   m_hNumRich1GasHpdSiliconRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"845",
                      title,100, 0.0, 100.0 );

   title = "Number of hits from Rich1 Gas Hpd Kovar reflections ";
   m_hNumRich1GasHpdKovarRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"844",
                      title,100, 0.0, 100.0 );
   title = "Number of hits from Rich1 Gas Hpd Kapton reflections ";
   m_hNumRich1GasHpdKaptonRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"846",
                      title,100, 0.0, 100.0 );

   title = "Number of hits from Rich1 Aerogel Hpd Kapton reflections ";
   m_hNumRich1AerogelHpdKaptonRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"817",
                      title,100, 0.0, 100.0 );


   title = "Number of hits from Rich2 Gas HpdReflections ";
   m_hNumRich2GasHpdRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"855",
                      title,100, 0.0, 1000.0 );
   title = "Number of hits from Rich2 Gas HpdReflections small sizeupto 50";
   m_hNumRich2GasHpdReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"856",
                      title,100, 0.0, 50.0 );

   title = "Number of hits from Rich2 Gas Hpd BackScattering ";
   m_hNumRich2GasHpdBackScat = CurHistoSvc->book(m_RichG4HistoPathSet3+"860",
                      title,100, 0.0, 100.0 );

   title = "Number of hits from Rich2 Gas Hpd QWPC reflections  ";
   m_hNumRich2GasHpdQWPCRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"880",
                      title,100, 0.0, 200.0 );
   title = "Number of hits from Rich2 Gas Hpd QWPC reflections smallupto 50 ";
   m_hNumRich2GasHpdQWPCReflSmall = CurHistoSvc->book(m_RichG4HistoPathSet3+"881",
                      title,100, 0.0, 50.0 );

   title = "Number of hits from Rich2 Gas Hpd Chromium reflections ";
   m_hNumRich2GasHpdChromiumRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"890",
                      title,100, 0.0, 100.0 );
   title = "Number of hits from Rich2 Gas Hpd Silicon reflections ";
   m_hNumRich2GasHpdSiliconRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"895",
                      title,100, 0.0, 100.0 );

   title = "Number of hits from Rich2 Gas Hpd Kovar reflections ";
   m_hNumRich2GasHpdKovarRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"894",
                      title,100, 0.0, 100.0 );
   title = "Number of hits from Rich2 Gas Hpd Kapton reflections ";
   m_hNumRich2GasHpdKaptonRefl = CurHistoSvc->book(m_RichG4HistoPathSet3+"896",
                      title,100, 0.0, 100.0 );

  RichG4Histolog << MSG::INFO << "End of Booking Rich G4 Histo Set3" << endreq;
   
}







