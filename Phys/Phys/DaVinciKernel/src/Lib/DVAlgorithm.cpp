#include "Kernel/DVAlgorithm.h"
#include "Kernel/IOnOffline.h"

/// Standard constructor
DVAlgorithm::DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) 
  : 
  GaudiTupleAlg ( name , pSvcLocator )
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
  m_vertexFitNames [ "Kalman"        ] = "BlindVertexFitter"   ;
  m_vertexFitNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_vertexFitNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  declareProperty ( "VertexFitters"     , m_vertexFitNames    ) ;
  //
  m_geomToolNames  [ "Offline" ] = "GeomDispCalculator" ;
  m_geomToolNames  [ "Trigger" ] = "TrgDispCalculator"  ;
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
  m_particleCombinerNames [ "Kalman"        ] = "BlindVertexFitter"   ;
  m_particleCombinerNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_particleCombinerNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  declareProperty ( "ParticleCombiners"  , m_particleCombinerNames ) ;
  //
  m_particleReFitterNames [ ""              ] = "OfflineVertexFitter" ;
  m_particleReFitterNames [ "Offline"       ] = "OfflineVertexFitter" ;
  m_particleReFitterNames [ "Kalman"        ] = "BlindVertexFitter"   ;
  m_particleReFitterNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_particleReFitterNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  declareProperty ( "ParticleReFitters" , m_particleReFitterNames ) ;
  //
  declareProperty ( "DecayDescriptor"   , m_decayDescriptor   = "not specified" ) ;
  declareProperty ( "AvoidSelResult"    , m_avoidSelResult    = false           ) ;
  declareProperty ( "PrintSelResult"    , m_printSelResult    = false           ) ;
  declareProperty ( "AvoidForcedOutput" , m_avoidEmptyOutput  = false           ) ;
  declareProperty ( "PreloadTools"      , m_preloadTools      = true            ) ;
};
//=============================================================================
// Initialize the thing
//=============================================================================
StatusCode DVAlgorithm::initialize () {
  
  StatusCode sc = GaudiTupleAlg::initialize();  
  if ( sc.isFailure() ) return sc;
  
  if ( m_avoidSelResult ) info() << "Avoiding SelResult" << endmsg; 
  
  // Load tools very
  sc = loadTools() ;
  if ( sc.isFailure() ) { return Error("Unable to load tools", sc ) ; }
  
  if (m_decayDescriptor == "not specified"){
    info() << "Decay Descriptor string not specified" << endreq;
  } else{
    info() << "Decay Descriptor: " << m_decayDescriptor << endreq;
  }
  
  debug() << "End of DVAlgorithm::initialize with " << sc << endreq;
  
  return sc;
}
//=============================================================================
// Load standard tools
//=============================================================================
StatusCode DVAlgorithm::loadTools() {
  
  if ( !m_preloadTools ) 
  { return Warning( "Not preloading tools", StatusCode::SUCCESS ) ; }
  
  debug() << ">>> Preloading tools" << endmsg;
  
  debug() << ">>> Preloading PhysDesktop" << endmsg;
  desktop();
  
  // vertex fitter
  IOnOffline* onof = NULL;
  if ( m_vertexFitNames.end() == m_vertexFitNames.find("") )
  {
    if ( 0==onof) onof = tool<IOnOffline>("OnOfflineTool",this);
    m_vertexFitNames[""] = onof->vertexFitter() ;
  }
  
  debug() << ">>> Preloading "
          << m_vertexFitNames[""] << " as IVertexFit " << endmsg;
  vertexFitter () ;
  
  // geometry
  if ( m_geomToolNames.end() == m_geomToolNames.find("") )
  {
    if ( 0==onof ) onof = tool<IOnOffline>("OnOfflineTool",this);
    m_geomToolNames[""] = onof->dispCalculator() ;
  }
  
  debug() << ">>> Preloading " 
          << m_geomToolNames[""] 
          << " as IGeomDispCalculator" << endmsg;
  geomDispCalculator();
  
  debug() << ">>> Preloading CheckOverlap Tool" << endmsg;
  checkOverlap();
  
  debug() << ">>> Preloading WriteSelResults Tool" << endmsg;
  writeSelResult();
  
  /*  Not preloading non-mandatory tools
  // particle filter
  for ( size_t i = 0; i < m_fileNames.size();++i) {
  debug() << ">>> Preloading ParticleFilter " << m_filterName.at(i) << " as " << i << endmsg;
  particleFilter(i); 
  }
  
  debug() << ">>> Preloading BTagging Tool" << endmsg;
  flavourTagging();

  debug() << ">>> Preloading ParticleDescendants Tool" << endmsg;
  descendants();
  */

  debug() << ">>> Preloading ParticlePropertySvc" << endmsg;
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode DVAlgorithm::sysExecute () 
{
  
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
  
  return sc ;
}
//=============================================================================
void DVAlgorithm::setFilterPassed  (  bool    state  ) 
{
  ///
  this->Algorithm::setFilterPassed(state); 
  m_setFilterCalled = true;
  return;
}
//=============================================================================
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

//=============================================================================
StatusCode DVAlgorithm::finalize () {
  
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
 
//=============================================================================
void DVAlgorithm::imposeOutputLocation(const std::string& outputLocationString){
  if ( 0==desktop() ) {
    fatal() << "Desktop has not been created yet" << endmsg;
  }
  desktop()->imposeOutputLocation(outputLocationString);  
  return;  
}
