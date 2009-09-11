// $Id: PhysDesktop.cpp,v 1.77 2009-09-11 17:13:17 jonrob Exp $
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
//#include "GaudiKernel/GaudiException.h"
//#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IToolSvc.h" 

// local
#include "PhysDesktop.h"
#include "Kernel/IOnOffline.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
#include "Event/RecVertex.h"
/*-----------------------------------------------------------------------------
 * Implementation file for class : PhysDesktop base class
 *
 * 18/02/2002 : Sandra Amato
 * 04/03/2004 : Hugo Ruiz : automatized outputLocation = algorithm name
 * 11/08/2004 : Patrick Koppenburg : Make it a GaudiTool
 * 17/12/2004 : Patrick Koppenburg : Add OnOffline tool
 * 08/02/2005 : Patrick Koppenburg : Split vertices into primaries and secondaries
 * 20/01/2006 : Patrick Koppenburg : Adapt to DC06 event model
 * 16/04/2007 : Patrick Koppenburg : Major revamp
 *-----------------------------------------------------------------------------
 */
typedef LHCb::Particle::ConstVector::const_iterator p_iter;
typedef LHCb::RecVertex::ConstVector::const_iterator rv_iter;
typedef LHCb::Vertex::ConstVector::const_iterator v_iter;

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( PhysDesktop );

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
  : GaudiTool ( type, name , parent ),
    m_primVtxLocn(""),
    m_outputLocn     (),
    m_OnOffline      (0),
    m_p2VtxTable(),
    m_pvRelator(0),
    m_pvRelatorName("")
{

  // Declaring implemented interfaces
  declareInterface<IPhysDesktop>(this);
  //  declareInterface<IIncidentListener>(this);

  // Declare properties
  //                    loading conditions

  //                    input & output locations
  declareProperty( "InputPrimaryVertices", m_primVtxLocn );

  m_p2PVInputLocations.clear();
  
  declareProperty("P2PVInputLocations", m_p2PVInputLocations);

  // instance of PV relator
  declareProperty( "RelatedPVFinderName", m_pvRelatorName );


} ;

//=============================================================================
// Initialize method, retrieve necessary services
//=============================================================================
StatusCode PhysDesktop::initialize()
{

  StatusCode sc = GaudiTool::initialize() ;
  if (sc.isFailure()) return Warning( "Failed to initialize base class GaudiTool", sc );
  if (msgLevel(MSG::DEBUG)) debug() << ">>>   PhysDesktop::initialize() " << endmsg;

  // check it is not global
  const IToolSvc* par = dynamic_cast<const IToolSvc*>( this->parent() );
  if ( 0!=par ){
    err() << "Parent of PhysDesktop is ToolSvc. PhysDesktop *must* be private" << endmsg ;
    return StatusCode::FAILURE;
  }

  m_OnOffline = tool<IOnOffline>("OnOfflineTool",this);

  if (m_primVtxLocn=="") m_primVtxLocn = m_OnOffline->primaryVertexLocation();

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Primary vertex location set to " << primaryVertexLocation() << endmsg;
  }
  
  if (""==m_pvRelatorName) m_pvRelatorName=m_OnOffline->relatedPVFinderType();

  // PV relator
  m_pvRelator = tool<IRelatedPVFinder>(m_pvRelatorName, this);

  // Output Location
  IInterface* p = const_cast<IInterface*>( this->parent() ) ;

  if ( 0 != p )
  {
    // set the default value using parent's name
    m_outputLocn = m_OnOffline->trunkOnTES() +"/"+ nameFromInterface ( p ) ;
    // try to overwrite the name from  parents's "OutputLocation" property:
    void* tmp = 0 ;
    StatusCode sc = p->queryInterface ( IProperty::interfaceID() , &tmp ) ;
    if ( sc.isSuccess() && 0 != tmp ){
      IProperty* pp = static_cast<IProperty*>( tmp ) ;
      StringProperty output = StringProperty ( "OutputLocation" , 
                                               "NOTDEFINED" ) ;
      sc = pp->getProperty( &output ) ;
      if ( sc.isSuccess() ) { {
        if ( output.value() != "" )
          m_outputLocn = output.value() ; }   // NB !!
      }
      // release the used interface
      pp->release() ;
    }
  }

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);

  // check that output location is set to *SOME* value
  if (m_outputLocn.empty()) Exception("OutputLocation is not set") ;

  return sc;
}

/*
//=============================================================================
// Implementation of Listener interface
//=============================================================================
void PhysDesktop::handle(const Incident&){
StatusCode sc = cleanDesktop();
if (!sc) Exception("Could not clean Desktop");
return ;
}
*/
//=============================================================================
// Provides a reference to its internal container of particles
//=============================================================================
const LHCb::Particle::ConstVector& PhysDesktop::particles() const{
  return m_parts;
}
//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const LHCb::RecVertex::Container* PhysDesktop::primaryVertices() const {
  verbose() << "Returning PVs!" << endmsg;
  if (0!=m_primVerts) {
    verbose() << "Size " << m_primVerts->size() << endmsg;
  } else {
    error() <<"NULL LHCb::RecVertex::Container*" << endmsg;
  }
  return m_primVerts;
}
//=============================================================================
// Provides a reference to its internal container of vertices
//=============================================================================
const LHCb::Vertex::ConstVector& PhysDesktop::secondaryVertices() const {
  return m_secVerts;
}
//============================================================================
const Particle2Vertex::LightTable& PhysDesktop::Particle2VertexRelations() const
{
  return i_p2PVTable();
}
//============================================================================
Particle2Vertex::LightTable& PhysDesktop::Particle2VertexRelations()
{
  return i_p2PVTable();
}
//============================================================================
// Clean local data, called by DVAlgorithm
//============================================================================
StatusCode PhysDesktop::cleanDesktop(){

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "PhysDesktop::cleanDesktop()" << endmsg;
    verbose() << "Removing all particles from desktop" << endmsg;
    // Some particle have been saved to the TES, so they belong to it
    // others do not and need to be deleted by the PhysDesktop
    verbose() << "Number of particles before cleaning = "
              << m_parts.size() << endmsg;
    if (!m_parts.empty())    verbose() << "First element\n" <<  *m_parts[0] << endmsg;
    
  }
  
  int iTESCount = DaVinci::safeContainerClear(m_parts);

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "LHCb::Particle in TES = " << iTESCount << endmsg;
    verbose() << "Removing all vertices from desktop" << endmsg;
    verbose() << "Number of secondary vertices before cleaning = "
              << m_secVerts.size() << endmsg;
  }

  iTESCount =  DaVinci::safeContainerClear(m_secVerts);

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "LHCb::Vertex in TES = " << iTESCount << endmsg;
    verbose() << "Removing all re-fitted PVs from desktop" << endmsg;
    verbose() << "Number of re-fitted PVs before cleaning = "
              << m_refitPVs.size() << endmsg;
  }

  iTESCount =  DaVinci::safeContainerClear(m_refitPVs);

    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Removing all entries from Particle2Vertex relations" << endmsg;

  i_p2PVTable().i_clear();

  if (msgLevel(MSG::VERBOSE)) verbose() << "PhysDesktop::cleanDesktop() DONE" << endmsg;
  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Register a new particle in the Desktop
//=============================================================================
const LHCb::Particle* PhysDesktop::keep( const LHCb::Particle* keptP ){

  if ( 0==keptP ){
    Exception("Attempt to keep NULL Particle") ;
    return 0;
  }
  if (msgLevel(MSG::VERBOSE)) printOut("Put Particle on desktop", keptP);
  
  // Input particle is given check if it already exist in the stack
  if( DaVinci::inTES( keptP ) ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "   -> Particle is in desktop" << endmsg ;
     return keptP;
  }
  // Create new particle on the heap
  LHCb::Particle* newP = new LHCb::Particle(*keptP);

  // Put in the desktop container
  m_parts.push_back(newP);

  // Check if link to endProducts exist and set it
  if( 0 != keptP->endVertex() ) {
    const LHCb::Vertex* newV = keep( keptP->endVertex() );
    newP->setEndVertex(newV);
  }

  // copy relations directly from table to avoid triggering any new P->PV 
  // relations calculation
  Particle2Vertex::Table::Range range = i_p2PVTable().i_relations(keptP);

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "keeping " << range.size() << " P->PV relations" << endmsg;
  }

  for ( Particle2Vertex::Table::Range::const_iterator i = range.begin();
        i != range.end();
        ++i) {
    i_p2PVTable().i_relate( newP, i->to() );
  }

  // Link to outgoing particles is followed through the keep(LHCb::Vertex)
  // Link to originators will be correct because they are in the heap
  // so their pointer is valid
  return newP;

}
//=============================================================================
// Create a new vertex
//=============================================================================
const LHCb::Vertex* PhysDesktop::keep( const LHCb::Vertex* keptV ){

  if ( 0==keptV ){
    Exception("Attempt to keep NULL Vertex") ;
    return 0; 
  }
  if (msgLevel(MSG::VERBOSE)) printOut("keep in Desktop", keptV);

  // Input vertex is given check if it already exist in the stack
  if( DaVinci::inTES( keptV ) ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << " Vertex is in TES" << endmsg;
    return keptV;
  }
  

  // Create new vertex on the heap
  LHCb::Vertex* newV = new LHCb::Vertex(*keptV);
  // Put in the desktop container
  m_secVerts.push_back(newV);

  if (msgLevel(MSG::VERBOSE)) verbose() << "   -> Create new and keep " << endmsg ;

  newV->clearOutgoingParticles();
  // Check if link to endProducts exist and set it
  SmartRefVector<LHCb::Particle> outP = keptV->outgoingParticles();
  SmartRefVector<LHCb::Particle>::iterator ip;
  if (msgLevel(MSG::VERBOSE)) verbose() << "Looking for daughters of vertex:" << endmsg ;
  for( ip = outP.begin(); ip != outP.end(); ip++ ) {
    if (msgLevel(MSG::VERBOSE)) printOut("    Daughter", (*ip));
    const LHCb::Particle* newP = keep( *ip );
    newV->addToOutgoingParticles(newP);
  }

  if (msgLevel(MSG::VERBOSE)) printOut("New vertex in desktop", newV);

  return newV;

}
//=============================================================================
// Create a new vertex
//=============================================================================
const LHCb::RecVertex* PhysDesktop::keep( const LHCb::RecVertex* keptV ){

  if ( 0==keptV ){
    Exception("Attempt to keep NULL Vertex") ;
    return 0; 
  }
  //  if (msgLevel(MSG::VERBOSE)) printOut("keep in Desktop", keptV);

  // Input vertex is given check if it already exist in the stack
  if( DaVinci::inTES( keptV ) ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << " Vertex is in TES" << endmsg;
    return keptV;
  }
  

  // Create new vertex on the heap
  LHCb::RecVertex* newV = new LHCb::RecVertex(*keptV);
  // Put in the desktop container
  m_refitPVs.push_back(newV);

  if (msgLevel(MSG::VERBOSE)) verbose() << "   -> Create new and keep " << endmsg ;


  return newV;

}

//=============================================================================
void PhysDesktop::saveParticles(const LHCb::Particle::ConstVector& pToSave) const 
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "Save Particles to TES " << endmsg;

  LHCb::Particles* particlesToSave = new LHCb::Particles();
  const std::string location( m_outputLocn+"/Particles");
  put(particlesToSave,location);

  LHCb::RecVertex::ConstVector verticesToSave;

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Going to save " 
              << pToSave.size() << " particles" << endmsg;
  }
  
  for( p_iter icand = pToSave.begin(); icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if (  !DaVinci::inTES(*icand) ) {
      if (msgLevel(MSG::VERBOSE)) printOut("  Saving", (*icand));
      particlesToSave->insert((LHCb::Particle*)*icand); // convert to non-const
      // store the related PV and the table. All relaitons and PVs should
      // already be "kept". On;y do it if PV is not in TES.
      const LHCb::VertexBase* vb = i_relatedVertexFromTable(*icand);
      if ( !DaVinci::inTES(vb) ) {
        const LHCb::RecVertex* pv =  dynamic_cast<const LHCb::RecVertex*>(vb);
        if (0!=pv) verticesToSave.push_back(pv);
      }
    } else {
      if (msgLevel(MSG::VERBOSE)) printOut("Skipping", (*icand));
    }
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Saving " << particlesToSave->size()
                                        << " new particles in " << location << " from " << pToSave.size()
                                        << " total particles in desktop " << endmsg;

  // now save re-fitted vertices
  if (msgLevel(MSG::VERBOSE)) verbose() << "Passing " << verticesToSave.size()
                                        << " to saveReFittedPVs" << endmsg;
  saveRefittedPVs(verticesToSave);
  // now save relations table
  if (msgLevel(MSG::VERBOSE)) verbose() << "Save P->PV relations" << endmsg;

  saveP2PVRelations(pToSave);
 
}
//=============================================================================
void PhysDesktop::saveVertices(const LHCb::Vertex::ConstVector& vToSave) const 
{

  LHCb::Vertices* verticesToSave = new LHCb::Vertices();

  const std::string location(m_outputLocn+"/Vertices");

  put(verticesToSave,location);

  for( v_iter iver = vToSave.begin(); iver != vToSave.end(); iver++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( !DaVinci::inTES(*iver) ) {
      verticesToSave->insert((LHCb::Vertex*)*iver); // insert non-const
    }
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "Saving " << verticesToSave->size()
                                        << " new vertices in " << location << " from " << vToSave.size()
                                        << " vertices in desktop " << endmsg;
  
}
//=============================================================================
void PhysDesktop::saveRefittedPVs(const LHCb::RecVertex::ConstVector& vToSave) const
{
  if ( vToSave.empty() ) return;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Saving " << vToSave.size() 
                                        << " Re-fitted PVs" << endmsg;
  LHCb::RecVertices* verticesToSave = new LHCb::RecVertex::Container();

  const std::string location(m_outputLocn+"/_RefitPVs");

  put(verticesToSave,location);

  for(rv_iter iver = vToSave.begin(); 
      iver != vToSave.end(); ++iver ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( !DaVinci::inTES(*iver) ) {
      verticesToSave->insert(const_cast<LHCb::RecVertex*>(*iver)); // insert non-const
    }
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "Saving " << verticesToSave->size()
                                        << " new vertices in " << location 
                                        << " from " << vToSave.size()
                                        << " vertices in desktop " << endmsg;
  
}
//=============================================================================
void PhysDesktop::saveP2PVRelations(const  LHCb::Particle::ConstVector& pToSave) const {
  // PK: save only the ones new to Desktop
  /// @todo One might accept saving a relation for an already saved particle (?)
  Particle2Vertex::Table* table = new Particle2Vertex::Table( pToSave.size() );

  const std::string location(m_outputLocn+"/Particle2VertexRelations");

  put(table, location);

  for ( p_iter p = pToSave.begin() ; p!=pToSave.end() ; ++p){
    Particle2Vertex::Table::Range r = i_p2PVTable().i_relations(*p);
    Particle2Vertex::Table::Range::const_iterator i = r.begin() ;
    for ( ; i!= r.end() ; ++i) table->relate( *p,i->to() );
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Saving table to " << m_outputLocn+"/Particle2VertexRelations" 
            << endmsg ;
  }

  return ;
}
//=============================================================================
// Save only this list of particles and their tree in TES
// If bottom of trees is in TES it will be copied in the new location
//=============================================================================
StatusCode PhysDesktop::saveTrees(const LHCb::Particle::ConstVector& pToSave) const
{

  if (msgLevel(MSG::VERBOSE)) verbose() << " PhysDesktop SaveTrees(LHCb::Particle::ConstVector)" << endmsg;

  // Find all particles that will need to be saved and put them in
  // a container
  LHCb::Particle::ConstVector allpToSave;
  LHCb::Vertex::ConstVector allvToSave;
  for( p_iter icand = pToSave.begin();
       icand != pToSave.end(); icand++ ) {
    if (msgLevel(MSG::VERBOSE)) printOut("  Getting", (*icand));
    // Find all descendendant from this particle
    findAllTree( *icand, allpToSave, allvToSave);
  }

  return saveDesktop( allpToSave, allvToSave );

}

//=============================================================================
// Save only particles in desktop corresponding to this code
//=============================================================================
StatusCode PhysDesktop::saveTrees( int partid ) const {

  if (msgLevel(MSG::VERBOSE)) verbose() << "PhysDesktop saveTrees(pid code)"
                                        << "type = " << partid << endmsg;

  LHCb::Particle::ConstVector pToSave;
  for( p_iter icand = m_parts.begin(); icand != m_parts.end(); icand++ ) {
    if( ((*icand)->particleID().pid()) == partid ) {
      if (msgLevel(MSG::VERBOSE)) printOut("Saving ", (*icand));
      pToSave.push_back(*icand);
    }
  }
  return saveTrees( pToSave );

}

//=============================================================================
// Clone selected particles
//=============================================================================
StatusCode PhysDesktop::cloneTrees( const LHCb::Particle::ConstVector& pToSave ) {

  LHCb::Particle::ConstVector cloned;
  Particle2Vertex::LightTable tmpTable;
  if (msgLevel(MSG::VERBOSE)) verbose() << "cloneTrees cloning " 
                                        << pToSave.size() << " Particles"
                                        << endmsg;

  for (p_iter i=pToSave.begin(); i!=pToSave.end(); ++i) {
    LHCb::Particle *clone = (*i)->clone();
    cloned.push_back(clone);
    if (msgLevel(MSG::VERBOSE)) printOut("Cloning", (*i));
    // now clone the relations too
    const Particle2Vertex::Table::Range r = i_p2PVTable().i_relations(*i);
    if (msgLevel(MSG::VERBOSE)) verbose() << "cloneTrees cloning " 
                                          << r.size() 
                                          << " relations" << endmsg;

    for ( Particle2Vertex::Table::Range::const_iterator j = r.begin() ; j!= r.end() ; ++j){
      tmpTable.i_relate(clone,j->to());
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << "Cloning relation between P\n"
                  << *clone 
                  << "\and PV " << *(j->to())
                  << endmsg;
      }
    }
  } // particle loop

  const Particle2Vertex::Table::Range tmpRange = tmpTable.i_relations();
  if (msgLevel(MSG::VERBOSE)) verbose() << "cloneTrees copying " 
                                        << tmpRange.size() << " relations" 
                                        << endmsg;
  i_p2PVTable().merge(tmpRange);

  return saveTrees(cloned);

}
//=============================================================================
// Find all particle and vertices connected to this tree
//=============================================================================
void PhysDesktop::findAllTree( const LHCb::Particle* part, 
                               LHCb::Particle::ConstVector& parts,
                               LHCb::Vertex::ConstVector& verts ) const {

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

  if (msgLevel(MSG::VERBOSE)) verbose() << ">>> Hello from getEventInput " << endmsg;

  if ( !m_secVerts.empty()) m_secVerts.clear(); // to make sure it is clean in this event


  // Retrieve Primary vertices
  if( "None" == m_primVtxLocn ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Not loading any primary vertices" << endmsg;
  } else {
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Loading any primary vertices from "
                << primaryVertexLocation() << endmsg;
    }
    StatusCode sc = getPrimaryVertices();
    if ( sc.isFailure() ) return sc;
    if ( 0==m_primVerts) {
      Info( "No primary vertex container at "+primaryVertexLocation() ) ;      
    } else if (m_primVerts->empty()) {
      Info( "Empty primary vertex container at "+primaryVertexLocation() ) ;      
    }
  }

  // Retrieve Particles & Vertices from all previous processing
  // as specified in jobOptions
  
  if (!m_inputLocations.empty()) {
    StatusCode sc = getParticles();
    if (!sc) return sc;
    sc = getInputRelations();
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// Get Particles
//=============================================================================
StatusCode PhysDesktop::getParticles(){

  if (msgLevel(MSG::DEBUG)) debug() << "Looking for particles in " 
                                    << m_inputLocations.size() 
                                    << " places" << endmsg ;

  for( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
       iloc != m_inputLocations.end(); iloc++ ) {

    // Retrieve the particles:
    std::string location = (*iloc)+"/Particles";
    if ( !exist<LHCb::Particles>( location ) ){ 
      Info("No particles at location "+location
              +((rootInTES().size()>0)?(" under "+rootInTES()):"") );
      continue ;
    }
    LHCb::Particles* parts = get<LHCb::Particles>( location );
    // Msg number of Particles retrieved
    if (msgLevel(MSG::VERBOSE)) verbose() << "    Number of Particles retrieved from "
                                          << location << " = " << parts->size() << endmsg;
    
    for( LHCb::Particles::iterator icand = parts->begin(); 
         icand != parts->end(); icand++ ) {
      m_parts.push_back(*icand);
    }
    
    if (msgLevel(MSG::VERBOSE)) verbose() << "Number of Particles after adding "
                                          << location << " = " << m_parts.size() << endmsg;
    
    
    // Retrieve the vertices:
    location = (*iloc)+"/Vertices";
    
    if (!exist<LHCb::Vertices>( location ))
    { return Error("No Vertices at location "+location); } 
    
    LHCb::Vertices* verts = get<LHCb::Vertices>( location );
    
    if( verts->empty() ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "No vertices retrieved from " << location << endmsg;
    } else {  
      if (msgLevel(MSG::VERBOSE)) verbose() << "    Number of vertices retrieved from "
                                            << location << " = " << verts->size() << endmsg;
      for( LHCb::Vertices::iterator ivert = verts->begin();
           ivert != verts->end(); ++ivert ) {
        m_secVerts.push_back(*ivert);
      }
      if (msgLevel(MSG::VERBOSE)) verbose() << "Number of vertices after adding "
                                            << location << " = " << m_secVerts.size() << endmsg;
    }
  }
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "    Total number of particles " << m_parts.size() << endmsg;
    verbose() << "    Total number of secondary vertices " << m_secVerts.size() << endmsg;
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Get Relations
//=============================================================================
StatusCode PhysDesktop::getInputRelations(){

  StatusCode sc = getInputRelations(m_p2PVDefaultLocations.begin(), 
                                    m_p2PVDefaultLocations.end());
  
  sc = getInputRelations(m_p2PVInputLocations.begin(), 
                         m_p2PVInputLocations.end());

  return sc;
}
//=============================================================================
StatusCode PhysDesktop::getInputRelations(std::vector<std::string>::const_iterator begin,
                                          std::vector<std::string>::const_iterator end)
{

  for( std::vector<std::string>::const_iterator iloc = begin; 
       iloc != end; iloc++ ) 
  {
    const std::string location( *iloc );
    if (exist<Particle2Vertex::Table>(location)){
      if (msgLevel(MSG::DEBUG)) 
        debug() << "Reading table from " << location << endmsg ;
      const Particle2Vertex::Table* table = 
        get<Particle2Vertex::Table>(location);
      if (0!=table) {
        const Particle2Vertex::Table::Range all = table->relations();
        overWriteRelations(all.begin(), all.end());
      } else {
        Info("NULL Particle2Vertex::Table* at "+location+" under "+rootInTES());
      }

    } else {
      Info("No relations table exists at "+location+" under "+rootInTES());
    }

  }

  return StatusCode::SUCCESS ; // could be sc
}
//=============================================================================
// Get PV
//=============================================================================
StatusCode PhysDesktop::getPrimaryVertices() {


  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Getting PV from " << primaryVertexLocation() << endmsg;
  }
  
  if ( !exist<LHCb::RecVertices>( primaryVertexLocation() ) ) {
    m_primVerts = 0 ;
    Info ("No PV container at "+primaryVertexLocation());
    return StatusCode::SUCCESS;
  }

  m_primVerts = get<LHCb::RecVertices>( primaryVertexLocation() );

  return StatusCode::SUCCESS ;

}
//=============================================================================
// Impose OutputLocation
//=============================================================================
void PhysDesktop::imposeOutputLocation(const std::string& outputLocationString){
  if (outputLocationString != m_outputLocn) {
    Warning( "Non-standard output location imposed: "+ outputLocationString, 
             StatusCode::SUCCESS , 1).ignore();
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
    if (msgLevel(MSG::DEBUG)) debug() << "Saved empty containers at " << m_outputLocn << endmsg ;
  }  
  return sc ;
}
//=============================================================================
const LHCb::VertexBase* PhysDesktop::relatedVertex(const LHCb::Particle* part) const {

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "PhysDesktop::relatedVertex" << endmsg;

  //m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  //if (0==m_dva) Error("Couldn't get parent DVAlgorithm", 
  //                    StatusCode::FAILURE).ignore();
  //return (0!=m_dva) ? m_dva->getRelatedPV(part) : 0 ;

  // cached during initialize()
  return m_dva->getRelatedPV(part);

  //return m_dva->calculateRelatedPV(part);

}
//=============================================================================
const IRelatedPVFinder* PhysDesktop::relatedPVFinder() const
{
  return m_pvRelator;
}
//=============================================================================
void PhysDesktop::relate(const LHCb::Particle*   part, 
                         const LHCb::VertexBase* vert)
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "Relating with  size: " 
                                        <<  i_p2PVTable().i_relations(part).size() << endmsg ;
  i_p2PVTable().i_relate(part, vert );
  if (msgLevel(MSG::VERBOSE)) verbose() << "Related  with size: " <<  i_p2PVTable().i_relations(part).size() << endmsg ;
}
//=============================================================================
Particle2Vertex::Table::Range
PhysDesktop::particle2Vertices(const LHCb::Particle* part ) const
{
  
  if (msgLevel(MSG::VERBOSE)) verbose() << "PhysDesktop::particle2Vertices.empty(): " 
                                        <<  i_p2PVTable().i_relations(part).empty() 
                                        << " .size() " << i_p2PVTable().i_relations(part).size()<< endmsg ;
  return i_p2PVTable().i_relations(part);
}
//=============================================================================
void PhysDesktop::overWriteRelations(Particle2Vertex::Table::Range::const_iterator begin,
                                     Particle2Vertex::Table::Range::const_iterator end)
{
  if (msgLevel(MSG::VERBOSE))
    verbose() << "overWriteRelations: Storing " << end-begin << " P->PV relations" << endmsg;
  for ( Particle2Vertex::Table::Range::const_iterator i = begin ; i!= end ; ++i){
    ( i_p2PVTable().i_removeFrom(i->from()) ).ignore();
    (i_p2PVTable().i_relate(i->from(),i->to())).ignore() ;
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Reading a " << i->from()->particleID().pid() 
                << " related to " <<  i->to()->position() << endmsg ;
    }
  }
}
//=========================================================================
//  set InputLocations
//=========================================================================
StatusCode PhysDesktop::setInputLocations ( const std::vector<std::string>& dv_il) {
  
  if (msgLevel(MSG::DEBUG)){
    debug() << "InputLocations from DVAlgorithm (" << dv_il.size() << ") : " << dv_il << endmsg ;
  }
  
  if (!dv_il.empty()){
    m_inputLocations = dv_il ;
    fixInputLocations(m_inputLocations.begin(),m_inputLocations.end());
  }

  // Check if InputLocation has been set
  if ( m_inputLocations.empty() ){
    debug() << "No inputLocations defined." << endmsg ;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Particles and Vertices will be loaded from :- " << m_inputLocations << endreq ;
    }
  }
  // set PV relation locations
  for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
        iloc != m_inputLocations.end(); ++iloc ) {
    m_p2PVDefaultLocations.push_back((*iloc)+"/Particle2VertexRelations");
  }

  // set PV relation locations
  fixInputLocations(m_inputLocations.begin(), m_inputLocations.end());  

  return StatusCode::SUCCESS ;
}
//=============================================================================
StatusCode 
PhysDesktop::setP2PVInputLocations ( const std::vector<std::string>& location) {

  if (location.empty()) {
    return Warning("Attempting to set empty P->PV locations", 
                   StatusCode::SUCCESS, 1);
  }
  m_p2PVInputLocations = location;

  return std::equal(location.begin(), 
                    location.end(), 
                    m_p2PVInputLocations.begin()) 
    ? 
    StatusCode::SUCCESS : 
    StatusCode::FAILURE;

}
//=============================================================================
void PhysDesktop::fixInputLocations(std::vector<std::string>::iterator begin,
                                    std::vector<std::string>::iterator end){
  const std::string& prefix = m_OnOffline->trunkOnTES();
 
  for (std::vector<std::string>::iterator loc = begin; loc!=end; ++loc) {
    if ( (*loc).find("/") == std::string::npos ) {
      *loc = prefix + "/" + *loc;
      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Input location changed to " << *loc << endmsg;
      }
    }
  }
}
//=============================================================================
