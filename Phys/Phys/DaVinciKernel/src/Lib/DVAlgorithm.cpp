// $Id: DVAlgorithm.cpp,v 1.36 2008-12-05 13:27:04 ibelyaev Exp $
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
  : GaudiTupleAlg ( name , pSvcLocator )
  //
  , m_desktop               ( 0 )
  , m_desktopName           ( "PhysDesktop" )
  // 
  , m_vertexFitNames        () 
  , m_vertexFits            () 
  //
  , m_geomToolNames         () 
  , m_geomTools             () 
  //
  , m_filterNames           () 
  , m_filters               () 
  //
  , m_criteriaNames         () 
  , m_criteria              () 
  //
  , m_particleCombinerNames ()
  , m_particleCombiners     ()
  // 
  , m_particleReFitterNames ()
  , m_particleReFitters     ()
  //
  , m_massFitterNames       ()
  , m_massFitters           ()
  //
  , m_massVertexFitterNames ()
  , m_massVertexFitters     ()
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
  , m_descendants           (0)
  , m_descendantsName         ("ParticleDescendants")
  , m_writeSelResultName      ( "WriteSelResult" )
  , m_writeSelResult          ( 0 )
  , m_ppSvc                  (0)
  , m_setFilterCalled          (false)
  , m_countFilterWrite        (0)
  , m_countFilterPassed       (0)
{
  // 
  m_vertexFitNames [ "Offline"       ] = "OfflineVertexFitter" ;
  m_vertexFitNames [ "Trigger"       ] = "TrgVertexFitter"     ;
  m_vertexFitNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_vertexFitNames [ "LoKi"          ] = "LoKi::VertexFitter"  ;
  m_vertexFitNames [ "Kalman"        ] = "LoKi::VertexFitter"  ;
  m_vertexFitNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  declareProperty ( "VertexFitters"     , m_vertexFitNames    ) ;
  //
  m_geomToolNames  [ "" ] = "GeomDispCalculator" ;
  m_geomToolNames  [ "Offline" ] = "GeomDispCalculator" ;
  declareProperty ( "GeomTools"         , m_geomToolNames     ) ;
  //
  declareProperty ( "CheckOverlapTool"  , m_checkOverlapName  ) ;
  //
  declareProperty ( "WriteSelResultTool"  , m_writeSelResultName  ) ;
  //
  m_filterNames    [ "" ]        = "ParticleFilter" ;
  declareProperty ( "ParticleFilters"   , m_filterNames       ) ;
  // 
  declareProperty ( "FilterCriteria"    , m_criteriaNames     ) ; // empty! 
  // 
  m_particleCombinerNames [ ""              ] = "OfflineVertexFitter" ;
  m_particleCombinerNames [ "Offline"       ] = "OfflineVertexFitter" ;
  m_particleCombinerNames [ "Trigger"       ] = "TrgVertexFitter"     ;
  m_particleCombinerNames [ "Kalman"        ] = "LoKi::VertexFitter"  ;
  m_particleCombinerNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_particleCombinerNames [ "LoKi"          ] = "LoKi::VertexFitter"  ;
  m_particleCombinerNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  declareProperty ( "ParticleCombiners"  , m_particleCombinerNames ) ;
  //
  m_particleReFitterNames [ ""              ] = "OfflineVertexFitter"   ;
  m_particleReFitterNames [ "Offline"       ] = "OfflineVertexFitter"   ;
  m_particleReFitterNames [ "Blind"         ] = "BlindVertexFitter"     ;
  m_particleReFitterNames [ "Kalman"        ] = "LoKi::VertexFitter"    ;
  m_particleReFitterNames [ "LoKi"          ] = "LoKi::VertexFitter"    ;
  m_particleReFitterNames [ "Mass"          ] = "LoKi::MassFitter"      ;
  m_particleReFitterNames [ "Direction"     ] = "LoKi::DirectionFitter" ;
  m_particleReFitterNames [ "ParticleAdder" ] = "ParticleAdder"         ;
  declareProperty  ( "ParticleReFitters" , m_particleReFitterNames ) ;
  //
  m_massFitterNames [ ""     ] = "LoKi::MassFitter" ;
  m_massFitterNames [ "LoKi" ] = "LoKi::MassFitter" ;
  declareProperty  
    ( "MassFitters"       , m_massFitterNames       , 
      "The mapping of nick/name/type for IMassFit tools"        ) ;
  //
  declareProperty  
    ( "MassVertexFitters" , m_massVertexFitterNames , 
      "The mapping of nick/name/type for IMassVertexFit tools"  ) ;
  //
  m_lifetimeFitterNames  [ ""     ] = "PropertimeFitter"     ;
  m_lifetimeFitterNames  [ "LoKi" ] = "LoKi::LifetimeFitter" ;
  declareProperty  
    ( "LifetimeFitters"    , m_lifetimeFitterNames , 
      "The mapping of nick/name/type for ILifetimeFitter tools" ) ;
  //
  m_directionFitterNames [ ""     ] = "DirectionFitter" ;
  m_directionFitterNames [ "LoKi" ] = "LoKi::DirectionFitter" ;
  declareProperty  
    ( "DirectionFitters"    , m_directionFitterNames , 
      "The mapping of nick/name/type for IDirectionFit tools"   ) ;
  //
  m_distanceCalculatorNames [ ""     ] = "LoKi::DistanceCalculator" ;
  m_distanceCalculatorNames [ "LoKi" ] = "LoKi::DistanceCalculator" ;
  declareProperty 
    ( "DistanceCalculators" , m_distanceCalculatorNames , 
      "The mapping of nick/name/type for IDistanceCalculator tools"   ) ;
  //
  declareProperty ( "DecayDescriptor"   , m_decayDescriptor   = "not specified" ) ;
  declareProperty ( "AvoidSelResult"    , m_avoidSelResult    = false           ) ;
  declareProperty ( "PrintSelResult"    , m_printSelResult    = false           ) ;
  declareProperty ( "AvoidForcedOutput" , m_avoidEmptyOutput  = false           ) ;
  declareProperty ( "PreloadTools"      , m_preloadTools      = true            ) ;
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
  

  if ( !registerContext() || 0 == contextSvc() ) 
  {
    std::string mgs = "Registration for Algorithm Context Service is disabled." ;
    Warning( mgs +  "Some tools/utilities could have the problems." );
  }
  
  if ( m_avoidSelResult ) { if (msgLevel(MSG::DEBUG)) debug() << "Avoiding SelResult" << endmsg; }
  
  
  // Load tools very
  sc = loadTools() ;
  if ( sc.isFailure() ) { return Error("Unable to load tools", sc ) ; }
  
  if (msgLevel(MSG::DEBUG)){
    if ( m_decayDescriptor == "not specified" )
    { debug() << "Decay Descriptor string not specified"   << endmsg; } 
    else
    { debug() << "Decay Descriptor: " << m_decayDescriptor << endmsg; }
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "End of DVAlgorithm::initialize with " << sc << endmsg;
  
  return sc;
}
// ============================================================================
// Load standard tools
// ============================================================================
StatusCode DVAlgorithm::loadTools() 
{
  
  if ( !m_preloadTools ) 
  { return Warning( "Not preloading tools", StatusCode::SUCCESS ) ; }
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading tools" << endmsg;
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading PhysDesktop" << endmsg;
  desktop();
  
  // vertex fitter
  IOnOffline* onof = NULL;
  if ( m_vertexFitNames.end() == m_vertexFitNames.find("") )
  {
    if ( 0==onof) onof = tool<IOnOffline>("OnOfflineTool",this);
    m_vertexFitNames[""] = onof->vertexFitterType() ;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading "
          << m_vertexFitNames[""] << " as IVertexFit " << endmsg;
  vertexFitter() ;
  
  // geometry THIS IS OBSOLETE
  if ( m_distanceCalculatorNames.end() == m_distanceCalculatorNames.find("") )
  {
    if ( 0==onof ) onof = tool<IOnOffline>("OnOfflineTool",this);
    m_distanceCalculatorNames[""] = onof->distanceCalculatorType() ;
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
  { debug() << ">>> Preloading WriteSelResults Tool" << endmsg; }
  
  writeSelResult();
  
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

  StatusCode sc = desktop()->getEventInput();
  if ( sc.isFailure()) 
  { return Error (  "Not able to fill PhysDesktop" , sc ) ; }
  
  // execute the algorithm 
  sc = this->Algorithm::sysExecute();
  if ( sc.isFailure() ) return sc;
  
  if ( !m_setFilterCalled ) 
  { Warning ( "SetFilterPassed not called for this event!" ) ; }
  
  /// count number of "effective filters"  
  counter("#accept") += filterPassed() ;

  if (!m_avoidSelResult) 
  { sc = fillSelResult () ; }
  else 
  { verbose() << "Avoiding selresult" << endmsg ; }
  
  // Reset for next event
  m_setFilterCalled = false;
  
  // Make sure each DVAlgorithm has written out something
  if ( !m_avoidEmptyOutput ) 
  { sc = desktop()->writeEmptyContainerIfNeeded(); }
  else 
  { verbose() << "Avoiding mandatory output" << endmsg ; }

  sc = desktop()->cleanDesktop();
  if (msgLevel(MSG::VERBOSE) && sc) verbose() << "Happily cleaned Desktop" << endmsg ;
  
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
StatusCode DVAlgorithm::fillSelResult () {

  // Create and fill selection result object
  LHCb::SelResult myResult;
  myResult.setFound(filterPassed());
  myResult.setLocation( ("/Event/Phys/"+name()));
  verbose() << "SelResult location set to " << "/Event/Phys/"+name() 
            << endmsg;
  myResult.setDecay(m_decayDescriptor);

  StatusCode sc = writeSelResult()->write(myResult);
    
  if (filterPassed()) m_countFilterPassed++;
  m_countFilterWrite++;
  verbose() << "wrote " << filterPassed() << " -> " <<
    m_countFilterWrite << " & " << m_countFilterPassed << endmsg ;

  return sc ;

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


  if ((m_printSelResult) && (!m_avoidSelResult)){
    
    if (m_countFilterWrite < m_countFilterPassed ){
      warning() << "Executed " << m_countFilterWrite << 
        " times and flagged as passed " << m_countFilterPassed <<
        " times" << endmsg;      
    } else if (m_countFilterWrite <= 0 ){      
      warning() << "Executed " << m_countFilterWrite << " times" 
                << endmsg;
    } else if (m_countFilterPassed <= 0 ){
      always() << "No events selected in " << 
        m_countFilterWrite << " calls." << endmsg;
    } else if (m_countFilterPassed == m_countFilterWrite ){
      always() << "All events selected in " << 
        m_countFilterWrite << " calls." << endmsg;
    } else {
      double eta = (double)m_countFilterPassed/(double)m_countFilterWrite ;
      double delta = sqrt( eta*((double)1.-eta)/(double)m_countFilterWrite );
      double r = (double)1./eta ;
      double re = r*(delta/eta) ;
      
      always() << "Passed " << m_countFilterPassed << 
        " times in " << m_countFilterWrite << " calls -> (" <<
        100.0*eta << "+/-" << 100.0*delta <<  ")%, rejection= " << 
        r << "+/-" << re << endmsg;
    }
  }
  
  // finalize GaudiTupleAlg base class
  return GaudiTupleAlg::finalize();

} 
// ============================================================================
void DVAlgorithm::imposeOutputLocation(const std::string& outputLocationString)
{
  if ( 0==desktop() ) {
    fatal() << "Desktop has not been created yet" << endmsg;
  }
  desktop()->imposeOutputLocation(outputLocationString);  
  return;  
}
// ============================================================================
// The END 
// ============================================================================
