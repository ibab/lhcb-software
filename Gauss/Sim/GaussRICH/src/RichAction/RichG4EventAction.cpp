// ======================================================================
// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "RichG4EventAction.h"
#include "../SensDet/RichG4Hit.h"
//GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
// Histogramming



/** @file 
 *
 * implementation of class RichG4EventAction
 * 
 *  @author SE 19-8-2002.
 */

// =======================================================================
// Factory
// =======================================================================
IMPLEMENT_GiGaFactory( RichG4EventAction );
// =======================================================================

// ========================================================================
/** standard constructor 
 *  @see GiGaEventActionBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// =========================================================================
RichG4EventAction::RichG4EventAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaEventActionBase( type , name , parent ) ,
    m_RichG4HistoFillSet1(0), 
    m_RichG4HistoFillTimer(0),
  m_RichEventActionVerboseLevel(0), 
    m_RichEventActionHistoFillActivateSet1(false),
    m_RichEventActionHistoFillActivateTimer(false)
{
  declareProperty( "RichEventActionVerbose",  
                   m_RichEventActionVerboseLevel );
  
  declareProperty( "RichEventActionHistoFillSet1",
                     m_RichEventActionHistoFillActivateSet1);
  declareProperty( "RichEventActionHistoFillTimer",
                     m_RichEventActionHistoFillActivateTimer);
  

  m_RichHitCName= new RichG4HitCollName();
  m_NumRichColl=m_RichHitCName->RichHCSize();
  m_RichG4CollectionID.reserve(m_NumRichColl);
  for (int ic=0; ic<m_NumRichColl; ic++) {
    m_RichG4CollectionID.push_back(-1);
  }
  //  if(m_RichEventActionHistoFillActivateSet1) {
    
     m_RichG4HistoFillSet1 = new RichG4HistoFillSet1();
  
     //  }
     // if(m_RichEventActionHistoFillActivateTimer) {
    
     m_RichG4HistoFillTimer = new RichG4HistoFillTimer();
  
     //  }
  

};
// ============================================================================

// ============================================================================
/// Desctructor 
// ============================================================================
RichG4EventAction::~RichG4EventAction(){};
// ============================================================================

// ============================================================================
/// G4 
// ============================================================================
void RichG4EventAction::BeginOfEventAction ( const G4Event* aEvt /* event */ ) 
{
  if(m_RichEventActionHistoFillActivateTimer) {
    m_RichG4HistoFillTimer->RichG4BeginEventTimer();
    
  }
  
  G4SDManager * SDman = G4SDManager::GetSDMpointer();
  G4String colNam;  
  for (int icol=0; icol<m_NumRichColl; icol++) {
    if(m_RichG4CollectionID[icol]<0){
      colNam=  m_RichHitCName->RichHCName(icol);
      m_RichG4CollectionID[icol] = SDman->GetCollectionID(colNam);
    }
  }


  // Print("'BeginOfEventAction' method is invoked by RichG4EventAction"); 
};
// ============================================================================

// ============================================================================
// G4
// ============================================================================
void RichG4EventAction::EndOfEventAction( const G4Event* anEvent  /* event */ ) 
{

   if(m_RichEventActionHistoFillActivateTimer) {
   
    m_RichG4HistoFillTimer->RichG4EndEventTimer();
  
   }

   MsgStream log(   msgSvc(), name());

  G4int CurEventNum=anEvent->GetEventID();

   
  if( m_RichEventActionHistoFillActivateSet1) {
    
  //Now for the histo ids which are filled in this class.
    m_RichG4HistoFillSet1->FillRichG4HistoSet1(anEvent, 
                     m_NumRichColl,m_RichG4CollectionID);
  
  }
  
     //get the trajectories
  G4TrajectoryContainer* trajectoryContainer=anEvent->GetTrajectoryContainer();
  G4int n_trajectories=0;
  if(trajectoryContainer){n_trajectories=trajectoryContainer->entries();
  G4cout << "     " << n_trajectories
         << " Tracks are stored in Trajectorycontainer in event       " 
         <<CurEventNum << G4endl;
  }


  
  //get the hits by 
  // looping through the the hit collections

  G4HCofThisEvent * HCE;
  G4int nHitTotRich1=0;
  for (int ihcol=0; ihcol<m_NumRichColl; ihcol++) {
    if(m_RichG4CollectionID[ihcol] >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      RichG4HitsCollection* RHC=NULL;
      if(HCE){
        RHC = (RichG4HitsCollection*)(HCE->GetHC(m_RichG4CollectionID[ihcol]));
      }
      if(RHC){
        G4int nHitInCurColl = RHC->entries();
        G4cout<<"EndEvAction      "<< nHitInCurColl
              <<"   are stored in RichHitCollection set   "<<ihcol<< G4endl;
        if(ihcol == 0 || ihcol == 1 ) {
          nHitTotRich1 += nHitInCurColl;

        }

      }
    }
  }


  // The drawing of hits should be on a switch.
  // For this is switched off.
  G4bool DrawRichHits= false;
  G4bool PrintRichHits=false;
  if( m_RichEventActionVerboseLevel > 2 ) PrintRichHits=true;
  
  
  //Now loop through the hits
    
  for (int ihcold=0; ihcold<m_NumRichColl; ihcold++) {
    G4cout<<"Now drawing Rich hits for collection  "<<ihcold <<G4endl;
    if(m_RichG4CollectionID[ihcold] >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      RichG4HitsCollection* RHCD=NULL;
      if(HCE){
       RHCD = (RichG4HitsCollection*)(HCE->GetHC(m_RichG4CollectionID[ihcold]));
      }
      if(RHCD){
        G4int nHitHC = RHCD->entries();
        for (G4int ih=0; ih<nHitHC; ih++ ) {
          RichG4Hit* aHit = (*RHCD)[ih];
         //Now to draw the hits
          if( DrawRichHits) {
           aHit->DrawPEOrigin();
         }
          // Now to print the info of each hit.

          if( PrintRichHits) {
            
            aHit->Print();
          }
          
          
          }

      }
    }
  }
  
  
  
  
  
  // Print("'EndOfEventAction' method is invoked by RichG4EventAction");

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================




