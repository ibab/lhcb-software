// $Id: GiGaPhysConstructorOp.cpp,v 1.23 2009-03-06 10:52:36 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"

// G4 
#include "globals.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4VProcess.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4Decay.hh"
#include "G4ios.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"

// local
#include "GiGaPhysConstructorOp.h"
#include "GaussRICH/RichPhotoElectron.h"
#include "G4Electron.hh"
#include "G4Transportation.hh"
//#include "G4MultipleScattering.hh"
#include "G4ProcessVector.hh"
#include "G4LossTableManager.hh"
#include <vector>
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4MatRadIdentifier.h"


// #include "DetDesc/DetectorElement.h"
// #include "RichDet/DeRichSystem.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaPhysConstructorOp
//
// 2002-12-12 : Witek Pokorski
// 2003-04-29 : sajan Easo
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysConstructorOp );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysConstructorOp::GiGaPhysConstructorOp
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ),
    m_RichActivateVerboseProcessInfoTag(false),
    m_MaxPhotonsPerRichCherenkovStep(40),
    m_ApplyMaxPhotCkvLimitPerRadiator(false),
    m_MaxPhotonsPerRichCherenkovStepInRich1Agel(900),
    m_MaxPhotonsPerRichCherenkovStepInRich1Gas(40),
    m_MaxPhotonsPerRichCherenkovStepInRich2Gas(40),
    m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator(5000),
    m_RichRadiatorMaterialName(std::vector<G4String> (3)),
    m_RichRadiatorMaterialIndex(std::vector<G4int> (3)),
    m_MaxAllowedPhotStepNumInRayleigh(5000),
    m_MaxNumberRayleighScatAllowed(50),
    m_UseHpdMagDistortions(false),
    m_IsPSFPreDc06Flag(false),
    m_HpdQEUseNominalTable(false),
    m_ActivateRICHOpticalPhysProc(true),
    m_activateRICHCF4Scintillation(true),
    m_ActivateRICHHitSmearing(false),
    m_RichAerogelHitSmearValue(0.0),
    m_Rich1GasHitSmearValue(0.0),
    m_Rich2GasHitSmearValue(0.0)
{
  // in the above 3 is for the three radiators.

  declareProperty("RichActivateRichPhysicsProcVerboseTag",
                  m_RichActivateVerboseProcessInfoTag);
  declareProperty("RichMaxNumPhotPerCherenkovStep",
                  m_MaxPhotonsPerRichCherenkovStep);  
  declareProperty("RichApplyMaxNumCkvPhotPerStepPerRadiator",
                  m_ApplyMaxPhotCkvLimitPerRadiator);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRich1Agel",
                  m_MaxPhotonsPerRichCherenkovStepInRich1Agel);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRich1Gas",
                  m_MaxPhotonsPerRichCherenkovStepInRich1Gas);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRich2Gas",
                  m_MaxPhotonsPerRichCherenkovStepInRich2Gas);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRichQuartzLikeRadiators",
                  m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator);
 
  declareProperty("RichMaxPhotonStepNumInRayleigh",
                  m_MaxAllowedPhotStepNumInRayleigh);

  declareProperty("MaxNumberRayleighScatAllowed",m_MaxNumberRayleighScatAllowed);
  

  declareProperty("RichUseHpdMagDistortions",
                  m_UseHpdMagDistortions);
  declareProperty("RichPSFPreDc06Flag", m_IsPSFPreDc06Flag);

  declareProperty("RichHpdUseNominalQETable", m_HpdQEUseNominalTable);
  declareProperty("RichOpticalPhysicsProcessActivate", m_ActivateRICHOpticalPhysProc);

  declareProperty("RichActivateCF4Scintillation",  m_activateRICHCF4Scintillation);
  declareProperty("RichApplyScintillationYieldScaleFactor", 
                                          m_RichApplyScintillationYieldScaleFactor);
  declareProperty("RichScintillationYieldScaleFactor", m_RichScintillationYieldScaleFactor);
  

  //  declareProperty("RichActivateCF4ScintHisto" , m_activateRICHCF4ScintillationHisto);
  declareProperty("RichHitSmearingActivate", m_ActivateRICHHitSmearing);
  declareProperty("RichAerogelHitSmearValue",m_RichAerogelHitSmearValue);
  declareProperty("Rich1GasHitSmearValue",m_Rich1GasHitSmearValue);
  declareProperty("Rich2GasHitSmearValue",m_Rich2GasHitSmearValue);
  

}

//=============================================================================
// Destructor
//=============================================================================
GiGaPhysConstructorOp::~GiGaPhysConstructorOp(){}


//=============================================================================
//
//=============================================================================
void GiGaPhysConstructorOp::ConstructParticle()
{
  //     RichPhotoElectron::PhotoElectronDefinition(); 
}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysConstructorOp::ConstructProcess()
{
  // activate RICH Optical Physics process only when the RICH Geometry
  // exists. This is done by checking the existence of the RICH1 detector element.
   MsgStream msg(msgSvc(), name());
  


   // IDetectorElement* Rich1DE = getDet<IDetectorElement> (Rich1DeStructurePathName );
   // if( !Rich1DE ){
   //  msg << MSG::INFO <<" No RICH1 detector element. Possibly RICH system not activated. " 
   //        <<" Hence No RICH Optical Physics Process Activated"<<endreq;
   //  setRICHOpticalPhysProcActivation(false); 
   // }
  
   msg << MSG::DEBUG <<"RICH Optical Process activation status "<< activateRICHOpticalPhysProcStatus ()<<endreq;
  
  //  ConstructPeProcess();
  if( activateRICHOpticalPhysProcStatus () )  ConstructOp();
}

//=============================================================================
// ConstructPeProcess
//=============================================================================
void  GiGaPhysConstructorOp::ConstructPeProcess() 
{

  //  MsgStream msg(msgSvc(), name());

  //        G4double aPeCut=10.0*km;
  // G4double aPeCut=0.1*mm;
  //  G4ParticleDefinition* photoelectronDef = 
  //  RichPhotoElectron::PhotoElectron();
  //  photoelectronDef->SetCuts(aPeCut);
  //  photoelectronDef->SetApplyCutsFlag(true);
  // photoelectronDef-> DumpTable() ;  
  // G4Transportation* theTransportationProcess= new G4Transportation();
  // G4MultipleScattering* theMultipleScattering = new G4MultipleScattering();  
  // theParticleIterator->reset();
  // while( (*theParticleIterator)() ){
  //  G4ParticleDefinition* particle = theParticleIterator->value();
  //  if(  particle->GetParticleName() == "pe-" )
  //    {
  //      G4ProcessManager* pmanager =  particle->GetProcessManager();
  //      pmanager ->AddProcess(theTransportationProcess,-1,2,2);
  //      pmanager->AddProcess(theMultipleScattering,-1,1,1);
  //      pmanager ->SetProcessOrderingToFirst
  //        (theTransportationProcess, idxAlongStep);
  //      pmanager ->SetProcessOrderingToFirst
  //        (theTransportationProcess, idxPostStep);
  //      pmanager ->SetProcessOrderingToFirst
  //        (theMultipleScattering, idxAlongStep);
  //      pmanager ->SetProcessOrderingToFirst(theMultipleScattering, idxPostStep);
  //      particle->SetCuts(aPeCut);
  //      particle->SetApplyCutsFlag(true);
        //       BuildPhysicsTable(particle);
        //  G4LossTableManager* aG4LossTableManager = G4LossTableManager::Instance();
        // aG4LossTableManager->BuildPhysicsTable(particle);
        
  //      G4int j;
        // Rebuild the physics tables for every process for this particle type
  //      G4ProcessVector* pVector = 
  //        (particle->GetProcessManager())->GetProcessList();
  //      msg << MSG::DEBUG << "size ProcList pe- "<< pVector->size()<< endreq;
  //      
  //      for ( j=0; j < pVector->size(); ++j) 
  //        {
  //          (*pVector)[j]->BuildPhysicsTable(*particle);
  //        }
  //      particle->DumpTable();
  //      pmanager->DumpInfo();
  //      G4int  an1 =  pmanager ->GetProcessListLength() ;
  //      msg << MSG::DEBUG << "Num proc for pe so far = " << an1 << endreq;
  //    }
  // }
  
  //       G4ProcessManager* pmanager =  photoelectronDef->GetProcessManager();
  //   G4ProcessManager* pmanager =  new  G4ProcessManager( photoelectronDef);
  // photoelectronDef->SetProcessManager( pmanager);
      
  //   pmanager->SetParticleType(photoelectronDef);
  //test by SE
  //   G4ParticleDefinition* aph =  pmanager->GetParticleType();
  // G4String aPhname = aph-> GetParticleName();
  // G4cout<<"Name of photoelectron "<<aPhname <<G4endl;
  //       G4ParticleDefinition* bel = G4Electron::Electron();
  //  G4ProcessManager* bpman =  bel->GetProcessManager(); 
  // bpman ->DumpInfo();
  // end of test by SE   
  
  //       AddTransportation();
  
  //       G4Transportation* theTransportationProcess= new G4Transportation();
  // pmanager ->AddProcess(theTransportationProcess);
  //  pmanager ->SetProcessOrderingToFirst(theTransportationProcess, idxAlongStep);
  //  pmanager ->SetProcessOrderingToFirst(theTransportationProcess, idxPostStep);
  
  // pmanager->DumpInfo();
  //       G4int  an1 =  pmanager ->GetProcessListLength() ;
  //  G4cout<<"NUm proc for pe so far = "<< an1<<G4endl;
}

//=============================================================================
#include "GaussRICH/RichG4Cerenkov.h"
#include "G4OpAbsorption.hh"
#include "GaussRICH/RichG4OpRayleigh.h"
#include "GaussRICH/RichG4OpBoundaryProcess.h"
#include "GaussRICH/RichHpdPhotoElectricEffect.h"
#include "GaussRICH/RichG4Scintillation.h"

//=============================================================================
// ConstructOp
//=============================================================================
void GiGaPhysConstructorOp::ConstructOp() {

  MsgStream msg(msgSvc(), name());
  
  msg << MSG::DEBUG <<" Activation for verbose Output in Rich Optical Proc = "
      << m_RichActivateVerboseProcessInfoTag << endreq;

  RichG4Cerenkov*   theCerenkovProcess = 
             new RichG4Cerenkov("RichG4Cerenkov", fOptical );
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  RichG4OpRayleigh*   theRayleighScatteringProcess = 
    new RichG4OpRayleigh("RichG4OpRayleigh", fOptical);
  RichG4OpBoundaryProcess* theBoundaryProcess = 
    new RichG4OpBoundaryProcess("RichG4OpBoundary", fOptical );
  
  //  G4cout<<"Now creating Photoelectric  processes"<<G4endl;
  RichHpdPhotoElectricEffect* theRichHpdPhotoElectricProcess= 
    new RichHpdPhotoElectricEffect(this,
                     "RichHpdPhotoelectricProcess", fOptical);

  RichG4Scintillation* theRichScintillationProcess= 0;
  
  if( m_activateRICHCF4Scintillation ) {
    
    theRichScintillationProcess = new RichG4Scintillation("RichG4Scintillation",fOptical);
    theRichScintillationProcess->SetVerboseLevel(0);
    if(m_RichApplyScintillationYieldScaleFactor) {
      
      theRichScintillationProcess-> 
        SetScintillationYieldFactor(m_RichScintillationYieldScaleFactor);
    }
    
  }
  
  theCerenkovProcess->SetVerboseLevel(0);
  theAbsorptionProcess->SetVerboseLevel(0);
  theRayleighScatteringProcess->SetVerboseLevel(0);
  theBoundaryProcess->SetVerboseLevel(0);
  
  theRichHpdPhotoElectricProcess->setUseHpdMagDistortions( (G4bool) m_UseHpdMagDistortions);
  theRichHpdPhotoElectricProcess->setPSFPreDc06Flag(m_IsPSFPreDc06Flag);
  theRichHpdPhotoElectricProcess->setHpdQEUsingNominalTable(m_HpdQEUseNominalTable);
  theRichHpdPhotoElectricProcess->setactivateRichHitSmear( m_ActivateRICHHitSmearing);
  theRichHpdPhotoElectricProcess->setRichAerogelHitSmearVal(m_RichAerogelHitSmearValue);
  theRichHpdPhotoElectricProcess->setRich1GasHitSmearVal(m_Rich1GasHitSmearValue);
  theRichHpdPhotoElectricProcess->setRich2GasHitSmearVal(m_Rich2GasHitSmearValue);
  theRichHpdPhotoElectricProcess->setHpdPhElecParam();  
  

  //  G4int MaxNumPhotons = 300;
  // The following is now input from options file. SE 2-2-2004
  // the default value was 900. now changed the default to 40.
  // G4int MaxNumPhotons = 40;

  G4int MaxNumPhotons = (G4int)m_MaxPhotonsPerRichCherenkovStep; 
  // msg << MSG::DEBUG << " Global value of Max Number of Photons per Cherenkov step=  "
  //    << MaxNumPhotons << endreq;
  msg<<MSG::DEBUG << "  Apply Flag for MaxNumCherenkov Phot per Step Radiator "
     <<   m_ApplyMaxPhotCkvLimitPerRadiator <<endreq;
  if(m_ApplyMaxPhotCkvLimitPerRadiator) {
    msg<< MSG::DEBUG <<" Agel Rich1Gas Rich2Gas have different Max Phot per Cherenkov Step = "  
       << m_MaxPhotonsPerRichCherenkovStepInRich1Agel<<"   "
       << m_MaxPhotonsPerRichCherenkovStepInRich1Gas<<"    "
       << m_MaxPhotonsPerRichCherenkovStepInRich2Gas  <<endreq;    
  }else {
    msg<<MSG::DEBUG <<"All Radiators have the same Max Phot per Cherenkov Step=  "
       << m_MaxPhotonsPerRichCherenkovStep <<endreq;
    
  }
  
  std::vector<G4int> aMaxPhotLim;
  aMaxPhotLim.clear();
  aMaxPhotLim.push_back((G4int) m_MaxPhotonsPerRichCherenkovStepInRich1Agel);
  aMaxPhotLim.push_back((G4int) m_MaxPhotonsPerRichCherenkovStepInRich1Gas);
  aMaxPhotLim.push_back((G4int) m_MaxPhotonsPerRichCherenkovStepInRich2Gas);
  aMaxPhotLim.push_back((G4int) m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator);

  RichG4MatRadIdentifier* aRichG4MatRadIdentifier = RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance();
  aRichG4MatRadIdentifier->InitializeRichCkvMatMaxNumPhot(aMaxPhotLim);
  // the following change made in March 2004 to fix the problem in G4.
  theCerenkovProcess->SetTrackSecondariesFirst(false);
  theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);
  theCerenkovProcess->
    SetRichVerboseInfoTag( (G4bool) m_RichActivateVerboseProcessInfoTag);
  theCerenkovProcess->
    SetMaxPhotonPerRadiatorFlag((G4bool) m_ApplyMaxPhotCkvLimitPerRadiator);  

  if( m_activateRICHCF4Scintillation ) {
    
    theRichScintillationProcess->
          SetRichVerboseInfoTag( (G4bool) m_RichActivateVerboseProcessInfoTag);
    theRichScintillationProcess->SetTrackSecondariesFirst(false);
  }
  
  
  G4OpticalSurfaceModel themodel = glisur;
  theBoundaryProcess->SetModel(themodel);
  theRayleighScatteringProcess->
   SetRichVerboseRayleighInfoTag( (G4bool) m_RichActivateVerboseProcessInfoTag);
  theRayleighScatteringProcess->
   SetRichMaxStepNumLimitInRayleigh((G4int) m_MaxAllowedPhotStepNumInRayleigh);
  theRayleighScatteringProcess->
    SetMaxNumRayleighScatAllowed((G4int)  m_MaxNumberRayleighScatAllowed);
  

  theParticleIterator->reset();

  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    // Avoid the cherenkov preocess for the photoelectron.
    G4String particleName = particle->GetParticleName();
    if(particleName != "pe-" )
      {

        G4ProcessManager* pmanager = particle->GetProcessManager();
        if (theCerenkovProcess->IsApplicable(*particle)) 
          {
            //      G4ProcessManager* pmanager = particle->GetProcessManager();
	          // pmanager->AddContinuousProcess(theCerenkovProcess);
            // this modif to be similar to that G4.9.1 onwards. SE 5-8-2008
            pmanager->AddProcess(theCerenkovProcess);
            pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
          }

        if(   m_activateRICHCF4Scintillation ) {
          
          if(theRichScintillationProcess->IsApplicable(*particle)){

              pmanager->AddProcess(theRichScintillationProcess);
              pmanager->SetProcessOrderingToLast(theRichScintillationProcess, idxAtRest);
              pmanager->SetProcessOrderingToLast(theRichScintillationProcess, idxPostStep);

          }
        }
        
        
      }
    //    G4cout<<"Particle name  "<<particleName<<endl;
    if (particleName == "opticalphoton") 
      {
         G4ProcessManager* pmanager = particle->GetProcessManager();
         //         G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
         pmanager->AddDiscreteProcess(theAbsorptionProcess);
         pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
         pmanager->AddDiscreteProcess(theBoundaryProcess);
         pmanager->AddDiscreteProcess(theRichHpdPhotoElectricProcess);
     }
  }
}

//=============================================================================
// FindRichRadiatorMaterialIndices
//=============================================================================
std::vector<G4int> GiGaPhysConstructorOp::FindRichRadiatorMaterialIndices
  (std::vector<G4String> aRadiatorMaterialNames)
{
    std::vector<G4int> aIndexVect;
   aIndexVect.clear();
  static const G4MaterialTable* theMaterialTable =
    G4Material::GetMaterialTable();
  int numberOfMat= (int) theMaterialTable->size() ;

  for(int iname=0; iname< (int) aRadiatorMaterialNames.size(); ++iname){
    //    G4cout<<" GigaPhysCons iname "<<iname<<G4endl;
    G4String curName= aRadiatorMaterialNames[iname];
    int im=0;
    bool nameFound=false;
    while( im< numberOfMat && ! (nameFound)  ) {
      if( curName == (*theMaterialTable)[im]->GetName() ) {  
        aIndexVect.push_back( (*theMaterialTable)[im]->GetIndex()   );
        nameFound=true;
      }
      im++;    
    }  
    
  }  

  return aIndexVect;
  
}


//=============================================================================
