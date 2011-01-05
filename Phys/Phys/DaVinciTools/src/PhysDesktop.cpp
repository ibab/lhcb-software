// $Id: PhysDesktop.cpp,v 1.114 2010-08-12 15:22:09 jpalac Exp $
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IToolSvc.h" 
// ============================================================================
// RecEvent 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// DaVinciTypes 
// ============================================================================
#include "Kernel/TreeCloners.h"
// ============================================================================
// DaVinciKErnel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
// local
// ============================================================================
#include "PhysDesktop.h"
// ============================================================================
/* @file 
 * Implementation file for class : PhysDesktop base class
 *
 * 18/02/2002 : Sandra Amato
 * 04/03/2004 : Hugo Ruiz : automatized outputLocation = algorithm name
 * 11/08/2004 : Patrick Koppenburg : Make it a GaudiTool
 * 17/12/2004 : Patrick Koppenburg : Add OnOffline tool
 * 08/02/2005 : Patrick Koppenburg : Split vertices into primaries and secondaries
 * 20/01/2006 : Patrick Koppenburg : Adapt to DC06 event model
 * 16/04/2007 : Patrick Koppenburg : Major revamp
 */
// ============================================================================
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
  : GaudiTool ( type, name , parent )
    , m_writeP2PV            (true)
    , m_usingP2PV            (true)
    , m_inputLocations       ()
    , m_outputLocn           ("")
    , m_p2PVInputLocations   ()
  //
    , m_parts     (   )
    , m_secVerts  (   )
    , m_refitPVs  (   )
  //
    , m_p2VtxTable    (    )
    , m_p2PVMap       (    )
  //
    , m_dva ( 0 )
{

  // Declaring implemented interfaces
  declareInterface<IPhysDesktop>(this);
  //  declareInterface<IIncidentListener>(this);

  // Declare properties

  m_p2PVInputLocations.clear();
  
  declareProperty("P2PVInputLocations", m_p2PVInputLocations);

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

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);

  return sc;
}
//=============================================================================
// Provides a reference to its internal container of particles
//=============================================================================
const LHCb::Particle::ConstVector& PhysDesktop::particles() const{
  return m_parts;
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
Particle2Vertex::Map& PhysDesktop::Particle2VertexMap() 
{
  return m_p2PVMap;
}
//============================================================================
const Particle2Vertex::Map& PhysDesktop::Particle2VertexMap() const 
{
  return m_p2PVMap;
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
const LHCb::Particle* PhysDesktop::keep( const LHCb::Particle* kept )
{
  //
  Assert ( 0 != kept , "keep: Attempt to keep invaild particle" ) ;
  if ( DaVinci::Utils::inTES ( kept ) ) { return kept ; }
  
  DaVinci::CloneMap clonemap ;
  //
  const LHCb::Particle* newp = 
    DaVinci::cloneTree ( kept , clonemap , m_parts , m_secVerts ) ;
  
  // copy relations directly from table to avoid triggering any new P->PV 
  // relations calculation
  Particle2Vertex::Table::Range range = i_p2PVTable().i_relations(kept);
  
  if ( msgLevel(MSG::VERBOSE))
  {
    verbose() << "keeping " << range.size() << " P->PV relations" << endmsg;
  }
  //
  for ( Particle2Vertex::Table::Range::const_iterator i = range.begin();
        i != range.end(); ++i ) 
  { i_p2PVTable().i_relate ( newp , i->to() ); }
  //
  return newp ;
}
//=============================================================================
// Create a new vertex
//=============================================================================
const LHCb::Vertex* PhysDesktop::keep( const LHCb::Vertex* /* keptV */ )
{
  Assert ( false , "One can't ``keep'' vertex anymore!!!") ;
  return 0 ;
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
  if( DaVinci::Utils::inTES( keptV ) ) {
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
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Saving "<< pToSave.size() << " Particles to TES " << endmsg;
  }
  
  LHCb::Particles* particlesToSave = new LHCb::Particle::Container();
  const std::string location( m_outputLocn+"/Particles");
  put(particlesToSave,location);

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Going to save " 
              << pToSave.size() << " particles" << endmsg;
  }
  
  for( p_iter icand = pToSave.begin(); icand != pToSave.end(); icand++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if (  !DaVinci::Utils::inTES(*icand) ) {
      if (msgLevel(MSG::VERBOSE)) printOut("  Saving", (*icand));
      if (0!=*icand) particlesToSave->insert((LHCb::Particle*)*icand); // convert to non-const
    } else {
      if (msgLevel(MSG::VERBOSE)) printOut("Skipping", (*icand));
    }
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "Saved " << particlesToSave->size()
                                        << " new particles in " << location << " from " << pToSave.size()
                                        << " total particles in desktop " << endmsg;


  // now save relations table
  if (msgLevel(MSG::VERBOSE)) verbose() << "Save P->PV relations" << endmsg;

  if ( saveP2PV() ) saveP2PVRelations(pToSave);
 
}
//=============================================================================
void PhysDesktop::saveVertices(const LHCb::Vertex::ConstVector& vToSave) const 
{

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Saving "<< vToSave.size() << " Vertices to TES " << endmsg;
  }

  LHCb::Vertices* verticesToSave = new LHCb::Vertices();

  const std::string location(m_outputLocn+"/decayVertices");

  put(verticesToSave,location);

  if (vToSave.empty()) return;

  for( v_iter iver = vToSave.begin(); iver != vToSave.end(); iver++ ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( !DaVinci::Utils::inTES(*iver) ) {
      if (0!=(*iver)) verticesToSave->insert((LHCb::Vertex*)*iver); // insert non-const
    }
  }

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Saved " << verticesToSave->size()
              << " new vertices in " << location << " from " << vToSave.size()
              << " from " << vToSave.size()
              << " total veritices in desktop " << endmsg;
  }
  
  
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
    if( !DaVinci::Utils::inTES(*iver) ) {
      verticesToSave->insert(const_cast<LHCb::RecVertex*>(*iver)); // insert non-const
    }
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "Saving " << verticesToSave->size()
                                        << " new vertices in " << location 
                                        << " from " << vToSave.size()
                                        << " vertices in desktop " << endmsg;
  
}
//=============================================================================
void PhysDesktop::saveP2PVRelations(const  LHCb::Particle::ConstVector& pToSave) const 
{
  // PK: save only the ones new to Desktop
  /// @todo One might accept saving a relation for an already saved particle (?)
  
  // V.B.
  
  if ( m_dva->primaryVertices().empty() )
  {
    if ( msgLevel ( MSG::DEBUG ) )
    { debug() << " skip saveP2PVRelations: No Primary Vertices" << endmsg ; }
    return ;
  }

  LHCb::RecVertex::ConstVector verticesToSave;
  
  Particle2Vertex::Table* table = new Particle2Vertex::Table( pToSave.size() );

  const std::string location(m_outputLocn+"/Particle2VertexRelations");

  put(table, location);
  
  for ( p_iter p = pToSave.begin() ; p!=pToSave.end() ; ++p) {

    const LHCb::VertexBase* vb = ( usingP2PV() ) ? i_relatedVertexFromTable(*p)
      : m_dva->getRelatedPV(*p);
      
    if (0!=vb) {
      const LHCb::RecVertex* pv = dynamic_cast<const LHCb::RecVertex*>(vb);
      if (0!=pv) {
        verticesToSave.push_back(pv);
        table->relate( *p, pv );
      } else {
        Error("VertexBase to RecVertex dynamic cast FAILED").ignore();
      }
    }
  }

  // now save re-fitted vertices                                                                                                                                    
  if (msgLevel(MSG::VERBOSE)) verbose() << "Passing " << verticesToSave.size()
                                        << " to saveReFittedPVs" << endmsg;

  saveRefittedPVs(verticesToSave);

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
                               LHCb::Vertex::ConstVector& verts ) const 
{
  if ( parts.end() == std::find ( parts.begin() , parts.end() , part ) )
  { parts.push_back ( part ) ; }
  //
  if ( 0 != part->endVertex() 
       && verts.end() == std::find ( verts.end() , verts.end() , 
                                     part->endVertex() ) ) 
  {
    verts.push_back( part->endVertex() ); // save Vertex
  }
  
  // Loop on daughters
  for (SmartRefVector<LHCb::Particle>::const_iterator d = part->daughters().begin();
       d!=part->daughters().end();++d){
    findAllTree( *d, parts, verts );
  }
  
  // loop over outgoing particles 
  if ( 0 != part->endVertex() )
  {
    for (SmartRefVector<LHCb::Particle>::const_iterator d = 
           part->endVertex()->outgoingParticles().begin();
         d!=part->endVertex()->outgoingParticles().end();++d){
      findAllTree( *d, parts, verts );
    }
    
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
    const std::string location = (*iloc)+"/Particles";
    if ( ! exist<LHCb::Particle::Range>( location ) ) 
    { 
      Info ( "Non-existing location "+(*iloc) + 
             ( rootInTES().empty() ?  "" : (" under " + rootInTES() ) ),
             StatusCode::SUCCESS, 0).ignore() ;
      continue ;
    }
    LHCb::Particle::Range parts = get<LHCb::Particle::Range>( location );
    
    // statistics: 
    counter ( "# " + (*iloc) ) += parts.size() ;
    
    // Msg number of Particles retrieved
    if (msgLevel(MSG::VERBOSE)) 
    { 
      verbose() << "    Number of Particles retrieved from "
                << location << " = " << parts.size() << endmsg;
    }
    
    m_parts    .reserve ( m_parts    .size () + parts.size() ) ;
    m_secVerts .reserve ( m_secVerts .size () + parts.size() ) ;
    for( LHCb::Particle::Range::const_iterator icand = parts.begin(); 
         icand != parts.end(); icand++ ) {
      m_parts.push_back(*icand);
      const LHCb::Vertex* endVtx = (*icand)->endVertex();
      if (endVtx) m_secVerts.push_back(endVtx);
    }
    
    if (msgLevel(MSG::VERBOSE)) 
    { 
      verbose() << "Number of Particles, Vertices after adding "
                << *iloc << " = " << m_parts.size() 
                << ", " << m_secVerts.size() << endmsg;
    }
    
    
  }

  if (msgLevel(MSG::VERBOSE)) 
  {
    verbose() << "    Total number of particles " << m_parts.size() << endmsg;
    verbose() << "    Total number of secondary vertices " << m_secVerts.size() << endmsg;
  }
  
  // statistics: 
  counter ("# input particles" ) += m_parts.size() ; 
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Get Relations
//=============================================================================
StatusCode PhysDesktop::getInputRelations(){

  return getInputRelations(m_p2PVInputLocations.begin(), 
                         m_p2PVInputLocations.end());

}
//=============================================================================
StatusCode PhysDesktop::getInputRelations(std::vector<std::string>::const_iterator begin,
                                          std::vector<std::string>::const_iterator end)
{

  for( std::vector<std::string>::const_iterator iloc = begin; 
       iloc != end; iloc++ ) 
  {
    if (exist<Particle2Vertex::Table>( *iloc ))
    {
      if (msgLevel(MSG::DEBUG)) 
      { debug() << "Reading table from " << (*iloc) << endmsg ; }
      
      const Particle2Vertex::Table* table = get<Particle2Vertex::Table>( *iloc );
      const Particle2Vertex::Table::Range all = table->relations();

      if (msgLevel(MSG::DEBUG)) checkRelations(all.begin(), all.end());

      overWriteRelations(all.begin(), all.end());
    } 
    else 
    {
      Info ( "No P->PV table at " + (*iloc)  + 
             ( rootInTES().empty() ? "" :  (" under "+rootInTES() ) ),
             StatusCode::SUCCESS, 0).ignore() ;
    }
    
  }

  return StatusCode::SUCCESS ; // could be sc
}
void PhysDesktop::checkRelations(const Particle2Vertex::Table::Range::const_iterator begin,
                                 const Particle2Vertex::Table::Range::const_iterator end) const
{
  if (msgLevel(MSG::VERBOSE))
    verbose() << "checkInputRelations: checking " << end-begin << " P->PV relations" << endmsg;
  
  for ( Particle2Vertex::Table::Range::const_iterator i = begin ; i!= end ; ++i){
    
    if (0==i->from()->parent()) {
      Error("checkRelations From is not in TES").ignore();
    }

    if (0==i->to()->parent()) {
      Error("checkRelations To is not in TES").ignore();
    }    
  }
  
}
//=============================================================================
// Write an empty container if needed
// To make sure each DVAlgorithm always writes out something
//=============================================================================
StatusCode PhysDesktop::writeEmptyContainerIfNeeded(){

  const std::string particleLocation( m_outputLocn+"/Particles");
  if (! exist<LHCb::Particle::Range>(particleLocation) ) {  
    if (msgLevel(MSG::DEBUG)) debug() << "Saving empty container at " 
                                      << particleLocation << endmsg ;
    
    LHCb::Particle::Container* dummy = new LHCb::Particle::Container();
    put(dummy, particleLocation);
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
const LHCb::VertexBase* PhysDesktop::relatedVertex(const LHCb::Particle* part) const {

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "PhysDesktop::relatedVertex" << endmsg;

  // cached during initialize()
  return m_dva->bestPV(part);

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
//  set OutputLocation
//=========================================================================
void  PhysDesktop::setOutputLocation(const std::string& location) 
{
  m_outputLocn = location;
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
  }

  // Check if InputLocation has been set
  if ( m_inputLocations.empty() ){
    debug() << "No inputLocations defined." << endmsg ;
  } else {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Particles and Vertices will be loaded from :- " << m_inputLocations << endreq ;
    }
  }

  return StatusCode::SUCCESS ;
}
//=============================================================================
StatusCode 
PhysDesktop::setP2PVInputLocations ( const std::vector<std::string>& location) {

  if (location.empty()) {
    return Info("No input P->PV locations", 
                StatusCode::SUCCESS, 0);
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
