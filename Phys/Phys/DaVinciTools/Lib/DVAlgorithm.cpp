#include "DaVinciTools/DVAlgorithm.h"

//=============================================================================
// Load standard tools
//=============================================================================
StatusCode DVAlgorithm::loadTools() {

  MsgStream  msg( msgSvc(), name() );

  if (m_toolsLoaded) {
    if (!m_loadToolsWarned){
      msg << MSG::WARNING << "Attempted to load tools twice -  " <<
        "loadTools() should be removed from execute()." 
          << endreq;
      m_loadToolsWarned = true;
    }
    return StatusCode::SUCCESS;
  }

  msg << MSG::INFO << ">>> Retrieving tools" << endreq;
  
  msg << MSG::DEBUG << ">>> Retreiving PhysDesktop" << endreq;
  StatusCode sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
  if( sc.isFailure() ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[PhysDesktop] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << ">>> Retreiving " << m_typeLagFit 
      << " as IMassVertexFitter" << endreq;
  sc = toolSvc()->retrieveTool(m_typeLagFit, m_pLagFit, this);
  if( sc.isFailure() ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[" << m_typeLagFit 
        << "] not found" << endreq;
    return StatusCode::FAILURE;
  }
     
  msg << MSG::DEBUG << ">>> Retreiving " << m_typeVertexFit 
      << " as IVertexFitter" << endreq;
  sc = toolSvc()->retrieveTool(m_typeVertexFit, m_pVertexFit, this);
  if( sc.isFailure() ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[" << m_typeVertexFit 
        << "] not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  msg << MSG::DEBUG << ">>> Retreiving GeomDispCalculator" << endreq;
  sc = toolSvc()->retrieveTool("GeomDispCalculator", m_pGeomDispCalc, this);
  if( sc.isFailure() ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[GeomDispCalculator] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  msg << MSG::DEBUG << ">>> Retreiving ParticleStuffer" << endreq;
  sc = toolSvc()->retrieveTool("ParticleStuffer", m_pStuffer, this);
  if( sc.isFailure() ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[ParticleStuffer] not found" 
        << endreq;
    return StatusCode::FAILURE;

  }
   
  msg << MSG::DEBUG << ">>> Retreiving one ParticleFilter" << endreq;
  sc = toolSvc()->retrieveTool("ParticleFilter", m_pFilter, this);
  if( sc.isFailure() ) {
    msg << MSG::ERROR << ">>> DVAlgorithm[ParticleFilter] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << ">>> Retrieving ParticlePropertySvc" << endreq;
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return StatusCode::FAILURE;
  }  
  
  m_toolsLoaded = true;
  

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode DVAlgorithm::releaseTools() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::INFO << ">>> Releasing tools" << endreq;
  if( m_pDesktop      ) toolSvc()->releaseTool( m_pDesktop );
  if( m_pLagFit       ) toolSvc()->releaseTool( m_pLagFit );
  if( m_pVertexFit    ) toolSvc()->releaseTool( m_pVertexFit );
  if( m_pGeomDispCalc ) toolSvc()->releaseTool( m_pGeomDispCalc );
  if( m_pStuffer      ) toolSvc()->releaseTool( m_pStuffer );
  if( m_pFilter       ) toolSvc()->releaseTool( m_pFilter );
  return StatusCode::SUCCESS;
}

//=============================================================================
IPhysDesktop* DVAlgorithm::desktop() const {
  if(!m_toolsLoaded) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::FATAL << 
      "Attempted to use desktop without having loaded tools" << endreq;
 }
  return m_pDesktop;
}  

//=============================================================================
IMassVertexFitter* DVAlgorithm::massVertexFitter() const {   
  return m_pLagFit;
}  

//=============================================================================
IVertexFitter* DVAlgorithm::vertexFitter() const {
  return m_pVertexFit;
}  

//=============================================================================
IGeomDispCalculator* DVAlgorithm::geomDispCalculator() const {  
  return m_pGeomDispCalc;
}  

//=============================================================================
IParticleStuffer* DVAlgorithm::particleStuffer() const {
  return m_pStuffer;
} 
 
//=============================================================================
IParticleFilter* DVAlgorithm::particleFilter() const {
  return m_pFilter;
}

//=============================================================================
IParticlePropertySvc* DVAlgorithm::ppSvc() const {
  return m_ppSvc;
}

//=============================================================================
StatusCode DVAlgorithm::setFilterPassed  (  bool    state  ) {
  
  StatusCode sc = this->Algorithm::setFilterPassed(state);
  
  m_setFilterCalled = true;

  return sc;

}

//=============================================================================
StatusCode DVAlgorithm::sysExecute () {

  MsgStream          msg( msgSvc(), name() );

  StatusCode scGI = desktop()->getEventInput();
  if (scGI.isFailure()) {
    msg << MSG::ERROR << "Not able to fill PhysDesktop" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc = this->Algorithm::sysExecute();


  if (!m_setFilterCalled) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "SetFilterPassed not called for this event!"
        << endreq;
  }
  

  if (!m_avoidSelResult){
    
    std::string location = SelResultLocation::Default;
    
    // Check if SelResult contained has been registered by PreDV
    SmartDataPtr<SelResults> existingSelRess ( eventSvc(), location);
    if(!existingSelRess ) {
      msg << MSG::FATAL<< "SelResult container does not exist. "  <<
        "PreDV algorithm should be run before any DVAlgorithm!"  
          << endreq;
      return StatusCode::FAILURE;
    }
    
    // Create and fill selection result object
    SelResult* myResult = new SelResult();
    myResult->setFound(filterPassed());
    myResult->setLocation(name());
    myResult->setDecay(m_decayDescriptor);
    
    if (filterPassed()) m_countFilterPassed++;
    m_countFilterWrite++;

    existingSelRess->insert(myResult);
    msg << MSG::DEBUG << "Number of objects in existingSelRes: "
        << existingSelRess->size() << endreq;
    
  }
  
  // Reset for next event
  m_setFilterCalled = false;

  return sc;

}

//=============================================================================
StatusCode DVAlgorithm::sysInitialize () {
  
  MsgStream          msg( msgSvc(), name() );
  
  if( isInitialized()) return StatusCode::SUCCESS;
 
  // Load tools
  StatusCode scLT = loadTools();
  if(scLT.isFailure()) {
    msg << MSG::ERROR << "Unable to load tools" << endreq;
    return scLT;
  }
  
  StatusCode sc = this->Algorithm::sysInitialize();  

  if (m_decayDescriptor == "not specified"){
    msg << MSG::WARNING << "Decay Descriptor string not specified" << endreq;
  }
  else{
    msg << MSG::INFO << "Decay Descriptor: " << m_decayDescriptor << endreq;
  }

  

  return sc;
}
//=============================================================================
StatusCode DVAlgorithm::sysFinalize () {
  
  if (m_printSelResult){
    
    MsgStream msg( msgSvc(), name() );
    if (m_countFilterWrite < m_countFilterPassed ){
      msg << MSG::WARNING << "Executed " << m_countFilterWrite << 
        " times and flagged as passed " << m_countFilterPassed <<
        " times" << endreq;      
    } else if (m_countFilterWrite <= 0 ){      
      msg << MSG::WARNING << "Executed " << m_countFilterWrite << " times" 
          << endreq;
    } else if (m_countFilterPassed <= 0 ){
      msg << MSG::ALWAYS << "No events selected in " << 
        m_countFilterWrite << " calls." << endreq;
    } else if (m_countFilterPassed == m_countFilterWrite ){
      msg << MSG::ALWAYS << "All events selected in " << 
        m_countFilterWrite << " calls." << endreq;
    } else {
      double eta = (double)m_countFilterPassed/(double)m_countFilterWrite ;
      double delta = sqrt( eta*((double)1.-eta)/(double)m_countFilterWrite );
      double r = (double)1./eta ;
      double re = r*(delta/eta) ;
      
      msg << MSG::ALWAYS << "Passed " << m_countFilterPassed << 
        " times in " << m_countFilterWrite << " calls -> (" <<
        100.0*eta << "+/-" << 100.0*delta <<  ")%, rejection= " << 
        r << "+/-" << re << endreq;
    }
  }
  
  return this->Algorithm::sysFinalize();
}
 
//=============================================================================
void DVAlgorithm::imposeOutputLocation 
(std::string outputLocationString){
  MsgStream          msg( msgSvc(), name() );
  if( !desktop() ) {
    msg << MSG::FATAL << "Desktop has not been created yet" << endreq;
  }
  desktop()->imposeOutputLocation(outputLocationString);  
  return;
  
}

