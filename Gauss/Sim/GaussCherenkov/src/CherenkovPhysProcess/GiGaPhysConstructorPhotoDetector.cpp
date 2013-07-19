// $Id:GiGaPhysConstructorPhotoDetector.cpp,v 1.12 2009-03-06 10:52:36 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"

// G4 
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4ios.hh"
#include "G4Transportation.hh"
// #include "G4MultipleScattering.hh"
#include "G4LossTableManager.hh"

// local
#include "GiGaPhysConstructorPhotoDetector.h"
#include "GaussRICH/RichPhotoElectron.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"

//#include "GaussRICH/RichG4GaussPathNames.h"
//#include "DetDesc/DetectorElement.h"
//#include "RichDet/DeRichSystem.h"

//-----------------------------------------------------------------------------
// Implementation file for class :GiGaPhysConstructorPhotoDetector
//
// 2002-12-12 : Witek Pokorski
// 2003-04-29 : Sajan Easo
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(GiGaPhysConstructorPhotoDetector );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysConstructorPhotoDetector::GiGaPhysConstructorPhotoDetector
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ),
    m_RichPmtSiDetEfficiency(1.0),
    m_RichPmtPixelChipEfficiency(0.90), 
    m_RichPmtPeBackScatterProb(0.0), /*RWL change 8th Nov 06*/
    m_ActivateRICHPmtPhysProc(true),
    m_ActivateTorchTBMcpEnergyLossProc(false),
    m_TorchTBMcpAnodeEfficiency(1.0),
    m_TorchMcpAnodeReadoutChipEfficiency(1.0)    
{ 
  // Pixel chip eff changed on Nov 7 for test.
  declareProperty("RichPmtSiDetEfficiency", m_RichPmtSiDetEfficiency);
  declareProperty("RichPmtPixelChipEfficiency", m_RichPmtPixelChipEfficiency);
  declareProperty("RichPmtBackScatterProb" , m_RichPmtPeBackScatterProb);
  declareProperty("RichPmtPhysicsProcessActivate",m_ActivateRICHPmtPhysProc); 

  declareProperty("TorchTBMcpEnergyLossActivate", m_ActivateTorchTBMcpEnergyLossProc);
  declareProperty("TorchTBMcpAnodeEfficiency",  m_TorchTBMcpAnodeEfficiency);
  declareProperty("TorchTBMcpAnodeReadoutChipEfficiency", m_TorchMcpAnodeReadoutChipEfficiency);
  


  
 }


//=============================================================================
// Destructor
//=============================================================================
GiGaPhysConstructorPhotoDetector::~GiGaPhysConstructorPhotoDetector(){}


//=============================================================================
// ConstructParticle
//=============================================================================
void GiGaPhysConstructorPhotoDetector::ConstructParticle()
{
      RichPhotoElectron::PhotoElectronDefinition(); 

}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysConstructorPhotoDetector::ConstructProcess()
{
  MsgStream msg(msgSvc(), name());


  //  IDetectorElement* Rich1DE = getDet<IDetectorElement> (Rich1DeStructurePathName );
  // if( !Rich1DE ){
  //  msg << MSG::INFO  <<" No RICH1 detector element. Possibly RICH system not activated. " 
  //         <<" Hence No RICH HPD Physics Process Activated"<<endreq;
  //  setRICHHPDPhysProcActivation(false); 
  // }


    ConstructPeGenericProcess();
    msg << MSG::INFO <<" RICHPmtPhysProcess Activation status  " << activateRICHPmtPhysProcStatus() << endreq;
    

    if( activateRICHPmtPhysProcStatus() || ActivateTorchTBMcpEnergyLossProc()  ){    

      // CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
     //  if(! (aCkvGeometrySetup->isSuperRich() ) ) {  
       // temporarily deactivate for superrich for test.
       

      ConstructPmtSiEnLoss();
      
      //     }
     
    }
  
}

//=============================================================================
// ConstructPeGenericProcess
//=============================================================================
void GiGaPhysConstructorPhotoDetector::ConstructPeGenericProcess() {
  // first remove any process assigned to this particle from elsewhere.
  // then add the Transportation, HpdSiEnergyloss process to it.
  // More processes to be added later.
  MsgStream msg(msgSvc(), name());

  //        G4double aPeCut=10.0*km;
  // G4double aPeCut=0.1*mm;
  //  G4ParticleDefinition* photoelectronDef = 
  //  RichPhotoElectron::PhotoElectron();
  G4Transportation* theTransportationProcess= new G4Transportation();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    if(  particle->GetParticleName() == "pe-" )
      {
        G4ProcessManager* pmanager =  particle->GetProcessManager();
        G4ProcessVector* pVector = 
          (particle->GetProcessManager())->GetProcessList();
        // msg << MSG::DEBUG << "size of ProcList for pe-  so far  "
        //  <<(G4int)  pVector->size() << endreq;
        if( (G4int)  pVector->size() > 0 ) { 
          //  msg << MSG::DEBUG 
          //  <<" For pe-  disassociating following processes "<<  endreq; 
          //  pmanager->DumpInfo();
          for(G4int ip=0; ip < (G4int)  pVector->size() ; ++ip ){
       	    pmanager->RemoveProcess(ip);
          }
          //  msg << MSG::DEBUG <<" For pe- end of process cleanup "<<  endreq; 
          pmanager ->AddProcess(theTransportationProcess,-1,1,2);
         } else {
           //  msg << MSG::DEBUG 
           //   <<"  pe-  only has no process so far"<<  endreq;
        
           pmanager ->AddProcess(theTransportationProcess,-1,1,2);          
         }
        
        
        //  pmanager->DumpInfo();
          //particle->SetCuts(aPeCut);
          // particle->SetApplyCutsFlag(true);
        // particle-> DumpTable() ;
      }
    
  }
  
}

//=============================================================================

#include "RichPmtSiEnergyLoss.h"
#include "TorchTBMcpEnergyLoss.h"

//=============================================================================
// ConstructPmtSiEnLoss
//=============================================================================
void GiGaPhysConstructorPhotoDetector::ConstructPmtSiEnLoss()
{
  // Add Decay Process
  //  G4Decay* theDecayProcess = new G4Decay();

  //  RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
  //  new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess", fUserDefined );
  RichPmtSiEnergyLoss* theRichPmtSiEnergyLossProcess =
    new RichPmtSiEnergyLoss("RichPmtSiEnergyLossProcess", fUserDefined );
  theRichPmtSiEnergyLossProcess->setPmtSiDetEff ( m_RichPmtSiDetEfficiency);
  theRichPmtSiEnergyLossProcess->setPmtSiPixelChipEff (m_RichPmtPixelChipEfficiency);
  theRichPmtSiEnergyLossProcess->setPmtPeBackScaProb(m_RichPmtPeBackScatterProb);
  theRichPmtSiEnergyLossProcess->InitializePmtProcParam();


  TorchTBMcpEnergyLoss* theTorchTBMcpEnergyLossProcess =0;
  

  if( m_ActivateTorchTBMcpEnergyLossProc) {
    
   theTorchTBMcpEnergyLossProcess =  new TorchTBMcpEnergyLoss("TorchTBMcpEnergyLossProcess", fUserDefined  );
   theTorchTBMcpEnergyLossProcess->setMcpAnodeDetEff(m_TorchTBMcpAnodeEfficiency );
   theTorchTBMcpEnergyLossProcess->setMcpAnodePixelChipEff(m_TorchMcpAnodeReadoutChipEfficiency);
   theTorchTBMcpEnergyLossProcess->InitializeMcpProcParam();
   
  }
  
  


  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    //    G4cout<<"ConstructPmtSiEnLoss: Now at particle:  "<< particle->GetParticleName()<<G4endl;
    
    pmanager->SetVerboseLevel(0);

    if(   activateRICHPmtPhysProcStatus() ) {
      
    if( theRichPmtSiEnergyLossProcess->IsApplicable(*particle) ){

            pmanager->AddProcess( theRichPmtSiEnergyLossProcess ,-1,2,2);
            
    }
    }
    
    if( m_ActivateTorchTBMcpEnergyLossProc ) {
      
      if( theTorchTBMcpEnergyLossProcess ->IsApplicable(*particle) ){

            pmanager->AddProcess( theTorchTBMcpEnergyLossProcess ,-1,2,2);
            
       }
     
    }
    
	      if(particle->GetParticleName() == "pe-")
	      {
                (RichPhotoElectron::PhotoElectron())->SetProcessManager(pmanager);
                
	      }
        
  }
  
  
}





//=============================================================================
