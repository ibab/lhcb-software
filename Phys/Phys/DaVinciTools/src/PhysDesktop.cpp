// $Id: PhysDesktop.cpp,v 1.1 2002-03-27 20:35:01 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/Event.h"
#include "GaudiKernel/GaudiException.h"

// data
#include "LHCbEvent/AxPartCandidate.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

// local
#include "PhysDesktop.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PhysDesktop base class 
//
// 18/02/2002 : Sandra Amato
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PhysDesktop>           s_factory ;
const        IToolFactory& PhysDesktopFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhysDesktop::PhysDesktop( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : AlgTool ( type, name , parent ),
  m_EDS(0),
  m_ppSvc(0) {

  // Declaring implemented interfaces
  declareInterface<IPhysDesktop>(this);

  // Clean data members
  m_particleNames.clear();
  m_ids.clear();
  m_confLevels.clear();

  // Declare properties
  //                    loading conditions
  m_inputLocn.clear();
  std::string location = ParticleLocation::Production;
  m_inputLocn.push_back(location);

  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "ConfLevelCuts", m_confLevels );
  //                    input & output locations
  declareProperty( "InputProtoParticles",
                   m_protoLocn = "/Event/Anal/AxPartCandidates");
  declareProperty( "InputPrimaryVertices", 
                   m_primVtxLocn = VertexLocation::Primary );
  declareProperty( "InputLocations", m_inputLocn );
  declareProperty( "OutputLocation", m_outputLocn = "/Event/Phys/User");
  
}

//=============================================================================
// Initialize method, retrieve necessary services and transfer 
// particle names to particle id codes
//=============================================================================
StatusCode PhysDesktop::initialize() {
  
  MsgStream log( msgSvc(), name() );
  
  // Retrieve the data service
  m_EDS = 0;
  StatusCode sc = StatusCode::FAILURE;
  sc = serviceLocator()->service("EventDataSvc", m_EDS, true);
  if( sc.isFailure ()) {
    throw GaudiException("EventDataSvc Not Found", "PhysDesktopException", 
                         StatusCode::FAILURE );
  }

  // This tool needs to use internally the ParticlePropertySvc
  // to retrieve the mass to be used and convertions between names and 
  // ids
  m_ppSvc = 0;
  sc = serviceLocator()->service( "ParticlePropertySvc", m_ppSvc );
  if( sc.isFailure ()) {
    throw GaudiException("ParticlePropertySvc Not Found", 
                         "PhysDesktopException", StatusCode::FAILURE );
  }
    
  // Retrieve the PDG Code of required particles 
  m_ids.clear();

  std::vector<std::string>::const_iterator ipartsName;
  if (m_particleNames.size() == 0) {
    log << MSG::ERROR << " ParticleNames is empty "  
        << "Please, initialize it in your job options file" <<  endreq;
    return StatusCode::FAILURE;
  }
  else {
    for ( ipartsName = m_particleNames.begin(); 
          ipartsName != m_particleNames.end(); ipartsName++ ) {
      ParticleProperty* partProp = m_ppSvc->find( *ipartsName );
      if ( 0 == partProp )   {
        log << MSG::ERROR << "Cannot retrieve properties for particle \"" 
            << *ipartsName << "\" " << endreq;
        return StatusCode::FAILURE;
      }
      m_ids.push_back( partProp->jetsetID() );
    }
  }
  
  // Test the Confidence Level Cuts Vector 
  if (m_confLevels.size() == 0) {
    log << MSG::ERROR << "PhysDesktopConfLevelCuts is empty " 
        << "Please, initialize it in your job options file " << endreq;
    return StatusCode::FAILURE;
  }

  // Test if the the Confidence Level Cuts Vector and ParticleNames 
  // have the same size
  if (m_confLevels.size() != m_particleNames.size() ) {
    log << MSG::ERROR << "PhysDesktopConfLevelCuts size is  " 
        << "different from  PhysDesktopParticleNames" << endreq;
    return StatusCode::FAILURE;
  }

  // Print out the conditions
  std::vector<int>::const_iterator ipartsID;
  for ( ipartsID = m_ids.begin(); ipartsID != m_ids.end(); ipartsID++ ) {
    log << MSG::DEBUG << "    Particle ID Requested " << (*ipartsID) << endreq;
  }

  return StatusCode::SUCCESS;
  
}

//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* PhysDesktop::eventSvc() const
{
  return m_EDS;
}

//=============================================================================
// Provides a reference to its internal container of particles
//=============================================================================
const ParticleVector& PhysDesktop::particles()
{
  return m_parts;
}

//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const VertexVector& PhysDesktop::vertices()
{
  return m_verts;
}

//============================================================================
// Clean local data, this is called by the handle method and at finalization
//============================================================================
StatusCode PhysDesktop::cleanDesktop() 
{

  MsgStream log( msgSvc(), name() );

  log << MSG::DEBUG << "cleanDesktop()" << endreq;
  log << MSG::DEBUG << "Removing all particles from desktop" << endreq;

  // The following is not necessary: to enter the Desktop particles have
  // done a new
  //  while ( m_parts.size() > 0 ) {
  //    Particle* ipart = m_parts.back();
  //    m_parts.pop_back();
  //    if( ipart->desktop() == 1 ) {
  //      delete ipart;
  //    }
  //  }
  m_parts.clear();
  
  log << MSG::DEBUG << "Removing all vertices from desktop" << endreq;
  
  //  while ( m_verts.size() > 0 ) {
  //    Vertex* ivert = m_verts.back();
  //    m_verts.pop_back();
  //    if( ivert->desktop() == 1 ) {
  //      delete ivert;
  //    }
  //  }
  m_verts.clear();

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize 
//=============================================================================
StatusCode PhysDesktop::finalize()
{
  cleanDesktop();
  return StatusCode::SUCCESS;
}


//=============================================================================
// Create a new particle in the DeskTop
//=============================================================================
Particle* PhysDesktop::createParticle( Particle* partToSave ){

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << "createParticle in desktop" << endreq;

  // Input particle is given check if it already exist in the stack
  if( ( 0 != partToSave ) && ( 0 != partToSave->desktop() ) ) {
    log << MSG::DEBUG << "Input particle momentum = " 
        << partToSave->momentum().px() << " ," 
        << partToSave->momentum().py() << " ," 
        << partToSave->momentum().pz() << endreq;
    log << MSG::DEBUG << "Particle address " << partToSave << endreq;
    return partToSave;
  }

  // Create new particle on the heap
  Particle* saveP = new Particle();
  log << MSG::DEBUG << "New particle momentum = " 
      << saveP->momentum().px() << " ," 
      << saveP->momentum().py() << " ," 
      << saveP->momentum().pz() << endreq;

  // Input Particle from stack is given as input to fill new created particle
  if( ( 0 != partToSave) && ( 0 == partToSave->desktop() ) ) {
    // Copy contents to newly created particle
    Particle& savePcont = *saveP;
    savePcont = *partToSave;
    log << MSG::DEBUG << "Input particle momentum = " 
        << saveP->momentum().px() << " ," 
        << saveP->momentum().py() << " ," 
        << saveP->momentum().pz() << endreq;
     // Check if link to endProducts exist and set it
    if( 0 != partToSave->endVertex() ) {
      Vertex* saveV = createVertex( partToSave->endVertex() );
      saveP->setEndVertex(saveV);
    }
    // Link to outgoing particles is followed through the createVertex
    // Link to protoParticles will be correct because they are in the heap
    // so their pointer is valis
  }

  // Put in the desktop container
  saveP->setDesktop(1);
  log << MSG::DEBUG << "Momentum of new particle in desktop = "
      << saveP->momentum().px() << " ," 
      << saveP->momentum().py() << " ," 
      << saveP->momentum().pz() << endreq;
  log << MSG::DEBUG << "Address of new particle in desktop = "
      << saveP << endreq;
  m_parts.push_back(saveP);     
  return saveP;

}
//=============================================================================
// Create a new vertex
//=============================================================================
Vertex* PhysDesktop::createVertex( Vertex* vtxToSave ){

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << "createVertex in desktop" << endreq;

  // Input vertex is given check if it already exist in the stack
  if( ( 0 != vtxToSave ) && ( 0 != vtxToSave->desktop() ) ) {
    log << MSG::DEBUG << "Input vertex position = " 
        << vtxToSave->position().x() << " ," 
        << vtxToSave->position().y() << " ," 
        << vtxToSave->position().z() << endreq;
    log << MSG::DEBUG << "Vertex address " << vtxToSave << endreq;
    return vtxToSave;
  }

  // Create new vertex on the heap
  Vertex* saveV = new Vertex();
  log << MSG::DEBUG << "New vertex position = " 
      << saveV->position().x() << " ," 
      << saveV->position().y() << " ," 
      << saveV->position().z() << endreq;
  
  // Input vertex from stack is given as input to fill new created vertex
  if( ( 0 != vtxToSave ) && ( 0 == vtxToSave->desktop() ) ) {
    log << MSG::DEBUG << "Input vertex position = " 
        << saveV->position().x() << " ," 
        << saveV->position().y() << " ," 
        << saveV->position().z() << endreq;
    // Copy contents to newly created vertex
    Vertex& saveVcont = *saveV;
    saveVcont = *vtxToSave;
    saveV->clearProducts();
    // Check if link to endProducts exist and set it
    SmartRefVector<Particle> outP = vtxToSave->products();
    SmartRefVector<Particle>::iterator ip;
    for( ip = outP.begin(); ip != outP.end(); ip++ ) {
      Particle* saveP = createParticle( *ip );
      saveV->addToProducts(saveP);
    }
  }

  // Put in the desktop container
  saveV->setDesktop(1);
  log << MSG::DEBUG << "Position of new vertex in desktop = "
      << saveV->position().x() << " ," 
      << saveV->position().y() << " ," 
      << saveV->position().z() << endreq;
  log << MSG::DEBUG << "Address of new vertex in desktop = "
      << saveV << endreq;
  m_verts.push_back(saveV);     
  return saveV;

}


//=============================================================================
// Save all particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop() {

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << " Save all new particles and vertices in desktop " 
      << endreq;
  
  return saveDesktop( m_parts, m_verts );

}

//=============================================================================
// Save all specified particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop( ParticleVector& pToSave, 
                                     VertexVector& vToSave) {
  
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << "Save specified particles and vertices " << endreq;
 
  //  Register the particles containers in the store
  Particles* particlesToSave = new Particles();

  ParticleVector::const_iterator icand = 0;
  for( icand = pToSave.begin(); icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( 0 == (*icand)->parent() ) {
      particlesToSave->insert(*icand);
      // Flag them as being in TES, not necessary use parent() later as well
      //(*icand)->setDesktop(2);
    }
  }

  std::string location = m_outputLocn+"/Particles";

  log << MSG::DEBUG << "Saving " << particlesToSave->size()
      << " new particles in " << location << " from " << pToSave.size() 
      << " total particles in desktop " << endreq;

  StatusCode sc = eventSvc()->registerObject(location,particlesToSave);

  if ( sc.isFailure() ) {
    delete particlesToSave;
    log << MSG::ERROR  
        << "    Unable to register" << location << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Removing particles saved to TES from desktop" 
        << endreq;
    pToSave.clear();
  }

  // Register the vertices in the store
  Vertices* verticesToSave = new Vertices();
  VertexVector::iterator iver = 0;
  for( iver = vToSave.begin(); iver != vToSave.end(); iver++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( 0 == (*iver)->parent() ) {
      verticesToSave->insert(*iver);
    }
  }

  location = m_outputLocn+"/Vertices";

  log << MSG::DEBUG << "Saving " << verticesToSave->size()
      << " new vertices in " << location << " from " << vToSave.size() 
      << " vertices in desktop " << endreq;

  sc = eventSvc()->registerObject(location,verticesToSave);
  
  if ( sc.isFailure() ) {
    delete verticesToSave;
    log << MSG::ERROR  
        << "    Unable to register" << location << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Removing vertices saved to TES from desktop" 
        << endreq;
    vToSave.clear();
  }
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Save only this list of particles and their tree in TES
//=============================================================================
StatusCode PhysDesktop::saveTrees(ParticleVector& pToSave) {

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>> Hello from PhysDesktop SaveTrees " << endreq;

  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  ParticleVector::iterator icand = 0;
  for( icand = pToSave.begin(); icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    findAllTree( *icand, allpToSave, allvToSave);
  }

  return saveDesktop( allpToSave, allvToSave );
  
}

//=============================================================================
// Save only this list of vertices and their trees in TES
//=============================================================================
StatusCode PhysDesktop::saveTrees(VertexVector& vToSave){
  
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>> Hello from PhysDesktop SaveTrees " << endreq;

  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  VertexVector::iterator ivert = 0;
  for( ivert = vToSave.begin(); ivert != vToSave.end(); ivert++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    findAllTree( *ivert, allpToSave, allvToSave);
  }

  return saveDesktop( allpToSave, allvToSave );
  
}

//=============================================================================
// Find all particle and vertices connected to this tree
//=============================================================================
void PhysDesktop::findAllTree( Particle* part, ParticleVector& parts,
                               VertexVector& verts ) {

  if( 0 == part->endVertex() ) {
    return;
  }
  verts.push_back( part->endVertex() );
  findAllTree( part->endVertex(), parts, verts );
  return;

}

//=============================================================================
// Find all particle and vertices connected to this tree
//=============================================================================
void PhysDesktop::findAllTree( Vertex* vert, ParticleVector& parts,
                               VertexVector& verts ) {

  SmartRefVector<Particle> prod = vert->products();
  for( SmartRefVector<Particle>::iterator ip = prod.begin();
       ip != prod.end(); ip++ ) {
    parts.push_back(*ip);
    if( 0 != (*ip)->endVertex() ) {
    findAllTree( (*ip)->endVertex(), parts, verts );
    }
    
  }
  return;
}

//=============================================================================
// Save only particles in desktop corresponding to this code
//=============================================================================
StatusCode PhysDesktop::saveTrees( int partid ) {
 
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>> Hello from PhysDesktop saveParticles " 
      << "type = " << partid << endreq;

  ParticleVector pToSave;
  ParticleVector::iterator icand = 0;
  for( icand = m_parts.begin(); icand != m_parts.end(); icand++ ) {
    if( ((*icand)->particleID().id()) == partid ) {
      pToSave.push_back(*icand);
    }
  }
  if( pToSave.size() > 0 ) {
    return saveTrees( pToSave );
  }

  return StatusCode::SUCCESS;

}


//=============================================================================
// Method retrieving the data from the store.
// Automatically called by the base class when notified that a new
// event came along
//=============================================================================
StatusCode PhysDesktop::getInput(){
  
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>> Hello from getInput " << endreq;

  cleanDesktop();
  
  // Retrieve Vertices from previous processing
  if( "" == m_primVtxLocn ) {
    log << MSG::DEBUG << "Not loading any primary vertices" 
        << endreq;
  }
  else {    
    SmartDataPtr<Vertices> verts ( eventSvc(), m_primVtxLocn );
    if( ! verts ) {
      log << MSG::INFO << " Unable to retrieve vertices from " 
          << m_primVtxLocn << endreq;
    }
    else if( 0 == verts->size() ) {
      log << MSG::INFO << " No vertices retrieved from  " 
          << m_primVtxLocn << endreq;
    }
    else { 
      log << MSG::DEBUG << "    Number of primary vertices  = " 
          << verts->size() << endreq;

      Vertices::iterator ivert = 0;
      int count = 0;
      for( ivert = verts->begin(); ivert != verts->end(); ivert++ ) {
        count++;
        log << MSG::DEBUG << "    Vertex coordinates = ( " 
            << (*ivert)->position().x() 
            << " , " << (*ivert)->position().y() 
            << " , " << (*ivert)->position().z() << " ) " << endreq;
      
        log << MSG::DEBUG << "    Vertex ChiSquare = " << (*ivert)->chi2() 
            << endreq;  
        // Set that they came from TES ( not necessary, check parent() )
        (*ivert)->setDesktop(2);
        m_verts.push_back(*ivert);
        //log << MSG::DEBUG << " Container " << (*ivert)->parent() << endreq;
      }
    }
  }
  
  log << MSG::DEBUG << "Number of Vertices from " << m_primVtxLocn
      << m_verts.size() << endreq;

  // Make particles from AxPartCandidate, this will be replaced with
  // ProtoParticles
  // The only place where a particleID + CL is applied at this 
  // point is for ProtoParticles
  if( "" == m_protoLocn ) {
    log << MSG::DEBUG << "Not making any Particle from ProtoParticles" 
        << endreq;
  }
  else {
    SmartDataPtr<AxPartCandidateVector> axparts ( eventSvc(), m_protoLocn );
    if ( ! axparts ) { 
      log << MSG::INFO << " Unable to retrieve AxPartCandidates from "
          << m_protoLocn << endreq;
    }
    else if ( 0 == axparts->size() ) {
      log << MSG::INFO << " No AxPartCandidates retrieved from "
          << m_protoLocn << endreq;
    }      
    else {
      // Log number of AxPartCandidates retrieved
      log << MSG::INFO << "    Number of AxPartCandidateVector retrieved from "
          << m_protoLocn << " = " << axparts->size() << endreq;
    
      /*
      const_iterator icand = 0;
      std::vector<int>::iterator ipartsID;
      for( icand = partProd->begin(); icand != partProd->end(); icand++ ) {
        int count=0;
        for ( ipartsID = m_ids.begin(); ipartsID != m_ids.end(); 
              ipartsID++ ) {
          count++;
        
          if ( (*icand)->particleID().id() == (*ipartsID) &&
               (*icand)->confLevel()>= m_confLevels[count-1]) {
          //Test if this particle was already retrieved from the User area
          //          ParticleVector::const_iterator iuser = 0;
          // for( iuser = m_parts.begin(); 
          //      iuser != m_parts.begin()+numberOfPartsFromUser; 
          //     iuser++ ) {
          //  if ((*iuser)->key() != (*icand)->key()) {
              m_parts.push_back(*icand);
              //  }
            
              // }
          
        }      
      }
      */
    }
  }

  log << MSG::DEBUG << "Number of Particles from AxPartCandidates = " 
      << m_parts.size() << endreq;

  // Retrieve Particles & their Vertices from all previous processing
  for( std::vector<std::string>::iterator iloc = m_inputLocn.begin(); 
       iloc != m_inputLocn.end(); iloc++ ) {

    std::string location = *iloc;

    SmartDataPtr<Particles> parts( eventSvc(), location );
    if ( ! parts ) { 
      log << MSG::INFO << "Unable to retrieve Particles from " 
          << location << endreq;
    }
    else if( 0 == parts->size() ) {
      log << MSG::INFO << "No Particles retrieved from " 
          << location << endreq;
    }      
    else {
    
      // Log number of Particles retrieved
      log << MSG::INFO << "    Number of Particles retrieved from "
          << location << " = " << parts->size() << endreq;

      Particles::iterator icand = 0;
      std::vector<int>::iterator ipartsID;
      for( icand = parts->begin(); icand != parts->end(); icand++ ) {
        int count=0;
        for ( ipartsID = m_ids.begin(); ipartsID != m_ids.end(); 
              ipartsID++ ) {
          count++;
          if ( (*icand)->particleID().id() == (*ipartsID) &&
               (*icand)->confLevel()>= m_confLevels[count-1]) {
            (*icand)->setDesktop(2);
            m_parts.push_back(*icand);
            //log << MSG::DEBUG << "parent " << (*icand)->parent() << endreq;
            Vertex* vtx = (*icand)->endVertex();
            if( 0 != vtx ) {
              // get the vertex and all of its tree, unless already in
              // Desktop use findAllTree( vtx, allParts, allVtxs ), then
              // check that particle&vertices not already in desktop
              // For the moment only load decay of a particle, it means
              // other particles will need to be loaded
              m_verts.push_back( vtx );
              vtx->setDesktop(2);
            }
          }
        }
      }
    }
    // Save the number of Particles saved from User area
    log << MSG::DEBUG << "Number of Particles from " << location << " = " 
        << m_parts.size() << endreq;
  }
  log << MSG::DEBUG << "    Total number of particles " << m_parts.size()
      << endreq;
  log << MSG::DEBUG << "    Total number of vertices " << m_verts.size()
      << endreq; 

  return StatusCode::SUCCESS;
  
}


