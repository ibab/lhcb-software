// $Id: DaVinciTestAlgorithm.cpp,v 1.1 2002-02-08 19:08:10 gcorti Exp $
#define DAVINCITESTALGORITHM_CPP 

// Include files
// from STL
#include <math.h>

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiTools/IAssociator.h"

// from Event 
#include "LHCbEvent/Event.h"
#include "LHCbEvent/AxPartCandidate.h"
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/RefTable.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PhysEvent/VtxCandidate.h"
#include "PhysEvent/PhysSel.h"
#include "Event/L0DUReport.h"

// from DaVinci
#include "DaVinciTools/IMCUtilityTool.h"
#include "DaVinciSicb/IPhysSelTool.h"
#include "DaVinciSicb/SelComb.h"
#include "DaVinciSicb/FlavourTag.h"

// local
#include "DaVinciTestAlgorithm.h"
//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciTestAlgorithm
// 
// Example of how to access various info from DaVinci
// 
// 19/06/2001 : Gloria Corti
// 07/02/2002 : modified to include use of new tools (G.Corti)
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
  m_pIDSearch(0),
  m_daugID(0),
  m_nEvents(0),
  m_nPhysSelMC(0),
  m_nPhysSelTKR(0),
  m_nPhysSelPQU(0),
  m_nPhysSelSEL(0),
  m_nPhysSelAGR(0),
  m_nPhysSelTAG(0),
  m_nL0Trigger(0),
  m_bIDLower(71),
  m_bIDUpper(78),
  m_pAsct(0),
  m_pSelTool(0),
  m_pUtilTool(0),
  m_ppSvc(0) {
 
    m_daugName.clear();
    declareProperty( "SearchParticle", m_pNameSearch = "B0" );
    declareProperty( "DecayProducts", m_daugName );
    declareProperty( "MCUTilityTool", m_nameUtilTool = "MCUtilityTool" );

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
  // Note that the ToolSvc is now available via the toolSvc() method
  //           you can hardcode the type of a tool or set  in the job options  
  StatusCode sc = toolSvc()->retrieveTool("AxPart2MCParticleAsct", m_pAsct);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve Associator tool" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool("PhysSelTool", m_pSelTool);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve PhysSel helper tool" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool(m_nameUtilTool, m_pUtilTool);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to retrieve " << m_nameUtilTool << endreq;
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
  ParticleProperty* partProp = m_ppSvc->find( m_pNameSearch );
  m_pIDSearch = partProp->geantID();
  
  // The same for decay products notice that the tool only works for 
  // direct decays
  std::vector<std::string>::const_iterator idau;
  for ( idau = m_daugName.begin(); idau != m_daugName.end(); idau++ ) {
    partProp = m_ppSvc->find( *idau );
    if ( 0 == partProp )   {
      log << MSG::ERROR << "Cannot retrieve properties for particle \"" 
          << *idau << "\" " << endreq;
      return StatusCode::FAILURE;
    }
    m_daugID.push_back( partProp->geantID() );
  }
  

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

    VtxCandidateVector::iterator ivert = 0;
    for( ivert = vertices->begin(); ivert != vertices->end(); ivert++ ) {
      log << MSG::DEBUG << "    Vertex coordinates = ( " 
          << (*ivert)->position().x() 
          << " , " << (*ivert)->position().y() 
          << " , " << (*ivert)->position().z() << endreq;
      log << MSG::DEBUG << "    Vertex ChiSquare = " << (*ivert)->chiSquare() 
          << " ) " << endreq;  
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
    AxPartCandidateVector::iterator icand = 0;
    int icount=0;
    for( icand = candidates->begin(); icand != candidates->end(); icand++ ) {
      if( icount < 5 ) {
        log << MSG::DEBUG << "    AXCandidate n. " << ++icount << endreq;
        log << MSG::DEBUG << "    Momentum = ( " 
            << (*icand)->threeMomentum().x()
            << " , " << (*icand)->threeMomentum().y() 
            << " , " << (*icand)->threeMomentum().z() << " ) " << endreq;

        MCParticle* mctry = 0;
        StatusCode sc = m_pAsct->retrieveDirect( *icand, mctry );
        if( sc.isSuccess() ) { 
          log << MSG::DEBUG << "    Corresponding MParticle found " << endreq;
          log << MSG::DEBUG << "    Momentum = " << mctry->fourMomentum().px()
              << " , " << mctry->fourMomentum().py()
              << " , " << mctry->fourMomentum().pz() << " ) " << endreq;
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
    // Log number of MCParticles retrieved
    log << MSG::INFO << "    Number of MCParticles retrieved   = " 
        << particles->size() << endreq;

    // Loop over all particles
    MCParticleVector::const_iterator ipart = 0;
    for( ipart = particles->begin(); ipart != particles->end(); ipart++ ) {

      // Find the requested particle
      if( (*ipart)->particleID().id() == m_pIDSearch ) {
        log << MSG::INFO << "    " << m_pNameSearch << " found " << endreq;

        // Now use tool to check if particle decays as wanted
        bool result = m_pUtilTool->matchDecayTree( *ipart, m_daugID );
        if ( result ) {
          log << MSG::INFO << "Found requested decay tree" << endreq;
        }
        else {
          log << MSG::INFO << "Requested decay not found" << endreq;
        }
        
        // Use tool to print all decay chain
        m_pUtilTool->printDecayTree( 0, " |", *ipart );
      }
      
      // Retrieve full history of a particle
      bool fromB = false;
      std::vector<long> ancestors;      
      StatusCode scode = m_pUtilTool->history(*ipart,ancestors);
      if ( scode.isSuccess() ) {
        for ( unsigned long i=0; i<2; i++ ) {
          int gid = ancestors[i];
          if( (gid >= m_bIDLower) && (gid<= m_bIDUpper) ) {
            fromB = true;
          }
        } 
      }
      if( fromB ) {
        //log << MSG::INFO << "This particle has b as ancestor" << endreq;
        m_pUtilTool->printHistory(*ipart);
      }
      
    }
  }
    
  /// Retrieve the results of PhysSelectAlgorithm 
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
    
    std::string selDec = selList[0];

    mysel->results(selDec,lMCFlag,lTKRFlag,lPQUALFlag,
                                 lSELFlag,lAGRFlag,lTAGFlag);
    log << MSG::DEBUG << "    PhysSel results for " << selDec << std::endl
        << "                              MCFlag    " << lMCFlag << std::endl
        << "                              TKRFlag   " << lTKRFlag << std::endl
        << "                              PQUALFlag " << lPQUALFlag << std::endl
        << "                              SELFlag   " << lSELFlag << std::endl
        << "                              AGRFlag   " << lAGRFlag << std::endl
        << "                              TAGFlag   " << lTAGFlag << endreq;

    if( mysel->decayIsInMCTree(selDec) ) {
      m_nPhysSelMC++;
      //log << MSG::DEBUG << "                             "
      //    << "The MCDecay is present" << std::endl;
    }

    if( mysel->decayHasTrkRecon(selDec) ) {  
      m_nPhysSelTKR++;
      //log << MSG::DEBUG << "                             "
      //    << "All stable particles are reconstructed" << std::endl;
    }

    if( mysel->decayHasTrkPQual(selDec) ) {
      m_nPhysSelPQU++;
      //log << MSG::DEBUG << "                             "
      //    << "and are of Physics Quality" << std::endl;
    }

    if( mysel->decayIsSelected(selDec) ) {
      m_nPhysSelSEL++;
      //log << MSG::DEBUG << "                             "
      //    << "the event passed the specified offline selection" << std::endl;
    }
 
    if( mysel->decaySelectedIsMC(selDec) ) {
      m_nPhysSelAGR++;
      //log << MSG::DEBUG << "                             "
      //    << "and a selected comb matches with the MCDecay" << std::endl;
    }

    //log << MSG::DEBUG << endreq;

    if( mysel->decayIsFlavourTagged(selDec) ) {
      m_nPhysSelTAG++;
      log << MSG::DEBUG << "One sel comb has been flavour tagged" << endreq;

      /// Now recover the information about the flavour tag
      std::vector<FlavourTag> ftags;
      m_pSelTool->fTagging( ftags);
      log << MSG::DEBUG << "ftags.size = " << ftags.size() << endreq;
      std::vector<FlavourTag>::iterator elem;
      for(elem=ftags.begin(); elem!=ftags.end(); elem++){
        FlavourTag iftag = *elem;
        log << MSG::INFO << " FlavourTag = " << std::endl
            << "       tagType = " << iftag.tagType() 
            << " bSign = " << iftag.bSign()
            << " tParticle P = " << (iftag.tParticle())->fitMomentum()
            << " selComb = " << iftag.selComb() << endreq;
        /// Now retrieve the MC B particle that has been tagged
        int isel = iftag.selComb();
        std::vector<SelComb> mySelComb;
        m_pSelTool->selCombs( mySelComb );
        const MCParticle* pmc = mySelComb[isel].selMCHead();
        log << MSG::INFO << "pID = " << pmc->particleID().id() 
            << " Energy = " << pmc->fourMomentum().e() << endreq;
      }
    }
  }

  // Example of how to access L0 Decision Unit info
  /*  SmartDataPtr<L0DUReport> L0Report( eventSvc(), "/Event/FE/L0/L0Decis" );
  
  if ( 0 != L0Report ) {
    if ( (*L0Report).decision() ) {
      log << MSG::DEBUG << "The event passed the L0 Decision Unit" << endreq;
      m_nL0Trigger++;
    }
    }*/

  // End of execution for each event
  return StatusCode::SUCCESS;
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
