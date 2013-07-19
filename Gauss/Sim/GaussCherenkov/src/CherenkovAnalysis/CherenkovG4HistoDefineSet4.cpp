// $Id: CherenkovG4HistoDefineSet4.cpp,v 1.4 2007-01-12 15:32:09 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local
#include "GaussCherenkov/CkvG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "globals.hh"

// local
#include "CherenkovG4HistoDefineSet4.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HistoDefineSet4
//
// 2003-09-22 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HistoDefineSet4::CherenkovG4HistoDefineSet4(  ) {
  // Declare job options
  m_RichG4HistoPathSet4 = "RICHG4HISTOSET4/";
  // Book histograms
  bookRichG4HistogramsSet4() ;

}
CherenkovG4HistoDefineSet4::~CherenkovG4HistoDefineSet4(  ) {

}
void CherenkovG4HistoDefineSet4::bookRichG4HistogramsSet4()
{

  MsgStream RichG4Histolog(CkvG4SvcLocator::RichG4MsgSvc(), 
                                         "RichG4HistoSet4" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set4" << endreq;
  
  IHistogramSvc* CurHistoSvc = CkvG4SvcLocator::RichG4HistoSvc();

    std::string title = 
         "Cherenkov angle D3E1 reconstruted from Rich1 Gas radiator";
  
    m_hCkvRich1GasD3E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1500",
                      title,1000,0.0,0.1);
    //   std::string titleA = 
    //     "Cherenkov angle D3E1 reconstruted from Rich1 Gas radiatorA";
    std::string titleA = 
         "Cherenkov angle D3E1 reconstruted from Rich1 Gas radiatorA";
  
    m_hCkvRich1GasD3E1A = CurHistoSvc->book(m_RichG4HistoPathSet4+"1500A",
                      title,1000,0.04,0.07);


    title = 
         "Cherenkov angle D4E1 reconstructed from Rich1 Gas radiator";
  
    m_hCkvRich1GasD4E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1512",
                      title,200,0.04,0.06);


    title = 
         "Cherenkov angle D4E1 reconstructed from Rich1 Gas radiator for primary tracks";
  
    m_hCkvRich1GasD4E1Prim = CurHistoSvc->book(m_RichG4HistoPathSet4+"1582",
                      title,200,0.04,0.06);


    title = 
         "Cherenkov angle D4E1 reconstructed -Generated ";
  
    m_hCkvRich1GasD4E1MinusGen = CurHistoSvc->book(m_RichG4HistoPathSet4+"1584",
                      title,200,-0.01,0.01);



    title = 
         "Cherenkov angle D4E1 reconstructed from Rich1 Gas radiator vs Phi";
  
    m_hCkvRich1GasD4E1VsPhi = CurHistoSvc->book(m_RichG4HistoPathSet4+"1532",
                      title,100,0.0,6.5,200,0.048,0.056);





    title = 
         "Cherenkov angle D9E1 reconstructed -Generated .     Using True flatmirrorReflpt ";
  
    m_hCkvRich1GasD9E1MinusGen = CurHistoSvc->book(m_RichG4HistoPathSet4+"1585",
                      title,200,-0.01,0.01);



    title = 
         "Cherenkov angle D4E4 reconstructed -Generated .   ";
  
    m_hCkvRich1GasD4E4MinusGen = CurHistoSvc->book(m_RichG4HistoPathSet4+"1588",
                      title,200,-0.02,0.02);

    title = 
         "Cherenkov angle D4E4 reconstructed -Generated . Primary  ";
  
    m_hCkvRich1GasD4E4MinusGenPrim = CurHistoSvc->book(m_RichG4HistoPathSet4+"1590",
                      title,200,-0.02,0.02);


    title = 
         "Cherenkov angle D4E4 reconstructed . ";
  
    m_hCkvRich1GasD4E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1589",
                      title,200,0.04,0.06);


    title = 
         "Cherenkov angle D9E1 reconstructed -Generated .     Using True flatmirrorrueReflpt and SphTrueReflPt ";
  
    m_hCkvRich1GasD9E1MinusGenTFTS = CurHistoSvc->book(m_RichG4HistoPathSet4+"1586",
                      title,200,-0.01,0.01);







    title = 
      "Cherenkov angle diff QW - PhCathWithCorr from Rich1 Gas Radiator ";
    

      m_hCkvRich1GasQwPh = CurHistoSvc->book(m_RichG4HistoPathSet4+"1514",
					   title, 200,-0.002, 0.002);


    title = 
         "Cherenkov angle D1E4 reconstruted from Rich1 Gas radiator";
  
    m_hCkvRich1GasD1E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1501",
                      title,1000,0.03,0.07);

    title = 
         "Cherenkov angle D1E4 reconstruted from Rich1 Gas radiator LimitedRegion ";
  
    m_hCkvRich1GasD1E4Rst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1501A",
                      title,1000,0.03,0.07);

    title = 
         "Cherenkov angle D8E4 reconstruted from Rich1 Gas radiator";
  
    m_hCkvRich1GasD8E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1591",
                      title,1000,0.03,0.07);

   title = 
         "Cherenkov angle generated in Rich1 Gas radiator";
  
    m_hCkvRich1GasGen = CurHistoSvc->book(m_RichG4HistoPathSet4+"1504",
                      title,1000,0.04,0.06);

       
  
    title = "Cherenkov angle D3E3 reconstruted from Rich1 Aerogel radiator";
    

     m_hCkvRich1AgelD3E3 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1600",
                      title,10000,0.15,0.35);

    title = "Cherenkov angle D1E4 reconstruted from Rich1 Aerogel radiator";
     m_hCkvRich1AgelD1E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1601",
                      title,10000,0.15,0.35);

    title = "Cherenkov angle generated in Rich1 Aerogel radiator";
    

     m_hCkvRich1AgelGen= CurHistoSvc->book(m_RichG4HistoPathSet4+"1604",
                      title,10000,0.15,0.35);

     title = "Cherenkov angle D3E1 reconstruted from Rich2 Gas radiator";
     
    m_hCkvRich2GasD3E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1700",
                      title,1000,0.0,0.05);

    //    title = "Cherenkov angle D3E1 reconstruted from Rich2 Gas radiatorA";

     title = "Cherenkov angle D4E1 reconstruted from Rich2 Gas radiatorA";

    m_hCkvRich2GasD3E1A = CurHistoSvc->book(m_RichG4HistoPathSet4+"1700A",
                      title,200,0.025,0.035);


     title = "Cherenkov angle D4E1 reconstructed from Rich2 Gas radiator";
     
    m_hCkvRich2GasD4E1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1712",
                      title,200,0.025,0.035);


   title = 
         "Cherenkov angle D4E1 reconstructed from Rich2 Gas radiator vs Phi";
  
      m_hCkvRich2GasD4E1VsPhi = CurHistoSvc->book(m_RichG4HistoPathSet4+"1732",
                      title,100,0.0,6.5,200,0.028,0.036);


     title = "Cherenkov angle D1E4 reconstruted from Rich2 Gas radiator";
     
    m_hCkvRich2GasD1E4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1701",
                      title,1000,0.025,0.035);

     title = "Cherenkov angle generated in Rich2 Gas radiator";
     
    m_hCkvRich2GasGen= CurHistoSvc->book(m_RichG4HistoPathSet4+"1704",
                      title,1000,0.025,0.035);


   title = 
    "Cherenkov angle diff QW - PhCath with Corrfrom Rich2 Gas Radiator ";
  

      m_hCkvRich2GasQwPh = CurHistoSvc->book(m_RichG4HistoPathSet4+"1714",
					   title, 200,-0.002, 0.002);



    title="Rich1Gas ReconsD3E1- generated ckv angle";
    
     m_hCkvRich1GasRes = CurHistoSvc->book(m_RichG4HistoPathSet4+"1510",
                                           title, 1000,-0.005,0.005);


    title="Rich1Gas ReconsD3E1- const generated ckv angle";
    
     m_hCkvRich1GasResConst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1511",
                                           title, 1000,-0.005,0.005);
     title="Rich1Agel ReconsD3E3- generated ckv angle";
     
     m_hCkvRich1AgelRes = CurHistoSvc->book(m_RichG4HistoPathSet4+"1610",
                                           title, 1000,-0.01,0.01);
     title="Rich1Agel ReconsD3E3- const generated ckv angle";
     
     m_hCkvRich1AgelResConst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1611",
                                           title, 1000,-0.005,0.005);
   
     title="Rich2Gas Recons D3E1- generated ckv angle";
     
     m_hCkvRich2GasRes = CurHistoSvc->book(m_RichG4HistoPathSet4+"1710",
                                           title, 1000,-0.01,0.01);
     title="Rich2Gas Recons D3E1- const generated ckv angle";
     
     m_hCkvRich2GasResConst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1711",
                                           title, 1000,-0.005,0.005);

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

      title="Rich1Gas ReconsD3E4- Recons D3E1 Emiss error";
    
     m_hCkvRich1GasResEmis = CurHistoSvc->book(m_RichG4HistoPathSet4+"1530",
                                           title, 1000,-0.005,0.005);


      title="Rich1Gas ReconsD4E4- Recons D4E1 Emiss error ";
    
     m_hCkvRich1GasResEmisD4 = CurHistoSvc->book(m_RichG4HistoPathSet4+"1531",
                                           title, 1000,-0.005,0.005);

      title="Rich1Gas  ReconsD4E4- Recons D4E1 Emiss error Primary track ";
    
     m_hCkvRich1GasResEmisD4Prim = CurHistoSvc->book(m_RichG4HistoPathSet4+"1538",
                                           title, 1000,-0.005,0.005);


      title="Rich1Gas ReconsD4E4- Recons D4E1 Emiss error  Limitedregion";
    
     m_hCkvRich1GasResEmisD4Rst = CurHistoSvc->book(m_RichG4HistoPathSet4+"1539",
                                           title, 1000,-0.005,0.005);




      title="Rich1Agel ReconsD3E4 - Recons D3E3 Emiss error";
    
      m_hCkvRich1AgelResEmis = CurHistoSvc->book(m_RichG4HistoPathSet4+"1630",
                                           title, 1000,-0.01,0.01);

       title="Rich2Gas ReconsD3E4- Recons D3E1 Emiss error";
    
      m_hCkvRich2GasResEmis = CurHistoSvc->book(m_RichG4HistoPathSet4+"1730",
                                           title, 1000,-0.005,0.005);
      // agel exit refraction error

      title="Rich1Agel ReconsD3E1 - Recons D3E3 AgelExitRef error";
    
      m_hCkvRich1AgelExitResRefraction = CurHistoSvc->book(m_RichG4HistoPathSet4+"1635",
                                           title, 1000,-0.01,0.01);

 
      // now for pixel error

      title="Rich1Gas ReconsD1E1- Recons D2E1 Pixel error";
    
      m_hCkvRich1GasResPixel = CurHistoSvc->book(m_RichG4HistoPathSet4+"1540",
                                           title, 1000,-0.005,0.005);

      title="Rich1Agel ReconsD1E3- Recons D2E3 Pixel error";
    
      m_hCkvRich1AgelResPixel = CurHistoSvc->book(m_RichG4HistoPathSet4+"1640",
                                           title, 1000,-0.005,0.005);

      title="Rich2Gas ReconsD1E1- Recons D2E1 Pixel error";
    
      m_hCkvRich2GasResPixel = CurHistoSvc->book(m_RichG4HistoPathSet4+"1740",
                                           title, 1000,-0.002,0.002);

      // now for any PSF type error.


      title="Rich1Gas ReconsD2E1- Recons D3E1 psf error";
    
      m_hCkvRich1GasResPsf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1550",
                                           title, 100,-0.005,0.005);
      title="Rich1Agel ReconsD2E3- Recons D3E3 psf error";
    
      m_hCkvRich1AgelResPsf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1650",
                                           title, 100,-0.005,0.005);

      title="Rich2Gas ReconsD2E1- Recons D3E1 psf error";
    
      m_hCkvRich2GasResPsf = CurHistoSvc->book(m_RichG4HistoPathSet4+"1750",
                                           title, 100,-0.001,0.001);

      //  total error


      title="Rich1Gas ReconsD1E4- generated ckv angle total error";
      m_hCkvRich1GasResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1560",
                                           title, 1000,-0.005,0.005);



      title="Rich1Gas ReconsD4E1- generated ckv angle  ";
      m_hCkvRich1GasResChr = CurHistoSvc->book(m_RichG4HistoPathSet4+"1562",
                                           title, 1000,-0.005,0.005);



      title="Rich1Agel ReconsD1E4- generated ckv angle total error";
      m_hCkvRich1AgelResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1660",
                                           title, 1000,-0.005,0.005);

      title="Rich2Gas ReconsD1E4- generated ckv angle total error";
      m_hCkvRich2GasResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1760",
                                           title, 1000,-0.005,0.005);

      title="Rich2Gas ReconsD4E1- generated ckv angle ";
      m_hCkvRich2GasResTotal = CurHistoSvc->book(m_RichG4HistoPathSet4+"1762",
                                           title, 1000,-0.005,0.005);

     

      title= "Focal distance for Rich1" ;
      m_hFocalDistR1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"2500",
                                         title,1000,1000.0,3000.0);
      title= "Focal distance for Rich2" ;
      m_hFocalDistR1 = CurHistoSvc->book(m_RichG4HistoPathSet4+"2600",
                                         title,1000,3500.0,5500.0);

      
}



//=============================================================================
