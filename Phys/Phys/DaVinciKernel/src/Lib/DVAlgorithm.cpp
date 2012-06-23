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
DVAlgorithm::DVAlgorithm( const std::string& name,
                          ISvcLocator* pSvcLocator )
  : base_class( name , pSvcLocator )
//
  , m_particleOutputLocation("")
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
  , m_defaultPVReFitter     (NULL)
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
  , m_checkOverlap          ( NULL  )
  , m_taggingToolName       ( "BTaggingTool" )
  , m_taggingTool           ( NULL  )
  , m_descendants           ( NULL  )
  , m_descendantsName       ("ParticleDescendants")
  , m_onOffline             ( NULL  )
  , m_pvRelator             ( NULL  )
  , m_ppSvc                 ( NULL  )
  , m_setFilterCalled       ( false )
  , m_refitPVs              ( false )
  , m_multiPV               ( false )
  , m_useP2PV               ( true  )
  , m_writeP2PV             ( true  )
  , m_forceP2PVBuild        ( true  )
  , m_ignoreP2PVFromInputLocations (false)
  , m_PVLocation            ( ""    )
  , m_noPVs                 ( false )
{
  declareProperty( "Output",
                   m_particleOutputLocation,
                   "Output Location of Particles" );

  m_inputLocations.clear() ;
  declareProperty( "Inputs",
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
StatusCode DVAlgorithm::initialize ()
{

  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;

  // initialize the base
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( !registerContext() || !contextSvc() )
  {
    Warning( "Registration for Algorithm Context Service is disabled. Some tools/utilities could have problems." ).ignore();
  }

  // Load tools very
  sc = loadTools() ;
  if ( sc.isFailure() ) { return Error("Unable to load tools",sc); }

  if (msgLevel(MSG::DEBUG))
  {
    if ( m_decayDescriptor.empty() )
    { debug() << "Decay Descriptor string not specified"   << endmsg; }
    else
    { debug() << "Decay Descriptor: " << m_decayDescriptor << endmsg; }
  }

  if ( m_PVLocation.empty() )
  {
    m_PVLocation = onOffline()->primaryVertexLocation();
  }

  m_noPVs = ( m_PVLocation.empty() || m_PVLocation == "None" );

  initializeLocations();

  // load PV processing tools.

  if (!m_noPVs)
  {
    if (refitPVs())
    {
      if (!defaultPVReFitter())
      {
        return Error("Default IPVReFitter could not be loaded");
      }
    }

    if (!relatedPVFinder())
    {
      return Error("IRelatedPVFinder could not be loaded");
    }
  }

  if (msgLevel(MSG::DEBUG))
  {
    debug() << "End of DVAlgorithm::initialize with " << sc << endmsg;
  }

  return sc;
}
// ============================================================================
void DVAlgorithm::initializeLocations()
{

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << ">>> Initialised locations " << m_inputLocations
            << endmsg;
  }

  for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
        iloc != m_inputLocations.end(); ++iloc )
  {
    DaVinci::StringUtils::removeEnding(*iloc,"/Particles");
  }

  if ( !m_ignoreP2PVFromInputLocations )
  {
    // load default P->PV locations.
    for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
          iloc != m_inputLocations.end(); ++iloc )
    {
      m_p2PVInputLocations.push_back((*iloc)+"/Particle2VertexRelations");
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << ">>> Initialised P->PV locations "
            << m_p2PVInputLocations << endmsg;
  }

  if ( m_particleOutputLocation.empty() )
  {
    m_outputLocation = this->name();
    DaVinci::StringUtils::expandLocation(m_outputLocation,
                                         "Phys");
    m_particleOutputLocation = m_outputLocation+"/Particles";
  }
  else
  {
    m_outputLocation = m_particleOutputLocation;
    DaVinci::StringUtils::removeEnding(m_outputLocation,"/");
  }

  m_decayVertexOutputLocation = m_outputLocation+"/decayVertices";
  m_tableOutputLocation = m_outputLocation+"/Particle2VertexRelations";

}

// ============================================================================
// Load standard tools
// ============================================================================
StatusCode DVAlgorithm::loadTools()
{

  if ( !m_preloadTools )
  {
    return Warning( "Not preloading tools", StatusCode::SUCCESS ) ;
  }

  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading tools" << endmsg;
  // vertex fitter

  if ( m_particleCombinerNames.end() == m_particleCombinerNames.find("") )
  {
    m_particleCombinerNames[""] = onOffline()->particleCombinerType() ;
  }

  if ( m_vertexFitNames.end() == m_vertexFitNames.find("") )
  {
    m_vertexFitNames[""] = onOffline()->vertexFitterType() ;
  }

  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading "
                                    << m_vertexFitNames[""] << " as IVertexFit " << endmsg;
  vertexFitter() ;

  // geometry THIS IS OBSOLETE
  if ( m_distanceCalculatorNames.end() == m_distanceCalculatorNames.find("") )
  {
    m_distanceCalculatorNames[""] = onOffline()->distanceCalculatorType() ;
  }

  // distance geometry
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading "
                                    << m_distanceCalculatorNames[""]
                                    << " as IDistanceCalculator" << endmsg;
  distanceCalculator();

  if (msgLevel(MSG::DEBUG))
  {
    debug() << ">>> Preloading CheckOverlap Tool" << endmsg;
  }
  checkOverlap();

  if (msgLevel(MSG::DEBUG))
  {
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
  IAlgContextSvc* ctx = NULL;
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

  if ( !m_setFilterCalled )
  {
    Warning ( "SetFilterPassed not called for this event!" ).ignore() ;
  }

  // count number of "effective filters"
  counter("#accept") += filterPassed() ;

  if ( filterPassed() )
  {
    sc = saveInTES();
  }
  else if ( m_forceOutput )
  {
    this->writeEmptyTESContainers();
  }

  // Reset for next event
  m_setFilterCalled = false;

  return sc ;
}
// ============================================================================
void DVAlgorithm::setFilterPassed( bool state )
{
  this->Algorithm::setFilterPassed(state);
  m_setFilterCalled = true;
}
// ============================================================================
void DVAlgorithm::writeEmptyTESContainers()
{
  LHCb::Particle::Container* container = new LHCb::Particle::Container();
  put(container, particleOutputLocation());

}
//=============================================================================
StatusCode DVAlgorithm::loadEventInput()
{

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << ">>> loadEventInput " << endmsg;
  }

  // Retrieve Particles & Vertices from all previous processing
  // as specified in jobOptions

  StatusCode sc = StatusCode::SUCCESS;

  if (!m_inputLocations.empty())
  {
    sc = loadParticles() && loadParticle2PVRelations();
  }

  return sc;
}
//=============================================================================
StatusCode DVAlgorithm::loadParticle2PVRelations()
{
  for( std::vector<std::string>::const_iterator iLoc = m_p2PVInputLocations.begin();
       iLoc != m_p2PVInputLocations.end(); ++iLoc )
  {
    if (exist<Particle2Vertex::Table>( *iLoc ))
    {
      if (msgLevel(MSG::DEBUG))
      {
        debug() << "Reading table from " << (*iLoc) << endmsg ;
      }

      const Particle2Vertex::Table * table = get<Particle2Vertex::Table>(*iLoc);
      const Particle2Vertex::Table::Range& relations = table->relations();
      loadRelations( relations );

    }
    else
    {
      Info ( "No P->PV table at " + (*iLoc)  +
             ( rootInTES().empty() ? "" :  (" under "+rootInTES() ) ),
             StatusCode::SUCCESS, 0).ignore() ;
    }

  }

  return StatusCode::SUCCESS ; // could be sc
}
//=============================================================================
void DVAlgorithm::loadRelations(const Particle2Vertex::Table::Range& relations)
{
  if (relations.empty()) return;

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "loadRelations reading " << relations.size()
              << " P->PV relations" << endmsg;
  }

  for ( Particle2Vertex::Table::Range::const_iterator i = relations.begin();
        i != relations.end(); ++i )
  {
    relate(i->from(), i->to());
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "Reading a " << i->from()->particleID().pid()
                << " related to " <<  i->to()->position() << endmsg ;
    }
  }
}
//=============================================================================
StatusCode DVAlgorithm::loadParticles()
{
  if (msgLevel(MSG::DEBUG))
  {
    debug() << "Looking for particles in " << m_inputLocations.size()
            << " places" << endmsg ;
  }

  for( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
       iloc != m_inputLocations.end(); ++iloc )
  {
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

    m_inputParts    .reserve ( m_inputParts    .size () + parts.size() ) ;

    for( LHCb::Particle::Range::const_iterator icand = parts.begin();
         icand != parts.end(); icand++ )
    {
      m_inputParts.push_back(*icand);
    }

    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "Number of Particles after adding "
                << *iloc << " = " << m_parts.size() << endmsg;
    }

  }

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "    Total number of particles " << m_parts.size() << endmsg;
  }

  // statistics:
  counter ("# input particles" ) += m_inputParts.size() ;

  return StatusCode::SUCCESS;
}
// ============================================================================
const LHCb::Particle* DVAlgorithm::markTree(const LHCb::Particle* particle)
{
  Assert ( NULL != particle ,
           "mark: Attempt to mark invalid particle for saving" );

  if ( DaVinci::Utils::inTES ( particle ) ) { return particle ; }

  DaVinci::CloneMap clonemap ;
  //
  const LHCb::Particle* newp =
    DaVinci::cloneTree ( particle , clonemap , m_parts , m_secVerts ) ;

  // copy relations directly from table to avoid triggering any new P->PV
  const LHCb::VertexBase* bestPV = getStoredBestPV(particle);

  if (bestPV) relate(newp, bestPV);

  return newp;
}
//=============================================================================
const LHCb::Particle* DVAlgorithm::cloneAndMarkTree(const LHCb::Particle* particle)
{
  return markTree(particle->clone());
}
//=============================================================================
const LHCb::RecVertex* DVAlgorithm::mark( const LHCb::RecVertex* keptV )const
{
  if ( !keptV )
  {
    Exception("DVAlgorithm::mark: Attempt to mark NULL Vertex for saving") ;
    return 0;
  }

  // Input vertex is given check if it already exist in the TES
  if( DaVinci::Utils::inTES(keptV) )
  {
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
StatusCode DVAlgorithm::saveP2PVRelations() const
{

  if ( !saveP2PV() )
  {
    return Info("Not saving P2PV", StatusCode::SUCCESS, 0);
  }

  if ( primaryVertices().empty() )
  {
    if ( msgLevel ( MSG::DEBUG ) )
    {
      debug() <<"Skip saveP2PVRelations: No Primary Vertices" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  if ( m_forceP2PVBuild ) buildP2PVMap();

  if ( m_p2PVMap.empty() )
  {
    if ( msgLevel ( MSG::DEBUG ) )
    {
      debug() <<"Skip saveP2PVRelations: No relations" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  LHCb::RecVertex::ConstVector verticesToSave;
  verticesToSave.reserve( m_p2PVMap.size() );

  Particle2Vertex::Table* table = new Particle2Vertex::Table(m_p2PVMap.size());

  put( table, tableOutputLocation() );

  for ( P2PVMap::const_iterator iRelation = m_p2PVMap.begin();
        iRelation != m_p2PVMap.end(); ++iRelation )
  {
    const LHCb::Particle* particle = (*iRelation).first;
    if (DaVinci::Utils::inTES(particle))
    {
      const LHCb::VertexBase* vb = (*iRelation).second;
      if (vb)
      {
        const LHCb::RecVertex* pv = dynamic_cast<const LHCb::RecVertex*>(vb);
        if (pv)
        {
          verticesToSave.push_back(pv);
          table->i_removeFrom( particle ).ignore();
          table->i_relate( particle, pv ).ignore();
        }
        else
        {
          return Error("VertexBase to RecVertex dynamic cast FAILED");
        }
      }
    }
  }

  saveRefittedPVs(verticesToSave);

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Saved table to "
            << m_outputLocation+"/Particle2VertexRelations"
            << endmsg ;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
void DVAlgorithm::buildP2PVMap() const
{

  if (m_parts.empty()) return;

  LHCb::Particle::ConstVector particles;
  LHCb::Vertex::ConstVector   vertices;

  for ( LHCb::Particle::ConstVector::const_iterator iPart = m_parts.begin();
        iPart != m_parts.end(); ++iPart )
  {
    DaVinci::Utils::findDecayTree( *iPart, particles, vertices);
  }

  if (particles.empty()) return;

  for ( LHCb::Particle::ConstVector::const_iterator iPart = particles.begin();
        iPart != particles.end() ; ++iPart )
  {
    if (DaVinci::Utils::inTES(*iPart) ) bestPV(*iPart);
  }
}
//=============================================================================
void DVAlgorithm::saveRefittedPVs(const LHCb::RecVertex::ConstVector& vToSave) const
{
  if ( vToSave.empty() ) return;

  LHCb::RecVertices* verticesToSave = new LHCb::RecVertex::Container();

  const std::string location(m_outputLocation+"/_RefitPVs");

  put( verticesToSave, location );

  for( LHCb::RecVertex::ConstVector::const_iterator iPV = vToSave.begin();
       iPV != vToSave.end(); ++iPV )
  {
    // Check if this was already in a Gaudi container (hence in TES)
    if( !DaVinci::Utils::inTES(*iPV) )
    {
      verticesToSave->insert(const_cast<LHCb::RecVertex*>(*iPV));
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Saved " << verticesToSave->size()
              << " new re-fitted PVs in " << location
              << " from " << vToSave.size()
              << " vertices in local storage " << endmsg;
  }

}
// ============================================================================
StatusCode DVAlgorithm::_saveInTES()
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "DVAlgorithm::_saveInTES "<< m_parts.size() << " Particles" << endmsg;
  }

  LHCb::Particle::Container* p_tes = new LHCb::Particle::Container () ;
  LHCb::Vertex::Container*   v_tes = new LHCb::Vertex::Container   () ;
  if ( !p_tes || !v_tes )
  {
    delete p_tes;
    delete v_tes;
    return Error("Could not open output TES containers");
  }
  put ( p_tes , particleOutputLocation()    ) ;
  put ( v_tes , decayVertexOutputLocation() ) ;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Going to save "
              << m_parts.size() << " particles" << endmsg;
  }

  for ( LHCb::Particle::ConstVector::const_iterator iParticle = m_parts.begin();
        iParticle != m_parts.end(); ++iParticle)
  {
    // Check if this was already in a Gaudi container (hence in TES)
    if ( !DaVinci::Utils::inTES(*iParticle) )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  Saving " <<  *iParticle << endmsg;
      }
      if (*iParticle)
      {
        p_tes->insert(const_cast<LHCb::Particle*>(*iParticle));
        const LHCb::Vertex* endVtx = (*iParticle)->endVertex();
        if ( endVtx && !DaVinci::Utils::inTES(endVtx))
        {
          v_tes->insert(const_cast<LHCb::Vertex*>(endVtx));
        }
      }
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Skipping " << *iParticle << endmsg;
      }
    }
  }

  // check that the decay trees are fully in the TES
  for ( LHCb::Particle::ConstVector::const_iterator iParticle = m_parts.begin();
        iParticle != m_parts.end() ; ++iParticle)
  {
    if ( ! DaVinci::Utils::decayTreeInTES(*iParticle) )
    {
      return Error("Element of saved decay tree not in TES. Likely memory leak!");
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
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
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "saveInTES "<< m_parts.size() << " Particles" << endmsg;
  }

  StatusCode sc = _saveInTES();
  if (sc.isFailure()) return sc;

  // now save relations table
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "Save P->PV relations" << endmsg;

  return saveP2PVRelations();
}
// ============================================================================
const LHCb::VertexBase*
DVAlgorithm::calculateRelatedPV(const LHCb::Particle* p) const
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "DVAlgorithm::calculateRelatedPV" << endmsg;
  }

  const LHCb::RecVertex::Range PVs = this->primaryVertices();

  if (PVs.empty())
  {
    Error("calculateRelatedPV: Empty primary vertex container",
          StatusCode::FAILURE, 0 ).ignore();
    return NULL;
  }

  // 1 PV

  const IPVReFitter* fitter = defaultPVReFitter();

  if ( PVs.size() == 1 )
  {
    if (!refitPVs()) return *(PVs.begin());
    LHCb::RecVertex reFittedPV = LHCb::RecVertex(**(PVs.begin()));
    return ( (fitter->remove(p, &reFittedPV)).isSuccess() ) ?
      mark(&reFittedPV) : NULL;
  }

  // more than one PV

  const IRelatedPVFinder* finder = this->relatedPVFinder();

  if ( !refitPVs() )
  { // no PV re-fit
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "Getting related PV from finder" << endmsg;
    }

    const LHCb::VertexBase* pv = finder->relatedPV(p, PVs);

    if (msgLevel(MSG::VERBOSE))
    {
      if (pv)
      {
        verbose() << "Returning related vertex\n"
                  << pv << endmsg;
      }
      else
      {
        verbose() << "no related PV found" << endmsg;
      }
    }
    return pv;
  }
  else
  {
    // re-fit vertices, then look for the best one.
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "Re-fitting " << PVs.size() << " PVs"<< endmsg;
    }

    LHCb::RecVertex::ConstVector reFittedPVs;
    DaVinci::Utils::PointerContainerGuard<LHCb::RecVertex::ConstVector> guard(reFittedPVs);
    for (LHCb::RecVertex::Range::const_iterator iPV = PVs.begin();
         iPV != PVs.end(); ++iPV)
    {
      LHCb::RecVertex* reFittedPV = new LHCb::RecVertex(**iPV);
      if ( (fitter->remove(p, reFittedPV)).isSuccess() )
      {
        reFittedPVs.push_back(reFittedPV);
      }
      else
      {
        delete reFittedPV;
        Error("PV re-fit failed", StatusCode::FAILURE, 1 ).ignore() ;
      }
    }
    if ( reFittedPVs.empty() )
    {

      Warning( "Failed to create refitted PV list for event with " +
               boost::lexical_cast<std::string>(PVs.size())+ " PVs",
               StatusCode::FAILURE, 0 ).ignore();
      return NULL;
    }

    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "have " << reFittedPVs.size()
                << " re-fitted PVs" << endmsg;
    }

    const LHCb::VertexBase* vb = finder->relatedPV(p, reFittedPVs);

    if (!vb)
    {
      Warning("IRelatedPVFinder found no best vertex",
              StatusCode::FAILURE, 0).ignore();
      return NULL;
    }

    const LHCb::RecVertex*  pv = dynamic_cast<const LHCb::RecVertex*>(vb) ;

    if (!pv)
    {
      Warning("VertexBase -> RecVertex dynamic cast failed",
              StatusCode::FAILURE, 0).ignore();
      return NULL;
    }
    else
    {
      return mark(pv);
    }
  }

}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::getRelatedPV(const LHCb::Particle* part) const
{

  if (msgLevel(MSG::VERBOSE)) verbose() << "getRelatedPV! Getting range" << endmsg;
  if (!part)
  {
    error() << "input particle is NULL" << endmsg;
    return NULL;
  }

  if ( !hasStoredRelatedPV(part) )
  {
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "particle2Vertices empty. Calling calculateRelatedPV"
                << endmsg;
    }
    const LHCb::VertexBase* pv = calculateRelatedPV(part);
    if (pv)
    {
      if (msgLevel(MSG::VERBOSE))
      {
        verbose() << "Found related vertex. Relating it" << endmsg;
      }
      relate( part, pv );
      return pv;
    }
    else
    {
      Warning("Found no related vertex", StatusCode::FAILURE, 0).ignore();
      return NULL;
    }
  }
  else
  {
    return getStoredBestPV(part);
  }
}

// ============================================================================
// Finalize the algorithm + post-actions
// ============================================================================
StatusCode DVAlgorithm::finalize ()
{
  // register for the algorithm context service
  IAlgContextSvc* ctx = NULL ;
  if ( registerContext() ) { ctx = contextSvc() ; }

  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;

  // finalize GaudiTupleAlg base class
  return GaudiTupleAlg::finalize();
}
// ============================================================================
// The END
// ============================================================================
