// $Id: RichG4HistoFillSet3.cpp,v 1.2 2004-02-10 14:24:11 jonesc Exp $
// Include files



// local
#include "RichG4HistoFillSet3.h"
#include "RichG4Counters.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// local
#include "../SensDet/RichG4Hit.h"
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
#include "RichG4SvcLocator.h"

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

void RichG4HistoFillSet3:: FillRichG4HistoSet3( const G4Event* anEvent,
                                                int NumRichColl,  
                                                const std::vector<int> & RichG4CollectionID )
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


  //    cout<<" NumPhot at Prod in C4F10  ="<< aNumPhotProdRich1Gas <<endl;

  //    cout<<"from C4f10 NumPhot at Prod Mirror1 Mirror2 GasQW HpdQW "
  //    <<"  BefQE AftQE elnSiDet "
  //    <<aNumPhotProdRich1Gas<<"   " <<aNumPhotGasOnRich1Mirror1
  //    <<"   "<<aNumPhotGasOnRich1Mirror2<<"   "
  //    <<aNumPhotGasOnGasQW<<"    "<< aNumPhotGasOnHpdQW<<"   "
  //    <<aNumPhotGasBeforeQE <<"   "<< aNumPhotGasAfterQE
  //    <<"  "<< aNumPeGasSiDet <<endl;

  //  cout<<"from Aerogel NumPhot at Prod Mirror1 Mirror2 "
  //  <<"  GasQW HpdQW BefQE AftQE elnSiDet "
  //    <<aNumPhotProdRich1Agel<<"   " <<aNumPhotAgelOnRich1Mirror1
  //    <<"   "<<aNumPhotAgelOnRich1Mirror2<<"   "
  //    <<aNumPhotAgelOnGasQW<<"    "<< aNumPhotAgelOnHpdQW<<"   "
  //    <<aNumPhotAgelBeforeQE <<"   "<< aNumPhotAgelAfterQE
  //    <<"  "<< aNumPeAgelSiDet <<endl;



  if(hNumPhotGasProdRich1)  hNumPhotGasProdRich1
                              ->fill(aNumPhotProdRich1Gas,1.0);
  if(hNumPhotGasMirror1Rich1)
    hNumPhotGasMirror1Rich1->fill( aNumPhotGasOnRich1Mirror1,1.0);
  if( hNumPhotGasMirror2Rich1)
    hNumPhotGasMirror1Rich1->fill( aNumPhotGasOnRich1Mirror2,1.0);
  if(  hNumPhotGasQWRich1)
    hNumPhotGasQWRich1->fill(  aNumPhotGasOnGasQW,1.0);
  if( hNumPhotGasHpdQWRich1)
    hNumPhotGasHpdQWRich1->fill( aNumPhotGasOnHpdQW,1.0);
  if( hNumPhotGasBeforeQERich1)
    hNumPhotGasBeforeQERich1->fill( aNumPhotGasBeforeQE,1.0);
  if( hNumPhotGasAfterQERich1)
    hNumPhotGasBeforeQERich1->fill(aNumPhotGasAfterQE);

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
    hNumPhotAgelMirror1Rich1->fill( aNumPhotAgelOnRich1Mirror2,1.0);
  if(  hNumPhotAgelQWRich1)
    hNumPhotAgelQWRich1->fill(  aNumPhotAgelOnGasQW,1.0);
  if( hNumPhotAgelHpdQWRich1)
    hNumPhotAgelHpdQWRich1->fill( aNumPhotAgelOnHpdQW,1.0);
  if( hNumPhotAgelBeforeQERich1)
    hNumPhotAgelBeforeQERich1->fill( aNumPhotAgelBeforeQE,1.0);
  if( hNumPhotAgelAfterQERich1)
    hNumPhotAgelBeforeQERich1->fill(aNumPhotAgelAfterQE);

  if( hNumPeAgelSiDetRich1 )
    hNumPeAgelSiDetRich1 ->fill(aNumPeAgelSiDet);



  //=============================================================================
}
