// $Id: $
// Include files 
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CkvG4SvcLocator.h"



// local
#include "TorchTBHitAnalysis.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBHitAnalysis
//
// 2012-06-01 : Sajan Easo
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBHitAnalysis::TorchTBHitAnalysis(  ) {

}
//=============================================================================
// Destructor
//=============================================================================
TorchTBHitAnalysis::~TorchTBHitAnalysis() {} 

//=============================================================================

void TorchTBHitAnalysis::CountNumHits(const G4Event* anEvent, const int TorchTBG4CollectionID) {
  
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream TorchTBReconlog( msgSvc,"TorchTBHitAnalysis");
  G4HCofThisEvent * HCE;
  int Current_RichG4CollectionID=TorchTBG4CollectionID;
   if(Current_RichG4CollectionID >=0 ) {

      HCE = anEvent->GetHCofThisEvent();
      CkvG4HitsCollection* RHC=NULL;
     if(HCE){
        RHC = (CkvG4HitsCollection*)(HCE->GetHC( Current_RichG4CollectionID));
     }
     if(RHC) {

         G4int nHitInCurColl = RHC->entries();
         for (G4int iha=0; iha<nHitInCurColl ; iha++ ) { // loop through the hits
          CkvG4Hit* aHit = (*RHC)[iha];
          
          //     const G4ThreeVector & GlobalPhcathCoord =   aHit->GetGlobalPEOriginPos();
          // const G4ThreeVector & GlobalXYZ =   aHit->GetGlobalPos();
          m_TorchTBHisto->    FillSet6TorchTBHitCoord(aHit);
          
         }
         
     }
     
   }
   
}
