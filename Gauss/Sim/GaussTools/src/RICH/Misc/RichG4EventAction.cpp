// ============================================================================
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
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"


/** @file 
 *
 * implementation of class RichG4EventAction
 * 
 *  @author SE 19-8-2002.
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( RichG4EventAction );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaEventActionBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
RichG4EventAction::RichG4EventAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaEventActionBase( type , name , parent ) 
{
  m_RichHitCName= new RichG4HitCollName();
  m_NumRichColl=m_RichHitCName->RichHCSize();
  m_RichG4CollectionID.reserve(m_NumRichColl);
  for (int ic=0; ic<m_NumRichColl; ic++) {
    m_RichG4CollectionID.push_back(-1);
  }



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
void RichG4EventAction::EndOfEventAction   ( const G4Event* anEvent  /* event */ ) 
{ 
  G4int CurEventNum=anEvent->GetEventID();
   
  //Now for the histo ids which are filled in this class.
  IHistogramSvc* CurrentHistoSvc;
  StatusCode sc = svcLoc()->service( "HistogramSvc", CurrentHistoSvc, true );

  SmartDataPtr<IHistogram1D>hNumTotHitRich1(CurrentHistoSvc,"RICHG4HISTO/1");
  SmartDataPtr<IHistogram2D>hGlobalPEOriginXY(CurrentHistoSvc,"RICHG4HISTO/2");
  SmartDataPtr<IHistogram2D>hGlobalHitXY(CurrentHistoSvc,"RICHG4HISTO/3");
  
  //test for filling
  //  hNumTotHitRich1->fill(200.0,1.0);
  //end of test
   
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
  if (hNumTotHitRich1) hNumTotHitRich1->fill(nHitTotRich1*1.0, 1.0) ;

  //Now to draw the hits
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
          aHit->DrawPEOrigin();
        }

      }
    }
  }
 
  // Now fill the hit coordinates into a histogram
  
  for (int ihcola=0; ihcola<m_NumRichColl; ihcola++) {
    G4cout<<"Now filling Rich1 hits into histogram  "<<ihcola <<G4endl;
    if(m_RichG4CollectionID[ihcola] >=0 ) {
      HCE = anEvent->GetHCofThisEvent();
      RichG4HitsCollection* RHCA=NULL;
      if(HCE){
        RHCA = (RichG4HitsCollection*)(HCE->GetHC(m_RichG4CollectionID[ihcola]));
      }
      if(RHCA){
        G4int nHitHCa = RHCA->entries();
        for (G4int iha=0; iha<nHitHCa; iha++ ) {
          RichG4Hit* aHit = (*RHCA)[iha];
          G4double aHitGlobalX=aHit->GetGlobalPos().x();
          G4double aHitGlobalY=aHit->GetGlobalPos().y();
          G4double aHitGlobalZ=aHit->GetGlobalPos().z();
          G4double aHitGlobalPEOriginX=aHit->GetGlobalPEOriginPos().x();
          G4double aHitGlobalPEOriginY=aHit->GetGlobalPEOriginPos().y();
          G4double aHitGlobalPEOriginZ=aHit->GetGlobalPEOriginPos().z();
          if(hGlobalPEOriginXY)hGlobalPEOriginXY->fill(aHitGlobalPEOriginX, 
                                                       aHitGlobalPEOriginY,1.0);
          if(hGlobalHitXY) hGlobalHitXY->fill( aHitGlobalX,aHitGlobalY,1.0);

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




