// $Id: $
// Include files 

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "RichG4SvcLocator.h"


// local
#include "TorchTBG4FillHistoSet6.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBG4FillHistoSet6
//
// 2012-06-01 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBG4FillHistoSet6::TorchTBG4FillHistoSet6(  )
  : m_NumHitInCurEvTorchTB(0),
    m_NumHitInMcp0TorchTB(0),
    m_NumHitInMcp1TorchTB(0) { }
//=============================================================================
// Destructor
//=============================================================================
TorchTBG4FillHistoSet6::~TorchTBG4FillHistoSet6() {} 

//=============================================================================
void TorchTBG4FillHistoSet6::InitTorchTBHistoFillSet6() {
  
  m_NumHitInCurEvTorchTB=0;
  m_NumHitInMcp0TorchTB=0;
  m_NumHitInMcp1TorchTB=0;
  
}

void TorchTBG4FillHistoSet6::FillSet6TorchTBMcpOccupancy() {
  
   IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
   SmartDataPtr<IHistogram1D>  hNumTotHitInTestBeam (CurrentHistoSvc,"TORCHTBG4HISTOSET6/1001");
   SmartDataPtr<IHistogram1D> hNumTotHitMcp0 (CurrentHistoSvc,"TORCHTBG4HISTOSET6/1002");
   SmartDataPtr<IHistogram1D> hNumTotHitMcp1 (CurrentHistoSvc,"TORCHTBG4HISTOSET6/1003");

   if((m_NumHitInCurEvTorchTB > 0) && hNumTotHitInTestBeam ) hNumTotHitInTestBeam->fill(m_NumHitInCurEvTorchTB*1.0);
   if(( m_NumHitInMcp0TorchTB >0)  &&  hNumTotHitMcp0 ) hNumTotHitMcp0->fill(m_NumHitInMcp0TorchTB*1.0);
   if((m_NumHitInMcp1TorchTB >0) && hNumTotHitMcp1 ) hNumTotHitMcp1->fill(m_NumHitInMcp1TorchTB*1.0);
   


}

void TorchTBG4FillHistoSet6::FillSet6TorchTBHitCoord(CkvG4Hit* adHit ) {
   IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  
  SmartDataPtr<IHistogram1D> hHitZLocationMcp0(CurrentHistoSvc,"TORCHTBG4HISTOSET6/1010");

  const G4ThreeVector aGlobalPos = adHit->GetGlobalPos();
  
  int CurrentMcpNum= adHit->CurPmtNum();

  if( CurrentMcpNum == 0 ) {

    if(hHitZLocationMcp0)  hHitZLocationMcp0->fill(aGlobalPos.z());

      m_NumHitInMcp0TorchTB++;
    
    
  }else if (CurrentMcpNum == 1 ) {
    
    m_NumHitInMcp1TorchTB++;
    
  }
  if( (CurrentMcpNum == 0) || (CurrentMcpNum == 1) ) {
    
    m_NumHitInCurEvTorchTB++;
    
  }
  
  
}

