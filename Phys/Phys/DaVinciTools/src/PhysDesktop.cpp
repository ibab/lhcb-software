// $Id: PhysDesktop.cpp,v 1.11 2004-03-11 13:02:14 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgorithm.h"

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
 * 04/03/2004 : Hugo Ruiz : automatized outputLocation = algorithm name
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
    m_pMaker(0),
    m_getInputWarned(false){
  
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

  //  REMOVED TO KILL COMPATIBILITY WITH OLD INPUT FILES
  //  declareProperty( "OutputLocation", m_outputLocn = "/Event/Phys/User");
  
  const IInterface* itemp = this->parent();
  const IAlgorithm* ialg = dynamic_cast<const IAlgorithm*>(itemp);
  if( !ialg ) {
    StatusCode scExc;
    throw GaudiException("Parent of PhysDesktop is not an algorithm",
                         this->name(),scExc);
  }
  
  m_outputLocn = "/Event/Phys/"+ialg->name();

  
}

//=============================================================================
// Initialize method, retrieve necessary services 
//=============================================================================
StatusCode PhysDesktop::initialize() {
  
  MsgStream msg( msgSvc(), name() );
  
  // Register to the Incident service to be notified at the end of one event
  IIncidentSvc* incsvc=0;
  StatusCode sc = StatusCode::FAILURE;
  sc = service("IncidentSvc", incsvc, true);
  if( sc.isSuccess() ) {
    incsvc->addListener(this,"EndEvent",100);
  }
  else {
    msg << MSG::FATAL << " Unable to locate Incident Service" << endreq;
    return sc;
  }  
  
  
  // Retrieve the data service
  sc = StatusCode::FAILURE;
  sc = service("EventDataSvc", m_EDS, true);  
  
  if( sc.isFailure() ) {
    msg << MSG::FATAL << " Unable to locate Event Data Service" << endreq;
    return sc; 
  }
  
  if ( m_pMakerType == "" ){
    msg << MSG::INFO << " No ParticleMaker requested in job options" 
        << endreq;
    msg << MSG::INFO << " Only previously produced particles will be loaded"
        << endreq;
  }
  else{
    
    // Retrieve the ParticleMaker tool:
    msg << MSG::INFO << " Using " << m_pMakerType << " to make particles"
        << endreq;
    sc = StatusCode::FAILURE;
    sc = toolSvc()->retrieveTool(m_pMakerType, m_pMaker,this); 
    
    if(sc.isFailure()) {
      msg << MSG::FATAL << " Unable to retrieve " << m_pMakerType << endreq;
      return sc;
    }
  }
  
  // Check if InputLocation has been set
  if( m_inputLocn.size() == 0 ) {
    msg << MSG::INFO << "No Input from previous processing requested"
        << endreq;
  }
  else {
    msg << MSG::INFO << "Particles and Vertices will be loaded from "
        << std::endl;
    for( std::vector<std::string>::iterator iloc = m_inputLocn.begin(); 
         iloc != m_inputLocn.end(); iloc++ ) {
      
      msg << MSG::INFO << "   ==>" << *iloc << std::endl;
    }
    msg << MSG::INFO << endreq;
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
  
  MsgStream msg( msgSvc(), name() );
  
  msg << MSG::DEBUG << "cleanDesktop()" << endreq;
  msg << MSG::DEBUG << "Removing all particles from desktop" << endreq;
  
  // Some particle have been saved to the TES, so they belong to it
  // others do not and need to be deleted by the PhysDesktop
  msg << MSG::VERBOSE << "Number of particles before cleaning = "
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
  
  msg << MSG::DEBUG << "Particle in TES = " << iTEScount << endreq;
  
  //m_parts.clear();
  
  msg << MSG::DEBUG << "Removing all vertices from desktop" << endreq;
  msg << MSG::VERBOSE << "Number of vertices before cleaning = "
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
  if( m_pMaker ) toolSvc()->releaseTool( m_pMaker );  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Create a new particle in the DeskTop
//=============================================================================
Particle* PhysDesktop::createParticle( Particle* partToSave ){
  
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::DEBUG << "createParticle in desktop" << endreq;
  
  // Input particle is given check if it already exist in the stack
  if( ( 0 != partToSave ) && ( 0 != partToSave->desktop() ) ) {
    msg << MSG::DEBUG << "Input particle momentum = " 
        << partToSave->momentum().px() << " ," 
        << partToSave->momentum().py() << " ," 
        << partToSave->momentum().pz() << endreq;
    msg << MSG::DEBUG << "Particle address " << partToSave << endreq;
    return partToSave;
  }
  
  // Create new particle on the heap
  Particle* saveP = new Particle();
  msg << MSG::DEBUG << "New particle momentum = " 
      << saveP->momentum().px() << " ," 
      << saveP->momentum().py() << " ," 
      << saveP->momentum().pz() << endreq;
  
  // Input Particle from stack is given as input to fill newly created particle
  if( ( 0 != partToSave) && ( 0 == partToSave->desktop() ) ) {
    // Copy contents to newly created particle
    Particle& savePcont = *saveP;
    savePcont = *partToSave;
    msg << MSG::DEBUG << "Input particle momentum = " 
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
  msg << MSG::DEBUG << "Momentum of new particle in desktop = "
      << saveP->momentum().px() << " ," 
      << saveP->momentum().py() << " ," 
      << saveP->momentum().pz() << endreq;
  msg << MSG::DEBUG << "Address of new particle in desktop = "
      << saveP << endreq;
  m_parts.push_back(saveP);     
  return saveP;
  
}
//=============================================================================
// Create a new vertex
//=============================================================================
Vertex* PhysDesktop::createVertex( Vertex* vtxToSave ){
  
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::DEBUG << "createVertex in desktop" << endreq;
  
  // Input vertex is given check if it already exist in the stack
  if( ( 0 != vtxToSave ) && ( 0 != vtxToSave->desktop() ) ) {
    msg << MSG::DEBUG << "Input vertex position = " 
        << vtxToSave->position().x() << " ," 
        << vtxToSave->position().y() << " ," 
        << vtxToSave->position().z() << endreq;
    msg << MSG::DEBUG << "Vertex address " << vtxToSave << endreq;
    return vtxToSave;
  }
  
  // Create new vertex on the heap
  Vertex* saveV = new Vertex();
  msg << MSG::DEBUG << "New vertex position = " 
      << saveV->position().x() << " ," 
      << saveV->position().y() << " ," 
      << saveV->position().z() << endreq;
  
  // Input vertex from stack is given as input to fill new created vertex
  if( ( 0 != vtxToSave ) && ( 0 == vtxToSave->desktop() ) ) {
    msg << MSG::DEBUG << "Input vertex position = " 
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
  msg << MSG::DEBUG << "Position of new vertex in desktop = "
      << saveV->position().x() << " ," 
      << saveV->position().y() << " ," 
      << saveV->position().z() << endreq;
  msg << MSG::DEBUG << "Address of new vertex in desktop = "
      << saveV << endreq;
  m_verts.push_back(saveV);     
  return saveV;
  
}


//=============================================================================
// Save all particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop() {
  
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::DEBUG << " Save all new particles and vertices in desktop " 
      << endreq;
  
  return saveDesktop( m_parts, m_verts );  
  
}

//=============================================================================
// Save all specified particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop( ParticleVector& pToSave, 
                                     VertexVector& vToSave) {
  
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Save specified particles and vertices " << endreq;
  
  // Register the particles containers in the store
  Particles* particlesToSave = new Particles();
  
  std::string location = m_outputLocn+"/Particles";
  
  msg << MSG::DEBUG << "Saving " << particlesToSave->size()
      << " new particles in " << location << " from " << pToSave.size() 
      << " total particles in desktop " << endreq;
  
  StatusCode sc = eventSvc()->registerObject(location,particlesToSave);
  
  for( ParticleVector::const_iterator icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( 0 == (*icand)->parent() ) {
      particlesToSave->insert(*icand);
    }
  }
  
  if ( sc.isFailure() ) {
    delete particlesToSave;
    msg << MSG::ERROR  
        << "    Unable to register" << location << endreq;
    return StatusCode::FAILURE;
  }
  //else {
  //  msg << MSG::DEBUG << "Removing particles saved to TES from desktop" 
  //      << endreq;
  //  pToSave.clear();
  //}
  
  // Register the vertices in the store
  Vertices* verticesToSave = new Vertices();
  for( VertexVector::iterator iver = vToSave.begin();
       iver != vToSave.end(); iver++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( 0 == (*iver)->parent() ) {
      verticesToSave->insert(*iver);
    }
  }
  
  location = m_outputLocn+"/Vertices";
  
  msg << MSG::DEBUG << "Saving " << verticesToSave->size()
      << " new vertices in " << location << " from " << vToSave.size() 
      << " vertices in desktop " << endreq;
  
  sc = eventSvc()->registerObject(location,verticesToSave);
  
  if ( sc.isFailure() ) {
    delete verticesToSave;
    msg << MSG::ERROR  
        << "    Unable to register" << location << endreq;
    return StatusCode::FAILURE;
  }
  //else {
  //  msg << MSG::DEBUG << "Removing vertices saved to TES from desktop" 
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
  
  MsgStream          msg( msgSvc(), name() );
  
  msg << MSG::DEBUG << " PhysDesktop SaveTrees(ParticleVector)" << endreq;
  
  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  for( ParticleVector::iterator icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    // Find all descendendant from this particle
    findAllTree( *icand, allpToSave, allvToSave);
  }
  
  return saveDesktop( allpToSave, allvToSave );
  
}

//=============================================================================
// Save only this list of vertices and their trees in TES
//=============================================================================
StatusCode PhysDesktop::saveTrees(VertexVector& vToSave){
  
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::DEBUG << " PhysDesktop SaveTrees(VertexVector) " << endreq;
  
  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  for( VertexVector::iterator ivert = vToSave.begin(); 
       ivert != vToSave.end(); ivert++ ) {
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
  
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::DEBUG << "PhysDesktop saveParticles(pid code)" 
      << "type = " << partid << endreq;
  
  ParticleVector pToSave;
  for( ParticleVector::iterator icand = m_parts.begin();
       icand != m_parts.end(); icand++ ) {
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
// Automatically called by DVAlgorithm::sysExecute()
//=============================================================================
StatusCode PhysDesktop::getEventInput(){
  
  MsgStream          msg( msgSvc(), name() );

  msg << MSG::DEBUG << ">>> Hello from getEventInput " << endreq;
  msg << MSG::DEBUG << "Initial size of local containers (P,V) = " 
      << m_parts.size() << ", " << m_verts.size() << endreq;
  
  //cleanDesktop();
  
  if ( 0 != m_pMaker  ) {   
    // Make particles starting from MC or reconstruction objects
    
    msg << MSG::DEBUG << "PhysDesktop:Calling " << m_pMakerType 
        << "::makeParticles() " 
        << endreq;
    
    // Remember that these particles belong to the Desktop and are not
    // in a TES container yet
    StatusCode scMaker = m_pMaker->makeParticles(m_parts);
    
    if (!scMaker) {
      msg << MSG::DEBUG << " not able to make particles " << endreq;
      return StatusCode::FAILURE;
    }
    
    msg << MSG::DEBUG << "Number of Particles from " << m_pMakerType
        << " are " << m_parts.size() << endreq;
    
    // Flag these particles to be in PhysDesktop
    for( ParticleVector::iterator ip = m_parts.begin(); 
         ip != m_parts.end(); ip++ ) {
      (*ip)->setDesktop(1);
    }
    
  }
  
  // Retrieve Primary vertices
  if( "None" == m_primVtxLocn ) {
    msg << MSG::DEBUG << "Not loading any primary vertices" 
        << endreq;
  }
  else {
    SmartDataPtr<Vertices> verts ( eventSvc(), m_primVtxLocn );
    if( ! verts ) {
      msg << MSG::DEBUG << " Unable to retrieve vertices from " 
          << m_primVtxLocn << endreq;
    }
    else if( 0 == verts->size() ) {
      msg << MSG::DEBUG << " No vertices retrieved from  " 
          << m_primVtxLocn << endreq;
    }
    else { 
      msg << MSG::DEBUG << "    Number of primary vertices  = " 
          << verts->size() << endreq;
      
      int count = 0;
      for( Vertices::iterator ivert = verts->begin();
           ivert != verts->end(); ivert++ ) {
        count++;
        msg << MSG::DEBUG << "    Vertex coordinates = ( " 
            << (*ivert)->position().x() 
            << " , " << (*ivert)->position().y() 
            << " , " << (*ivert)->position().z() << " ) " << endreq;
        
        msg << MSG::DEBUG << "    Vertex ChiSquare = " << (*ivert)->chi2() 
            << endreq;  
        // Put them in local containers
        (*ivert)->setDesktop(1);
        m_verts.push_back(*ivert);
      }
    }
  }
  
  msg << MSG::DEBUG << "Number of Vertices from " << m_primVtxLocn
      << " are " << m_verts.size() << endreq;
  
  // Retrieve Particles & Vertices from all previous processing
  // as specified in jobOptions
  for( std::vector<std::string>::iterator iloc = m_inputLocn.begin(); 
       iloc != m_inputLocn.end(); iloc++ ) {
    
    // Retrieve the particles:
    std::string location = (*iloc)+"/Particles";
    
    SmartDataPtr<Particles> parts( eventSvc(), location );
    if ( !parts ) { 
      msg << MSG::DEBUG << "Unable to retrieve Particles from " 
          << location << endreq;
    }
    else if( 0 == parts->size() ) {
      msg << MSG::DEBUG << "No Particles retrieved from " 
          << location << endreq;
    }      
    else {
      
      // Msg number of Particles retrieved
      msg << MSG::DEBUG << "    Number of Particles retrieved from "
          << location << " = " << parts->size() << endreq;
      
      for( Particles::iterator icand = parts->begin();
           icand != parts->end(); icand++ ) {
        (*icand)->setDesktop(1);
        m_parts.push_back(*icand);
      }
    }
    msg << MSG::DEBUG << "Number of Particles after adding " 
        << location << " = " << m_parts.size() << endreq;
    
    // Retrieve the vertices:
    location = (*iloc)+"/Vertices";
    
    SmartDataPtr<Vertices> verts ( eventSvc(), location );    
    
    if ( !verts ) { 
      msg << MSG::DEBUG << "Unable to retrieve vertices from " 
          << location << endreq;
    }
    else if( 0 == verts->size() ) {
      msg << MSG::DEBUG << "No vertices retrieved from " 
          << location << endreq;
    }      
    else {
      
      // Msg number of vertices retrieved
      msg << MSG::DEBUG << "    Number of vertices retrieved from "
          << location << " = " << verts->size() << endreq;
      
      for( Vertices::iterator ivert = verts->begin();
           ivert != verts->end(); ++ivert ) {
        (*ivert)->setDesktop(1);
        m_verts.push_back(*ivert);
      }
    }
    msg << MSG::DEBUG << "Number of vertices after adding " 
        << location << " = " << m_verts.size() << endreq;
    
    
  }
  msg << MSG::DEBUG << "    Total number of particles " << m_parts.size()
      << endreq;
  msg << MSG::DEBUG << "    Total number of vertices " << m_verts.size()
      << endreq; 
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
// Obsolete version of getEventInput, kept for backwards compatibility
// To be removed in some months (now 02-2004)
//=============================================================================
StatusCode PhysDesktop::getInput(){
  if (!m_getInputWarned){
    MsgStream          msg( msgSvc(), name() );
    msg << MSG::WARNING << "Obsolete function getInput() called ";
    msg << MSG::WARNING << "This is now automatically called by DVAlgorithm ";
    m_getInputWarned = true;
  }
  return StatusCode::SUCCESS;
  }

// Impose output location
void PhysDesktop::imposeOutputLocation(std::string outputLocationString){
  MsgStream          msg( msgSvc(), name() );
  msg << MSG::WARNING << "Non-standard output location imposed: "
      << outputLocationString << endreq;
  m_outputLocn = outputLocationString;
  return;
}



