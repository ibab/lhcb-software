// Include files


//-----------------------------------------------------------------------------
// Implementation file for class : CkvGrandSensDet
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


#include "GaussCherenkov/CkvGrandSensDet.h"
#include "GaussCherenkov/CkvCommonSensDet.h"
using namespace std;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CkvGrandSensDet );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvGrandSensDet::CkvGrandSensDet(const std::string& type ,
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
bool CkvGrandSensDet::ProcessHits( G4Step* step,    G4TouchableHistory* history )
{
  CkvCommonSensDet* aCkvCommonSensDet =  CkvCommonSensDet::getCkvCommonSensDetInstance();
  MsgStream log( msgSvc() , name() );

  
  StatusCode sc  =  aCkvCommonSensDet->ProcessRichPmtHits( step, history );
  if(sc == StatusCode::FAILURE) return false;



  CkvG4Hit * anewHit = aCkvCommonSensDet->  newHit();

  int CurrentRichCollectionSet=-1;
  // const int CurrentRichDetNumber =  anewHit-> GetCurRichDetNum() ;
  const int CurrentRichDetSector =  anewHit->  GetCurSectorNum() ;
  const bool StoreCurrentHit =  anewHit->  FlagHitAsStore();
  
 
  // if ( CurrentRichDetNumber == 0 ) {
  //  if ( CurrentRichDetSector == 0 ) {
  //    CurrentRichCollectionSet=0;

  //    } else {
  //    CurrentRichCollectionSet=1;
  //  }
  //  } else if ( CurrentRichDetNumber == 1 ) {

  //  G4cout<<" Start CkvGrandSensDet sector richdet store "<< CurrentRichDetSector <<"  "<< CurrentRichDetNumber
  //      <<"   "<< StoreCurrentHit<<   G4endl;

    if ( CurrentRichDetSector == 0 ) {
      CurrentRichCollectionSet=4;
    } else {
      CurrentRichCollectionSet=5;
    }

    //}
    //    G4cout<<" CkvGrandsesdet richdet collection set "<<CurrentRichDetNumber<<"  "<<CurrentRichCollectionSet <<G4endl;
    

  if ( CurrentRichCollectionSet >= 0 ) {
    if(StoreCurrentHit ) {

      int aGrPmtBegin= m_RichG4HCName->InitCollectListNumForGrandPmtOutput();
      
      int NumHitsInCurHC =m_RichHC[CurrentRichCollectionSet-aGrPmtBegin] ->insert( anewHit );       

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
CkvGrandSensDet::~CkvGrandSensDet() {}
//=============================================================================
StatusCode CkvGrandSensDet::initialize() 
{
  StatusCode sc = GiGaSensDetBase::initialize();

  CkvCommonSensDet* aCkvCommonSensDet = CkvCommonSensDet::getCkvCommonSensDetInstance();
  aCkvCommonSensDet -> InitGeomProp();
  m_RichG4HCName = new CkvG4HitCollName();
  if(aCkvCommonSensDet -> Rich2UseGrandPmt()) { 

    // This line already in the constructor above.
    //    m_RichG4HCName -> setCollConfigWithMixedPmtSet();


  collectionName.clear();
  m_PhdHCID.clear();
  m_RichHC.clear();
  
  m_NumberOfHCInRICH=m_RichG4HCName->NumberOfGrandHCollectionInRICH();
  m_PhdHCID.reserve(m_NumberOfHCInRICH);
  m_RichHC.reserve(m_NumberOfHCInRICH);
  for(int ihc=m_RichG4HCName->InitCollectListNumForGrandPmtOutput();
          ihc <= m_RichG4HCName->FinalCollectListNumForGrandPmtOutput()   ; ++ihc ) {
      G4String HCName=(m_RichG4HCName->RichHCName(ihc));
      collectionName.push_back(HCName);  
      m_PhdHCID.push_back(-1);

  }
  }

  return sc;
  
    
}
//=============================================================================
StatusCode CkvGrandSensDet::finalize()
{

  return GiGaSensDetBase::finalize();  
  
}

//=============================================================================
void  CkvGrandSensDet::clear() {  }
void  CkvGrandSensDet::DrawAll() {  }
void  CkvGrandSensDet::PrintAll() {  }
//=============================================================================
void CkvGrandSensDet::Initialize(G4HCofThisEvent*  HCE) 
{
  CkvG4HitsCollection* CurColl;
  m_RichHC.clear();


  int aGrPmtBegin= m_RichG4HCName->InitCollectListNumForGrandPmtOutput();
  int aGrPmtEnd = m_RichG4HCName->FinalCollectListNumForGrandPmtOutput();
  
  for(int ihhc=aGrPmtBegin ; ihhc <= aGrPmtEnd   ; ++ihhc ) {


    int ihhd = ihhc -aGrPmtBegin;
    

     CurColl =
        new  CkvG4HitsCollection(SensitiveDetectorName,collectionName[ihhd]);

      m_RichHC.push_back(CurColl);

      if(m_PhdHCID[ihhd] < 0  ){
        m_PhdHCID[ihhd] = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[ihhd]);
      }

      HCE->AddHitsCollection( m_PhdHCID[ihhd] , m_RichHC[ihhd]  );
    }



   CkvCommonSensDet* aCkvCommonSensDet =  CkvCommonSensDet::getCkvCommonSensDetInstance();
   aCkvCommonSensDet->ResetPmtMapInCurrentEvent();
   

}
//void CkvGrandSensDet::EndOfEvent(G4HCofThisEvent* HCE) 
//{
//   CkvSensDet::EndOfEvent(HCE);
//}

