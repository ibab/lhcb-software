// $Id: DaVinciTestAlgorithm.cpp,v 1.3 2001-10-08 11:51:26 gcorti Exp $
#define DAVINCITESTALGORITHM_CPP 

// Include files
#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiTools/IAssociator.h"

//                 data to be accessed
#include "LHCbEvent/Event.h"
#include "LHCbEvent/AxPartCandidate.h"
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/RefTable.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PhysEvent/VtxCandidate.h"
#include "PhysEvent/PhysSel.h"
#include "L0DU/L0DUReport.h"

#include "DaVinciTestAlgorithm.h"
//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciTestAlgorithm
// 
// Example of how to access various info from DaVinci
// 
// 19/06/2001 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const AlgFactory<DaVinciTestAlgorithm>    Factory;
const IAlgFactory& DaVinciTestAlgorithmFactory = Factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
DaVinciTestAlgorithm::DaVinciTestAlgorithm(const std::string& name,
                                           ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_selDec("B0PP"),
  m_nEvents(0),
  m_nPhysSelMC(0),
  m_nPhysSelTKR(0),
  m_nPhysSelPQU(0),
  m_nPhysSelSEL(0),
  m_nPhysSelAGR(0),
  m_nPhysSelTAG(0),
  m_pNameSearch("B0"),
  m_pIDSearch(0),
  m_nL0Trigger(0),
  m_pAsct(0),
  m_ppSvc(0) {
 
    declareProperty( "PhysSelDecayChannel", m_selDec);  
    declareProperty( "SearchParticle", m_pNameSearch);

}

//=============================================================================
// Standard destructor
//=============================================================================
DaVinciTestAlgorithm::~DaVinciTestAlgorithm() { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DaVinciTestAlgorithm::initialize() {

  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initialize" << endreq;
  
  // Retrieve the tools used by this algorithm
  log << MSG::INFO << "    Looking for Tool Service." << endreq;

  IToolSvc* toolsvc = 0;
  StatusCode sc = service("ToolSvc", toolsvc );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Tool Service" << endreq;
    return sc;
  }

  sc = toolsvc->retrieveTool("AxPart2MCParticleAsct", m_pAsct);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create Associator tool" << endreq;
    return sc;
  }

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::DEBUG << "    Looking for Particle Property Service." << endreq;

  sc = service("ParticlePropertySvc", m_ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  
  // Retrieve Geant3 code for particle to search for: this is what is
  // stored in MCParticle
  ParticleProperty* pp1 = m_ppSvc->find( m_pNameSearch );
  m_pIDSearch = pp1->geantID();

  // Initialization terminated
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DaVinciTestAlgorithm::execute() {

  MsgStream          log( msgSvc(), name() );

  // Counter of events processed
  log << MSG::INFO << ">>> Execute" << endreq;
  log << MSG::INFO << "    processing event number " << ++m_nEvents << endreq;

  StatusCode         sc = StatusCode::SUCCESS;

  // Retrieve informations about event
  SmartDataPtr<Event> evt(eventSvc(), "/Event" );

  if ( evt ) {   
    log << MSG::INFO << "    retrieved EVENT: " << evt->event()
        << " RUN: " << evt->run() << endreq;
  }
  else {
    log << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrive VtxCandidate
  SmartDataPtr<VtxCandidateVector> vertices (evt, "/Phys/PrimaryVertices");
  if( ! vertices ) {
    log << MSG::INFO << "    unable to retrieve vertices " << endreq;
  }
  else { 
    log << MSG::INFO << "    Number of primary vertices  = " 
        << vertices->size() << endreq;

    VtxCandidateVector::iterator iterV = 0;

    for( iterV = vertices->begin(); iterV != vertices->end(); iterV++ ) {
      log << MSG::DEBUG << "    Vertex coordinates = ( " 
          << (*iterV)->position().x() 
          << " , " << (*iterV)->position().y() 
          << " , " << (*iterV)->position().z() << endreq;
      log << MSG::DEBUG << "    Vertex ChiSquare = " << (*iterV)->chiSquare() 
          << " ) " << endreq;  
      //std::cout << *(*iterV) << std::endl;
    }
  }


  // Retrive AxParCandidates
  SmartDataPtr<AxPartCandidateVector> 
               candidates (evt,"/Anal/AxPartCandidates");
  if ( ! candidates ) { 
    log << MSG::INFO << "    No AxPartCandidates retrieved" << endreq;
  }
  else {
    
    // Log number of AxPartCandidates retrieved
    log << MSG::INFO << "    Number of AxPartCandidates retrieved   = "
        << candidates->size() << endreq;

    // Example of use of Associator, only for first five particles in
    // container, for details on all possible uses refer to
    // AsctExampleAlgorithm in package Ex/AssociatorExample
    AxPartCandidateVector::iterator iterP = 0;

    int icount=0;
    for( iterP = candidates->begin(); iterP != candidates->end(); iterP++ ) {
      if( icount < 5 ) {
        log << MSG::DEBUG << "    AXCandidate n. " << ++icount << endreq;
        log << MSG::DEBUG << "    Momentum = ( " 
            << (*iterP)->threeMomentum().x()
            << " , " << (*iterP)->threeMomentum().y() 
            << " , " << (*iterP)->threeMomentum().z() << " ) " << endreq;
        //std::cout << *(*iterP) << std::endl;

        MCParticle* mctry = 0;
        StatusCode sc = m_pAsct->retrieveDirect( *iterP, mctry );
        if( sc.isSuccess() ) { 
          log << MSG::DEBUG << "    Corresponding MParticle found " << endreq;
          log << MSG::DEBUG << "    Momentum = " << mctry->fourMomentum().px()
              << " , " << mctry->fourMomentum().py()
              << " , " << mctry->fourMomentum().pz() << " ) " << endreq;
          //std::cout << *mctry << std::endl;
        }
        else {
          log << MSG::DEBUG << "    No associated MCParticle found" << endreq;
        }
      }
    }
  }
  
  

  // Retrive MCParticles
  SmartDataPtr<MCParticleVector> particles (evt,"/MC/MCParticles");
  if ( ! particles ) { 
    log << MSG::INFO << "    No MCParticles retrieved" << endreq;
  }
  else {
    // Log number of AxPartCandidates retrieved
    log << MSG::INFO << "    Number of MCParticles retrieved   = " 
        << particles->size() << endreq;

    // Loop over all particles and find the requested particle, print first
    // level daughters
    MCParticleVector::const_iterator ip = 0;
    for( ip = particles->begin(); ip != particles->end(); ip++ ) {
      int igid = (*ip)->particleID().id(); 
      if( igid == m_pIDSearch ) {
        log << MSG::INFO << "    " << m_pNameSearch << " found " << endreq;
        // std::cout << m_pNameSearch << " found " << *(*ip) << std::endl;
        const SmartRefVector<MCVertex>& decay = (*ip)->decayMCVertices();
        SmartRefVector<MCVertex>::const_iterator iv;
        for( iv = decay.begin(); iv != decay.end(); iv++ ) {
          const SmartRefVector<MCParticle>& daughters = 
                                          (*iv)->daughterMCParticles();
          SmartRefVector<MCParticle>::const_iterator idau;
          log << MSG::INFO << "    particle has " << daughters.size() 
              << " daughters " << endreq;
          log << MSG::DEBUG << "    They are : " << endreq;
          for( idau = daughters.begin(); idau != daughters.end(); idau++ ) {
            igid = (*idau)->particleID().id(); 
            ParticleProperty* pp1 = m_ppSvc->find( igid );
            std::string pname = pp1->particle();
            log << MSG::DEBUG << "    " << pname << endreq;
            //std::cout << "Daughters of particle" << *(*idau) << std::endl;
          } 
        }
      }
    }
  }
    
  /// retrieve the results of PhysSelectAlgorithm
  SmartDataPtr<PhysSel> mysel(eventSvc(), "/Event/Phys/Selection" );

  if ( ! mysel ) {
    log << MSG::INFO << "    Unable to retrieve Axsel data " << endreq;
  }
  else {
    bool lMCFlag = false;
    bool lTKRFlag = false;
    bool lPQUALFlag = false;
    bool lAGRFlag = false;
    bool lSELFlag = false;
    bool lTAGFlag = false;

    std::vector<std::string> selList;    
    mysel->whichPhysSelDecays(selList);
    
    if( selList.size() < 1 ) {
      return StatusCode::SUCCESS;
    }
    
    std::string tmpName = selList[0];
    mysel->results(tmpName,lMCFlag,lTKRFlag,lPQUALFlag,
                                 lSELFlag,lAGRFlag,lTAGFlag);
    //    mysel->results(m_selDec,lMCFlag,lTKRFlag,lPQUALFlag,
    //                             lSELFlag,lAGRFlag,lTAGFlag);
    log << MSG::DEBUG << "    PhysSel results for " << m_selDec << std::endl
        << "                              MCFlag    " << lMCFlag << std::endl
        << "                              TKRFlag   " << lTKRFlag << std::endl
        << "                              PQUALFlag " << lPQUALFlag << std::endl
        << "                              SELFlag   " << lSELFlag << std::endl
        << "                              AGRFlag   " << lAGRFlag << std::endl
        << "                              TAGFlag   " << lTAGFlag << endreq;

    if( mysel->decayIsInMCTree(m_selDec) ) {
      m_nPhysSelMC++;
    }

    if( mysel->decayHasTrkRecon(m_selDec) ) {  
      m_nPhysSelTKR++;
    }

    if( mysel->decayHasTrkPQual(m_selDec) ) {
      m_nPhysSelPQU++;
    }

    if( mysel->decayIsSelected(m_selDec) ) {
      m_nPhysSelSEL++;
    }

    if( mysel->decaySelectedIsMC(m_selDec) ) {
      m_nPhysSelAGR++;
    }

    if( mysel->decayIsFlavourTagged(m_selDec) ) {
      m_nPhysSelTAG++;
    }
  }

  // Example of how to access L0 Decision Unit info
  SmartDataPtr<L0DUReport> L0Report( eventSvc(), "/Event/FE/L0/L0Decis" );
  
  if ( 0 != L0Report ) {
    if ( (*L0Report).decision() ) {
      log << MSG::DEBUG << "The event passed the L0 Decision Unit" << endreq;
      m_nL0Trigger++;
    }
  }

  // End of execution for each event
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DaVinciTestAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Finalize" << endreq;
 
  // Print out counters
  log << MSG::INFO << "    Number of events processed      = " 
      << m_nEvents << endreq;
  log << MSG::INFO << "    Number of events with PhysSeL MC    = "
      << m_nPhysSelMC << endreq;
  log << MSG::INFO << "    Number of events with PhysSeL TKR   = "
      << m_nPhysSelTKR << endreq;
  log << MSG::INFO << "    Number of events with PhysSeL PQUAL = "
      << m_nPhysSelPQU << endreq;
  log << MSG::INFO << "    Number of events with PhysSeL SEL   = "
      << m_nPhysSelSEL << endreq;
  log << MSG::INFO << "    Number of events with PhysSeL AGR   = "
      << m_nPhysSelAGR << endreq;
  log << MSG::INFO << "    Number of events with PhysSeL FTAG  = "
      << m_nPhysSelTAG << endreq;
  
  log << MSG::INFO << "    Number of events passing L0 trigger = "
      << m_nL0Trigger << endreq;

  // End of finalization step
  return StatusCode::SUCCESS;
}
