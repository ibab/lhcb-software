// $Id: RichG4HistoDefineSet4.cpp,v 1.4 2007-01-12 15:32:09 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local
#include "RichG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "globals.hh"

// local
#include "RichG4HistoDefineSet4.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoDefineSet4
//
// 2003-09-22 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HistoDefineSet4::RichG4HistoDefineSet4(  ) {
  // Declare job options
  m_RichG4HistoPathSet4 = "RICHG4HISTOSET4/";
  // Book histograms
  bookRichG4HistogramsSet4() ;

}
RichG4HistoDefineSet4::~RichG4HistoDefineSet4(  ) {

}
void RichG4HistoDefineSet4::bookRichG4HistogramsSet4()
{

  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                                         "RichG4HistoSet4" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set4" << endreq;
  
  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

  std::string title = 
         "Cherenkov angle D3E1 reconstructed from Rich1 Gas radiator";
  
    m_hCkvRich1GasD3E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1500",
                      title,200,0.04,0.06);
     title = 
         "Cherenkov angle D4E1 reconstructed from Rich1 Gas radiator";
  
    m_hCkvRich1GasD4E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1512",
                      title,200,0.04,0.06);


    title = 
         "Cherenkov angle D5E4 reconstructed from Rich1 Gas radiator";
  
    m_hCkvRich1GasD5E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1515",
                      title,200,0.04,0.06);


    title = 
         "Cherenkov angle D1E4 reconstructed from Rich1 Gas radiator";
  
    m_hCkvRich1GasD1E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1501",
                      title,200,0.04,0.06);

    title = 
      "Cherenkov angle diff QW - PhCath from Rich1 Gas Radiator ";
    

      m_hCkvRich1GasQw = CurHistoSvc->book(m_RichG4HistoPathSet4+"1513",
					   title, 400,-0.005, 0.005);

    title = 
      "Cherenkov angle diff QW - PhCathWithCorr from Rich1 Gas Radiator ";
    

      m_hCkvRich1GasQwPh = CurHistoSvc->book(m_RichG4HistoPathSet4+"1514",
					   title, 200,-0.002, 0.002);

   title = 
         "Cherenkov angle generated in Rich1 Gas radiator";
  
    m_hCkvRich1GasGen = CurHistoSvc->book(m_RichG4HistoPathSet4+"1504",
                      title,200,0.04,0.06);

       
  
    title = "Cherenkov angle D3E3 reconstructed from Rich1 Aerogel radiator";
    

     m_hCkvRich1AgelD3E3 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1600",
                      title,1000,0.2,0.3);

    title = "Cherenkov angle D4E3 reconstructed from Rich1 Aerogel radiator";    

     m_hCkvRich1AgelD4E3 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1612",
                      title,1000,0.2,0.3);

    title = "Cherenkov angle D1E4 reconstructed from Rich1 Aerogel radiator";
     m_hCkvRich1AgelD1E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1601",
                      title,1000,0.2,0.3);


    title = "Cherenkov angle D5E4 reconstructed from Rich1 Aerogel radiator";
     m_hCkvRich1AgelD5E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1615",
                      title,1000,0.2,0.3);

    title = "Cherenkov angle generated in Rich1 Aerogel radiator";
    

     m_hCkvRich1AgelGen= CurHistoSvc->book(m_RichG4HistoPathSet4+"1604",
                      title,1000,0.2,0.3);


   title = 
     "Cherenkov angle diff QW - PhCath from Aerogel Radiator ";
   

     m_hCkvRich1AgelQw = CurHistoSvc->book(m_RichG4HistoPathSet4+"1613",
					   title, 200,-0.005, 0.005);

   title = 
     "Cherenkov angle diff QW - PhCath withCorr from Aerogel Radiator ";
   

     m_hCkvRich1AgelQwPh = CurHistoSvc->book(m_RichG4HistoPathSet4+"1614",
					   title, 200,-0.025, 0.025);


     title = "Cherenkov angle D3E1 reconstructed from Rich2 Gas radiator";
     
    m_hCkvRich2GasD3E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1700",
                      title,200,0.02,0.04);

     title = "Cherenkov angle D4E1 reconstructed from Rich2 Gas radiator";
     
    m_hCkvRich2GasD4E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1712",
                      title,200,0.02,0.04);

     title = "Cherenkov angle D1E4 reconstructed from Rich2 Gas radiator";
     
    m_hCkvRich2GasD1E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1701",
                      title,200,0.02,0.04);

     title = "Cherenkov angle D5E4 reconstructed from Rich2 Gas radiator";
     
    m_hCkvRich2GasD5E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1715",
                      title,200,0.02,0.04);

     title = "Cherenkov angle generated in Rich2 Gas radiator";
     
    m_hCkvRich2GasGen= CurHistoSvc->book(m_RichG4HistoPathSet4+"1704",
                      title,200,0.02,0.04);


  title = 
    "Cherenkov angle diff QW - PhCath from Rich2 Gas Radiator ";
  

      m_hCkvRich2GasQw = CurHistoSvc->book(m_RichG4HistoPathSet4+"1713",
					   title, 200,-0.002, 0.002);

  title = 
    "Cherenkov angle diff QW - PhCath with Corr from Rich2 Gas Radiator ";
  

      m_hCkvRich2GasQwPh = CurHistoSvc->book(m_RichG4HistoPathSet4+"1714",
					   title, 200,-0.002, 0.002);

    title="Rich1Gas ReconsD3E1- generated ckv angle";
    
     m_hCkvRich1GasRes = CurHistoSvc->book(m_RichG4HistoPathSet4+"1510",
                                           title, 1000,-0.01,0.01);


    title="Rich1Gas ReconsD3E1- const generated ckv angle";
    
     m_hCkvRich1GasResConst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1511",
                                           title, 200,-0.005,0.005);
     title="Rich1Agel ReconsD3E3- generated ckv angle";
     
     m_hCkvRich1AgelRes = CurHistoSvc->book(m_RichG4HistoPathSet4+"1610",
                                           title, 200,-0.005,0.005);
     title="Rich1Agel ReconsD3E3- const generated ckv angle";
     
     m_hCkvRich1AgelResConst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1611",
                                           title, 200,-0.01,0.01);
   
     title="Rich2Gas Recons D3E1- generated ckv angle";
     
     m_hCkvRich2GasRes = CurHistoSvc->book(m_RichG4HistoPathSet4+"1710",
                                           title, 1000,-0.01,0.01);
     title="Rich2Gas Recons D3E1- const generated ckv angle";
     
     m_hCkvRich2GasResConst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1711",
                                           title, 200,-0.002,0.002);

     title="Rich1Gas Ckvdiff vs Generated Phi";
     
     m_hCkvRich1GasCkvPhi = CurHistoSvc->book(m_RichG4HistoPathSet4+"1520",
                      title,100,0.0,6.5,100,-0.001,0.001);
     title="Rich1Gas CkvdiffD3E1 vs Generated Phi Profile";
     
     //  m_hCkvRich1GasCkvResPhiProf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1522",
     //                 title,100,0.0,6.5,-0.001,0.001);

     title="Rich1Agel Ckvdiff D3E1 vs Generated Phi";

      m_hCkvRich1AgelCkvPhi= CurHistoSvc->book(m_RichG4HistoPathSet4+"1620",
                     title,100,0.0,6.5,100,-0.001,0.001);

      //     title="Rich1Agel Ckvdiff vs Generated Phi Profile";

      //      m_hCkvRich1AgelCkvResPhiProf= CurHistoSvc->book(m_RichG4HistoPathSet4+"1622",
      //               title,100,0.0,6.5,-0.005,0.005);
      
      // title= "Rich2Gas Ckvdiff vs Generated Phi";
         
      //  m_hCkvRich2GasCkvPhi = CurHistoSvc->book(m_RichG4HistoPathSet4+"1720",
      //                title,100,0.0,6.5,100,-0.001,0.001);
      //  title= "Rich2Gas Ckvdiff vs Generated Phi Profile";
      //    
      //  m_hCkvRich2GasCkvResPhiProf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1722",
      //                title,100,0.0,6.5,-0.001,0.001);

      // now for emission point error

      // title="Rich1Gas ReconsD3E4- Recons D3E1 Emiss error";
      title="Rich1Gas ReconsD4E4- Recons D4E1 Emiss error";
    
     m_hCkvRich1GasResEmis = CurHistoSvc->book(m_RichG4HistoPathSet4+"1530",
                                           title, 500,-0.003,0.003);

     // title="Rich1Agel ReconsD3E4 - Recons D3E3 Emiss error";
      title="Rich1Agel ReconsD4E4 - Recons D4E3 Emiss error";
    
      m_hCkvRich1AgelResEmis = CurHistoSvc->book(m_RichG4HistoPathSet4+"1630",
                                           title, 500,-0.003,0.003);

      //  title="Rich2Gas ReconsD3E4- Recons D3E1 Emiss error";
       title="Rich2Gas ReconsD4E4- Recons D4E1 Emiss error";
    
      m_hCkvRich2GasResEmis = CurHistoSvc->book(m_RichG4HistoPathSet4+"1730",
                                           title, 500,-0.003,0.003);
      // agel exit refraction error

      title="Rich1Agel ReconsD3E1 - Recons D3E3 AgelExitRef error";
    
      m_hCkvRich1AgelExitResRefraction = CurHistoSvc->book(m_RichG4HistoPathSet4+"1635",
                                           title, 200,-0.001,0.001);

 
      // now for pixel error

      title="Rich1Gas ReconsD1E1- Recons D2E1 Pixel error";
    
      m_hCkvRich1GasResPixel = CurHistoSvc->book(m_RichG4HistoPathSet4+"1540",
                                           title, 1000,-0.01,0.01);

      title="Rich1Agel ReconsD1E3- Recons D2E3 Pixel error";
    
      m_hCkvRich1AgelResPixel = CurHistoSvc->book(m_RichG4HistoPathSet4+"1640",
                                           title, 500,-0.005,0.005);

      title="Rich2Gas ReconsD1E1- Recons D2E1 Pixel error";
    
      m_hCkvRich2GasResPixel = CurHistoSvc->book(m_RichG4HistoPathSet4+"1740",
                                           title, 500,-0.005,0.005);

      // now for any PSF type error.


      title="Rich1Gas ReconsD2E1- Recons D3E1 psf error";
    
      m_hCkvRich1GasResPsf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1550",
                                           title, 500,-0.005,0.005);
      title="Rich1Agel ReconsD2E3- Recons D3E3 psf error";
    
      m_hCkvRich1AgelResPsf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1650",
                                           title, 500,-0.005,0.005);

      title="Rich2Gas ReconsD2E1- Recons D3E1 psf error";
    
      m_hCkvRich2GasResPsf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1750",
                                           title, 500,-0.005,0.005);

      //  total error


      title="Rich1Gas ReconsD1E4- generated ckv angle total error";
      m_hCkvRich1GasResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1560",
                                           title, 200,-0.005,0.005);

      title="Rich1Agel ReconsD1E4- generated ckv angle total error";
      m_hCkvRich1AgelResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1660",
                                           title, 200,-0.005,0.005);

      title="Rich2Gas ReconsD1E4- generated ckv angle total error";
      m_hCkvRich2GasResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1760",
                                           title, 200,-0.005,0.005);

     

}


//=============================================================================
