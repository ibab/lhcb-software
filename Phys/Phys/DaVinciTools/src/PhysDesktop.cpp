// $Id: PhysDesktop.cpp,v 1.4 2002-05-20 23:15:32 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"

// data
#include "Event/EventHeader.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

// local
#include "PhysDesktop.h"
#include "DaVinciTools/IParticleMaker.h"

/**--------------------------------------------------------------------------
 * Implementation file for class : PhysDesktop base class 
 *
 * 18/02/2002 : Sandra Amato
 *-----------------------------------------------------------------------------
 */

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
    m_pMaker(0){
  
  // Declaring implemented interfaces
  declareInterface<IPhysDesktop>(this);
  declareInterface<IIncidentListener>(this);
  
  // Declare properties
  //                    loading conditions
  
  // Type of input particles (Ax, MC, Proto) maker.
  declareProperty( "ParticleMakerType",m_pMakerType="" );
  
  //                    input & output locations
  declareProperty( "InputPrimaryVertices", 
                   m_primVtxLocn = VertexLocation::Primary );
  m_inputLocn.clear();
  declareProperty( "InputLocations", m_inputLocn );
  declareProperty( "OutputLocation", m_outputLocn = "/Event/Phys/User");
  
}

//=============================================================================
// Initialize method, retrieve necessary services 
//=============================================================================
StatusCode PhysDesktop::initialize() {
  
  MsgStream log( msgSvc(), name() );

  // Register to the Incident service to be notified at the end of one event
  IIncidentSvc* incsvc=0;
  StatusCode sc = StatusCode::FAILURE;
  sc = service("IncidentSvc", incsvc, true);
  if( sc.isSuccess() ) {
    incsvc->addListener(this,"EndEvent",100);
  }
  else {
    log << MSG::FATAL << " Unable to locate Incident Service" << endreq;
    return sc;
  }  
  
  
  // Retrieve the data service
  sc = StatusCode::FAILURE;
  sc = service("EventDataSvc", m_EDS, true);  

  if( sc.isFailure() ) {
    log << MSG::FATAL << " Unable to locate Event Data Service" << endreq;
    return sc; 
  }
  
  if ( m_pMakerType == "" ){
    log << MSG::WARNING << " No ParticleMaker requested in job options" 
        << endreq;
    log << MSG::WARNING << " Only previously produced particles will be loaded"
        << endreq;
  }
  else{
    
    // Retrieve the ParticleMaker tool:
    log << MSG::INFO << " Using " << m_pMakerType << "to make particles"
        << endreq;
    sc = StatusCode::FAILURE;
    sc = toolSvc()->retrieveTool(m_pMakerType, m_pMaker,this); 
   
    if(sc.isFailure()) {
      log << MSG::FATAL << " Unable to retrieve " << m_pMakerType << endreq;
      return sc;
    }
  }
  
  // Check if InputLocation has been set
  if( m_inputLocn.size() == 0 ) {
    log << MSG::INFO << "No Input from previous processing requested"
        << endreq;
  }
  else {
    log << MSG::INFO << "Particles will be loaded from ";

    for( std::vector<std::string>::iterator iloc = m_inputLocn.begin(); 
       iloc != m_inputLocn.end(); iloc++ ) {
    
      log << MSG::INFO << "   ==> *iloc ";
    }
    log << MSG::INFO << endreq;
  }

  return StatusCode::SUCCESS;
  
}

//=============================================================================
// Implementation of Listener interface 
//=============================================================================
void PhysDesktop::handle(const Incident&) 
{
  cleanDesktop();
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
  
  // Some particle have been saved to the TES, so they belong to it
  // others do not and need to be deleted by the PhysDesktop
  log << MSG::VERBOSE << "Number of particles before cleaning = "
      << m_parts.size() << endreq;

  int iTEScount = 0;
  while ( m_parts.size() > 0 ) {
    Particle* ipart = m_parts.back();
    m_parts.pop_back();
    // Particles in KeyedContainers (=>TES) have parent 
    if( ipart->parent() ) {
      iTEScount++;
    }
    else {
      delete ipart;
    }
  }
  
  log << MSG::DEBUG << "Particle in TES = " << iTEScount << endreq;
  
  //m_parts.clear();
  
  log << MSG::DEBUG << "Removing all vertices from desktop" << endreq;
  log << MSG::VERBOSE << "Number of vertices before cleaning = "
      << m_parts.size() << endreq;
  
  iTEScount = 0;
  while ( m_verts.size() > 0 ) {
    Vertex* ivert = m_verts.back();
    m_verts.pop_back();
    if( ivert->parent() ) {
      iTEScount++;
    }
    else {
      delete ivert;
    }
  }
  //m_verts.clear();
  
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
  
  // Input Particle from stack is given as input to fill newly created particle
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
    // Link to originators will be correct because they are in the heap
    // so their pointer is valid
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
  
  // Register the particles containers in the store
  Particles* particlesToSave = new Particles();
  
  ParticleVector::const_iterator icand = 0;
  for( icand = pToSave.begin(); icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( 0 == (*icand)->parent() ) {
      particlesToSave->insert(*icand);
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
  //else {
  //  log << MSG::DEBUG << "Removing particles saved to TES from desktop" 
  //      << endreq;
  //  pToSave.clear();
  //}
  
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
  //else {
  //  log << MSG::DEBUG << "Removing vertices saved to TES from desktop" 
  //      << endreq;
  //  vToSave.clear();
  //}
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Save only this list of particles and their tree in TES
// If bottom of trees is in TES it will be copied in the new location
//=============================================================================
StatusCode PhysDesktop::saveTrees(ParticleVector& pToSave) {
  
  MsgStream          log( msgSvc(), name() );

  log << MSG::DEBUG << " PhysDesktop SaveTrees(ParticleVector)" << endreq;
  
  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  ParticleVector::iterator icand = 0;
  for( icand = pToSave.begin(); icand != pToSave.end(); icand++ ) {
    // Find all descendendant from this particle
    findAllTree( *icand, allpToSave, allvToSave);
  }

  return saveDesktop( allpToSave, allvToSave );
  
}

//=============================================================================
// Save only this list of vertices and their trees in TES
//=============================================================================
StatusCode PhysDesktop::saveTrees(VertexVector& vToSave){
  
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << " PhysDesktop SaveTrees(VertexVector) " << endreq;
  
  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  VertexVector::iterator ivert = 0;
  for( ivert = vToSave.begin(); ivert != vToSave.end(); ivert++ ) {
    // Find all descendendant from this vertex
    findAllTree( *ivert, allpToSave, allvToSave);
  }
  
  return saveDesktop( allpToSave, allvToSave );
  
}

//=============================================================================
// Find all particle and vertices connected to this tree
//=============================================================================
void PhysDesktop::findAllTree( Particle* part, ParticleVector& parts,
                               VertexVector& verts ) {
  
  parts.push_back( part );
  if( 0 == part->endVertex() ) {
    return;
  }
  //verts.push_back( part->endVertex() );
  findAllTree( part->endVertex(), parts, verts );
  return;
  
}

//=============================================================================
// Find all particle and vertices connected to this tree
//=============================================================================
void PhysDesktop::findAllTree( Vertex* vert, ParticleVector& parts,
                               VertexVector& verts ) {
  
  verts.push_back( vert );
  SmartRefVector<Particle> prod = vert->products();
  for( SmartRefVector<Particle>::iterator ip = prod.begin();
       ip != prod.end(); ip++ ) {
    //parts.push_back(*ip);
    //if( 0 != (*ip)->endVertex() ) {
    //  findAllTree( (*ip)->endVertex(), parts, verts );
    //}
    findAllTree( *ip, parts, verts );
  }
  return;
}

//=============================================================================
// Save only particles in desktop corresponding to this code
//=============================================================================
StatusCode PhysDesktop::saveTrees( int partid ) {
  
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << "PhysDesktop saveParticles(pid code)" 
      << "type = " << partid << endreq;
  
  ParticleVector pToSave;
  ParticleVector::iterator icand = 0;
  for( icand = m_parts.begin(); icand != m_parts.end(); icand++ ) {
    if( ((*icand)->particleID().pid()) == partid ) {
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
  log << MSG::DEBUG << "Initial size of local containers (P,V) = " 
      << m_parts.size() << ", " << m_verts.size() << endreq;

  //cleanDesktop();
  
  if ( 0 != m_pMaker  ) {   
    // Make particles starting from MC or reconstruction objects
    
    log << MSG::DEBUG << "PhysDesktop:Calling " << m_pMakerType 
        << "::makeParticles() " 
        << endreq;
    
    // Remember that these particles belong to the Desktop and are not
    // in a TES container yet
    StatusCode scMaker = m_pMaker->makeParticles(m_parts);
    
    if (!scMaker) {
      log << MSG::DEBUG << " not able to make particles " << endreq;
      return StatusCode::FAILURE;
    }
    
    log << MSG::DEBUG << "Number of Particles from " << m_pMakerType
        << " are " << m_parts.size() << endreq;

    // Flag these particles to be in PhysDesktop
    //for( ParticleVector::iterator ip = m_parts.begin(); 
    //     ip != m_parts.end(), ip++ ) {
    //  *ip->setDesktop(1);
    //}
    
  }
  
  // Retrieve Primary vertices
  if( "None" == m_primVtxLocn ) {
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
        // Put them in local containers
        m_verts.push_back(*ivert);
      }
    }
  }
  
  log << MSG::DEBUG << "Number of Vertices from " << m_primVtxLocn
      << " are " << m_verts.size() << endreq;
  
  // Retrieve Particles & their Vertices from all previous processing
  // as specified in jobOptions
  for( std::vector<std::string>::iterator iloc = m_inputLocn.begin(); 
       iloc != m_inputLocn.end(); iloc++ ) {
    
    std::string location = (*iloc)+"/Particles";
    
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
      for( icand = parts->begin(); icand != parts->end(); icand++ ) {
        //(*icand)->setDesktop(1);
        m_parts.push_back(*icand);
        Vertex* vtx = (*icand)->endVertex();
        if( 0 != vtx ) {
          // get the vertex and all of its tree, unless already in
          // Desktop use findAllTree( vtx, allParts, allVtxs ), then
          // check that particle&vertices not already in desktop
          // For the moment only load decay of a particle, it means
          // other particles will need to be loaded
          m_verts.push_back( vtx );
          //vtx->setDesktop(1);
        }
      }
    }
    log << MSG::DEBUG << "Number of Particles after adding " 
        << location << " = " << m_parts.size() << endreq;
  }
  log << MSG::DEBUG << "    Total number of particles " << m_parts.size()
      << endreq;
  log << MSG::DEBUG << "    Total number of vertices " << m_verts.size()
      << endreq; 
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================

  

