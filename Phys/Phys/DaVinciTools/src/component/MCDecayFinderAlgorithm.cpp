// $Id: MCDecayFinderAlgorithm.cpp,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $

// Include files 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/Event.h"
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/MCVertex.h"

#include "MCDecayFinderAlgorithm.h"

//--------------------------------------------------------------------
//
//  MCDecayFinderAlgorithm : Retrieves MCParticles objects and look for
//                           1 step decay as defined in the jobOptions.txt
//                           Has as default decay B0->pi+pi-.
//                           Fill Ntuple with found decay infos.
//
//  Author                 : G.Corti  
//
//--------------------------------------------------------------------

//===============================================================
// Factory declaration 
//===============================================================
static const AlgFactory<MCDecayFinderAlgorithm>    Factory;
const IAlgFactory& MCDecayFinderAlgorithmFactory = Factory;


//===============================================================
// Constructor
//===============================================================
MCDecayFinderAlgorithm::MCDecayFinderAlgorithm(const std::string& name, 
                        ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),m_ppSvc(0),  m_nEvents(0), m_nEvSuccess(0) {

  // Declare the algorithm's properties which can be set at run time and their
  // default values
  declareProperty( "ParentOfDecay", m_nameParent = "B0" );
  std::vector<std::string> decay;
  decay.push_back("pi+");
  decay.push_back("pi-");
  declareProperty( "DecayProducts", m_nameProducts = decay );
}


//===============================================================
// Destructor
//===============================================================
MCDecayFinderAlgorithm::~MCDecayFinderAlgorithm() 
{ 
}

//===============================================================
// Initialize
//===============================================================
StatusCode MCDecayFinderAlgorithm::initialize() {

  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initialize" << endreq;

  // Log the defined decay
  std::vector<std::string>::const_iterator iname;
  log << MSG::INFO << "    looking for the following decay : " << endreq;
  log << MSG::INFO << "    " << m_nameParent << " -> "; 
  for( iname = m_nameProducts.begin(); iname != m_nameProducts.end(); 
       iname++ ) {
    log << MSG::INFO << *iname << " "; 
  }
  log << MSG::INFO << endreq;

  // Access the  ParticlePropertySvc
  log << MSG::INFO << "    Looking for Particle Property Service." << endreq;

  StatusCode sc = service("ParticlePropertySvc", m_ppSvc );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to locate Particle Property Service" << endreq;
    return sc;
  }

  // Now declare the ntuple to be filled during processing and put it in output
  // file generated at run time from jobOptions.
  // First create a directory in the file, then define the n-tuple, that in 
  // this case is a column wise n-tuple.
  log << MSG::INFO << "    Defining the ntuple" << endreq;
  NTuple::Directory *dir = 0;
  
  NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");
  if ( !file1 ) {
    log << MSG::ERROR << "    Cannot access file /NTUPLES/FILE1" << endreq;
    return StatusCode::FAILURE;
  }

  if ( !(dir=ntupleSvc()->createDirectory("/NTUPLES/FILE1/MCDECAY")) )   {
    log << MSG::ERROR << "    Cannot create directory /NTUPLES/FILE1/MCDECAY" 
        << endreq;
    return StatusCode::FAILURE;
  }

  // Book n-tuple if not already done
  NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/MCDECAY/100");
  if ( !nt ) {
    nt = ntupleSvc()->book ("/NTUPLES/FILE1/MCDECAY", 100, 
                             CLID_ColumnWiseTuple, "MCDecay");
    if ( !nt ) {
      log << MSG::FATAL << "    Could not book the ntuple 100: MCDecay" 
          << endreq;
      return StatusCode::FAILURE;
    }
    // Add elements to ntuple definition
    StatusCode status;
    status = nt->addItem ("Event", m_eventN);
    status = nt->addItem ("ParentID", m_parentID, 0, 500);
    status = nt->addItem ("ParPx", m_parentPx);
    status = nt->addItem ("ParPy", m_parentPy);
    status = nt->addItem ("ParPz", m_parentPz);
    status = nt->addItem ("ParOrigX", m_originX);
    status = nt->addItem ("ParOrigY", m_originY);
    status = nt->addItem ("ParOrigZ", m_originZ);
    status = nt->addItem ("ParDecX", m_decayX);
    status = nt->addItem ("ParDecY", m_decayY);
    status = nt->addItem ("ParDecZ", m_decayZ);
    status = nt->addItem ("Ndecprod", m_nDecProd, 0, 20);
    status = nt->addItem ("DecPID", m_nDecProd, m_decayPID, 0, 500);
    status = nt->addItem ("DecPx", m_nDecProd, m_decayPx);
    status = nt->addItem ("DecPy", m_nDecProd, m_decayPy);
    status = nt->addItem ("DecPz", m_nDecProd, m_decayPz);
  }
  else {
    log << MSG::INFO << "    The ntuple was already booked" << endreq;
  }
  // Initialization terminated
  return StatusCode::SUCCESS;
}


//===============================================================
// Execution
//===============================================================
StatusCode MCDecayFinderAlgorithm::execute() {

  MsgStream          log( msgSvc(), name() );
  StatusCode         sc = StatusCode::SUCCESS;

  log << MSG::INFO << ">>> Execute" << endreq;

  // Counter for events processed
  m_nEvents++;

  // This should be done in the initalization step but then it would still try
  // to execute the Algorithm: put it here so that it will stop.
  // For convenience of the Algorithm transform the name of the particles to 
  // their Geant ID. 
  ParticleProperty* pp = m_ppSvc->find(m_nameParent);
  if ( !pp ) {
    log << MSG::FATAL << "Parent" << m_nameParent 
        << " not found in ParticlePropertySvc: check the name" << endreq;
    return StatusCode::FAILURE;
  }
  long gIDParent = pp->geantID();
  
  std::vector<std::string>::const_iterator iname;
  std::vector<long> gIDProducts;
  for( iname = m_nameProducts.begin(); 
       iname != m_nameProducts.end(); iname++ ) {
    pp = m_ppSvc->find(*iname);
    if ( !pp ) {
      log << MSG::FATAL << "Decay Product " << *iname 
          << " not found in ParticlePropertySvc: check the name" << endreq;
      return StatusCode::FAILURE;
    }
    gIDProducts.push_back( pp->geantID() );
  }

  // Retrieve EventID
  SmartDataPtr<Event> evt(eventSvc(), "/Event");

  if ( !evt ) {
    log << MSG::ERROR << " Unable to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve MCParticles
  SmartDataPtr<MCParticleVector> particles(eventSvc(), "/Event/MC/MCParticles");

  if ( !particles ) {
    log << MSG::ERROR << " Unable to retrieve MCParticles for this event" 
        << endreq;
    return StatusCode::FAILURE;
  }

  // Loop over all particles and find the parentOfDecay
  long nDecSize = gIDProducts.size();

  long nParent = 0;
  long nParentN = 0;
  long nParentD = 0;

  // Loop over all MCParticles and find parent of Decay
  MCParticleVector::const_iterator iterP;

  std::vector<MCParticle*> parentV;
  for ( iterP = particles->begin() ; iterP != particles->end(); iterP++ ) {
    if ( (*iterP)->particleID().id() == gIDParent ) {
      nParent++;
      // SICb data can have more then one decay vertex...
      MCVertex* origin = (*iterP)->originMCVertex();
      long nDecVtx = (*iterP)->decayMCVertices().size();
      for ( long nn = 0; nn < nDecVtx; nn++ ) {
        MCVertex* decayVtx = (*iterP)->decayMCVertices()[nn];
        long nDaught = decayVtx->daughterMCParticles().size();
        if ( nDaught == nDecSize ) {
          // The parent has the correct number of daughters, now check if they
          // are of requested type and right number for each type
          nParentN++;
          // Flag daughters found
          std::vector<bool> decayFound;
          long ii;
          for ( ii = 0; ii < nDecSize; ii++ ) {
            decayFound.push_back(false);
          }
          
          std::vector<MCParticle*> productsTmp;
          for ( ii = 0; ii < nDecSize; ii++ ) {
            MCParticle* decayP = decayVtx->daughterMCParticles()[ii];
            long idP = decayP->particleID().id();
            for( long in = 0; in < nDecSize; in++ ) {
              if( ( idP == gIDProducts[in] ) && ( decayFound[in] == false ) ) {
                decayFound[in] = true;
                
                // store products in an array
                productsTmp.push_back(decayP);
                break;
              }
            }
            // break to here 
          }
          // Now check if all products have been found and if yes fill ntuple
          // I could define my user class and give it out for later comparison
          long nFound = 0;
          for ( ii = 0; ii < nDecSize; ii++ ) {
            if ( decayFound[ii] ) {
              nFound++;
            }
          }
          if ( nFound == nDecSize ) {
            nParentD++;
            parentV.push_back(*iterP);
            // event ID
            m_eventN = evt->event();
            // parent pID and kinematic
            m_parentID = (*iterP)->particleID().id();
            m_parentPx = (*iterP)->fourMomentum().px();
            m_parentPy = (*iterP)->fourMomentum().py();
            m_parentPz = (*iterP)->fourMomentum().pz();
            // its origin vertex
            m_originX = origin->position().x();
            m_originY = origin->position().y();
            m_originZ = origin->position().z();
            // its decay vertex
            m_decayX = decayVtx->position().x();
            m_decayY = decayVtx->position().y();
            m_decayZ = decayVtx->position().z();
            // Now the decay products, ensuring that we are not looking for 
            // more than 20 decay products, as set for n-tuple at 
            // initialization time
            m_nDecProd = 0;
            std::vector<MCParticle*>::const_iterator idau;
            for ( idau = productsTmp.begin(); idau != productsTmp.end(); 
                  idau++ ) {     
              MCParticle decayp = *(*idau);
              m_decayPID[m_nDecProd] = decayp.particleID().id();
              m_decayPx[m_nDecProd] = decayp.fourMomentum().px();
              m_decayPy[m_nDecProd] = decayp.fourMomentum().py();
              m_decayPz[m_nDecProd] = decayp.fourMomentum().pz();
              m_nDecProd++;
              if ( m_nDecProd > m_nDecProd->range().distance() ) break;
            }
            // Now store the ntuple
            StatusCode snt = ntupleSvc()->writeRecord("/NTUPLES/FILE1/MCDECAY/100");
            if( snt.isFailure() ) {
              log << MSG::ERROR << "Cannot fill id 1" << endreq;
            }
          }
        }
      }
    }
  }

  // Log for steps
  log << MSG::DEBUG << "    Number of parents found                 = " 
      << nParent << endreq;
  log << MSG::DEBUG << "    ... and with correct number of products = " 
      << nParentN << endreq;
  log << MSG::DEBUG << "    ...... and with right type of products  = " 
      << nParentD << endreq;
  
  // For all decays found 
  long size = parentV.size();
  if ( size == 0 ) {
    log << MSG::INFO << "    Not found any of the desidered decay in the event"
        << endreq;
  }
  else {
    log << MSG::INFO << "    Number of desired decay found in the event = " 
        << size << endreq;
  }

  // Counter for successful events
  if ( size > 0 ) {
    m_nEvSuccess++;
  }
  // End of execution for each event
  return sc;
}

//==================================================================
// Finalization
//==================================================================
StatusCode MCDecayFinderAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " >>> Finalize" << endreq;
 
  // Print out counters
  log << MSG::INFO << "     Number of events processed = " << m_nEvents 
      << endreq;
  log << MSG::INFO << "       with decay found         = " << m_nEvSuccess 
      << endreq;


  // End of finalization step
  return StatusCode::SUCCESS;
}
