// $Id: AnalysisAlgorithm.cpp,v 1.1.1.1 2001-10-19 17:06:05 gcorti Exp $
#define TOOLSTUTORIAL_ANALYSISALGORITHM_CPP

// Framework include files
#include "GaudiKernel/AlgFactory.h"

// Printing
#include "GaudiKernel/MsgStream.h"

// Using Particle properties
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Accessing data:
#include "CLHEP/Units/PhysicalConstants.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/MCParticle.h"

// ToolService and Tools
#include "GaudiKernel/IToolSvc.h"
#include "Analysis/IMCUtilityTool.h"

// Example related include files
#include "AnalysisAlgorithm.h"

static const AlgFactory<AnalysisAlgorithm>  Factory;
const IAlgFactory& AnalysisAlgorithmFactory = Factory;

/**  Algorithm parameters which can be set at run time must be declared.
     This should be done in the constructor.
*/
AnalysisAlgorithm::AnalysisAlgorithm(const std::string& nam, 
                                       ISvcLocator* pSvcLocator) 
:	Algorithm(nam, pSvcLocator),
        m_partID(0),
        m_daugID(0),
        m_pUtilTool(0) {

  declareProperty("DecayParticle", m_partName = "B0");
  declareProperty("DecayProducts", m_daugName );
  declareProperty("ToolToUse",     m_toolName = "MCUtilityTool");
}

// Standard destructor
AnalysisAlgorithm::~AnalysisAlgorithm()   {
}

// Algorithm initialization
StatusCode AnalysisAlgorithm::initialize()   {

  MsgStream log(msgSvc(), name());

  // Accessing particle properties
  IParticlePropertySvc* ppSvc = 0;
  StatusCode sc = service("ParticlePropertySvc", ppSvc );
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "Unable to locate Particle Property Service" << endreq;
    return sc;
  }
  // Retrieve Geant3 ID code for requested particle 
  ParticleProperty* partProp = ppSvc->find( m_partName );
  if ( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for particle \"" <<
           m_partName << "\" " << endreq;
    return StatusCode::FAILURE;
  }
  m_partID = partProp->geantID();
  log << MSG::INFO << "GEANT 3 particle ID of " << m_partName << " is " << 
         m_partID << endreq;

  // and all its daughters
  std::vector<std::string>::const_iterator idau;
  for ( idau = m_daugName.begin(); idau != m_daugName.end(); idau++ ) {
    partProp = ppSvc->find( *idau );
    if ( 0 == partProp )   {
      log << MSG::ERROR << "Cannot retrieve properties for particle \"" 
          << *idau << "\" " << endreq;
      return StatusCode::FAILURE;
    }
    m_daugID.push_back( partProp->geantID() );
  }

  // Accessing the Tool Service
  IToolSvc* toolsvc = 0;
  sc = service("ToolSvc", toolsvc );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to locate Tool Service" << endreq;
    return sc;
  }

  // Retrieve public tool 
  sc = toolsvc->retrieveTool( m_toolName, m_pUtilTool );
  if ( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to retrieve requested tool " << m_toolName 
        << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;  
}

// Algorithm finalization
StatusCode AnalysisAlgorithm::finalize()   {
  StatusCode status = StatusCode::SUCCESS;
  m_pUtilTool = 0;
  m_daugName.clear();
  m_daugID.clear();
  return status;
}


// Event callback
StatusCode AnalysisAlgorithm::execute() {

  // Retrieve Monte Carlo particles from data store
  MsgStream log(msgSvc(), name());
  SmartDataPtr<MCParticleVector> parts(eventSvc(), "/Event/MC/MCParticles");
  if ( parts )  {
    MCParticleVector::const_iterator ipart;
    for ( ipart = parts->begin(); ipart != parts->end(); ipart++ )  {
      if ( (*ipart)->particleID().id() == m_partID )   {
        log << MSG::INFO << "Found Particle of type " << m_partName << endreq;
        // Now use tool to print tree
        m_pUtilTool->printDecayTree( 0, " |", *ipart );
        bool result = m_pUtilTool->matchDecayTree( *ipart, m_daugID );
        if ( result ) {
          log << MSG::INFO << "Found requested decay tree" << endreq;
        }
        else {
          log << MSG::INFO << "Requested decay not found" << endreq;
        }
      }
    }
  }
  else  {
    log << MSG::ERROR << "Cannot find Monte-Carlo Particles..." << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
