// $Id: $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 


// local
#include "TorchTBEventAction.h"
#include "CkvG4Hit.h"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TorchTBEventAction );

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBEventAction
//
// 2012-06-01 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBEventAction::TorchTBEventAction(const std::string& type,
                                      const std::string& name,
                                       const IInterface*  parent)
 :GiGaEventActionBase( type , name , parent ),
 m_TorchTBCollName(0),
 m_TorchTBG4CollectionID(-1),
 m_TorchTBG4FillHistoSet6(0),
 m_TorchTBHitAnalysis(0),
 m_ActivateTorchTBHistoFill6(false)   {

  declareProperty("ActivateTorchTBHistoFill6",m_ActivateTorchTBHistoFill6);
  m_TorchTBCollName= new TorchTBHitCollName();
  
}
//=============================================================================
// Destructor
//=============================================================================
TorchTBEventAction::~TorchTBEventAction() {
  delPointer(m_TorchTBCollName);
  delPointer(m_TorchTBG4FillHistoSet6);
  delPointer(m_TorchTBHitAnalysis);
} 

//=============================================================================
StatusCode TorchTBEventAction::initialize() 
{
 StatusCode sc = GiGaEventActionBase::initialize();
  if (sc.isFailure()) return sc;

  m_TorchTBHitAnalysis = new TorchTBHitAnalysis();
  
   if(m_ActivateTorchTBHistoFill6) {
  
     m_TorchTBG4FillHistoSet6= new TorchTBG4FillHistoSet6();
   }
   
  return sc;
  
}
StatusCode TorchTBEventAction::finalize()
{
 return GiGaEventActionBase::finalize(); 
}

void TorchTBEventAction::BeginOfEventAction ( const G4Event* /* aEvt */ )
{
    G4SDManager * SDman = G4SDManager::GetSDMpointer();
   G4String colNam = m_TorchTBCollName->TorchTBHCName();
   m_TorchTBG4CollectionID=SDman->GetCollectionID(colNam);

   if(m_ActivateTorchTBHistoFill6) {

     m_TorchTBG4FillHistoSet6->InitTorchTBHistoFillSet6();
     m_TorchTBHitAnalysis->setTorchTBHisto(m_TorchTBG4FillHistoSet6);
     
     
   }
 
  
}
void TorchTBEventAction::EndOfEventAction( const G4Event* anEvent  /* event */ )
{

  //  G4int CurEventNum=anEvent->GetEventID();
  
   if(m_ActivateTorchTBHistoFill6) {

     m_TorchTBHitAnalysis->CountNumHits(anEvent, m_TorchTBG4CollectionID );
     m_TorchTBG4FillHistoSet6->FillSet6TorchTBMcpOccupancy();
     
   }
   
   
}

