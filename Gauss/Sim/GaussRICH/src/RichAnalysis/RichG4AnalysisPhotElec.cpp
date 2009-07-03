// $Id: RichG4AnalysisPhotElec.cpp,v 1.4 2009-07-03 11:59:49 seaso Exp $
// Include files



// local
#include "RichG4AnalysisConstGauss.h"
#include "RichG4AnalysisPhotElec.h"
#include "RichG4Counters.h"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4PionMinus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"


/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

#include <math.h>
 

//-----------------------------------------------------------------------------
//
// 2003-06-08 : Sajan EASO
//-----------------------------------------------------------------------------
void RichG4AnalysisPhotElecA ( const G4Step& aStep,
                               G4int currentRichDetNumber)
{
  // Histogram and analysis from before the QE.


  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();

  if(currentRichDetNumber == 0) {

    const G4Track* bTrack= aStep.GetTrack();
    const G4DynamicParticle* bParticle=bTrack->GetDynamicParticle();
    const G4double  bParticleKE= bParticle->GetKineticEnergy();
    if(    bParticleKE > 0.0 ) {

      G4int aRadiatorNumber = -1;

     G4VUserTrackInformation* aUserTrackInfo=bTrack->GetUserInformation();
     GaussTrackInformation* aRichPhotTrackInfo
        = (GaussTrackInformation*)aUserTrackInfo;
    
   if( aRichPhotTrackInfo) {
     if( aRichPhotTrackInfo->detInfo() ){
 
       RichInfo* aRichTypeInfo =
        ( RichInfo*) (aRichPhotTrackInfo->detInfo());
       if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() ){
         RichPhotInfo* aRichPhotInfo =
                    aRichTypeInfo-> RichPhotInformation();
         if( aRichPhotInfo ) {
           
	         aRadiatorNumber = aRichPhotInfo->PhotProdRadiatorNum() ;
           if(  aRichCounter ) {

              if(    aRadiatorNumber == 1 ) {
                aRichCounter->bumpNumPhotGasRich1BeforeQE();              
	      }else if ( aRadiatorNumber >=10 && aRadiatorNumber <= 25 ) {
                aRichCounter->bumpNumPhotAgelRich1BeforeQE();
              } 

              } 

	 }}}}}}

}

void RichG4AnalysisPhotElecB ( const G4Step& aStep,
                               G4int currentRichDetNumber)
{
  // Histogram and analysis from after the QE.


  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();


  if(currentRichDetNumber == 0) {

    const G4Track* bTrack= aStep.GetTrack();
    const G4DynamicParticle* bParticle=bTrack->GetDynamicParticle();
    const G4double  bParticleKE= bParticle->GetKineticEnergy();
    if(    bParticleKE > 0.0 ) {

      G4int aRadiatorNumber = -1;


     G4VUserTrackInformation* aUserTrackInfo=bTrack->GetUserInformation();
     GaussTrackInformation* aRichPhotTrackInfo
        = (GaussTrackInformation*)aUserTrackInfo;
    
   if( aRichPhotTrackInfo) {
     if( aRichPhotTrackInfo->detInfo() ){
 
       RichInfo* aRichTypeInfo =
        ( RichInfo*) (aRichPhotTrackInfo->detInfo());
       if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() ){
         RichPhotInfo* aRichPhotInfo =
                    aRichTypeInfo-> RichPhotInformation();
         if( aRichPhotInfo ) {
           
	         aRadiatorNumber = aRichPhotInfo->PhotProdRadiatorNum() ;
           if(  aRichCounter ) {

              if(    aRadiatorNumber == 1 ) {
                aRichCounter->bumpNumPhotGasRich1AfterQE();              
	      }else if ( aRadiatorNumber >=10 && aRadiatorNumber <= 25 ) {
                aRichCounter->bumpNumPhotAgelRich1AfterQE();
              } 

              } 

	 }}}}}}

}


//=============================================================================
