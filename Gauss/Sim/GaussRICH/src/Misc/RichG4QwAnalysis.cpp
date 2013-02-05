// $Id: RichG4QwAnalysis.cpp,v 1.2 2006-02-27 14:10:30 seaso Exp $
// Include files 
#include "globals.hh"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h" 

#include "GaussRICH/RichG4MatRadIdentifier.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
// local
#include "GaussRICH/RichG4QwAnalysis.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4VProcess.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4QwAnalysis
//
// 2006-02-21 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4QwAnalysis* RichG4QwAnalysis::RichG4QwAnalysisInstance=0;

RichG4QwAnalysis::RichG4QwAnalysis(  ) 
  : m_qNtuple(0),
   m_qPartMax(100000), 
  m_qPart( ),
    m_ChProdX(),
    m_ChProdY(),
    m_ChProdZ(),
    m_RDetNum(),
    m_ChPartType(),
    m_ChProcType(),
    m_ChTotEner(),
    m_qwAnalysisNtupleBooked(false)  
{

 
}
RichG4QwAnalysis* RichG4QwAnalysis::getRichG4QwAnalysisInstance() 
{
  if(  RichG4QwAnalysisInstance == 0 ) {
    RichG4QwAnalysisInstance= new RichG4QwAnalysis();
    
     
  }
  
  return RichG4QwAnalysisInstance;
}


//=============================================================================
// Destructor
//=============================================================================
RichG4QwAnalysis::~RichG4QwAnalysis() {} 

//=============================================================================
void RichG4QwAnalysis::InitQwAnalysis() 
{
   
  INTupleSvc* CurrentNtupleSvc=RichG4SvcLocator::RichG4NtupleSvc();
 

  std::string afilestr = "/NTUPLES/FILE1";
  
  NTupleFilePtr antfile ( CurrentNtupleSvc, afilestr) ;
  if( antfile ) {
    std::string antstr =  afilestr+  "/QwHisto/1001";
    
    NTuplePtr nt ( CurrentNtupleSvc ,antstr ) ;
 
    if( !nt ) {
      
     nt= CurrentNtupleSvc->book (antstr , 
                  CLID_ColumnWiseTuple , "RichQw");
      
    }
    if( nt ) {
      m_qNtuple = nt;
      StatusCode sc = nt->addItem("NPart", m_qPart,0,m_qPartMax);
      sc = nt->addIndexedItem("ChProdX",  m_qPart, m_ChProdX);
      sc = nt->addIndexedItem("ChProdY",  m_qPart, m_ChProdY);
      sc = nt->addIndexedItem("ChProdZ",  m_qPart, m_ChProdZ);
      sc = nt->addIndexedItem("RDetNum",  m_qPart, m_RDetNum);
      sc = nt->addIndexedItem("ChPType",  m_qPart, m_ChPartType);
      sc = nt->addIndexedItem("ChProc" , m_qPart, m_ChProcType);
      sc = nt->addIndexedItem("ChEner",  m_qPart, m_ChTotEner);
      sc = nt->addIndexedItem("RadtNum",  m_qPart,  m_RadNum);
      

    if ( ! sc.isSuccess() ) {
      G4cout << "Error declaring RichG4Analysis QwNTuple" << G4endl ;
    }
                                                                                                                  
      
    }else {
      
    G4cout <<"No RichG4Analysis  ntuple  pointer "<<G4endl;
    }
    
    
  }else {

     G4cout <<"No RichG4Analysis  ntuple file pointer "<<G4endl;
     
  }
    
  
  m_qwAnalysisNtupleBooked = true;    
  
}
void  RichG4QwAnalysis::FillQwAnalysisHisto( const G4Track& aChTrack) 
{
  if( !  m_qwAnalysisNtupleBooked ) return;
  if(  m_qPart >  m_qPartMax ) return;

   RichG4RadiatorMaterialIdValues* aRMIdValues= RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
   
   RichG4MatRadIdentifier* aRichG4MatRadIdentifier =
                          RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance();
   const G4DynamicParticle* aChTrackParticle
    = aChTrack.GetDynamicParticle();
  
   G4ThreeVector aCurPos = aChTrack.GetStep()->GetPreStepPoint()->GetPosition();
 
   G4int aRadiatorNum = aRichG4MatRadIdentifier->
                        getRadiatorNumForG4MatIndex(aChTrack.GetMaterial()->GetIndex());
   if( ( aRadiatorNum == (aRMIdValues -> RichHpdQuartzWindowCkvRadiatorNum()) )  ||
       ( aRadiatorNum == ( aRMIdValues -> Rich1GasQWindowCkvRadiatorNum () )  )  ||
       ( aRadiatorNum == ( aRMIdValues -> Rich2GasQWindowCkvRadiatorNum() ) )   ) {
      const G4ThreeVector& aChTrackProdPos = aChTrack.GetVertexPosition();
 
//    const G4ThreeVector& aMomAtProd = aChTrack.GetVertexMomentumDirection();
      G4String aParticleName = aChTrackParticle->GetDefinition()->GetParticleName();
      G4int aPartPdgNum =  aChTrackParticle->GetDefinition()->GetPDGEncoding() ;
      G4double aParticleEnergy = aChTrack.GetTotalEnergy() ;
      G4String aCreatorProc = "unknownProc";
      if( aChTrack.GetCreatorProcess() ) {
        aCreatorProc = aChTrack.GetCreatorProcess()->GetProcessName() ;
      }
      //      G4cout<<"QwAna ParticleEnergy in Mev "<<  aParticleEnergy /MeV <<G4endl;      
      //      G4cout<<" QwAna ChProc "<<aCreatorProc<<G4endl;
      float aptype =0.0 ;
      
      if( aCreatorProc == "conv" ) {

        aptype = 1.0;
      }else if ( aCreatorProc == "compt" ) {
          
         aptype = 2.0;
       
      }else if (aCreatorProc == "Decay" ) { 
         aptype = 3.0;
        
      }
      
      m_ChProdX[m_qPart] = aChTrackProdPos.x();
      m_ChProdY[m_qPart] = aChTrackProdPos.y();
      m_ChProdZ[m_qPart] = aChTrackProdPos.z();
      m_ChPartType[m_qPart] = aPartPdgNum*1.0;
      m_ChProcType[m_qPart] =  aptype;
      m_ChTotEner[m_qPart] = aParticleEnergy;
 
     m_RadNum[m_qPart] = aRadiatorNum*1.0;
      if( aCurPos.z() < ZDnsRich1Analysis ) {
     // we are in Rich1  
        m_RDetNum[m_qPart] = 0.0;
      }else {
     // we are in Rich2 
        m_RDetNum[m_qPart] = 1.0;
      }
   

      //   m_qNtuple->write();
     ++m_qPart;  

   }


}
