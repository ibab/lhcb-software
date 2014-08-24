// Include files


//-----------------------------------------------------------------------------
// Implementation file for class : CkvStdSensDet
//
// 2014-08-19 : Sajan Easo
//-----------------------------------------------------------------------------
// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/StatusCode.h"
// GiGa
#include "GiGa/GiGaBase.h"
// Geant4
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"


#include "GaussCherenkov/CkvStdSensDet.h"
#include "GaussCherenkov/CkvCommonSensDet.h"

using namespace std;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CkvStdSensDet );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvStdSensDet::CkvStdSensDet(const std::string& type ,
                             const std::string& name   ,
                             const IInterface*  parent)
  : G4VSensitiveDetector ( name  ),  
    GiGaSensDetBase      ( type , name , parent ),
    m_RichPmtAviodDuplicateHitsActivate(false),
    m_RichPmtFlagDuplicateHitsActivate(false)
{
  declareProperty("RichPmtAviodDuplicateHitsActivate", m_RichPmtAviodDuplicateHitsActivate);
  declareProperty("RichPmtFlagDuplicateHitsactivate", m_RichPmtFlagDuplicateHitsActivate);

}

//=============================================================================
bool CkvStdSensDet::ProcessHits( G4Step* step,    G4TouchableHistory* history )
{
  CkvCommonSensDet* aCkvCommonSensDet =  CkvCommonSensDet::getCkvCommonSensDetInstance();
  MsgStream log( msgSvc() , name() );
  
  StatusCode sc =  aCkvCommonSensDet->ProcessRichPmtHits( step, history );
  if(sc == StatusCode::FAILURE) return false;
  
  CkvG4Hit * anewHit = aCkvCommonSensDet->  newHit();
  
  int CurrentRichCollectionSet=-1;
  const int CurrentRichDetNumber =  anewHit-> GetCurRichDetNum() ;
  const int CurrentRichDetSector =  anewHit->  GetCurSectorNum() ;
  const bool StoreCurrentHit =  anewHit->  FlagHitAsStore();
  
  if ( CurrentRichDetNumber == 0 ) {
    if ( CurrentRichDetSector == 0 ) {
      CurrentRichCollectionSet=0;

    } else {
      CurrentRichCollectionSet=1;
    }
  } else if ( CurrentRichDetNumber == 1 ) {
    if ( CurrentRichDetSector == 0 ) {
      CurrentRichCollectionSet=2;
    } else {
      CurrentRichCollectionSet=3;
    }
  }

  // G4cout<<" CkvStdSensDet richdet Collection set "<< CurrentRichDetNumber<<"  "<<  CurrentRichCollectionSet<<G4endl;
  
  if ( CurrentRichCollectionSet >= 0 ) {
    if(StoreCurrentHit ) {
      int NumHitsInCurHC =m_RichHC[CurrentRichCollectionSet] ->insert( anewHit ); 

     log << MSG::VERBOSE
       << "CkvStdSensdet: Current collection  Hit number stored = "
       << CurrentRichCollectionSet << "  "<<NumHitsInCurHC << "   " <<endmsg;
       

    }
    

  }
  

  return true;
  
  
}

//=============================================================================
// Destructor
//=============================================================================
CkvStdSensDet::~CkvStdSensDet() {}
//=============================================================================
StatusCode CkvStdSensDet::initialize() 
{
  StatusCode sc = GiGaSensDetBase::initialize();
  
  CkvCommonSensDet* aCkvCommonSensDet =  CkvCommonSensDet::getCkvCommonSensDetInstance();
  aCkvCommonSensDet -> InitGeomProp();
  m_RichG4HCName = new CkvG4HitCollName();
  // the following line already in the above constructor.
  // if(aCkvCommonSensDet -> Rich2UseGrandPmt()) m_RichG4HCName -> setCollConfigWithMixedPmtSet();
  aCkvCommonSensDet -> RichPmtAviodDuplicateHitsActivate ( m_RichPmtAviodDuplicateHitsActivate);
  aCkvCommonSensDet -> RichPmtFlagDuplicateHitsActivate  ( m_RichPmtFlagDuplicateHitsActivate);
  

  collectionName.clear();
  m_PhdHCID.clear();
  m_RichHC.clear();
  
  m_NumberOfHCInRICH=m_RichG4HCName->NumberOfStdHCollectionInRICH();
  m_PhdHCID.reserve(m_NumberOfHCInRICH);
  m_RichHC.reserve(m_NumberOfHCInRICH);
  for(int ihc=m_RichG4HCName->InitCollectListNumForStdPmtOutput();
          ihc <= m_RichG4HCName->FinalCollectListNumForStdPmtOutput()   ; ++ihc ) {
      G4String HCName=(m_RichG4HCName->RichHCName(ihc));
      collectionName.push_back(HCName);  
      m_PhdHCID.push_back(-1);

  }
  
  return sc;
  
    
}

//=============================================================================
StatusCode CkvStdSensDet::finalize()
{

  return GiGaSensDetBase::finalize();  
  
}

//=============================================================================
void  CkvStdSensDet::clear() {  }
void  CkvStdSensDet::DrawAll() {  }
void  CkvStdSensDet::PrintAll() {  }
//=============================================================================
void CkvStdSensDet::Initialize(G4HCofThisEvent*  HCE) 
{

  CkvG4HitsCollection* CurColl;
  m_RichHC.clear();


  for(int ihhc=m_RichG4HCName->InitCollectListNumForStdPmtOutput();
          ihhc <= m_RichG4HCName->FinalCollectListNumForStdPmtOutput()   ; ++ihhc ) {

     CurColl =
        new  CkvG4HitsCollection(SensitiveDetectorName,collectionName[ihhc]);

      m_RichHC.push_back(CurColl);

      if(m_PhdHCID[ihhc] < 0  ){
        m_PhdHCID[ihhc] = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[ihhc]);
      }

      HCE->AddHitsCollection( m_PhdHCID[ihhc] , m_RichHC[ihhc]  );
    }


   CkvCommonSensDet* aCkvCommonSensDet = CkvCommonSensDet::getCkvCommonSensDetInstance();
   aCkvCommonSensDet->ResetPmtMapInCurrentEvent(); 



}

//void CkvStdSensDet::EndOfEvent(G4HCofThisEvent* ) 
//{
//
//}

