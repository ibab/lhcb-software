// $Id: DVAlgorithm.cpp,v 1.84 2010-08-27 13:48:25 jpalac Exp $
// ============================================================================
// Include 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IOnOffline.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Kernel/DaVinciFun.h"
#include "Kernel/DaVinciGuards.h"
#include "Kernel/DaVinciStringUtils.h"
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
  , m_desktopName           ( "PhysDesktop" )
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
  , m_ignoreP2PVFromInputLocations (false)
  , m_PVLocation            ("")
  , m_noPVs                 ( false )
{
  m_inputLocations.clear() ;
  declareProperty( "InputLocations", 
                   m_inputLocations, 
                   "Input Locations forwarded to PhysDesktop" );

  m_p2PVInputLocations.clear() ;
  declareProperty( "P2PVInputLocations", 
                   m_p2PVInputLocations, 
                   "Particle -> PV Relations Input Locations forwarded to PhysDesktop" );

  declareProperty( "InputPrimaryVertices", m_PVLocation );

  declareProperty("UseP2PVRelations", m_useP2PV,
                  "Use P->PV relations internally. Forced to true if re-fitting PVs. Otherwise disabled for single PV events. Default: true.");

  declareProperty("WriteP2PVRelations", m_writeP2PV, 
                  "Write out P->PV relations table to TES. Default: true");

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
  declareProperty ( "AvoidForcedOutput" , m_avoidEmptyOutput  = false , "If true TES location is written" ) ;
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
  


  if (msgLevel(MSG::DEBUG)) debug() << "End of DVAlgorithm::initialize with " << sc << endmsg;
  
  return sc;
}
// ============================================================================
// Load standard tools
// ============================================================================
StatusCode DVAlgorithm::loadTools() 
{
  
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading tools" << endmsg;
 
  DaVinci::StringUtils::expandLocations( m_inputLocations.begin(),
                                         m_inputLocations.end(),
                                         onOffline()->trunkOnTES() );
 
  if (msgLevel(MSG::DEBUG)) {
    debug() << ">>> Preloading PhysDesktop locations " << m_inputLocations << endmsg;
  }

  for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
        iloc != m_inputLocations.end(); ++iloc ) {
    DaVinci::StringUtils::removeEnding(*iloc,"/Particles");
  }
  
  desktop()->setInputLocations(m_inputLocations);
  std::vector<std::string> p2pvInputLocations;
  

  if (!m_ignoreP2PVFromInputLocations) {
    // load default P->PV locations.
    for ( std::vector<std::string>::iterator iloc = m_inputLocations.begin();
          iloc != m_inputLocations.end(); ++iloc ) {
      p2pvInputLocations.push_back((*iloc)+"/Particle2VertexRelations");
    }
    
  }

  if (!m_p2PVInputLocations.empty() ) 
  {
    // load user-defined P->PV locations.
    DaVinci::StringUtils::expandLocations( m_p2PVInputLocations.begin(),
                                           m_p2PVInputLocations.end(),
                                           onOffline()->trunkOnTES()     );

    for ( std::vector<std::string>::iterator iloc = m_p2PVInputLocations.begin();
          iloc != m_p2PVInputLocations.end(); ++iloc ) {
      p2pvInputLocations.push_back((*iloc));
    }

  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << ">>> Preloading PhysDesktop with P->PV locations " 
            << p2pvInputLocations << endmsg;
  }
  

  desktop()->setP2PVInputLocations(p2pvInputLocations);   

  desktop()->setWriteP2PV( m_writeP2PV && !m_noPVs );
 
  std::string outputLocation = this->name();
  
  DaVinci::StringUtils::expandLocation(outputLocation,
                                       onOffline()->trunkOnTES());
  
  desktop()->setOutputLocation(outputLocation);
  
 
  if ( !m_preloadTools ) 
  { return Warning( "Not preloading tools", StatusCode::SUCCESS ) ; }


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
  { debug() << ">>> Preloading CheckOverlap Tool" << endmsg; }
  checkOverlap();
  
  if (msgLevel(MSG::DEBUG)) 
  { debug() << ">>> Preloading LHCb::ParticlePropertySvc" << endmsg; }
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

  DaVinci::Guards::CleanDesktopGuard desktopGuard(desktop());

  StatusCode sc = desktop()->getEventInput();
  if ( sc.isFailure()) 
  { return Error (  "Not able to fill PhysDesktop" , sc ) ; }
  
  // execute the algorithm 
  sc = this->Algorithm::sysExecute();
  if ( sc.isFailure() ) return sc;
  
  if ( !m_setFilterCalled ) 
  { Warning ( "SetFilterPassed not called for this event!" ).ignore() ; }
  
  /// count number of "effective filters"  
  counter("#accept") += filterPassed() ;
  
  // Reset for next event
  m_setFilterCalled = false;
  
  // Make sure each DVAlgorithm has written out something
  if ( !m_avoidEmptyOutput ) 
  { sc = this->writeEmptyContainerIfNeeded(); }
  else 
  { verbose() << "Avoiding mandatory output" << endmsg ; }

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
StatusCode DVAlgorithm::writeEmptyContainerIfNeeded() 
{
  return desktop()->writeEmptyContainerIfNeeded();
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::calculateRelatedPV(const LHCb::Particle* p) const
{

  if (msgLevel(MSG::VERBOSE)) verbose() << "DVAlgorithm::calculateRelatedPV" << endmsg;
  const IRelatedPVFinder* finder = this->relatedPVFinder();
  const LHCb::RecVertex::Range PVs = this->primaryVertices();
  if ( 0==finder || PVs.empty() ) {
    Error("NULL IRelatedPVFinder or primary vertex container", StatusCode::FAILURE, 1 ).ignore() ;
    return 0;
  }
  // re-fit vertices, then look for the best one.
  if ( refitPVs() ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Re-fitting " 
                                          << PVs.size() << " PVs"<< endmsg;
    const IPVReFitter* fitter = primaryVertexReFitter();
    if (0==fitter) {
      Error("NULL IPVReFitter", StatusCode::FAILURE, 1).ignore();
      return 0;
    }
    LHCb::RecVertex::ConstVector reFittedPVs;
    DaVinci::Guards::PointerContainerGuard<LHCb::RecVertex::ConstVector> guard(reFittedPVs);
    for (LHCb::RecVertex::Range::const_iterator iPV = PVs.begin();
         iPV != PVs.end(); ++iPV) {
      LHCb::RecVertex* reFittedPV = new LHCb::RecVertex(**iPV);
      if ( (fitter->remove(p, reFittedPV)).isSuccess() ) {
        reFittedPVs.push_back(reFittedPV); 
      } else {
        delete  reFittedPV;
        Error("PV re-fit failed", StatusCode::FAILURE, 1 ).ignore() ;
      } 
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "have " << reFittedPVs.size()
                                          << " re-fitted PVs" 
                                          << endmsg;

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
      const LHCb::RecVertex* returnPV = desktop()->keep(pv);
      return returnPV;
    }
  } else { // no PV re-fit
    if (msgLevel(MSG::VERBOSE)) verbose() << "Getting related PV from finder" << endmsg;
    const LHCb::VertexBase* pv  = finder->relatedPV(p, LHCb::RecVertex::ConstVector(PVs.begin(), PVs.end()));

    if (0!=pv) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Returning related vertex\n" 
                                            << pv << endmsg;
    } else {
      if (msgLevel(MSG::VERBOSE)) verbose() << "no related PV found" << endmsg;
    }
     return pv;
  }
  
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::_getRelatedPV(const LHCb::Particle* part) const
{
  //  always() << "ATTENTION! called getRelatedPV!" << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << "_getRelatedPV! Getting range" << endmsg;
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
      relateWithOverwrite(part, pv);
      return pv;
    } else {
      Warning("Found no related vertex", StatusCode::FAILURE, 0).ignore();
      return 0;
    }
  } else {
    const Particle2Vertex::Table::Range range = desktop()->particle2Vertices(part);
    return DaVinci::bestVertexBase(range);
  }
  
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::getStoredBestPV(const LHCb::Particle* particle) const
{
  if ( hasStoredRelatedPV(particle) ) {
    const Particle2Vertex::Table::Range range = desktop()->particle2Vertices(particle);
    return DaVinci::bestVertexBase(range);
  } else {
    return 0;
  }
  
  
}
// ============================================================================
bool DVAlgorithm::hasStoredRelatedPV(const LHCb::Particle* particle) const
{
  const Particle2Vertex::Table::Range p2pvRange = desktop()->particle2Vertices(particle);
  return !p2pvRange.empty();
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
void DVAlgorithm::imposeOutputLocation(const std::string& outputLocationString)
{
  if ( 0==desktop() ) {
    fatal() << "Desktop has not been created yet" << endmsg;
  }
  desktop()->setOutputLocation(outputLocationString);  
  return;  
}
// ============================================================================
// The END 
// ============================================================================
