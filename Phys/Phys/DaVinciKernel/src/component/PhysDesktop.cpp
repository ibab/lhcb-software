// from Gaudi
#include "GaudiKernel/ToolFactory.h"
//#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgorithm.h"

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
 * 20/01/2006 : Patrick Koppenburg : Adapt to DC06 event model
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
      if ( sc.isSuccess() ) { {
        if ( output.value() != "" )
          m_outputLocn = output.value() ; }   // NB !!
      }
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

  if ( m_pMakerType == "" ){
    info() << " No ParticleMaker requested in job options"
           << endmsg
           << " Only previously produced particles will be loaded"
           << endmsg;
  } else {
    // Retrieve the ParticleMaker tool:
    info() << "Using ParticleMaker : " << m_pMakerType << endmsg;
    m_pMaker = tool<IParticleMaker>(m_pMakerType, this);
    if (!m_pMaker)
    {
      return Error( "Unable to retrieve ParticleMaker : " + m_pMakerType );
    }
  }

  // Check if InputLocation has been set
  if ( m_inputLocn.empty() ){
    info() << "Empty list of input locations -> No particles from previous processing" << endmsg;
  } else {
    info() << "Particles and Vertices will be loaded from :- "  << endreq ;
    for ( std::vector<std::string>::iterator iloc = m_inputLocn.begin();
          iloc != m_inputLocn.end(); ++iloc ){
      info() << "  -> " << *iloc << endreq;
    }
  }

  // OnOffline tool
  m_OnOffline = tool<IOnOffline>("OnOfflineTool",this);

  return sc;
}

//=============================================================================
// Implementation of Listener interface
//=============================================================================
void PhysDesktop::handle(const Incident&){
  cleanDesktop();
}

//=============================================================================
// Provides a reference to its internal container of particles
//=============================================================================
const LHCb::Particle::ConstVector PhysDesktop::particles(){
  return m_parts;
}
//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const LHCb::PrimVertex::ConstVector& PhysDesktop::primaryVertices(){
  // @todo Find a smarter way of checking this is done only once...
  if ( m_primVerts.empty()) getPrimaryVertices();
  if ( m_primVerts.empty()) {
    if ( m_primVtxLocn == "" ){
      Warning("Empty primary vertex container at"+m_OnOffline->getPVLocation()) ;      
    } else {
      Warning("Empty primary vertex container at "+m_primVtxLocn);
    } 
  }
  return m_primVerts;
}
//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const LHCb::Vertex::ConstVector PhysDesktop::secondaryVertices(){
  return m_secVerts;
}

//============================================================================
// Clean local data, this is called by the handle method and at finalization
//============================================================================
StatusCode PhysDesktop::cleanDesktop(){

  verbose() << "cleanDesktop():: Removing all particles from desktop" << endmsg;
  // Some particle have been saved to the TES, so they belong to it
  // others do not and need to be deleted by the PhysDesktop
  verbose() << "Number of particles before cleaning = "
            << m_parts.size() << endmsg;

  int iTEScount = 0;
  while ( !m_parts.empty() ) {
    const LHCb::Particle* ipart = m_parts.back();
    m_parts.pop_back();
    // Particles in KeyedContainers (=>TES) have parent
    if( ipart->parent() ) ++iTEScount; 
    else delete ipart; 
  }

  verbose() << "LHCb::Particle in TES = " << iTEScount << endmsg;
  verbose() << "Removing all vertices from desktop" << endmsg;
  verbose() << "Number of vertices before cleaning = "
            << m_parts.size() << endmsg;

  iTEScount = 0;
  while ( m_secVerts.size() > 0 ) {
    const LHCb::Vertex* ivert = m_secVerts.back();
    m_secVerts.pop_back();
    if( ivert->parent() ) iTEScount++;
    else delete ivert;
  }
  while ( m_primVerts.size() > 0 ) {
    const LHCb::Vertex* ivert = m_primVerts.back();
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
StatusCode PhysDesktop::finalize(){
  cleanDesktop();
  //if( m_pMaker ) toolSvc()->releaseTool( m_pMaker ); // CRJ : Done automatically by GaudiTool
  return GaudiTool::finalize() ;
}
//=============================================================================
// Save a new particle in the DeskTop
//=============================================================================
const LHCb::Particle* PhysDesktop::save( const LHCb::Particle* partToSave ){

  verbose() << "save(const LHCb::Particle) in desktop" << endmsg;

  // Input particle is given check if it already exist in the stack
  if( ( 0 != partToSave ) && ( inDesktop( partToSave ) )) {
    verbose() << "Input particle momentum = "
              << partToSave->momentum().px() << " ,"
              << partToSave->momentum().py() << " ,"
              << partToSave->momentum().pz() << endmsg;
    return partToSave;
  }

  // Create new particle on the heap
  LHCb::Particle* saveP = new LHCb::Particle();
  verbose() << "New particle momentum = "
            << saveP->momentum().px() << " ,"
            << saveP->momentum().py() << " ,"
            << saveP->momentum().pz() << endmsg;

  // Input LHCb::Particle from stack is given as input to fill newly created particle
  if( ( 0 != partToSave) && ( !inDesktop ( partToSave ) ) ) {
    // Copy contents to newly created particle
    LHCb::Particle& savePcont = *saveP;
    savePcont = *partToSave;
    verbose() << "Input particle momentum = "
              << saveP->momentum().px() << " ,"
              << saveP->momentum().py() << " ,"
              << saveP->momentum().pz() << endmsg;
    // Check if link to endProducts exist and set it
    if( 0 != partToSave->endVertex() ) {
      const LHCb::Vertex* saveV = save( partToSave->endVertex() );
      saveP->setEndVertex(saveV);
    }
    // Link to outgoing particles is followed through the save(LHCb::Vertex)
    // Link to originators will be correct because they are in the heap
    // so their pointer is valid
  }

  // Put in the desktop container
  setInDesktop(saveP);
  verbose() << "Momentum of new particle in desktop = "
            << saveP->momentum().px() << " ,"
            << saveP->momentum().py() << " ,"
            << saveP->momentum().pz() << endmsg;
  m_parts.push_back(saveP);
  return saveP;

}
//=============================================================================
// Create a new vertex
//=============================================================================
const LHCb::Vertex* PhysDesktop::save( const LHCb::Vertex* vtxToSave ){

  verbose() << "save (LHCb::Vertex) in desktop" << endmsg;

  // Input vertex is given check if it already exist in the stack
  if( ( 0 != vtxToSave ) && ( inDesktop( vtxToSave ) ) ) {
    verbose() << "Input vertex position = "
              << vtxToSave->position().x() << " ,"
              << vtxToSave->position().y() << " ,"
              << vtxToSave->position().z() << endmsg;
    return vtxToSave;
  }

  // Create new vertex on the heap
  LHCb::Vertex* saveV = new LHCb::Vertex();
  verbose() << "New vertex position = "
            << saveV->position().x() << " ,"
            << saveV->position().y() << " ,"
            << saveV->position().z() << endmsg;

  // Input vertex from stack is given as input to fill new created vertex
  if( ( 0 != vtxToSave ) && ( !inDesktop( vtxToSave ) ) ) {
    verbose() << "Input vertex position = "
              << saveV->position().x() << " ,"
              << saveV->position().y() << " ,"
              << saveV->position().z() << endmsg;
    // Copy contents to newly created vertex
    LHCb::Vertex& saveVcont = *saveV;
    saveVcont = *vtxToSave;
    saveV->clearOutgoingParticles();
    // Check if link to endProducts exist and set it
    SmartRefVector<LHCb::Particle> outP = vtxToSave->outgoingParticles();
    SmartRefVector<LHCb::Particle>::iterator ip;
    for( ip = outP.begin(); ip != outP.end(); ip++ ) {
      const LHCb::Particle* saveP = save( *ip );
      saveV->addToOutgoingParticles(saveP);
    }
  }

  // Put in the desktop container
  setInDesktop(saveV);
  verbose() << "Position of new vertex in desktop = "
            << saveV->position().x() << " ,"
            << saveV->position().y() << " ,"
            << saveV->position().z() << endmsg;
  m_secVerts.push_back(saveV);
  return saveV;

}


//=============================================================================
// Save all particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop() {

  verbose() << "Save all new particles and vertices in desktop " << endmsg;
  return saveDesktop( m_parts, m_secVerts );
}

//=============================================================================
// Save all specified particles & vertices in the Desktop to the TES
//=============================================================================
StatusCode PhysDesktop::saveDesktop(const LHCb::Particle::ConstVector& pToSave,
                                    LHCb::Vertex::ConstVector& vToSave) {

  verbose() << "Save specified particles and vertices " << endmsg;

  // Register the particles containers in the store
  LHCb::Particles* particlesToSave = new LHCb::Particles();

  std::string location = m_outputLocn+"/Particles";

  for( LHCb::Particle::ConstVector::const_iterator icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if(  0 == (*icand)->parent() ) {
      particlesToSave->insert((LHCb::Particle*)*icand); // convert to non-const
      verbose() << "  Saving " << (*icand)->particleID().pid()
                << " with P= " << (*icand)->momentum() << " m="
                << (*icand)->measuredMass() << endmsg ;
    } else {
      verbose() << "Skipping " << (*icand)->particleID().pid()
                << " with P= " << (*icand)->momentum() << " m="
                << (*icand)->measuredMass() << endmsg ;
    }
  }
  verbose() << "Saving " << particlesToSave->size()
            << " new particles in " << location << " from " << pToSave.size()
            << " total particles in desktop " << endmsg;

  put(particlesToSave,location);

  // Register the vertices in the store
  LHCb::Vertices* verticesToSave = new LHCb::Vertices();
  for( LHCb::Vertex::ConstVector::iterator iver = vToSave.begin();
       iver != vToSave.end(); iver++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( 0 == (*iver)->parent() ) {
      verticesToSave->insert((LHCb::Vertex*)*iver); // insert non-const
    }
  }

  location = m_outputLocn+"/Vertices";

  verbose() << "Saving " << verticesToSave->size()
            << " new vertices in " << location << " from " << vToSave.size()
            << " vertices in desktop " << endmsg;

  put(verticesToSave,location);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Save only this list of particles and their tree in TES
// If bottom of trees is in TES it will be copied in the new location
//=============================================================================
StatusCode PhysDesktop::saveTrees(const LHCb::Particle::ConstVector& pToSave)
{

  verbose() << " PhysDesktop SaveTrees(LHCb::Particle::ConstVector)" << endmsg;

  // Find all particles that will need to be saved and put them in
  // a container
  LHCb::Particle::ConstVector allpToSave;
  LHCb::Vertex::ConstVector allvToSave;
  for( LHCb::Particle::ConstVector::const_iterator icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    verbose() << "  Getting " << (*icand)->particleID().pid()
              << " with P= " << (*icand)->momentum() << " parent "
              << (*icand)->parent() << endmsg ;
    // Find all descendendant from this particle
    findAllTree( *icand, allpToSave, allvToSave);
  }

  return saveDesktop( allpToSave, allvToSave );

}

//=============================================================================
// Save only particles in desktop corresponding to this code
//=============================================================================
StatusCode PhysDesktop::saveTrees( int partid ) {

  verbose() << "PhysDesktop saveParticles(pid code)"
            << "type = " << partid << endmsg;

  LHCb::Particle::ConstVector pToSave;
  for( LHCb::Particle::ConstVector::iterator icand = m_parts.begin();
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
StatusCode PhysDesktop::cloneTrees( const LHCb::Particle::ConstVector& pToSave ) {

  LHCb::Particle::ConstVector cloned;
  for (LHCb::Particle::ConstVector::const_iterator i=pToSave.begin();
       i!=pToSave.end();++i) {
    LHCb::Particle *clone = (*i)->clone();
    cloned.push_back(clone);
    verbose() << "Clone " << clone->particleID().pid() << " with momentum " <<
      clone->momentum() << " m=" << clone->measuredMass() << endmsg ;
  }
  return saveTrees(cloned);

}
//=============================================================================
// Find all particle and vertices connected to this tree
//=============================================================================
void PhysDesktop::findAllTree( const LHCb::Particle* part, 
                               LHCb::Particle::ConstVector& parts,
                               LHCb::Vertex::ConstVector& verts ) {

  parts.push_back( part ); // save Particle
  if ( 0 != part->endVertex() ) verts.push_back( part->endVertex() ); // save Vertex
  // Loop on daughters
  for (SmartRefVector<LHCb::Particle>::const_iterator d = part->daughters().begin();
       d!=part->daughters().end();++d){
    findAllTree( *d, parts, verts );
  }
  
  return;

}

//=============================================================================
// Method retrieving the data from the store.
// Automatically called by DVAlgorithm::sysExecute()
//=============================================================================
StatusCode PhysDesktop::getEventInput(){

  verbose() << ">>> Hello from getEventInput " << endmsg;
  verbose() << "Initial size of local containers (P,PV,SV) = " << m_parts.size()
            << ", " << m_primVerts.size() << ", " <<  m_secVerts.size()<< endmsg;
  
  // Make particles with particle maker
  if ( m_pMaker ) {
    StatusCode sc = makeParticles();
    if (!sc) return sc;
  }
  if ( !m_primVerts.empty()) m_primVerts.clear(); // to make sure it is clean in this event
  if ( !m_secVerts.empty()) m_secVerts.clear(); // to make sure it is clean in this event

  // Retrieve Primary vertices
  if( "None" == m_primVtxLocn ) {
    verbose() << "Not loading any primary vertices" << endmsg;
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

  verbose() << "PhysDesktop:Calling " << m_pMakerType
            << "::makeParticles() "
            << endmsg;

  // Remember that these particles belong to the Desktop and are not
  // in a TES container yet
  StatusCode sc = m_pMaker->makeParticles(m_parts);

  if (!sc) {
    verbose() << " not able to make particles " << endmsg;
    return sc;
  }

  verbose() << "Number of Particles from " << m_pMakerType
            << " are " << m_parts.size() << endmsg;

  // Flag these particles to be in PhysDesktop
  for( LHCb::Particle::ConstVector::iterator ip = m_parts.begin(); ip != m_parts.end(); ip++ ) {
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

    bool foundpart = exist<LHCb::Particles>( location ) ;
    if (!foundpart){
      //      Warning("No particles at location "+location);
    } else {
      LHCb::Particles* parts = get<LHCb::Particles>( location );
      if ( !parts ) {
        verbose() << "Unable to retrieve Particles from " << location << endmsg;
      } else if ( parts->empty() ) {
        verbose() << "No Particles retrieved from " << location << endmsg;
      } else {

        // Msg number of Particles retrieved
        verbose() << "    Number of Particles retrieved from "
                  << location << " = " << parts->size() << endmsg;

        for( LHCb::Particles::iterator icand = parts->begin(); icand != parts->end(); icand++ ) {
          setInDesktop(*icand);
          m_parts.push_back(*icand);
        }
      }
      verbose() << "Number of Particles after adding "
                << location << " = " << m_parts.size() << endmsg;
    }

    // Retrieve the vertices:
    location = (*iloc)+"/Vertices";

    if (!exist<LHCb::Vertices>( location )){
      if (foundpart) Warning("No Vertices at location "+location);
    } else {

      LHCb::Vertices* verts = get<LHCb::Vertices>( location );

      if ( !verts ) verbose() << "Unable to retrieve vertices from " << location << endmsg;
      else if( verts->empty() ) {
        verbose() << "No vertices retrieved from " << location << endmsg;
      } else {

        verbose() << "    Number of vertices retrieved from "
                  << location << " = " << verts->size() << endmsg;
 
        for( LHCb::Vertices::iterator ivert = verts->begin();
             ivert != verts->end(); ++ivert ) {
          setInDesktop(*ivert);
          m_secVerts.push_back(*ivert);
        }
      }
      verbose() << "Number of vertices after adding "
                << location << " = " << m_secVerts.size() << endmsg;

    }
  }
  verbose() << "    Total number of particles " << m_parts.size() << endmsg;
  verbose() << "    Total number of secondary vertices " << m_secVerts.size() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Get PV
//=============================================================================
StatusCode PhysDesktop::getPrimaryVertices(){

  std::string primVtxLocn ;
  if ( m_primVtxLocn == "" ) primVtxLocn = m_OnOffline->getPVLocation() ;
  else primVtxLocn = m_primVtxLocn ;

  verbose() << "Getting PV from " << primVtxLocn << endmsg;

  if ( !exist<LHCb::PrimVertices>( primVtxLocn )){
    return StatusCode::SUCCESS; // no PV
  }
  LHCb::PrimVertices* verts = get<LHCb::PrimVertices>( primVtxLocn );
  if( ! verts ) {
    verbose() << " Unable to retrieve vertices from " << primVtxLocn << endmsg;
  } else if( verts->empty() ) {
    verbose() << " No vertices retrieved from  " << primVtxLocn << endmsg;
  } else {
    verbose() << "    Number of primary vertices  = " << verts->size() << endmsg;

    for( LHCb::PrimVertices::const_iterator ivert = verts->begin();
         ivert != verts->end(); ivert++ ) {
      verbose() << "    Vertex coordinates = ( "
                << (*ivert)->position().x()
                << " , " << (*ivert)->position().y()
                << " , " << (*ivert)->position().z() << " ) " << endmsg;
      verbose() << "    Vertex ChiSquare = " << (*ivert)->chi2()
                << endmsg;
      // Put them in local containers
      setInDesktop(*ivert);
      m_primVerts.push_back(*ivert);
    }
  }
  verbose() << "Number of Vertices from " << primVtxLocn << " are " << m_primVerts.size() << endmsg;
  return StatusCode::SUCCESS;
}
//=============================================================================
// Impose OutputLocation
//=============================================================================
void PhysDesktop::imposeOutputLocation(const std::string& outputLocationString){
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
//=============================================================================
// Write an empty container if needed
// To make sure each DVAlgorithm always writes out something
//=============================================================================
StatusCode PhysDesktop::writeEmptyContainerIfNeeded(){
  StatusCode sc = StatusCode::SUCCESS;
  if ((!exist<LHCb::Particles>(m_outputLocn+"/Particles")) || 
      (!exist<LHCb::Vertices>(m_outputLocn+"/Vertices"))){
    LHCb::Particle::ConstVector pEmpty;
    LHCb::Vertex::ConstVector vEmpty;
    sc = saveDesktop(pEmpty,vEmpty);
    debug() << "Saved empty containers at " << m_outputLocn << endmsg ;
  }  
  return sc ;
}
