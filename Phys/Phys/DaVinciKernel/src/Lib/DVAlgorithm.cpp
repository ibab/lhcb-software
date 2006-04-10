#include "Kernel/DVAlgorithm.h"
#include "Kernel/IOnOffline.h"

/// Standard constructor
DVAlgorithm::DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) 
  : 
  GaudiTupleAlg ( name , pSvcLocator ),
  m_desktop(0),
  m_desktopName("PhysDesktop"),
  m_geomTools(),
  m_checkOverlap(0),
  m_filterNames(),
  m_algorithm2IDTool(0),
  m_algorithm2IDToolName("Algorithm2ID"),
  m_particleCombiner(0),
  m_particleCombinerName(""),
  m_particleReFitter(0),
  m_particleReFitterName(""),
  m_taggingTool(0),
  m_taggingToolName("BTaggingTool"),
  m_descendants(0),
  m_descendantsName("ParticleDescendants"),
  m_ppSvc(0),
  m_setFilterCalled(false),
  m_countFilterWrite(0),
  m_countFilterPassed(0),
  m_algorithmID(-1)
{

  declareProperty("VertexFitter", m_vertexFitNames );
  declareProperty("GeomTool", m_geomToolNames);
  declareProperty("CheckOverlapTool",m_checkOverlapName = "CheckOverlap");
  m_filterNames.push_back("ParticleFilter");
  declareProperty("ParticleFilter", m_filterNames );
  declareProperty("ParticleCombiner", m_particleCombinerName);
  declareProperty("ParticleReFitter", m_particleReFitterName);

  declareProperty("DecayDescriptor", m_decayDescriptor = "not specified");
  declareProperty("AvoidSelResult", m_avoidSelResult = false );
  declareProperty("PrintSelResult", m_printSelResult = false );
  declareProperty("PreloadTools", m_preloadTools = true );
  

};
//=============================================================================
// Initialize the thing
//=============================================================================
StatusCode DVAlgorithm::initialize () {
  
  StatusCode sc = GaudiTupleAlg::initialize();  
  if (!sc) return sc;

  if (m_avoidSelResult) warning() << "Avoiding SelResult" << endreq ;

  // Load tools very
  if (!loadTools()){
    err() << "Unable to load tools" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_decayDescriptor == "not specified"){
    warning() << "Decay Descriptor string not specified" << endreq;
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

  if ( !m_preloadTools ) {
    info() << "Not preloading tools" << endmsg;
    return StatusCode::SUCCESS;
  }

  debug() << ">>> Preloading tools" << endmsg;

  debug() << ">>> Preloading PhysDesktop" << endmsg;
  desktop();

  // vertex fitter
  IOnOffline* onof = NULL;
  if ( m_vertexFitNames.empty() ){
    if (0==onof) onof = tool<IOnOffline>("OnOfflineTool",this);
    m_vertexFitNames.push_back(onof->vertexFitter());
  }
  for ( size_t i = 0; i < m_vertexFitNames.size();++i) {
    debug() << ">>> Preloading " << m_vertexFitNames.at(i) << " as IVertexFit "
            << i << endmsg;
    vertexFitter(i) ;
  }
  
  // geometry
  if ( m_geomToolNames.empty() ){
    if (0==onof) onof = tool<IOnOffline>("OnOfflineTool",this);
    m_geomToolNames.push_back( onof->dispCalculator() );
  }
  debug() << ">>> Preloading" << m_geomToolNames[0] 
          << " as IGeomDispCalculator" << endmsg;
  geomDispCalculator();

  debug() << ">>> Preloading CheckOverlap Tool" << endmsg;
  checkOverlap();

  /*  Not preloading non-mandatory tools
  // particle filter
  for ( size_t i = 0; i < m_fileNames.size();++i) {
    debug() << ">>> Preloading ParticleFilter " << m_filterName.at(i) << " as " << i << endmsg;
    particleFilter(i); 
  }
  
  debug() << ">>> Preloading Algorithm2ID Tool" << endmsg;
  algorithm2ID();

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
StatusCode DVAlgorithm::sysExecute () {

  StatusCode scGI = desktop()->getEventInput();
  if (scGI.isFailure()) {
    err() << "Not able to fill PhysDesktop" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc = this->Algorithm::sysExecute();
  if (!sc) return sc;

  if (!m_setFilterCalled) {
    warning() << "SetFilterPassed not called for this event!" << endmsg;
  }

  if (!m_avoidSelResult) sc = fillSelResult () ;
  else verbose() << "Avoiding selresult" << endmsg ;
  
  // Reset for next event
  m_setFilterCalled = false;

  // Make sure each DVAlgorithm has written out something
  return desktop()->writeEmptyContainerIfNeeded();

}
//=============================================================================
StatusCode DVAlgorithm::setFilterPassed  (  bool    state  ) {
  
  StatusCode sc = this->Algorithm::setFilterPassed(state); 
  m_setFilterCalled = true;
  return sc;
}

//=============================================================================
StatusCode DVAlgorithm::fillSelResult () {

  std::string location = LHCb::SelResultLocation::Default;
  verbose() << "SelResult to be saved to " << location << endmsg ;

  LHCb::SelResults* resultsToSave ;
  // Check if SelResult contained has been registered by another algorithm
  if ( exist<LHCb::SelResults>(location) ){
    verbose() << "SelResult exists already " << endmsg ;
    resultsToSave = get<LHCb::SelResults>(location);
  } else {
    verbose() << "Putting new SelResult container " << endmsg ;
    resultsToSave = new LHCb::SelResults();
    put(resultsToSave,location);
  }

  // Create and fill selection result object
  LHCb::SelResult* myResult = new LHCb::SelResult();
  myResult->setFound(filterPassed());
  myResult->setLocation( ("/Event/Phys/"+name()));
  verbose() << "SelResult location set to " << "/Event/Phys/"+name() 
            << endmsg;
  myResult->setDecay(m_decayDescriptor);
    
  if (filterPassed()) m_countFilterPassed++;
  m_countFilterWrite++;
  verbose() << "wrote " << filterPassed() << " -> " <<
    m_countFilterWrite << " & " << m_countFilterPassed << endmsg ;

  resultsToSave->insert(myResult);
  verbose() << "Number of objects in existingSelRes: "
            << resultsToSave->size() << endmsg;
  return StatusCode::SUCCESS ;

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
void DVAlgorithm::imposeOutputLocation(std::string outputLocationString){
  if ( 0==desktop() ) {
    fatal() << "Desktop has not been created yet" << endmsg;
  }
  desktop()->imposeOutputLocation(outputLocationString);  
  return;  
}
// ============================================================================
// Algorithm ID
//=============================================================================
int DVAlgorithm::getAlgorithmID(){
  if ( m_algorithmID < 0 ) {
    m_algorithmID = algorithmID()->idFromName(name());
  }
  return  m_algorithmID ;
}
