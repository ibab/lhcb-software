// $Id: DVAlgorithm.cpp,v 1.84 2010-08-27 13:48:25 jpalac Exp $
// ============================================================================
// Include 
// ============================================================================
#include <boost/lexical_cast.hpp>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinci
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/DaVinciStringUtils.h"
#include "DaVinciUtils/Functions.h"
#include "Kernel/TreeCloners.h"
// ============================================================================
/** @file
 *  The implementation for class DVAlgorithm
 */                                                                 
// ============================================================================
// Standard constructor
// ============================================================================
DVAlgorithm::DVAlgorithm 
( const std::string& name, 
  ISvcLocator* pSvcLocator ) 
  : base_class    ( name , pSvcLocator )
//
  , m_desktop               ( 0 )
  , m_desktopName           ( "MockPhysDesktop" )
// 
  , m_vertexFitNames        () 
  , m_vertexFits            () 
  //
  , m_filterNames           () 
  , m_filters               () 
  //
  , m_particleCombinerNames ()
  , m_particleCombiners     ()
  // 
  , m_particleReFitterNames ()
  , m_particleReFitters     ()
  //
  , m_pvReFitterNames       ()
  , m_pvReFitters           ()
  , m_defaultPVReFitter     (0)
  //
  , m_decayTreeFitterNames  ()
  , m_decayTreeFitters      ()
  //
  , m_massFitterNames       ()
  , m_massFitters           ()
  //
  , m_lifetimeFitterNames   ()
  , m_lifetimeFitters       ()
  //
  , m_directionFitterNames  ()
  , m_directionFitters      ()
    //
  , m_distanceCalculatorNames  ()
  , m_distanceCalculators      ()
  //
  , m_checkOverlapName      ( "CheckOverlap" ) 
  , m_checkOverlap          ( 0 )
  , m_taggingToolName       ( "BTaggingTool" )
  , m_taggingTool           ( 0 )
  , m_descendants           ( 0 )
  , m_descendantsName       ("ParticleDescendants")
  , m_onOffline             ( 0 )
  , m_pvRelator             ( 0 )
  , m_ppSvc                 ( 0 )
  , m_setFilterCalled       ( false )
  , m_countFilterWrite      ( 0 )
  , m_countFilterPassed     ( 0 )
  , m_refitPVs              ( false )
  , m_multiPV               ( false )
  , m_useP2PV               ( true  )
  , m_writeP2PV             ( true  )
  , m_forceP2PVBuild        ( true  )
  , m_ignoreP2PVFromInputLocations (false)
  , m_PVLocation            ("")
  , m_noPVs                 ( false )
{

  m_outputLocation = this->name();

  m_inputLocations.clear() ;
  declareProperty( "InputLocations", 
                   m_inputLocations, 
                   "Input Locations forwarded of Particles" );

  m_p2PVInputLocations.clear() ;
  declareProperty( "P2PVInputLocations", 
                   m_p2PVInputLocations, 
                   "Particle -> PV Relations Input Locations" );

  declareProperty( "InputPrimaryVertices", m_PVLocation );

  declareProperty("UseP2PVRelations", m_useP2PV,
                  "Use P->PV relations internally. Forced to true if re-fitting PVs. Otherwise disabled for single PV events. Default: true.");

  declareProperty("WriteP2PVRelations", m_writeP2PV, 
                  "Write out P->PV relations table to TES. Default: true");

  declareProperty("ForceP2PVBuild", m_forceP2PVBuild, 
                  "Force construction of P->PV relations table. Default: false");


  declareProperty( "IgnoreP2PVFromInputLocations", m_ignoreP2PVFromInputLocations);
  

  // 
  m_vertexFitNames [ "Offline"       ] = "OfflineVertexFitter"     ;
  m_vertexFitNames [ "Trigger"       ] = "TrgVertexFitter"         ;
  m_vertexFitNames [ "LoKi"          ] = "LoKi::VertexFitter"      ;
  m_vertexFitNames [ "Kalman"        ] = "LoKi::VertexFitter"      ;
  m_vertexFitNames [ "ParticleAdder" ] = "ParticleAdder"           ;
  m_vertexFitNames [ "LoKiFast"      ] = "LoKi::FastVertexFitter"  ;
  m_vertexFitNames [ "FastLoKi"      ] = "LoKi::FastVertexFitter"  ;
  declareProperty ( "VertexFitters"     , m_vertexFitNames, "Names of vertex fitters" ) ;
  //
  declareProperty ( "CheckOverlapTool"  , m_checkOverlapName, "Name of Overlap Tool"  ) ;
  //
  m_filterNames    [ ""       ] = "LoKi::Hybrid::FilterCriterion" ;
  m_filterNames    [ "LoKi"   ] = "LoKi::Hybrid::FilterCriterion" ;
  m_filterNames    [ "Hybrid" ] = "LoKi::Hybrid::FilterCriterion" ;
  declareProperty ( "ParticleFilters"            , 
                    m_filterNames                , 
                    "Names of ParticleFilters"   ) ;
  // 
  declareProperty ( "ReFitPVs"    , m_refitPVs, "Refit PV"     ) ; 
  
  // ==========================================================================
  // Note: there is no default value! 
  // ==========================================================================
  m_particleCombinerNames [ "Offline"          ] = "OfflineVertexFitter"         ;
  m_particleCombinerNames [ "Trigger"          ] = "TrgVertexFitter"             ;
  m_particleCombinerNames [ "Kalman"           ] = "LoKi::VertexFitter"          ;
  m_particleCombinerNames [ "LoKi"             ] = "LoKi::VertexFitter"          ;
  m_particleCombinerNames [ "Smart"            ] = "LoKi::SmartParticleCombiner" ;
  m_particleCombinerNames [ "ParticleAdder"    ] = "ParticleAdder"               ;
  m_particleCombinerNames [ "Adder"            ] = "ParticleAdder"               ;
  m_particleCombinerNames [ "Combiner"         ] = "MomentumCombiner"            ;
  m_particleCombinerNames [ "MomentumCombiner" ] = "MomentumCombiner"            ;
  m_particleCombinerNames [ "Momenta"          ] = "MomentumCombiner"            ;
  m_particleCombinerNames [ "LoKiFast"         ] = "LoKi::FastVertexFitter"      ;
  m_particleCombinerNames [ "FastLoKi"         ] = "LoKi::FastVertexFitter"      ;
  declareProperty
    ( "ParticleCombiners"     , 
      m_particleCombinerNames , 
      "Names of particle combiners, the basic tools for creation of composed particles" ) ;
  //
  m_particleReFitterNames [ ""              ] = "OfflineVertexFitter"    ;
  m_particleReFitterNames [ "Offline"       ] = "OfflineVertexFitter"    ;
  m_particleReFitterNames [ "Vertex"        ] = "OfflineVertexFitter"    ;
  m_particleReFitterNames [ "Kalman"        ] = "LoKi::VertexFitter"     ;
  m_particleReFitterNames [ "LoKi"          ] = "LoKi::VertexFitter"     ;
  m_particleReFitterNames [ "Mass"          ] = "LoKi::MassFitter"       ;
  m_particleReFitterNames [ "Direction"     ] = "LoKi::DirectionFitter"  ;
  m_particleReFitterNames [ "ParticleAdder" ] = "ParticleAdder"          ;
  m_particleReFitterNames [ "Adder"         ] = "ParticleAdder"          ;
  m_particleReFitterNames [ "LoKiFast"      ] = "LoKi::FastVertexFitter" ;
  m_particleReFitterNames [ "FastLoKi"      ] = "LoKi::FastVertexFitter" ;
  declareProperty  ( "ParticleReFitters" , m_particleReFitterNames, "Names of particle refitters" ) ;
  //
  m_pvReFitterNames [ ""           ] = "AdaptivePVReFitter" ;
  m_pvReFitterNames [ "PVReFitter" ] = "PVReFitter"         ;
  m_pvReFitterNames [ "Adaptive"   ] = "AdaptivePVReFitter" ;
  m_pvReFitterNames [ "Cheated"    ] = "CheatedPVReFitter"  ;
  declareProperty  ( "PVReFitters" , m_pvReFitterNames, "Names of PV refitters" ) ;
  //
  
  m_decayTreeFitterNames [ ""     ] = "LoKi::DecayTreeFit" ;
  m_decayTreeFitterNames [ "LoKi" ] = "LoKi::DecayTreeFit" ;
  declareProperty  
    ( "DecayTreeFitters"       , 
      m_decayTreeFitterNames   , 
      "The mapping of nick/name/type for IDecaytreeFitFit tools" ) ;
  //
  m_massFitterNames [ ""     ] = "LoKi::MassFitter" ;
  m_massFitterNames [ "LoKi" ] = "LoKi::MassFitter" ;
  declareProperty  
    ( "MassFitters"            , 
      m_massFitterNames        , 
      "The mapping of nick/name/type for IMassFit tools"        ) ;
  //
  m_lifetimeFitterNames  [ ""     ] = "PropertimeFitter"     ;
  m_lifetimeFitterNames  [ "LoKi" ] = "LoKi::LifetimeFitter" ;
  declareProperty  
    ( "LifetimeFitters"     , 
      m_lifetimeFitterNames , 
      "The mapping of nick/name/type for ILifetimeFitter tools" ) ;
  //
  m_directionFitterNames [ ""     ] = "DirectionFitter" ;
  m_directionFitterNames [ "LoKi" ] = "LoKi::DirectionFitter" ;
  declareProperty  
    ( "DirectionFitters"     , 
      m_directionFitterNames , 
      "The mapping of nick/name/type for IDirectionFit tools"   ) ;
  //
  m_distanceCalculatorNames [ ""        ] = "LoKi::DistanceCalculator"    ;
  m_distanceCalculatorNames [ "LoKi"    ] = "LoKi::DistanceCalculator"    ;
  m_distanceCalculatorNames [ "Trg"     ] = "LoKi::TrgDistanceCalculator" ;
  m_distanceCalculatorNames [ "Hlt"     ] = "LoKi::TrgDistanceCalculator" ;
  m_distanceCalculatorNames [ "Trigger" ] = "LoKi::TrgDistanceCalculator" ;
  m_distanceCalculatorNames [ "Fast"    ] = "LoKi::TrgDistanceCalculator" ;
  declareProperty 
    ( "DistanceCalculators"     , 
      m_distanceCalculatorNames , 
      "The mapping of nick/name/type for IDistanceCalculator tools"   ) ;
  //
  declareProperty ( "DecayDescriptor"   , m_decayDescriptor   = "", "Describes the decay" ) ;
  declareProperty ( "ForceOutput" , m_forceOutput  = true , "If true TES location is written" ) ;
  declareProperty ( "PreloadTools"      , m_preloadTools      = true, "If true all tools are pre-loaded in initialize" ) ;
  //
  // enforce the registration for algorithm context service
  setProperty ( "RegisterForContextService" , true ).ignore() ;
}
// ============================================================================
// Initialize the thing
// ============================================================================
StatusCode DVAlgorithm::initialize () {
  
  // register for the algorithm context service 
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }  
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;
  
  // initialize the base  
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }                          // RETURN
  if (msgLevel(MSG::DEBUG)) debug() << "GaudiTupleAlg is initialized" <<endmsg ;

  if ( !registerContext() || 0 == contextSvc() ) 
  {
    std::string mgs = "Registration for Algorithm Context Service is disabled." ;
    Warning( mgs +  "Some tools/utilities could have the problems." );
  }
  
  // Load tools very
  sc = loadTools() ;
  if ( sc.isFailure() ) { return Error("Unable to load tools", sc ) ; }
  
  if (msgLevel(MSG::DEBUG)){
    if ( m_decayDescriptor == "" )
    { debug() << "Decay Descriptor string not specified"   << endmsg; } 
    else
    { debug() << "Decay Descriptor: " << m_decayDescriptor << endmsg; }
  }

  if (m_PVLocation=="") {
    m_PVLocation = onOffline()->primaryVertexLocation();
  }

  m_noPVs = (m_PVLocation=="None"||m_PVLocation=="") ? true : false;

  initializeLocations();

  // load PV processing tools.

  if (!m_noPVs) {
    if (refitPVs()) {
      if (0==defaultPVReFitter()) {
        return Error("Default IPVReFitter could not be loaded");
      } 
    }
  
    if (0==relatedPVFinder()) {
      return Error("IRelatedPVFinder could not be loaded");
    }
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "End of DVAlgorithm::initialize with " << sc << endmsg;
  }
  
  return sc;
}
// ============================================================================
void DVAlgorithm::initializeLocations() {

  DaVinci::StringUtils::expandLocations( m_inputLocations.begin(),
                                         m_inputLocations.end(),
                                         onOffline()->trunkOnTES() );
 
  if (msgLevel(MSG::DEBUG)) {
    debug() << ">>> Initialised locations " << m_inputLocations 
	    << endmsg;
  }

  for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
        iloc != m_inputLocations.end(); ++iloc ) {
    DaVinci::StringUtils::removeEnding(*iloc,"/Particles");
  }
  
  if (!m_ignoreP2PVFromInputLocations) {
    // load default P->PV locations.
    for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
          iloc != m_inputLocations.end(); ++iloc ) {
      m_p2PVInputLocations.push_back((*iloc)+"/Particle2VertexRelations");
    }
  }

  // load user-defined P->PV locations.
  DaVinci::StringUtils::expandLocations( m_p2PVInputLocations.begin(),
                                         m_p2PVInputLocations.end(),
                                         onOffline()->trunkOnTES()     );

  if (msgLevel(MSG::DEBUG)) {
    debug() << ">>> Initialised P->PV locations " 
            << m_p2PVInputLocations << endmsg;
  }
  
  DaVinci::StringUtils::expandLocation(m_outputLocation,
                                       onOffline()->trunkOnTES());
  
  m_particleOutputLocation = m_outputLocation+"/Particles";
  m_decayVertexOutputLocation = m_outputLocation+"/decayVertices";
  m_tableOutputLocation = m_outputLocation+"/Particle2VertexRelations";

}

// ============================================================================
// Load standard tools
// ============================================================================
StatusCode DVAlgorithm::loadTools() {
 
  if ( !m_preloadTools ){ 
    return Warning( "Not preloading tools", StatusCode::SUCCESS ) ; 
  }

  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading tools" << endmsg;
  // vertex fitter
  
  if ( m_particleCombinerNames.end() == m_particleCombinerNames.find("") ) {
    m_particleCombinerNames[""] = onOffline()->particleCombinerType() ;
  }
  
  if ( m_vertexFitNames.end() == m_vertexFitNames.find("") ) {
    m_vertexFitNames[""] = onOffline()->vertexFitterType() ;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading "
          << m_vertexFitNames[""] << " as IVertexFit " << endmsg;
  vertexFitter() ;
  
  // geometry THIS IS OBSOLETE
  if ( m_distanceCalculatorNames.end() == m_distanceCalculatorNames.find("") ) {
    m_distanceCalculatorNames[""] = onOffline()->distanceCalculatorType() ;
  }
  
  // distance geometry
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading " 
          << m_distanceCalculatorNames[""] 
          << " as IDistanceCalculator" << endmsg;
  distanceCalculator();
  
  if (msgLevel(MSG::DEBUG)) { 
    debug() << ">>> Preloading CheckOverlap Tool" << endmsg; 
  }
  checkOverlap();
  
  if (msgLevel(MSG::DEBUG)) { 
    debug() << ">>> Preloading LHCb::ParticlePropertySvc" << endmsg; 
  }
  ppSvc() ;

  return StatusCode::SUCCESS;
}
// ============================================================================
// Execute
// ============================================================================
StatusCode DVAlgorithm::sysExecute () 
{
  
  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;

  // Make sure particles, secondary vertices, primary vertices and relations
  // tables are cleared each event.
  DVAlgorithmGuard guard(m_inputParts,
                         m_parts,
                         m_secVerts,
                         m_refittedPVs,
                         m_p2PVMap);

  StatusCode sc = loadEventInput();

  if ( sc.isFailure() ) return Error ( "Not able to load event input" , sc ) ;

  // execute the algorithm 
  sc = this->Algorithm::sysExecute();
  if ( sc.isFailure() ) return sc;
  
  if ( !m_setFilterCalled ) { 
    Warning ( "SetFilterPassed not called for this event!" ).ignore() ; 
  }
  
  /// count number of "effective filters"  
  counter("#accept") += filterPassed() ;

  if (filterPassed()) {
    sc = saveInTES();
  } else if ( m_forceOutput ) { 
    this->writeEmptyTESContainers(); 
  }
  
  // Reset for next event
  m_setFilterCalled = false;

  return sc ;

}
// ============================================================================
void DVAlgorithm::setFilterPassed  (  bool    state  ) 
{
  ///
  this->Algorithm::setFilterPassed(state); 
  m_setFilterCalled = true;
  return;
}
// ============================================================================
void DVAlgorithm::writeEmptyTESContainers() 
{
  LHCb::Particle::Container* container = new LHCb::Particle::Container();
  put(container, particleOutputLocation());

}
//=============================================================================
StatusCode DVAlgorithm::loadEventInput() {

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << ">>> loadEventInput " << endmsg;
  }

  // Retrieve Particles & Vertices from all previous processing
  // as specified in jobOptions
  
  if (!m_inputLocations.empty()) {
    StatusCode sc = loadParticles();
    if (!sc) return sc;
    sc = loadParticle2PVRelations();
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;

}
//=============================================================================
StatusCode DVAlgorithm::loadParticle2PVRelations() {

  std::vector<std::string>::const_iterator iLoc = m_p2PVInputLocations.begin();
  std::vector<std::string>::const_iterator iLocEnd = m_p2PVInputLocations.end();
  for( ; iLoc != iLocEnd; iLoc++ ) {
    if (exist<Particle2Vertex::Table>( *iLoc )) {
      if (msgLevel(MSG::DEBUG)) { 
        debug() << "Reading table from " << (*iLoc) << endmsg ; 
      }
      
      const Particle2Vertex::Table* table = get<Particle2Vertex::Table>(*iLoc);
      const Particle2Vertex::Table::Range relations = table->relations();
      loadRelations( relations );

    } else {
      Info ( "No P->PV table at " + (*iLoc)  + 
             ( rootInTES().empty() ? "" :  (" under "+rootInTES() ) ),
             StatusCode::SUCCESS, 0).ignore() ;
    }
    
  }

  return StatusCode::SUCCESS ; // could be sc
}
//=============================================================================
void DVAlgorithm::loadRelations(const Particle2Vertex::Table::Range relations)
{
  if (relations.empty()) return;

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "loadRelations reading " << relations.size()
              << " P->PV relations" << endmsg;
  }

  Particle2Vertex::Table::Range::const_iterator i = relations.begin() ;
  Particle2Vertex::Table::Range::const_iterator iEnd = relations.end() ;

  for (  ; i!= iEnd ; ++i) {
    relate(i->from(), i->to());
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Reading a " << i->from()->particleID().pid() 
                << " related to " <<  i->to()->position() << endmsg ;
    }
  }
}
//=============================================================================
StatusCode DVAlgorithm::loadParticles() {

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Looking for particles in " << m_inputLocations.size() 
            << " places" << endmsg ;
  }

  for( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
       iloc != m_inputLocations.end(); iloc++ ) {
    // Retrieve the particles:
    const std::string location = (*iloc)+"/Particles";
    if ( ! exist<LHCb::Particle::Range>( location ) ) { 
      Info ( "Non-existing location "+(*iloc) + 
             ( rootInTES().empty() ?  "" : (" under " + rootInTES() ) ),
             StatusCode::SUCCESS, 0).ignore() ;
      continue ;
    }
    LHCb::Particle::Range parts = get<LHCb::Particle::Range>( location );
    
    // statistics: 
    counter ( "# " + (*iloc) ) += parts.size() ;
    
    // Msg number of Particles retrieved
    if (msgLevel(MSG::VERBOSE)) { 
      verbose() << "    Number of Particles retrieved from "
                << location << " = " << parts.size() << endmsg;
    }
    
    m_inputParts    .reserve ( m_inputParts    .size () + parts.size() ) ;
    //    m_secVerts .reserve ( m_secVerts .size () + parts.size() ) ;

    for( LHCb::Particle::Range::const_iterator icand = parts.begin(); 
         icand != parts.end(); icand++ ) {
      m_inputParts.push_back(*icand);
//       const LHCb::Vertex* endVtx = (*icand)->endVertex();
//       if (endVtx) m_secVerts.push_back(endVtx);
    }
    
    if (msgLevel(MSG::VERBOSE)) { 
      verbose() << "Number of Particles after adding "
                << *iloc << " = " << m_parts.size() << endmsg; 
    }

  }

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "    Total number of particles " << m_parts.size() << endmsg;
  }
  
  // statistics: 
  counter ("# input particles" ) += m_inputParts.size() ; 
  
  return StatusCode::SUCCESS;
}
// ============================================================================
const LHCb::Particle* DVAlgorithm::markTree(const LHCb::Particle* particle) {
  
  Assert ( 0 != particle , 
           "mark: Attempt to mark invalid particle for saving" );

  if ( DaVinci::Utils::inTES ( particle ) ) { return particle ; }
  
  DaVinci::CloneMap clonemap ;
  //
  const LHCb::Particle* newp = 
    DaVinci::cloneTree ( particle , clonemap , m_parts , m_secVerts ) ;
  
  // copy relations directly from table to avoid triggering any new P->PV 

  const LHCb::VertexBase* bestPV = getStoredBestPV(particle);

  if (0!=bestPV) relate(newp, bestPV);

  return newp ;
}
//=============================================================================
const LHCb::Particle* DVAlgorithm::cloneAndMarkTree(const LHCb::Particle* particle){
  return markTree(particle->clone());

}
//=============================================================================
const LHCb::RecVertex* DVAlgorithm::mark( const LHCb::RecVertex* keptV )const {

  if ( 0==keptV ){
    Exception("DVAlgorithm::mark: Attempt to mark NULL Vertex for saving") ;
    return 0; 
  }

  // Input vertex is given check if it already exist in the TES
  if( DaVinci::Utils::inTES( keptV ) ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << " Vertex is in TES" << endmsg;
    return keptV;
  }
  
  // Create new vertex on the heap
  LHCb::RecVertex* newV = new LHCb::RecVertex(*keptV);
  // Put in the local container
  m_refittedPVs.push_back(newV);

  if (msgLevel(MSG::VERBOSE)) verbose() << "   -> Create new and keep " << endmsg ;

  return newV;

}
//=============================================================================
StatusCode DVAlgorithm::saveP2PVRelations() const {
  
  if (!saveP2PV()) {
    return Info("Not saving P2PV", StatusCode::SUCCESS, 0);
  }

  if ( primaryVertices().empty()) {
    if ( msgLevel ( MSG::DEBUG ) ) { 
      debug() <<"Skip saveP2PVRelations: No Primary Vertices" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  if (m_forceP2PVBuild) buildP2PVMap();

  if (m_p2PVMap.empty() ) {
    if ( msgLevel ( MSG::DEBUG ) ) { 
      debug() <<"Skip saveP2PVRelations: No relations" << endmsg;
    }
    return StatusCode::SUCCESS;    
  }
  

  LHCb::RecVertex::ConstVector verticesToSave;
  verticesToSave.reserve(m_p2PVMap.size());
  
  Particle2Vertex::Table* table = new Particle2Vertex::Table(m_p2PVMap.size());

  put(table, tableOutputLocation() );

  P2PVMap::const_iterator iRelation = m_p2PVMap.begin();
  P2PVMap::const_iterator iRelationEnd = m_p2PVMap.end();
  
  for ( ; iRelation != iRelationEnd ; ++iRelation) {
    
    const LHCb::Particle* particle = (*iRelation).first;
    if (DaVinci::Utils::inTES(particle)) {
      const LHCb::VertexBase* vb = (*iRelation).second;
      if (0!=vb) {
        const LHCb::RecVertex* pv = dynamic_cast<const LHCb::RecVertex*>(vb);
        if (0!=pv) {
          verticesToSave.push_back(pv);
          table->i_removeFrom( particle ).ignore();
          table->i_relate( particle, pv ).ignore();
        } else {
          return Error("VertexBase to RecVertex dynamic cast FAILED");
        }
      }
    }
  }
  
  // now save re-fitted vertices
  if (msgLevel(MSG::VERBOSE)) verbose() << "Passing " << verticesToSave.size()
                                        << " to saveReFittedPVs" << endmsg;

  saveRefittedPVs(verticesToSave);

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Saved table to " 
            << m_outputLocation+"/Particle2VertexRelations" 
            << endmsg ;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
void DVAlgorithm::buildP2PVMap() const{

  if (m_parts.empty()) return;
  
  LHCb::Particle::ConstVector particles;
  LHCb::Vertex::ConstVector vertices;

  LHCb::Particle::ConstVector::const_iterator iPart = m_parts.begin();
  LHCb::Particle::ConstVector::const_iterator iPartEnd = m_parts.end();

  for ( ; iPart != iPartEnd ; ++iPart ) {
    DaVinci::Utils::findDecayTree( *iPart, particles, vertices);
  }

  if (particles.empty()) return;

  iPart = particles.begin();
  iPartEnd = particles.end();

  for ( ; iPart != iPartEnd ; ++iPart ) {
    if (DaVinci::Utils::inTES(*iPart) ) bestPV(*iPart);
  }

  return;
}
//=============================================================================
  void DVAlgorithm::saveRefittedPVs(const LHCb::RecVertex::ConstVector& vToSave) const
{
  if ( vToSave.empty() ) return;

  LHCb::RecVertices* verticesToSave = new LHCb::RecVertex::Container();

  const std::string location(m_outputLocation+"/_RefitPVs");

  put(verticesToSave,location);

  LHCb::RecVertex::ConstVector::const_iterator iPV = vToSave.begin();
  LHCb::RecVertex::ConstVector::const_iterator iPVEnd = vToSave.end();

  for( ; iPV != iPVEnd; ++iPV ) {
    // Check if this was already in a Gaudi container (hence in TES)
    if( !DaVinci::Utils::inTES(*iPV) ) {
      verticesToSave->insert(const_cast<LHCb::RecVertex*>(*iPV));
    }
  }

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Saved " << verticesToSave->size()
	      << " new re-fitted PVs in " << location 
	      << " from " << vToSave.size()
	      << " vertices in local storage " << endmsg;
  }

}
// ============================================================================
StatusCode DVAlgorithm::_saveInTES() 
{
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "DVAlgorithm::_saveInTES "<< m_parts.size() << " Particles" << endmsg;
  }
 
  StatusCode sc = StatusCode::SUCCESS;
 
  LHCb::Particle::Container* p_tes = new LHCb::Particle::Container () ;
  LHCb::Vertex::Container*   v_tes = new LHCb::Vertex::Container   () ;

  if (0==p_tes || 0==v_tes) {
    return Error("Could not open output TES containers");
  }

  put ( p_tes , particleOutputLocation()        ) ;
  put ( v_tes , decayVertexOutputLocation() ) ;
 
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Going to save " 
              << m_parts.size() << " particles" << endmsg;
  }
  
  LHCb::Particle::ConstVector::const_iterator iParticle = m_parts.begin();
  LHCb::Particle::ConstVector::const_iterator iParticleEnd = m_parts.end();
  
  for ( ; iParticle != iParticleEnd ; ++iParticle) {
    // Check if this was already in a Gaudi container (hence in TES)
    if (  !DaVinci::Utils::inTES(*iParticle) ) {
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << "  Saving " <<  *iParticle << endmsg;
      }
      if (0!=*iParticle) {
        p_tes->insert(const_cast<LHCb::Particle*>(*iParticle));
        const LHCb::Vertex* endVtx = (*iParticle)->endVertex();
        if (0!=endVtx && !DaVinci::Utils::inTES(endVtx)) {
          v_tes->insert(const_cast<LHCb::Vertex*>(endVtx));
        }
      }
    } else {
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << "Skipping " << *iParticle << endmsg;
      }
    }
  }

  // check that the decay trees are fully in the TES
  iParticle = m_parts.begin();
  for ( ; iParticle != iParticleEnd ; ++iParticle) {
    if (! DaVinci::Utils::decayTreeInTES(*iParticle) ) {
      return Error("Element of saved decay tree not in TES. Likely memory leak!");
    }    
  }
  
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Saved " << p_tes->size()
              << " new particles in " << particleOutputLocation() 
              << " from " << m_parts.size()
              << " total particles in local storage" << endmsg;
  }
 
  return StatusCode::SUCCESS;
 
}

// ============================================================================
StatusCode DVAlgorithm::saveInTES() 
{

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "saveInTES "<< m_parts.size() << " Particles" << endmsg;
  }
  

  
  StatusCode sc = _saveInTES();
  if (sc.isFailure()) return sc;

  // now save relations table
  if (msgLevel(MSG::VERBOSE)) verbose() << "Save P->PV relations" << endmsg;

  saveP2PVRelations();

  return StatusCode::SUCCESS;
 
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::calculateRelatedPV(const LHCb::Particle* p) const
{

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "DVAlgorithm::calculateRelatedPV" << endmsg;
  }

  const LHCb::RecVertex::Range PVs = this->primaryVertices();

  if (PVs.empty()) {
    Error("calculateRelatedPV: Empty primary vertex container", 
          StatusCode::FAILURE, 0 ).ignore();
    return 0;
  }

  // 1 PV

  const IPVReFitter* fitter = defaultPVReFitter();

  if (PVs.size()==1) {
    if (!refitPVs()) return *(PVs.begin());
    LHCb::RecVertex reFittedPV = LHCb::RecVertex(**(PVs.begin()));
    return ( (fitter->remove(p, &reFittedPV)).isSuccess() ) ?
      mark(&reFittedPV) : 0;
  }

  // more than one PV
  
  const IRelatedPVFinder* finder = this->relatedPVFinder();

  if (!refitPVs()) { // no PV re-fit
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Getting related PV from finder" << endmsg;
    }
    
    const LHCb::VertexBase* pv = finder->relatedPV(p, PVs);

    if (msgLevel(MSG::VERBOSE)) {
      if (0!=pv) {
        verbose() << "Returning related vertex\n" 
                  << pv << endmsg;
      } else {
        verbose() << "no related PV found" << endmsg;
      }
    }
     return pv;
  } else {
  // re-fit vertices, then look for the best one.
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Re-fitting " << PVs.size() << " PVs"<< endmsg;
    }

    LHCb::RecVertex::ConstVector reFittedPVs;
    DaVinci::Utils::PointerContainerGuard<LHCb::RecVertex::ConstVector> guard(reFittedPVs);
    for (LHCb::RecVertex::Range::const_iterator iPV = PVs.begin();
         iPV != PVs.end(); ++iPV) {
      LHCb::RecVertex* reFittedPV = new LHCb::RecVertex(**iPV);
      if ( (fitter->remove(p, reFittedPV)).isSuccess() ) {
        reFittedPVs.push_back(reFittedPV); 
      } else {
        delete reFittedPV;
        Error("PV re-fit failed", StatusCode::FAILURE, 1 ).ignore() ;
      }
    }
    if (reFittedPVs.empty()) {

      Warning("Failed to create refitted PV list for event with "+boost::lexical_cast<std::string>(PVs.size())+ " PVs", 
            StatusCode::FAILURE, 0).ignore();
      return 0;
    }
    
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "have " << reFittedPVs.size()
                << " re-fitted PVs" << endmsg;
    }
    
    const LHCb::VertexBase* vb = finder->relatedPV(p, reFittedPVs);

    if (0==vb) {
      Warning("IRelatedPVFinder found no best vertex",
              StatusCode::FAILURE, 0).ignore();
      return 0;
    }
    
    const LHCb::RecVertex*  pv = dynamic_cast<const LHCb::RecVertex*>(vb) ;

    if (0==pv) {
      Warning("VertexBase -> RecVertex dynamic cast failed",
              StatusCode::FAILURE, 0).ignore();
      return 0;
    } else {
      return mark(pv);
    }
  }
  
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::getRelatedPV(const LHCb::Particle* part) const
{

  if (msgLevel(MSG::VERBOSE)) verbose() << "getRelatedPV! Getting range" << endmsg;
  if (0==part) {
    error() << "input particle is NULL" << endmsg;
    return 0;
  }

  if (!hasStoredRelatedPV(part) ) {
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "particle2Vertices empty. Calling calculateRelatedPV" 
                << endmsg;
    }
    const LHCb::VertexBase* pv = calculateRelatedPV(part);
    if (0!=pv) {
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << "Found related vertex. Relating it" << endmsg;
      }
      relate(part, pv);
      return pv;
    } else {
      Warning("Found no related vertex", StatusCode::FAILURE, 0).ignore();
      return 0;
    }
  } else {
    return getStoredBestPV(part);    
  }
  
}
// ============================================================================
// Finalize the algorithm + post-actions
// ============================================================================
StatusCode DVAlgorithm::finalize () 
{

  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;
  
  // finalize GaudiTupleAlg base class
  return GaudiTupleAlg::finalize();

} 
// ============================================================================
// The END 
// ============================================================================
