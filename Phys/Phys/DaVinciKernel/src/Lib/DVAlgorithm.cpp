#include "Kernel/DVAlgorithm.h"
#include "Kernel/IOnOffline.h"

/// Standard constructor
DVAlgorithm::DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) 
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_pDesktop(0)
  , m_pLagFit(0)
  , m_pVertexFit(0)
  , m_pGeomDispCalc(0)
  , m_pStuffer(0)
  , m_pFilter(0)
  , m_ppSvc(0)
  , m_checkOverlap(0)
  , m_algorithm2IDTool()
  , m_taggingTool()
  , m_setFilterCalled(false)
  , m_countFilterWrite(0)
  , m_countFilterPassed(0)
  , m_loadToolsWarned(false)
  , m_algorithmID(-1)
{  
    
  declareProperty("VertexFitter", m_typeVertexFit = "Default");
  declareProperty("GeomTool", m_typeGeomTool = "Default");
  declareProperty("MassVertexFitter",m_typeLagFit = "LagrangeMassVertexFitter");
  declareProperty("CheckOverlapTool",m_typeCheckOverlap = "CheckOverlap");
  declareProperty("DecayDescriptor", m_decayDescriptor = "not specified");
  declareProperty("AvoidSelResult", m_avoidSelResult = false );
  declareProperty("PrintSelResult", m_printSelResult = false );

};
//=============================================================================
// Initialize the thing
//=============================================================================
StatusCode DVAlgorithm::sysInitialize () {
  
  if( isInitialized()) return StatusCode::SUCCESS;
 
  // copied from Algorithm.cpp
  StatusCode sc = setProperties();
  if( sc.isFailure() ) return StatusCode::FAILURE;
  //  setOutputLevel( m_outputLevel );

  if (m_avoidSelResult) {
    MsgStream msg( msgSvc(), name() ); // warning() not yet initialized
    msg << MSG::WARNING << "Avoiding SelResult" << endreq ;
  }

  // Load tools very first because it needs to be done before initialize()
  sc = loadTools();
  if(sc.isFailure()) {
    MsgStream msg( msgSvc(), name() ); // err() not yet initialized
    msg << MSG::ERROR << "Unable to load tools" << endreq;
    return sc;
  }

  // initialize Algorithm base class first -> calls initialize()
  sc = this->Algorithm::sysInitialize();
  if (!sc ) return sc;

  if (m_decayDescriptor == "not specified"){
    warning() << "Decay Descriptor string not specified" << endreq;
  } else{
    info() << "Decay Descriptor: " << m_decayDescriptor << endreq;
  }

  // initialize GaudiTupleAlg base class then
  sc = this->GaudiTupleAlg::initialize();  
  if (!sc ) return sc;

  debug() << "End of DVAlgorithm::sysInitialize with " << sc << endreq;

  return sc;
}
//=============================================================================
// Load standard tools
//=============================================================================
StatusCode DVAlgorithm::loadTools() {

  MsgStream msg( msgSvc(), name() ); // err() not yet initialized
  msg << MSG::INFO << ">>> Retrieving tools" << endreq;

  msg << MSG::DEBUG << ">>> Retrieving PhysDesktop" << endreq;
  m_pDesktop = tool<IPhysDesktop>("PhysDesktop",this);  

  msg << MSG::DEBUG << ">>> Retrieving " << m_typeLagFit 
      << " as IMassVertexFitter" << endreq;
  m_pLagFit = tool<IMassVertexFitter>(m_typeLagFit, this);

  // vertex fitter
  IOnOffline* onof = NULL;
  std::string tvf = m_typeVertexFit ;
  if ( tvf == "Default" ){
    onof = tool<IOnOffline>("OnOfflineTool",this);
    tvf = onof->vertexFitter();
  }
  msg << MSG::DEBUG << ">>> Retrieving " << tvf
        << " as IVertexFitter" << endreq;
  m_pVertexFit = tool<IVertexFitter>(tvf, this);
  
  // geom
  std::string gdc = m_typeGeomTool ;
  if ( gdc == "Default" ){
    if (onof==0) onof = tool<IOnOffline>("OnOfflineTool",this);
    gdc = onof->dispCalculator();
  }
  msg << MSG::DEBUG << ">>> Retrieving" << gdc << " as IGeomDispCalculator" << endreq;
  m_pGeomDispCalc = tool<IGeomDispCalculator>(gdc, this);
  
  msg << MSG::DEBUG << ">>> Retrieving ParticleStuffer" << endreq;
  m_pStuffer = tool<IParticleStuffer>("ParticleStuffer", this);

  msg << MSG::DEBUG << ">>> Retrieving one ParticleFilter" << endreq;
  m_pFilter = tool<IParticleFilter>("ParticleFilter", this);

  msg << MSG::DEBUG << ">>> Retrieving ParticlePropertySvc" << endreq;
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  
  msg << MSG::DEBUG << ">>> Retrieving CheckOverlap Tool" << endreq;
  m_checkOverlap = tool<ICheckOverlap>(m_typeCheckOverlap);

  msg << MSG::DEBUG << ">>> Retrieving Algorithm2ID Tool" << endreq;
  m_algorithm2IDTool = tool<IAlgorithm2ID>("Algorithm2ID");

  msg << MSG::DEBUG << ">>> Retrieving BTagging Tool" << endreq;
  m_taggingTool = tool<IBTaggingTool>("BTaggingTool");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode DVAlgorithm::sysExecute () {

  StatusCode scGI = desktop()->getEventInput();
  if (scGI.isFailure()) {
    err() << "Not able to fill PhysDesktop" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc = this->Algorithm::sysExecute();
  if (!sc) return sc;

  if (!m_setFilterCalled) {
    warning() << "SetFilterPassed not called for this event!" << endreq;
  }

  if (!m_avoidSelResult) sc = fillSelResult () ;
  else verbose() << "Avoiding selresult" << endreq ;
  
  // Reset for next event
  m_setFilterCalled = false;

  return sc;

}
//=============================================================================
IPhysDesktop* DVAlgorithm::desktop() const {return m_pDesktop;}  

//=============================================================================
IMassVertexFitter* DVAlgorithm::massVertexFitter() const {return m_pLagFit;}  

//=============================================================================
IVertexFitter* DVAlgorithm::vertexFitter() const {return m_pVertexFit;}  

//=============================================================================
IGeomDispCalculator* DVAlgorithm::geomDispCalculator() const {return m_pGeomDispCalc;}  

//=============================================================================
IParticleStuffer* DVAlgorithm::particleStuffer() const {return m_pStuffer;} 
 
//=============================================================================
IParticleFilter* DVAlgorithm::particleFilter() const {return m_pFilter;}

//=============================================================================
IParticlePropertySvc* DVAlgorithm::ppSvc() const {return m_ppSvc;}

//=============================================================================
ICheckOverlap* DVAlgorithm::checkOverlap() const {return m_checkOverlap;}

//=============================================================================
IAlgorithm2ID* DVAlgorithm::algorithmID() const {return m_algorithm2IDTool;}

//=============================================================================
IBTaggingTool* DVAlgorithm::flavourTagging() const {return m_taggingTool;}

//=============================================================================
StatusCode DVAlgorithm::setFilterPassed  (  bool    state  ) {
  
  StatusCode sc = this->Algorithm::setFilterPassed(state);
  
  m_setFilterCalled = true;

  return sc;

}

//=============================================================================
StatusCode DVAlgorithm::fillSelResult () {

  std::string location = SelResultLocation::Default;
  verbose() << "SelResult to be saved to " << location << endreq ;

  SelResults* resultsToSave ;
  // Check if SelResult contained has been registered by another algorithm
  if ( exist<SelResults>(location) ){
    verbose() << "SelResult exists already " << endreq ;
    resultsToSave = get<SelResults>(location);
  } else {
    verbose() << "Putting new SelResult container " << endreq ;
    resultsToSave = new SelResults();
    StatusCode scRO = put(resultsToSave,location);
    if (scRO.isFailure()){
      err() << "Cannot register Selection Result summary at location: " 
            << location << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Create and fill selection result object
  SelResult* myResult = new SelResult();
  myResult->setFound(filterPassed());
  myResult->setLocation( ("/Event/Phys/"+name()));
  verbose() << "SelResult location set to " << "/Event/Phys/"+name() 
            << endreq;
  myResult->setDecay(m_decayDescriptor);
    
  if (filterPassed()) m_countFilterPassed++;
  m_countFilterWrite++;
  verbose() << "wrote " << filterPassed() << " -> " <<
    m_countFilterWrite << " & " << m_countFilterPassed << endreq ;

  resultsToSave->insert(myResult);
  verbose() << "Number of objects in existingSelRes: "
            << resultsToSave->size() << endreq;
  return StatusCode::SUCCESS ;

}

//=============================================================================
StatusCode DVAlgorithm::sysFinalize () {
  
  if ((m_printSelResult) && (!m_avoidSelResult)){
    
    if (m_countFilterWrite < m_countFilterPassed ){
      warning() << "Executed " << m_countFilterWrite << 
        " times and flagged as passed " << m_countFilterPassed <<
        " times" << endreq;      
    } else if (m_countFilterWrite <= 0 ){      
      warning() << "Executed " << m_countFilterWrite << " times" 
                << endreq;
    } else if (m_countFilterPassed <= 0 ){
      always() << "No events selected in " << 
        m_countFilterWrite << " calls." << endreq;
    } else if (m_countFilterPassed == m_countFilterWrite ){
      always() << "All events selected in " << 
        m_countFilterWrite << " calls." << endreq;
    } else {
      double eta = (double)m_countFilterPassed/(double)m_countFilterWrite ;
      double delta = sqrt( eta*((double)1.-eta)/(double)m_countFilterWrite );
      double r = (double)1./eta ;
      double re = r*(delta/eta) ;
      
      always() << "Passed " << m_countFilterPassed << 
        " times in " << m_countFilterWrite << " calls -> (" <<
        100.0*eta << "+/-" << 100.0*delta <<  ")%, rejection= " << 
        r << "+/-" << re << endreq;
    }
  }
  
  // finalize Algorithm base class -> calls finalize() 
  StatusCode sc = Algorithm::sysFinalize();
  if (!sc) return sc;
  
  // finalize GaudiTupleAlg base class
  sc = GaudiTupleAlg::finalize();
  return sc;

}
 
//=============================================================================
void DVAlgorithm::imposeOutputLocation(std::string outputLocationString)
{
  if ( !m_pDesktop ) {
    fatal() << "Desktop has not been created yet" << endreq;
  }
  m_pDesktop->imposeOutputLocation(outputLocationString);  
  return;
  
}
// ============================================================================
// Algorithm ID
//=============================================================================
int DVAlgorithm::getAlgorithmID(){
  if ( m_algorithmID < 0 ) {
    m_algorithmID = m_algorithm2IDTool->idFromName(name());
  }
  return  m_algorithmID ;
}
