// ======================================================================
// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "RichG4EventAction.h"
#include "../SensDet/RichG4Hit.h"
#include "RichG4Counters.h"
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
    m_RichEventActionHistoFillActivateTimer(false),
    m_RichG4EventHitActivateCount(false)
{
  declareProperty( "RichEventActionVerbose",  
                   m_RichEventActionVerboseLevel );
  
  declareProperty( "RichEventActionHistoFillSet1",
                     m_RichEventActionHistoFillActivateSet1);
  declareProperty( "RichEventActionHistoFillTimer",
                     m_RichEventActionHistoFillActivateTimer);  
  declareProperty("RichG4EventActivateCounting" ,
                  m_RichG4EventHitActivateCount);

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
RichG4EventAction::~RichG4EventAction(){
  delete  m_RichG4HistoFillSet1;
  
  delete m_RichG4HistoFillTimer;
  

};
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

  // Now to initialize the Rich Counters

  RichG4Counters* aRichCounter=RichG4Counters::getInstance();
  aRichCounter->InitRichEventCounters();
  

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

  //   if( m_RichG4EventHitActivateCount) {
  
     //    m_RichG4EventHitCounter-> RichG4CountAndClassifyHits(
     //    anEvent, m_NumRichColl,m_RichG4CollectionID);

     //    if( m_RichEventActionVerboseLevel >= 2 ) {
        
     //  PrintRichG4HitCounters();
      
  //    }
    
    
  // }
   

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
void RichG4EventAction::PrintRichG4HitCounters() 
{

  RichG4Counters* aRichG4Counter =  RichG4Counters::getInstance();

  int NumTotHitRich1 = aRichG4Counter-> NumHitTotRich1All();
  int NumHitGasRich1 = aRichG4Counter-> NumHitTotRich1Gas();
  int NumHitAgelRich1= aRichG4Counter->  NumHitTotRich1Agel();
  int NumHitSatGasRich1 =
                    aRichG4Counter-> NumHitSaturatedRich1Gas();
  int NumHitSatAgelRich1 = 
                    aRichG4Counter->  NumHitSaturatedRich1Agel();
  
  int NumHitSinglePartGunPrimaryGasRich1 =
    aRichG4Counter->  NumHitPartGunPrimaryPartRich1Gas();
  
  int NumHitSinglePartGunPrimaryAgelRich1=
    aRichG4Counter-> NumHitPartGunPrimaryPartRich1Agel();
  

  cout<<" Rich1 Hits : Total Overall FromGas FromAgel "
      <<  NumTotHitRich1<<"   "<<  NumHitGasRich1
      << "    "<<  NumHitAgelRich1<<endl;
  
  cout<<"Rich1 Hits: SaturatedFromGas SaturatedFromAgel  "
      <<NumHitSatGasRich1<<"   "<<NumHitSatAgelRich1
      <<endl;
  
  cout<<"Rich1Hits: SinglePartGun FromPrimaryPartFromGas "
      <<"   FromPrimaryPartFromAerogel   "
      << NumHitSinglePartGunPrimaryGasRich1<<"     "
      << NumHitSinglePartGunPrimaryAgelRich1<<endl;

  
  int aNumPhotProdRich1Gas = aRichG4Counter->NumPhotProdRich1Gas() ;
  int aNumPhotGasOnRich1Mirror1 = 
    aRichG4Counter->NumPhotGasOnRich1Mirror1();
  int aNumPhotGasOnRich1Mirror2 =
     aRichG4Counter->NumPhotGasOnRich1Mirror2();
   int  aNumPhotGasOnGasQW =
      aRichG4Counter-> NumPhotGasOnRich1GasQW();
    int aNumPhotGasOnHpdQW  =
      aRichG4Counter-> NumPhotGasOnRich1HpdQW();
    
    int aNumPhotGasBeforeQE =
      aRichG4Counter-> NumPhotGasRich1BeforeQE();
    int aNumPhotGasAfterQE =
      aRichG4Counter->  NumPhotGasRich1AfterQE();

    int aNumPeGasSiDet = aRichG4Counter->NumPhotGasRich1SiDet();

    cout<<"Rich1Hits: FromC4F10 NumPhot at Prod Mirror1 Mirror2 "
        <<"  GasQW HpdQW BefQE AftQE elnSiDet "
        <<aNumPhotProdRich1Gas<<"   " <<aNumPhotGasOnRich1Mirror1
        <<"   "<<aNumPhotGasOnRich1Mirror2<<"   "
        <<aNumPhotGasOnGasQW<<"    "<< aNumPhotGasOnHpdQW<<"   "
        <<aNumPhotGasBeforeQE <<"   "<< aNumPhotGasAfterQE
        <<"  "<< aNumPeGasSiDet <<endl;



    int aNumPhotProdRich1Agel = 
       aRichG4Counter->NumPhotProdRich1Agel();

    int aNumPhotDownstrAgel =
      aRichG4Counter->NumPhotAgelAtAgelDownstrZ();
    
    int aNumPhotAgelOnRich1Mirror1 = 
         aRichG4Counter->NumPhotAgelOnRich1Mirror1();
    int aNumPhotAgelOnRich1Mirror2 =
         aRichG4Counter->NumPhotAgelOnRich1Mirror2();
   
    int aNumPhotAgelOnGasQW =
         aRichG4Counter-> NumPhotAgelOnRich1GasQW();
    int aNumPhotAgelOnHpdQW  =
        aRichG4Counter-> NumPhotAgelOnRich1HpdQW();
    
    int aNumPhotAgelBeforeQE =
        aRichG4Counter-> NumPhotAgelRich1BeforeQE();
    int aNumPhotAgelAfterQE =
        aRichG4Counter->  NumPhotAgelRich1AfterQE();

    int aNumPeAgelSiDet = 
       aRichG4Counter->NumPhotAgelRich1SiDet();

   
    cout<<"Rich1 Hits: fromAerogel NumPhot at Prod AgelDownstrZ "
        <<"   Mirror1 Mirror2 "
        <<"   GasQW HpdQW BefQE AftQE elnSiDet "
        <<aNumPhotProdRich1Agel<<"    "
        <<  aNumPhotDownstrAgel<<"    "
        <<aNumPhotAgelOnRich1Mirror1
        <<"   "<<aNumPhotAgelOnRich1Mirror2<<"   "
        <<aNumPhotAgelOnGasQW<<"    "<< aNumPhotAgelOnHpdQW<<"   "
        <<aNumPhotAgelBeforeQE <<"   "<< aNumPhotAgelAfterQE
        <<"  "<< aNumPeAgelSiDet <<endl;

  
}

// ============================================================================

// ============================================================================
// The END 
// ============================================================================




