#include "DaVinciTools/DVAlgorithm.h"

  /// Standard constructor
DVAlgorithm::DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) 
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_typeVertexFit("UnconstVertexFitter")
  , m_typeLagFit("LagrangeMassVertexFitter")
  , m_decayDescriptor("not specified")
  , m_avoidSelResult(false)
  , m_printSelResult(true)
  , m_pDesktop(0)
  , m_pLagFit(0)
  , m_pVertexFit(0)
  , m_pGeomDispCalc(0)
  , m_pStuffer(0)
  , m_pFilter(0)
  , m_ppSvc(0)
  , m_setFilterCalled(false)
  , m_countFilterWrite(0)
  , m_countFilterPassed(0)
{  
    
  declareProperty("VertexFitter", m_typeVertexFit);
  declareProperty("MassVertexFitter",m_typeLagFit);
  declareProperty("DecayDescriptor", m_decayDescriptor);
  declareProperty("AvoidSelResult", m_avoidSelResult);
  declareProperty("PrintSelResult", m_printSelResult);

};
//=============================================================================
// Initialize the thing
//=============================================================================
StatusCode DVAlgorithm::sysInitialize () {
  
  if( isInitialized()) return StatusCode::SUCCESS;
 
  if (m_avoidSelResult) {
    MsgStream msg( msgSvc(), name() ); // warning() not yet initialized
    msg << MSG::WARNING << "Avoiding SelResult" << endreq ;
  }
  
  // Load tools very first
  StatusCode scLT = loadTools();
  if(scLT.isFailure()) {
    MsgStream msg( msgSvc(), name() ); // err() not yet initialized
    msg << MSG::ERROR << "Unable to load tools" << endreq;
    return scLT;
  }

  // initialize Algorithm base class first -> calls initialize()
  StatusCode sc = this->Algorithm::sysInitialize();
  if (!sc ) return sc;

  // initialize GaudiTupleAlg base class then
  sc = this->GaudiTupleAlg::initialize();  
  if (!sc ) return sc;

  if (m_decayDescriptor == "not specified"){
    warning() << "Decay Descriptor string not specified" << endreq;
  }
  else{
    info() << "Decay Descriptor: " << m_decayDescriptor << endreq;
  }
  debug() << "End of DVAlgorithm::sysInitialize with " << sc << endreq;

  return sc;
}
//=============================================================================
// Load standard tools
//=============================================================================
StatusCode DVAlgorithm::loadTools() {

  MsgStream msg( msgSvc(), name() ); // err() not yet initialized
  msg << MSG::INFO << ">>> Retrieving tools" << endreq;

  msg << MSG::DEBUG << ">>> Retreiving PhysDesktop" << endreq;
  m_pDesktop = tool<IPhysDesktop>("PhysDesktop",this);  
  if( !m_pDesktop ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[PhysDesktop] not found" << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << ">>> Retreiving " << m_typeLagFit 
      << " as IMassVertexFitter" << endreq;
  m_pLagFit = tool<IMassVertexFitter>(m_typeLagFit, this);
  if ( !m_pLagFit ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[" << m_typeLagFit 
        << "] not found" << endreq;
    return StatusCode::FAILURE;
  }
     
  msg << MSG::DEBUG << ">>> Retreiving " << m_typeVertexFit 
      << " as IVertexFitter" << endreq;
  m_pVertexFit = tool<IVertexFitter>(m_typeVertexFit, this);
  if ( !m_pVertexFit ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[" << m_typeVertexFit 
        << "] not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  msg << MSG::DEBUG << ">>> Retreiving GeomDispCalculator" << endreq;
  m_pGeomDispCalc = tool<IGeomDispCalculator>("GeomDispCalculator", this);
  if ( !m_pGeomDispCalc ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[GeomDispCalculator] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  msg << MSG::DEBUG << ">>> Retreiving ParticleStuffer" << endreq;
  m_pStuffer = tool<IParticleStuffer>("ParticleStuffer", this);
  if ( !m_pStuffer  ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[ParticleStuffer] not found" 
        << endreq;
    return StatusCode::FAILURE;

  }
  msg << MSG::DEBUG << ">>> Retreiving one ParticleFilter" << endreq;
  m_pFilter = tool<IParticleFilter>("ParticleFilter", this);
  if ( !m_pFilter ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[ParticleFilter] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << ">>> Retrieving ParticlePropertySvc" << endreq;
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  if( !m_ppSvc ) {
    msg << MSG::ERROR << "    Unable to locate Particle Property Service" 
        << endreq;
    return StatusCode::FAILURE;
  }  
  
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

  if (!m_setFilterCalled) {
    warning() << "SetFilterPassed not called for this event!" << endreq;
  }

  if (!m_avoidSelResult) sc = fillSelResult () ;
  else debug() << "Avoiding selresult" << endreq ;
  
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
StatusCode DVAlgorithm::setFilterPassed  (  bool    state  ) {
  
  StatusCode sc = this->Algorithm::setFilterPassed(state);
  
  m_setFilterCalled = true;

  return sc;

}

//=============================================================================
StatusCode DVAlgorithm::fillSelResult () {

  std::string location = SelResultLocation::Default;
  debug() << "SelResult to be saved to " << location << endreq ;

  SelResults* resultsToSave ;
  // Check if SelResult contained has been registered by another algorithm
  if ( exist<SelResults>(location) ){
    debug() << "SelResult exists already " << endreq ;
    resultsToSave = get<SelResults>(location);
  } else {
    debug() << "Putting new SelResult container " << endreq ;
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
  debug() << "wrote " << filterPassed() << " -> " << 
    m_countFilterWrite << " & " << m_countFilterPassed << endreq ;

  resultsToSave->insert(myResult);
  debug() << "Number of objects in existingSelRes: "
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
