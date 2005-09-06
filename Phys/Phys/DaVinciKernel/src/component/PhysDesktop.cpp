// $Id: PhysDesktop.cpp,v 1.27 2005-09-06 12:57:17 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
//#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IAlgorithm.h"

// local
#include "PhysDesktop.h"
#include "Kernel/IParticleMaker.h"
#include "Kernel/IOnOffline.h"

#include "Event/PrimVertex.h"

/*-----------------------------------------------------------------------------
 * Implementation file for class : PhysDesktop base class
 *
 * 18/02/2002 : Sandra Amato
 * 04/03/2004 : Hugo Ruiz : automatized outputLocation = algorithm name
 * 11/08/2004 : Patrick Koppenburg : Make it a GaudiTool
 * 17/12/2004 : Patrick Koppenburg : Add OnOffline tool
 * 08/02/2005 : Patrick Koppenburg : Split vertices into primaries and secondaries
 *-----------------------------------------------------------------------------
 */

// Declaration of the Tool Factory
static const  ToolFactory<PhysDesktop>           s_factory ;
const        IToolFactory& PhysDesktopFactory = s_factory ;

/// anonymous namespace to make functions local
namespace
{
  typedef std::pair<bool,std::string>  _StringPair ;

  template <class TYPE>
  inline _StringPair _getNameFromI ( IInterface* aif )
  {
    if ( 0 == aif ) { return _StringPair( false ,"NULL" ) ; }
    // try to use dynamic_cast first
    TYPE* cmp = dynamic_cast<TYPE*> ( aif ) ;
    if ( 0 != cmp ) { return _StringPair( true , cmp->name() ) ; }
    // else try to extract the interface using "queryInterface"
    void *tmp  = 0 ;
    StatusCode sc = aif->queryInterface( TYPE::interfaceID() , &tmp ) ;
    if (  sc.isSuccess() && 0 != tmp )
    {
      cmp = static_cast<TYPE*>( tmp ) ;
      _StringPair res = _StringPair( true , cmp->name() ) ;
      cmp->release() ; // NB: queryInterface increment the references
      return res ;
    }
    return _StringPair( false , "" ) ;
  } ;

  std::string nameFromInterface
  ( IInterface* aif )
  {
    if ( 0 == aif ) { return "NULL" ; }
    // try IAlgorithm
    _StringPair alg = _getNameFromI<IAlgorithm> ( aif ) ;
    if ( alg.first ) { return alg.second ; }
    // try IService
    _StringPair svc = _getNameFromI<IService>   ( aif ) ;
    if ( svc.first ) { return svc.second ; }
    // try IAlgTool
    _StringPair iat = _getNameFromI<IAlgTool>   ( aif ) ;
    if ( iat.first ) { return iat.second ; }
    // use type as name
    return System::typeinfoName( typeid( *aif ) ) ;
  };
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhysDesktop::PhysDesktop( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_outputLocn     ()
  , m_pMaker         (0)
  , m_locationWarned (false)
  , m_OnOffline      (0)
{

  // Declaring implemented interfaces
  declareInterface<IPhysDesktop>(this);
  declareInterface<IIncidentListener>(this);

  // Declare properties
  //                    loading conditions

  // Type of input particles (Ax, MC, Proto) maker.
  declareProperty( "ParticleMakerType",m_pMakerType="" );

  //                    input & output locations
  declareProperty( "InputPrimaryVertices", m_primVtxLocn = "" );
  m_inputLocn.clear();
  declareProperty( "InputLocations", m_inputLocn );

  //  REMOVED TO KILL COMPATIBILITY WITH OLD INPUT FILES
  //  declareProperty( "OutputLocation", m_outputLocn = "/Event/Phys/User");

  IInterface* p = const_cast<IInterface*>( parent ) ;
  if ( 0 != p )
  {
    // set the default value using parent's name
    m_outputLocn = "/Event/Phys/" + nameFromInterface ( p ) ;
    // try to overwrite the name from  parents's "OutputLocation" property:
    void* tmp = 0 ;
    StatusCode sc = p->queryInterface ( IProperty::interfaceID() , &tmp ) ;
    if ( sc.isSuccess() && 0 != tmp ){
      IProperty* pp = static_cast<IProperty*>( tmp ) ;
      StringProperty output = StringProperty ( "OutputLocation" , "NOTDEFINED" ) ;
      sc = pp->getProperty( &output ) ;
      if ( sc.isSuccess() ) { m_outputLocn = output.value() ; }   // NB !!
      // release the used interface
      pp->release() ;
    }
  };
  // check that output location is set to *SOME* value
  if (m_outputLocn.empty()) Exception("OutputLocation is not set") ;

} ;

//=============================================================================
// Initialize method, retrieve necessary services
//=============================================================================
StatusCode PhysDesktop::initialize()
{

  const StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return Warning( "Failed to initialize base class GaudiTool", sc );
  debug() << ">>>   PhysDesktop::initialize() " << endmsg;

  // Register to the Incident service to be notified at the end of one event
  incSvc()->addListener( this, IncidentType::EndEvent, 100 );

  if ( m_pMakerType == "" )
  {
    info() << " No ParticleMaker requested in job options"
           << endmsg
           << " Only previously produced particles will be loaded"
           << endmsg;
  }
  else
  {
    // Retrieve the ParticleMaker tool:
    info() << "Using ParticleMaker : " << m_pMakerType << endmsg;
    m_pMaker = tool<IParticleMaker>(m_pMakerType, this);
    if (!m_pMaker)
    {
      return Error( "Unable to retrieve ParticleMaker : " + m_pMakerType );
    }
  }

  // Check if InputLocation has been set
  if ( m_inputLocn.empty() )
  {
    info() << "Empty list of input locations -> No particles from previous processing" << endmsg;
  }
  else
  {
    info() << "Particles and Vertices will be loaded from :- "  << endreq ;
    for ( std::vector<std::string>::iterator iloc = m_inputLocn.begin();
          iloc != m_inputLocn.end(); ++iloc )
    {
      info() << "  -> " << *iloc << endreq;
    }
  }

  // OnOffline tool
  m_OnOffline = tool<IOnOffline>("OnOfflineTool");
  if( !m_OnOffline )
  {
    return Error( " Unable to retrieve OnOfflineTool" );
  }

  return sc;
}

//=============================================================================
// Implementation of Listener interface
//=============================================================================
void PhysDesktop::handle(const Incident&)
{
  cleanDesktop();
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

  if ( ( m_verts.empty()) && !((m_primVerts.empty()) && (m_secVerts.empty()) )){
    Warning("Deprecated method PhysDesktop::vertices(): Use faster primaryVertices() and secondaryVertices() methods instead",
            StatusCode::SUCCESS,1);
    m_verts = m_primVerts ;
    m_verts.insert(m_verts.end(),m_secVerts.begin(),m_secVerts.end()); // concatenate
  }

  return m_verts;
}

//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const VertexVector& PhysDesktop::primaryVertices()
{
  // @todo Find a smarter way of checking this is done only once...
  if ( m_primVerts.empty()) getPrimaryVertices();
  return m_primVerts;
}
//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const VertexVector& PhysDesktop::secondaryVertices()
{
  return m_secVerts;
}

//============================================================================
// Clean local data, this is called by the handle method and at finalization
//============================================================================
StatusCode PhysDesktop::cleanDesktop()
{

  if ( ( msgLevel(MSG::VERBOSE)) && ( !m_parts.empty() )){
    verbose() << "cleanDesktop():: Removing all particles from desktop" << endmsg;
    // Some particle have been saved to the TES, so they belong to it
    // others do not and need to be deleted by the PhysDesktop
    verbose() << "Number of particles before cleaning = "
              << m_parts.size() << endmsg;
  }

  int iTEScount = 0;
  while ( !m_parts.empty() ) {
    Particle* ipart = m_parts.back();
    m_parts.pop_back();
    // Particles in KeyedContainers (=>TES) have parent
    if( ipart->parent() ) ++iTEScount; 
    else delete ipart; 
  }

  if (( msgLevel(MSG::VERBOSE) )&& ( !m_parts.empty() )){
    verbose() << "Particle in TES = " << iTEScount << endmsg;
    verbose() << "Removing all vertices from desktop" << endmsg;
    verbose() << "Number of vertices before cleaning = "
              << m_parts.size() << endmsg;
  }

  iTEScount = 0;
  while ( m_secVerts.size() > 0 ) {
    Vertex* ivert = m_secVerts.back();
    m_secVerts.pop_back();
    if( ivert->parent() ) iTEScount++;
    else delete ivert;
  }
  while ( m_primVerts.size() > 0 ) {
    Vertex* ivert = m_primVerts.back();
    m_primVerts.pop_back();
    if( ivert->parent() ) iTEScount++;
    else delete ivert;
  }

  m_partsInTES.clear();
  m_vertsInTES.clear();

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode PhysDesktop::finalize()
{
  cleanDesktop();
  //if( m_pMaker ) toolSvc()->releaseTool( m_pMaker ); // CRJ : Done automatically by GaudiTool
  return GaudiTool::finalize() ;
}
//=============================================================================
// Create a new particle in the DeskTop
//=============================================================================
Particle* PhysDesktop::createParticle( Particle* partToSave ){

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "createParticle in desktop" << endmsg;

  // Input particle is given check if it already exist in the stack
  if( ( 0 != partToSave ) && ( inDesktop( partToSave ) )) {
    if ( msgLevel(MSG::VERBOSE) ){
      verbose() << "Input particle momentum = "
                << partToSave->momentum().px() << " ,"
                << partToSave->momentum().py() << " ,"
                << partToSave->momentum().pz() << endmsg;
    }
    return partToSave;
  }

  // Create new particle on the heap
  Particle* saveP = new Particle();
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << "New particle momentum = "
              << saveP->momentum().px() << " ,"
              << saveP->momentum().py() << " ,"
              << saveP->momentum().pz() << endmsg;
  }

  // Input Particle from stack is given as input to fill newly created particle
  if( ( 0 != partToSave) && ( !inDesktop ( partToSave ) ) ) {
    // Copy contents to newly created particle
    Particle& savePcont = *saveP;
    savePcont = *partToSave;
    if ( msgLevel(MSG::VERBOSE) ){
      verbose() << "Input particle momentum = "
                << saveP->momentum().px() << " ,"
                << saveP->momentum().py() << " ,"
                << saveP->momentum().pz() << endmsg;
    }
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
  setInDesktop(saveP);
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << "Momentum of new particle in desktop = "
              << saveP->momentum().px() << " ,"
              << saveP->momentum().py() << " ,"
              << saveP->momentum().pz() << endmsg;
  }
  m_parts.push_back(saveP);
  return saveP;

}
//=============================================================================
// Create a new vertex
//=============================================================================
Vertex* PhysDesktop::createVertex( Vertex* vtxToSave ){

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "createVertex in desktop" << endmsg;

  // Input vertex is given check if it already exist in the stack
  if( ( 0 != vtxToSave ) && ( inDesktop( vtxToSave ) ) ) {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Input vertex position = "
                << vtxToSave->position().x() << " ,"
                << vtxToSave->position().y() << " ,"
                << vtxToSave->position().z() << endmsg;
    }
    return vtxToSave;
  }

  // Create new vertex on the heap
  Vertex* saveV = new Vertex();
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "New vertex position = "
              << saveV->position().x() << " ,"
              << saveV->position().y() << " ,"
              << saveV->position().z() << endmsg;
  }

  // Input vertex from stack is given as input to fill new created vertex
  if( ( 0 != vtxToSave ) && ( !inDesktop( vtxToSave ) ) ) {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Input vertex position = "
                << saveV->position().x() << " ,"
                << saveV->position().y() << " ,"
                << saveV->position().z() << endmsg;
    }
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
  setInDesktop(saveV);
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << "Position of new vertex in desktop = "
              << saveV->position().x() << " ,"
              << saveV->position().y() << " ,"
              << saveV->position().z() << endmsg;
  }
  m_secVerts.push_back(saveV);
  return saveV;

}


//=============================================================================
// Save all particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop() {

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Save all new particles and vertices in desktop " << endmsg;
  }
  return saveDesktop( m_parts, m_secVerts );

}

//=============================================================================
// Save all specified particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop( ParticleVector& pToSave,
                                     VertexVector& vToSave) {

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Save specified particles and vertices " << endmsg;
  }

  // Register the particles containers in the store
  Particles* particlesToSave = new Particles();

  std::string location = m_outputLocn+"/Particles";

  for( ParticleVector::const_iterator icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if(  0 == (*icand)->parent() ) {
      particlesToSave->insert(*icand);
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  Saving " << (*icand)->particleID().pid()
                  << " with P= " << (*icand)->momentum() << " m="
                  << (*icand)->mass() << endmsg ;
      }
    } else {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Skipping " << (*icand)->particleID().pid()
                  << " with P= " << (*icand)->momentum() << " m="
                  << (*icand)->mass() << endmsg ;
      }
    }
  }
  if ( msgLevel(MSG::VERBOSE) )
  {

    verbose() << "Saving " << particlesToSave->size()
              << " new particles in " << location << " from " << pToSave.size()
              << " total particles in desktop " << endmsg;
  }

  StatusCode sc = put(particlesToSave,location);
  if ( sc.isFailure() )
  {
    delete particlesToSave;
    return Error( " Unable to register " + location );
  }
  //else {
  //  verbose() << "Removing particles saved to TES from desktop"
  //      << endmsg;
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

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Saving " << verticesToSave->size()
              << " new vertices in " << location << " from " << vToSave.size()
              << " vertices in desktop " << endmsg;
  }

  sc = put(verticesToSave,location);
  if ( !sc ) {
    delete verticesToSave;
    return Error( "Unable to register " + location );
  }
  //else {
  //  verbose() << "Removing vertices saved to TES from desktop"
  //      << endmsg;
  //  vToSave.clear();
  //}

  return StatusCode::SUCCESS;
}

//=============================================================================
// Save only this list of particles and their tree in TES
// If bottom of trees is in TES it will be copied in the new location
//=============================================================================
StatusCode PhysDesktop::saveTrees(ParticleVector& pToSave) {

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " PhysDesktop SaveTrees(ParticleVector)" << endmsg;

  // Find all particles that will need to be saved and put them in
  // a container
  ParticleVector allpToSave;
  VertexVector allvToSave;
  for( ParticleVector::iterator icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    // Find all descendendant from this particle
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  Getting " << (*icand)->particleID().pid()
                << " with P= " << (*icand)->momentum() << " parent "
                << (*icand)->parent() << endmsg ;
    }
    findAllTree( *icand, allpToSave, allvToSave);
  }

  return saveDesktop( allpToSave, allvToSave );

}

//=============================================================================
// Save only this list of vertices and their trees in TES
//=============================================================================
StatusCode PhysDesktop::saveTrees(VertexVector& vToSave){

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " PhysDesktop SaveTrees(VertexVector) " << endmsg;
  }

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
// Save only particles in desktop corresponding to this code
//=============================================================================
StatusCode PhysDesktop::saveTrees( int partid ) {

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "PhysDesktop saveParticles(pid code)"
              << "type = " << partid << endmsg;
  }

  ParticleVector pToSave;
  for( ParticleVector::iterator icand = m_parts.begin();
       icand != m_parts.end(); icand++ ) {
    if( ((*icand)->particleID().pid()) == partid ) {
      pToSave.push_back(*icand);
    }
  }
  //  if( pToSave.size() > 0 ) {
  return saveTrees( pToSave );
  //   }

  //  return StatusCode::SUCCESS;

}

//=============================================================================
// Clone selected particles
//=============================================================================
StatusCode PhysDesktop::cloneTrees( ParticleVector& pToSave ) {

  ParticleVector cloned;
  for (ParticleVector::iterator i=pToSave.begin();i!=pToSave.end();++i) {
    Particle *clone = (*i)->clone();
    cloned.push_back(clone);
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Clone " << clone->particleID().pid() << " with momentum " <<
        clone->momentum() << " m=" << clone->mass() << endmsg ;
    }
  }
  return saveTrees(cloned);

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
// Method retrieving the data from the store.
// Automatically called by DVAlgorithm::sysExecute()
//=============================================================================
StatusCode PhysDesktop::getEventInput(){

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << ">>> Hello from getEventInput " << endmsg;
    verbose() << "Initial size of local containers (P,PV,SV) = " << m_parts.size()
              << ", " << m_primVerts.size() << ", " <<  m_secVerts.size()<< endmsg;
  }

  // Make particles with particle maker
  if ( m_pMaker ) {
    StatusCode sc = makeParticles();
    if (!sc) return sc;
  }
  if ( !m_verts.empty()) m_verts.clear(); // to make sure it is clean in this event

  // Retrieve Primary vertices
  if( "None" == m_primVtxLocn ) {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Not loading any primary vertices" << endmsg;
    }
  } else {
    StatusCode sc = getPrimaryVertices();
    if (!sc) return sc;
  }

  // Retrieve Particles & Vertices from all previous processing
  // as specified in jobOptions
  if (!m_inputLocn.empty()) {
    StatusCode sc = getParticles();
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// Make Particles
//=============================================================================
StatusCode PhysDesktop::makeParticles(){
  // Make particles starting from MC or reconstruction objects

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "PhysDesktop:Calling " << m_pMakerType
              << "::makeParticles() "
              << endmsg;
  }

  // Remember that these particles belong to the Desktop and are not
  // in a TES container yet
  StatusCode sc = m_pMaker->makeParticles(m_parts);

  if (!sc) {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " not able to make particles " << endmsg;
    }
    return sc;
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Number of Particles from " << m_pMakerType
              << " are " << m_parts.size() << endmsg;
  }

  // Flag these particles to be in PhysDesktop
  for( ParticleVector::iterator ip = m_parts.begin(); ip != m_parts.end(); ip++ ) {
    setInDesktop(*ip);
  }
  return sc;
}
//=============================================================================
// Get Particles
//=============================================================================
StatusCode PhysDesktop::getParticles(){

  for( std::vector<std::string>::iterator iloc = m_inputLocn.begin();
       iloc != m_inputLocn.end(); iloc++ ) {

    // Retrieve the particles:
    std::string location = (*iloc)+"/Particles";

    bool foundpart = exist<Particles>( location ) ;
    if (!foundpart){
      Warning("No particles at location "+location);
    } else {
      Particles* parts = get<Particles>( location );
      if ( !parts ) {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Unable to retrieve Particles from " << location << endmsg;
        }
      } else if ( parts->empty() ) {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "No Particles retrieved from " << location << endmsg;
        }
      } else {

        // Msg number of Particles retrieved
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "    Number of Particles retrieved from "
                    << location << " = " << parts->size() << endmsg;
        }

        for( Particles::iterator icand = parts->begin(); icand != parts->end(); icand++ ) {
          setInDesktop(*icand);
          m_parts.push_back(*icand);
        }
      }
      if ( msgLevel(MSG::VERBOSE) ){
        verbose() << "Number of Particles after adding "
                  << location << " = " << m_parts.size() << endmsg;
      }
    }

    // Retrieve the vertices:
    location = (*iloc)+"/Vertices";

    if (!exist<Vertices>( location )){
      if (foundpart) Warning("No Vertices at location "+location);
    } else {

      Vertices* verts = get<Vertices>( location );

      if ( !verts ) {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Unable to retrieve vertices from " << location << endmsg;
        }
      }
      else if( 0 == verts->size() ) {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "No vertices retrieved from " << location << endmsg;
        }
      }
      else {

        // Msg number of vertices retrieved
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "    Number of vertices retrieved from "
                    << location << " = " << verts->size() << endmsg;
        }

        for( Vertices::iterator ivert = verts->begin();
             ivert != verts->end(); ++ivert ) {
          setInDesktop(*ivert);
          m_secVerts.push_back(*ivert);
        }
      }
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Number of vertices after adding "
                  << location << " = " << m_secVerts.size() << endmsg;
      }

    }
  }
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "    Total number of particles " << m_parts.size() << endmsg;
    verbose() << "    Total number of secondary vertices " << m_secVerts.size() << endmsg;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Get PV
//=============================================================================
StatusCode PhysDesktop::getPrimaryVertices(){

  std::string primVtxLocn ;
  if ( m_primVtxLocn == "" ) primVtxLocn = m_OnOffline->getPVLocation() ;
  else primVtxLocn = m_primVtxLocn ;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Getting PV from " << primVtxLocn << endmsg;
  }

  if ( !exist<Vertices>( primVtxLocn )){
    Warning("Primary vertex location `"+primVtxLocn+"' does not exist") ;
    if ( m_primVtxLocn == "" ) Warning("This location is obtained from OnOffline tool");
    return StatusCode::SUCCESS; // no PV
  }

  Vertices* verts = get<Vertices>( primVtxLocn );
  if( ! verts ) {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Unable to retrieve vertices from " << primVtxLocn << endmsg;
    }
  } else if( verts->empty() ) {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " No vertices retrieved from  " << primVtxLocn << endmsg;
    }
  } else {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "    Number of primary vertices  = " << verts->size() << endmsg;
    }

    for( Vertices::iterator ivert = verts->begin();
         ivert != verts->end(); ivert++ ) {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "    Vertex coordinates = ( "
                  << (*ivert)->position().x()
                  << " , " << (*ivert)->position().y()
                  << " , " << (*ivert)->position().z() << " ) " << endmsg;
        verbose() << "    Vertex ChiSquare = " << (*ivert)->chi2()
                  << endmsg;
      }
      // Put them in local containers
      setInDesktop(*ivert);
      m_primVerts.push_back(*ivert);
    }
  }
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Number of Vertices from " << primVtxLocn << " are " << m_primVerts.size() << endmsg;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Impose OutputLocation
//=============================================================================
void PhysDesktop::imposeOutputLocation(std::string outputLocationString){
  if (outputLocationString != m_outputLocn) {
    if (m_locationWarned)
    {
      Warning( "Non-standard output location imposed: "
               + outputLocationString, StatusCode::SUCCESS );
      m_locationWarned = true ;
    }
    m_outputLocn = outputLocationString;
  }
  return;
}
