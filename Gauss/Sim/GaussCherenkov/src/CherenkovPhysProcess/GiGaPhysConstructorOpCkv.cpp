// $Id: GiGaPhysConstructorOpCkv.cpp,v 1.23 2009-03-06 10:52:36 seaso Exp $
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
#include "GiGaPhysConstructorOpCkv.h"
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
// Implementation file for class : GiGaPhysConstructorOpCkv
//
// 2002-12-12 : Witek Pokorski
// 2003-04-29 : sajan Easo
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysConstructorOpCkv );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysConstructorOpCkv::GiGaPhysConstructorOpCkv
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ),
    m_RichActivateVerboseProcessInfoTag(false),
    m_MaxPhotonsPerRichCherenkovStep(40),
    m_ApplyMaxPhotCkvLimitPerRadiator(false),
    m_MaxPhotonsPerRichCherenkovStepInRich1Gas(40),
    m_MaxPhotonsPerRichCherenkovStepInRich2Gas(40),
    m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator(5000),
    m_RichRadiatorMaterialName(std::vector<G4String> (3)),
    m_RichRadiatorMaterialIndex(std::vector<G4int> (3)),
    m_PmtQEUseNominalTable(true),
    m_ActivateRICHOpticalPhysProc(true),
    m_ActivatePmtPhotoElectricPhysProc(true),
    m_activateRICHCF4Scintillation(true),
    m_RichApplyScintillationYieldScaleFactor(true),
    m_RichScintillationYieldScaleFactor(1.0),
    m_PmtQESource(0),
    m_PmtQEScaleFactor(1.0),
    m_activateTorchTestBeamSimulation(false),
    m_activatePmtModuleSupSet3(false),
    m_activatePmtModuleSupSet4(false),
    m_activatePmtModuleSupSet5(false),
    m_activatePmtModuleSupSet6(false),
    //
    m_activatePmtSupSet0(false),
    m_activatePmtSupSet1(false),
    m_activatePmtSupSet2(false)

{
  // in the above 3 is for the three radiators.

  declareProperty("RichActivateRichPhysicsProcVerboseTag",
                  m_RichActivateVerboseProcessInfoTag);
  declareProperty("RichMaxNumPhotPerCherenkovStep",
                  m_MaxPhotonsPerRichCherenkovStep);  
  declareProperty("RichApplyMaxNumCkvPhotPerStepPerRadiator",
                  m_ApplyMaxPhotCkvLimitPerRadiator);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRich1Gas",
                  m_MaxPhotonsPerRichCherenkovStepInRich1Gas);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRich2Gas",
                  m_MaxPhotonsPerRichCherenkovStepInRich2Gas);
  declareProperty("RichMaxPhotonsPerCherenkovStepInRichQuartzLikeRadiators",
                  m_MaxPhotonsPerRichCherenkovStepInRichQuartzLikeRadiator);  

  declareProperty("RichPmtUseNominalQETable", m_PmtQEUseNominalTable);
  declareProperty("RichOpticalPhysicsProcessActivate", m_ActivateRICHOpticalPhysProc);
  declareProperty("RichPmtPhotoElectricPhysicsProcessActivate", m_ActivatePmtPhotoElectricPhysProc);

  declareProperty("RichActivateCF4Scintillation",  m_activateRICHCF4Scintillation);
  declareProperty("RichApplyScintillationYieldScaleFactor", 
                                          m_RichApplyScintillationYieldScaleFactor);
  declareProperty("RichScintillationYieldScaleFactor", m_RichScintillationYieldScaleFactor);

  //  declareProperty("RichActivateCF4ScintHisto" , m_activateRICHCF4ScintillationHisto);
  
  declareProperty("RichPmtQESource", m_PmtQESource);
  declareProperty("RichPmtQEOverallScaling", m_PmtQEScaleFactor);  // overall scaling

  // Now for the TORCH Testebeam

  declareProperty("ActivateTorchTBSimulation", m_activateTorchTestBeamSimulation );

  // now for Rich PMT suppression studies

  declareProperty("ActivatePmtModuleSuppressSet3",m_activatePmtModuleSupSet3);
  declareProperty("ActivatePmtModuleSuppressSet4",m_activatePmtModuleSupSet4);
  declareProperty("ActivatePmtModuleSuppressSet5",m_activatePmtModuleSupSet5);
  declareProperty("ActivatePmtModuleSuppressSet6",m_activatePmtModuleSupSet6);

  declareProperty("ActivatePmtSuppressSet0", m_activatePmtSupSet0 );
  declareProperty("ActivatePmtSuppressSet1", m_activatePmtSupSet1 );
  declareProperty("ActivatePmtSuppressSet2", m_activatePmtSupSet2 );  
  
}

//=============================================================================
// Destructor
//=============================================================================
GiGaPhysConstructorOpCkv::~GiGaPhysConstructorOpCkv(){}


//=============================================================================
//
//=============================================================================
void GiGaPhysConstructorOpCkv::ConstructParticle()
{
  //     RichPhotoElectron::PhotoElectronDefinition(); 
}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysConstructorOpCkv::ConstructProcess()
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
  
   msg << MSG::INFO <<"RICH Optical Process activation status "<< activateRICHOpticalPhysProcStatus ()<<endreq;
  
  //  ConstructPeProcess();
  if( activateRICHOpticalPhysProcStatus () )  ConstructOp();
}

//=============================================================================
// ConstructPeProcess
//=============================================================================
void  GiGaPhysConstructorOpCkv::ConstructPeProcess() 
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
#include "GaussCherenkov/CkvG4OpBoundaryProcess.h"
#include "RichPmtPhotoElectricEffect.h"
#include "GaussRICH/RichG4Scintillation.h"
#include  "TorchTBMcpPhotoElectricEffect.h"

//=============================================================================
// ConstructOp
//=============================================================================
void GiGaPhysConstructorOpCkv::ConstructOp() {

  MsgStream msg(msgSvc(), name());
  
  msg << MSG::INFO <<" Activation for verbose Output in Rich Optical Proc = "
      << m_RichActivateVerboseProcessInfoTag << endreq;

  RichG4Cerenkov*   theCerenkovProcess = 
             new RichG4Cerenkov("RichG4Cerenkov", fOptical );
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();

  //  RichG4OpRayleigh*   theRayleighScatteringProcess = 
  //  new RichG4OpRayleigh("RichG4OpRayleigh", fOptical);


  CkvG4OpBoundaryProcess* theBoundaryProcess = 
    new CkvG4OpBoundaryProcess("CkvG4OpBoundary", fOptical );
  
  //  G4cout<<"Now creating Photoelectric  processes"<<G4endl;
  //  RichHpdPhotoElectricEffect* theRichHpdPhotoElectricProcess= 
  //  new RichHpdPhotoElectricEffect(this,
  //                   "RichHpdPhotoelectricProcess", fOptical);
  RichPmtPhotoElectricEffect* theRichPmtPhotoElectricProcess = 0;
  
  if(m_ActivatePmtPhotoElectricPhysProc) {
    
       theRichPmtPhotoElectricProcess = new RichPmtPhotoElectricEffect(this,
                       "RichPmtPhotoelectricProcess", fOptical);
                      
  }
  

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

  //  theRayleighScatteringProcess->SetVerboseLevel(0);

  theBoundaryProcess->SetVerboseLevel(0);

  if(m_ActivatePmtPhotoElectricPhysProc) {
  
    // theRichPmtPhotoElectricProcess->setUsePmtMagDistortions( (G4bool) m_UseHpdMagDistortions);
    //  theRichPmtPhotoElectricProcess->setPSFPreDc06Flag(m_IsPSFPreDc06Flag);
    theRichPmtPhotoElectricProcess->setPmtQEUsingNominalTable(m_PmtQEUseNominalTable);
    theRichPmtPhotoElectricProcess->SetPmtQESourceTable(m_PmtQESource);
    theRichPmtPhotoElectricProcess->SetPmtQEOverallScaling(m_PmtQEScaleFactor); 
    theRichPmtPhotoElectricProcess->setPmtModuleSupFlag3(m_activatePmtModuleSupSet3);
    theRichPmtPhotoElectricProcess->setPmtModuleSupFlag4(m_activatePmtModuleSupSet4);
    theRichPmtPhotoElectricProcess->setPmtModuleSupFlag5(m_activatePmtModuleSupSet5);
    theRichPmtPhotoElectricProcess->setPmtModuleSupFlag6(m_activatePmtModuleSupSet6);
    //
    theRichPmtPhotoElectricProcess->setPmtSupFlag0(m_activatePmtSupSet0);
    theRichPmtPhotoElectricProcess->setPmtSupFlag1(m_activatePmtSupSet1);
    theRichPmtPhotoElectricProcess->setPmtSupFlag2(m_activatePmtSupSet2);

    theRichPmtPhotoElectricProcess->setPmtPhElecParam();
 
    msg << MSG::INFO <<"Current PMT QE source "<<m_PmtQESource<<endreq;
    msg << MSG::INFO <<"Overall PMT QE scaling: " << m_PmtQEScaleFactor <<endreq;

  }
  
  // Now for the TORCH TestBeam
  TorchTBMcpPhotoElectricEffect* theTorchMcpPhotoElectricProcess =0;
  

  if( m_activateTorchTestBeamSimulation ) {
    
    theTorchMcpPhotoElectricProcess =
      new TorchTBMcpPhotoElectricEffect (this, "TorchTBMcpPhotoElectricProcess", fOptical);
    
    theTorchMcpPhotoElectricProcess->setMcpPhElecParam();
    

    
    
  }
  


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
    msg<< MSG::DEBUG <<" Rich1Gas Rich2Gas have different Max Phot per Cherenkov Step = "<<"   "
       << m_MaxPhotonsPerRichCherenkovStepInRich1Gas<<"    "
       << m_MaxPhotonsPerRichCherenkovStepInRich2Gas  <<endreq;    
  }else {
    msg<<MSG::DEBUG <<"All Radiators have the same Max Phot per Cherenkov Step=  "
       << m_MaxPhotonsPerRichCherenkovStep <<endreq;
    
  }
  
  std::vector<G4int> aMaxPhotLim;
  aMaxPhotLim.clear();
  //  aMaxPhotLim.push_back((G4int) m_MaxPhotonsPerRichCherenkovStepInRich1Agel);
  aMaxPhotLim.push_back(900); // temporary until aerogel is fully removed.
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



  //  theRayleighScatteringProcess->
  //   SetRichVerboseRayleighInfoTag( (G4bool) m_RichActivateVerboseProcessInfoTag);
  //  theRayleighScatteringProcess->
  //  SetRichMaxStepNumLimitInRayleigh((G4int) m_MaxAllowedPhotStepNumInRayleigh);
  // theRayleighScatteringProcess->
  //  SetMaxNumRayleighScatAllowed((G4int)  m_MaxNumberRayleighScatAllowed);
  

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


            // msg<<MSG::DEBUG <<"Activaed Cherenkov Process   "<<endreq;


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
         //         pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
         pmanager->AddDiscreteProcess(theBoundaryProcess);
         //         pmanager->AddDiscreteProcess(theRichHpdPhotoElectricProcess)
       if(m_ActivatePmtPhotoElectricPhysProc) {
         pmanager->AddDiscreteProcess(theRichPmtPhotoElectricProcess);
       }

       if( m_activateTorchTestBeamSimulation ) {
         
         pmanager->AddDiscreteProcess(theTorchMcpPhotoElectricProcess);

         
         
       }
  
       
     }
  }
}

//=============================================================================
// FindRichRadiatorMaterialIndices
//=============================================================================
std::vector<G4int> GiGaPhysConstructorOpCkv::FindRichRadiatorMaterialIndices
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
