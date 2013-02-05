// $Id: RichG4HistoFillSet3.cpp,v 1.8 2009-07-03 11:59:49 seaso Exp $
// Include files



// local
#include "GaussRICH/RichG4HistoFillSet3.h"
#include "GaussRICH/RichG4Counters.h"
// local
#include "GaussRICH/RichG4Hit.h"
//GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaussRICH/RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoFillSet1
//
// 2003-05-20 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

RichG4HistoFillSet3::RichG4HistoFillSet3(  ) {

}
RichG4HistoFillSet3::~RichG4HistoFillSet3(  ) {
}

void RichG4HistoFillSet3:: FillRichG4HistoSet3()
{



  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  // the following line does nto work since this is not a gaudi algorithm
  // and hence svcLoc is not automatically defined.
  // Hence the line above is used instead.

  // StatusCode sc = svcLoc()->service( "HistogramSvc", CurrentHistoSvc, true );


  RichG4Counters* aRichCounter = RichG4Counters::getInstance();

  SmartDataPtr<IHistogram1D> hNumPhotGasProdRich1 (CurrentHistoSvc,
                                                   "RICHG4HISTOSET3/300");
  SmartDataPtr<IHistogram1D> hNumPhotGasMirror1Rich1 (CurrentHistoSvc,
                                                      "RICHG4HISTOSET3/320");
  SmartDataPtr<IHistogram1D> hNumPhotGasMirror2Rich1 (CurrentHistoSvc,
                                                      "RICHG4HISTOSET3/340");
  SmartDataPtr<IHistogram1D> hNumPhotGasQWRich1 (CurrentHistoSvc,
                                                 "RICHG4HISTOSET3/360");
  SmartDataPtr<IHistogram1D> hNumPhotGasHpdQWRich1 (CurrentHistoSvc,
                                                    "RICHG4HISTOSET3/380");
  SmartDataPtr<IHistogram1D> hNumPhotGasBeforeQERich1 (CurrentHistoSvc,
                                                       "RICHG4HISTOSET3/400");
  SmartDataPtr<IHistogram1D>  hNumPhotGasAfterQERich1 (CurrentHistoSvc,
                                                       "RICHG4HISTOSET3/420");
  SmartDataPtr<IHistogram1D>  hNumPeGasSiDetRich1 (CurrentHistoSvc,
                                                   "RICHG4HISTOSET3/440");


  SmartDataPtr<IHistogram1D> hNumPhotAgelProdRich1 (CurrentHistoSvc,
                                                    "RICHG4HISTOSET3/500");
  SmartDataPtr<IHistogram1D> hNumPhotAgelDownstrAgelRich1 (CurrentHistoSvc,
                                                           "RICHG4HISTOSET3/510");
  SmartDataPtr<IHistogram1D> hNumPhotAgelMirror1Rich1 (CurrentHistoSvc,
                                                       "RICHG4HISTOSET3/520");
  SmartDataPtr<IHistogram1D> hNumPhotAgelMirror2Rich1 (CurrentHistoSvc,
                                                       "RICHG4HISTOSET3/540");
  SmartDataPtr<IHistogram1D> hNumPhotAgelQWRich1 (CurrentHistoSvc,
                                                  "RICHG4HISTOSET3/560");
  SmartDataPtr<IHistogram1D> hNumPhotAgelHpdQWRich1 (CurrentHistoSvc,
                                                     "RICHG4HISTOSET3/580");
  SmartDataPtr<IHistogram1D> hNumPhotAgelBeforeQERich1 (CurrentHistoSvc,
                                                        "RICHG4HISTOSET3/600");
  SmartDataPtr<IHistogram1D>  hNumPhotAgelAfterQERich1 (CurrentHistoSvc,
                                                        "RICHG4HISTOSET3/620");
  SmartDataPtr<IHistogram1D>  hNumPeAgelSiDetRich1 (CurrentHistoSvc,
                                                    "RICHG4HISTOSET3/640");
  SmartDataPtr<IHistogram1D> hNumAgelRadiator (CurrentHistoSvc,
                                                   "RICHG4HISTOSET3/710");

  SmartDataPtr<IHistogram1D> hNumc4f10Radiator(CurrentHistoSvc,
                                                   "RICHG4HISTOSET3/711");

  SmartDataPtr<IHistogram1D> hNumcf4Radiator(CurrentHistoSvc, "RICHG4HISTOSET3/712");

  SmartDataPtr<IHistogram1D> hNumfiltergenericRadiator(CurrentHistoSvc,"RICHG4HISTOSET3/714");
  SmartDataPtr<IHistogram1D> hNumfilterd263Radiator(CurrentHistoSvc,"RICHG4HISTOSET3/715");
  SmartDataPtr<IHistogram1D> hNumRich1GasqwRadiator(CurrentHistoSvc, "RICHG4HISTOSET3/716");
  SmartDataPtr<IHistogram1D> hNumRich2GasqwRadiator(CurrentHistoSvc, "RICHG4HISTOSET3/717");
  SmartDataPtr<IHistogram1D> hNumRich1HpdqwRadiator(CurrentHistoSvc,"RICHG4HISTOSET3/718");
  SmartDataPtr<IHistogram1D>  hNumRich2HpdqwRadiator(CurrentHistoSvc,"RICHG4HISTOSET3/719");

  SmartDataPtr<IHistogram1D> hNumRich1HpdNoRefl(CurrentHistoSvc,"RICHG4HISTOSET3/808");
  SmartDataPtr<IHistogram1D> hNumRich1HpdNoReflSmall(CurrentHistoSvc,"RICHG4HISTOSET3/809");
  SmartDataPtr<IHistogram1D> hNumRich2HpdNoRefl(CurrentHistoSvc,"RICHG4HISTOSET3/848");
  SmartDataPtr<IHistogram1D>  hNumRich2HpdNoReflSmall(CurrentHistoSvc,"RICHG4HISTOSET3/849");

  SmartDataPtr<IHistogram1D> hNumRich1HpdRefl(CurrentHistoSvc,"RICHG4HISTOSET3/810");
  SmartDataPtr<IHistogram1D> hNumRich1HpdReflSmall(CurrentHistoSvc,"RICHG4HISTOSET3/811");
  SmartDataPtr<IHistogram1D> hNumRich2HpdRefl(CurrentHistoSvc,"RICHG4HISTOSET3/850");
  SmartDataPtr<IHistogram1D>  hNumRich2HpdReflSmall(CurrentHistoSvc,"RICHG4HISTOSET3/851");

  SmartDataPtr<IHistogram1D>  hNumRich1GasHpdRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/815");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdReflSmall (CurrentHistoSvc,"RICHG4HISTOSET3/816");
  SmartDataPtr<IHistogram1D>  hNumRich1GasHpdBackScat   (CurrentHistoSvc,"RICHG4HISTOSET3/820");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdQWPCRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/830");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdQWPCReflSmall (CurrentHistoSvc,"RICHG4HISTOSET3/831");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdChromiumRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/840");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdSiliconRefl   (CurrentHistoSvc,"RICHG4HISTOSET3/845");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdKovarRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/844");
  SmartDataPtr<IHistogram1D> hNumRich1GasHpdKaptonRefl   (CurrentHistoSvc,"RICHG4HISTOSET3/846");

  SmartDataPtr<IHistogram1D> hNumRich1AerogelHpdKaptonRefl   (CurrentHistoSvc,"RICHG4HISTOSET3/817");

  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/855");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdReflSmall (CurrentHistoSvc,"RICHG4HISTOSET3/856");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdBackScat   (CurrentHistoSvc,"RICHG4HISTOSET3/860");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdQWPCRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/880");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdQWPCReflSmall (CurrentHistoSvc,"RICHG4HISTOSET3/881");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdChromiumRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/890");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdSiliconRefl   (CurrentHistoSvc,"RICHG4HISTOSET3/895");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdKovarRefl  (CurrentHistoSvc,"RICHG4HISTOSET3/894");
  SmartDataPtr<IHistogram1D>  hNumRich2GasHpdKaptonRefl   (CurrentHistoSvc,"RICHG4HISTOSET3/896");


  double aNumPhotProdRich1Gas = (aRichCounter->NumPhotProdRich1Gas())* 1.0 ;
  double aNumPhotGasOnRich1Mirror1 =
    (aRichCounter->NumPhotGasOnRich1Mirror1())*1.0;
  double aNumPhotGasOnRich1Mirror2 =
    (aRichCounter->NumPhotGasOnRich1Mirror2())*1.0;

  double aNumPhotGasOnGasQW =
    (aRichCounter-> NumPhotGasOnRich1GasQW())*1.0;
  double aNumPhotGasOnHpdQW  =
    (aRichCounter-> NumPhotGasOnRich1HpdQW())*1.0;

  double aNumPhotGasBeforeQE =
    (aRichCounter-> NumPhotGasRich1BeforeQE())*1.0;
  double aNumPhotGasAfterQE =
    (aRichCounter->  NumPhotGasRich1AfterQE())*1.0;

  double aNumPeGasSiDet =
    (aRichCounter->NumPhotGasRich1SiDet())*1.0;


  double aNumPhotProdRich1Agel =
    (aRichCounter->NumPhotProdRich1Agel())* 1.0 ;

  double aNumPhotAgelOnDownstrAgel =
    (aRichCounter->NumPhotAgelAtAgelDownstrZ())*1.0;

  double aNumPhotAgelOnRich1Mirror1 =
    (aRichCounter->NumPhotAgelOnRich1Mirror1())*1.0;
  double aNumPhotAgelOnRich1Mirror2 =
    (aRichCounter->NumPhotAgelOnRich1Mirror2())*1.0;

  double aNumPhotAgelOnGasQW =
    (aRichCounter-> NumPhotAgelOnRich1GasQW())*1.0;
  double aNumPhotAgelOnHpdQW  =
    (aRichCounter-> NumPhotAgelOnRich1HpdQW())*1.0;

  double aNumPhotAgelBeforeQE =
    (aRichCounter-> NumPhotAgelRich1BeforeQE())*1.0;
  double aNumPhotAgelAfterQE =
    (aRichCounter->  NumPhotAgelRich1AfterQE())*1.0;

  double aNumPeAgelSiDet =
    (aRichCounter->NumPhotAgelRich1SiDet())*1.0;

  double aNumRadc4f10= (aRichCounter->NumHitTotRich1Gas())*1.0;
  double aNumRadcf4 = (aRichCounter->NumHitTotRich2Gas())*1.0;
  double aNumRadagel = (aRichCounter->NumHitTotRich1Agel() )*1.0;
  double aNumRadr1gasqw = (aRichCounter->NumHitTotRich1GasQw() )*1.0;
  double aNumRadr2gasqw = (aRichCounter->NumHitTotRich2GasQw() )*1.0;
  double aNumRadr1hpdqw = (aRichCounter->NumHitTotRich1HpdQw() )*1.0;
  double aNumRadr2hpdqw = (aRichCounter->NumHitTotRich2HpdQw() )*1.0;
  double aNumRadr1Filterd263 = (aRichCounter->NumHitTotRich1FilterD263() )*1.0;
  double aNumRadr1FilterGeneric = (aRichCounter->NumHitTotRich1FilterGeneric())*1.0;

  double aNumHitTotRich1HpdRefl = (aRichCounter->NumHitTotRich1HpdRefl())*1.0;
  double aNumHitTotRich2HpdRefl = (aRichCounter->NumHitTotRich2HpdRefl())*1.0;

  double aNumHitTotRich1HpdNoRefl = (aRichCounter->NumHitTotRich1All())*1.0-aNumHitTotRich1HpdRefl;
  double aNumHitTotRich2HpdNoRefl = (aRichCounter->NumHitTotRich2All())*1.0-aNumHitTotRich2HpdRefl;


  double aNumHitTotRich1GasHpdRefl = (aRichCounter->NumHitTotRich1GasHpdRefl())*1.0;
  double aNumHitTotRich1GasHpdBackScat =(aRichCounter->NumHitTotRich1GasHpdBackScat())*1.0;
  double aNumHitTotRich1GasHpdQWPCRefl = (aRichCounter->NumHitTotRich1GasHpdQWPCRefl())*1.0;
  double aNumHitTotRich1GasHpdChromRefl = (aRichCounter->NumHitTotRich1GasHpdChromRefl())*1.0;
  double aNumHitTotRich1GasHpdSiliconRefl= (aRichCounter->NumHitTotRich1GasHpdSiliconRefl())*1.0;
  double aNumHitTotRich1GasHpdKovarRefl= (aRichCounter->NumHitTotRich1GasHpdKovarRefl())*1.0;
  double aNumHitTotRich1GasHpdKaptonRefl= (aRichCounter->NumHitTotRich1GasHpdKaptonRefl())*1.0;

  double aNumHitTotRich1AerogelHpdKaptonRefl= (aRichCounter->NumHitTotRich1AerogelHpdKaptonRefl())*1.0;

  double aNumHitTotRich2GasHpdRefl = (aRichCounter->NumHitTotRich2GasHpdRefl())*1.0;
  double aNumHitTotRich2GasHpdBackScat =(aRichCounter->NumHitTotRich2GasHpdBackScat())*1.0;
  double aNumHitTotRich2GasHpdQWPCRefl = (aRichCounter->NumHitTotRich2GasHpdQWPCRefl())*1.0;
  double aNumHitTotRich2GasHpdChromRefl = (aRichCounter->NumHitTotRich2GasHpdChromRefl())*1.0;
  double aNumHitTotRich2GasHpdSiliconRefl= (aRichCounter->NumHitTotRich2GasHpdSiliconRefl())*1.0;
  double aNumHitTotRich2GasHpdKovarRefl= (aRichCounter->NumHitTotRich2GasHpdKovarRefl())*1.0;
  double aNumHitTotRich2GasHpdKaptonRefl= (aRichCounter->NumHitTotRich2GasHpdKaptonRefl())*1.0;


  //   std::cout<<" NumPhot at Prod in C4F10  ="<< aNumPhotProdRich1Gas 
  //             <<std::endl;

  //    std::cout<<"from C4f10 NumPhot at Prod Mirror1 Mirror2 GasQW HpdQW "
  //    <<"  BefQE AftQE elnSiDet "
  //    <<aNumPhotProdRich1Gas<<"   " <<aNumPhotGasOnRich1Mirror1
  //    <<"   "<<aNumPhotGasOnRich1Mirror2<<"   "
  //     <<aNumPhotGasOnGasQW<<"    "<< aNumPhotGasOnHpdQW<<"   "
  //    <<aNumPhotGasBeforeQE <<"   "<< aNumPhotGasAfterQE
  //    <<"  "<< aNumPeGasSiDet <<std::endl;
  //
  //  std::cout<<"from Aerogel NumPhot at Prod Mirror1 Mirror2 "
  //  <<"  GasQW HpdQW BefQE AftQE elnSiDet "
  //    <<aNumPhotProdRich1Agel<<"   " <<aNumPhotAgelOnRich1Mirror1
  //    <<"   "<<aNumPhotAgelOnRich1Mirror2<<"   "
  //    <<aNumPhotAgelOnGasQW<<"    "<< aNumPhotAgelOnHpdQW<<"   "
  //    <<aNumPhotAgelBeforeQE <<"   "<< aNumPhotAgelAfterQE
  //    <<"  "<< aNumPeAgelSiDet <<std::endl;



  if(hNumPhotGasProdRich1)  hNumPhotGasProdRich1
                              ->fill(aNumPhotProdRich1Gas,1.0);
  if(hNumPhotGasMirror1Rich1)
    hNumPhotGasMirror1Rich1->fill( aNumPhotGasOnRich1Mirror1,1.0);
  if( hNumPhotGasMirror2Rich1)
    hNumPhotGasMirror2Rich1->fill( aNumPhotGasOnRich1Mirror2,1.0);
  if(  hNumPhotGasQWRich1)
    hNumPhotGasQWRich1->fill(  aNumPhotGasOnGasQW,1.0);
  if( hNumPhotGasHpdQWRich1)
    hNumPhotGasHpdQWRich1->fill( aNumPhotGasOnHpdQW,1.0);
  if( hNumPhotGasBeforeQERich1)
    hNumPhotGasBeforeQERich1->fill( aNumPhotGasBeforeQE,1.0);
  if( hNumPhotGasAfterQERich1)
    hNumPhotGasAfterQERich1->fill(aNumPhotGasAfterQE);

  if( hNumPeGasSiDetRich1 )
    hNumPeGasSiDetRich1 ->fill(aNumPeGasSiDet);

  if(hNumPhotAgelProdRich1)  hNumPhotAgelProdRich1
                               ->fill(aNumPhotProdRich1Agel,1.0);

  if(hNumPhotAgelDownstrAgelRich1)
    hNumPhotAgelDownstrAgelRich1->
      fill(aNumPhotAgelOnDownstrAgel,1.0);

  if(hNumPhotAgelMirror1Rich1)
    hNumPhotAgelMirror1Rich1->fill( aNumPhotAgelOnRich1Mirror1,1.0);
  if( hNumPhotAgelMirror2Rich1)
    hNumPhotAgelMirror2Rich1->fill( aNumPhotAgelOnRich1Mirror2,1.0);
  if(  hNumPhotAgelQWRich1)
    hNumPhotAgelQWRich1->fill(  aNumPhotAgelOnGasQW,1.0);
  if( hNumPhotAgelHpdQWRich1)
    hNumPhotAgelHpdQWRich1->fill( aNumPhotAgelOnHpdQW,1.0);
  if( hNumPhotAgelBeforeQERich1)
    hNumPhotAgelBeforeQERich1->fill( aNumPhotAgelBeforeQE,1.0);
  if( hNumPhotAgelAfterQERich1)
    hNumPhotAgelAfterQERich1->fill(aNumPhotAgelAfterQE);

  if( hNumPeAgelSiDetRich1 )
    hNumPeAgelSiDetRich1 ->fill(aNumPeAgelSiDet);

  if( hNumAgelRadiator ) 
    hNumAgelRadiator->fill(aNumRadagel);
  if(hNumc4f10Radiator)
    hNumc4f10Radiator->fill(aNumRadc4f10);
  if(hNumcf4Radiator)
    hNumcf4Radiator->fill(aNumRadcf4);
  if(hNumfiltergenericRadiator)
    hNumfiltergenericRadiator->fill(aNumRadr1FilterGeneric);
  if(hNumfilterd263Radiator)
    hNumfilterd263Radiator->fill(aNumRadr1Filterd263);
  if(hNumRich1GasqwRadiator)
    hNumRich1GasqwRadiator->fill(aNumRadr1gasqw);
  if(hNumRich2GasqwRadiator)
    hNumRich2GasqwRadiator->fill(aNumRadr2gasqw);
  if(hNumRich1HpdqwRadiator)
    hNumRich1HpdqwRadiator->fill(aNumRadr1hpdqw);
  if(hNumRich2HpdqwRadiator)
    hNumRich2HpdqwRadiator->fill(aNumRadr2hpdqw);


  if( hNumRich1HpdNoRefl) hNumRich1HpdNoRefl->fill(aNumHitTotRich1HpdNoRefl );
  if( hNumRich2HpdNoRefl) hNumRich2HpdNoRefl->fill(aNumHitTotRich2HpdNoRefl );
  if( hNumRich1HpdNoReflSmall) hNumRich1HpdNoReflSmall->fill(aNumHitTotRich1HpdNoRefl);
  if (hNumRich2HpdNoReflSmall) hNumRich2HpdNoReflSmall->fill(aNumHitTotRich2HpdNoRefl);


  if( hNumRich1HpdRefl) hNumRich1HpdRefl->fill(aNumHitTotRich1HpdRefl );
  if( hNumRich2HpdRefl) hNumRich2HpdRefl->fill(aNumHitTotRich2HpdRefl );
  if( hNumRich1HpdReflSmall) hNumRich1HpdReflSmall->fill(aNumHitTotRich1HpdRefl);
  if (hNumRich2HpdReflSmall) hNumRich2HpdReflSmall->fill(aNumHitTotRich2HpdRefl);

  if (hNumRich1GasHpdRefl) hNumRich1GasHpdRefl->fill(aNumHitTotRich1GasHpdRefl);
  if (hNumRich1GasHpdReflSmall) hNumRich1GasHpdReflSmall->fill(aNumHitTotRich1GasHpdRefl);
  if (hNumRich1GasHpdBackScat) hNumRich1GasHpdBackScat->fill(aNumHitTotRich1GasHpdBackScat);
  if (hNumRich1GasHpdQWPCRefl) hNumRich1GasHpdQWPCRefl ->fill(aNumHitTotRich1GasHpdQWPCRefl);
  if (hNumRich1GasHpdQWPCReflSmall) hNumRich1GasHpdQWPCReflSmall->fill(aNumHitTotRich1GasHpdQWPCRefl);
  if (hNumRich1GasHpdChromiumRefl) hNumRich1GasHpdChromiumRefl->fill(aNumHitTotRich1GasHpdChromRefl);
  if (hNumRich1GasHpdSiliconRefl) hNumRich1GasHpdSiliconRefl->fill(aNumHitTotRich1GasHpdSiliconRefl);
  if (hNumRich1GasHpdKovarRefl) hNumRich1GasHpdKovarRefl->fill(aNumHitTotRich1GasHpdKovarRefl);
  if (hNumRich1GasHpdKaptonRefl) hNumRich1GasHpdKaptonRefl->fill(aNumHitTotRich1GasHpdKaptonRefl);

  if (hNumRich1AerogelHpdKaptonRefl) hNumRich1AerogelHpdKaptonRefl->fill(aNumHitTotRich1AerogelHpdKaptonRefl);

  if (hNumRich2GasHpdRefl) hNumRich2GasHpdRefl->fill(aNumHitTotRich2GasHpdRefl);
  if (hNumRich2GasHpdReflSmall) hNumRich2GasHpdReflSmall->fill(aNumHitTotRich2GasHpdRefl);
  if (hNumRich2GasHpdBackScat) hNumRich2GasHpdBackScat->fill(aNumHitTotRich2GasHpdBackScat);
  if (hNumRich2GasHpdQWPCRefl) hNumRich2GasHpdQWPCRefl ->fill(aNumHitTotRich2GasHpdQWPCRefl);
  if (hNumRich2GasHpdQWPCReflSmall) hNumRich2GasHpdQWPCReflSmall->fill(aNumHitTotRich2GasHpdQWPCRefl);
  if (hNumRich2GasHpdChromiumRefl) hNumRich2GasHpdChromiumRefl->fill(aNumHitTotRich2GasHpdChromRefl);
  if (hNumRich2GasHpdSiliconRefl) hNumRich2GasHpdSiliconRefl->fill(aNumHitTotRich2GasHpdSiliconRefl);
  if (hNumRich2GasHpdKovarRefl) hNumRich2GasHpdKovarRefl->fill(aNumHitTotRich2GasHpdKovarRefl);
  if (hNumRich2GasHpdKaptonRefl) hNumRich2GasHpdKaptonRefl->fill(aNumHitTotRich2GasHpdKaptonRefl);


  //=============================================================================
}
