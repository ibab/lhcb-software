#ifndef DAVINCITOOLS_DVALGORITHM_H
#define DAVINCITOOLS_DVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/SelResult.h"

// from DaVinciTools
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IParticleStuffer.h"
#include "DaVinciTools/IParticleFilter.h"



/** @class DVAlgorithm DVAlgorithm.h DaVinciTools/DVAlgorithm.h
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *  
 *  @author Claudia Pereira Nunes
 *  @date   19/04/2002
 *  04/03/2004: Hugo Ruiz: automatically produce SelResult object
 *  use always algorithm name as output location in TES
 */
class DVAlgorithm : public Algorithm {
public:
  /// Standard constructor
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ) 
    : Algorithm ( name , pSvcLocator )
      , m_pDesktop(0)
      , m_pLagFit(0)
      , m_pVertexFit(0)
      , m_pGeomDispCalc(0)
      , m_pStuffer(0)
      , m_pFilter(0)
      , m_ppSvc(0)
      , m_setFilterCalled(false)
      , m_toolsLoaded(false){  
    
    declareProperty("VertexFitter", m_typeVertexFit="UnconstVertexFitter");
    declareProperty("MassVertexFitter", 
                    m_typeLagFit="LagrangeMassVertexFitter");
    declareProperty("DecayDescriptor", m_decayDescriptor="not specified");
    declareProperty("AvoidSelResult", m_avoidSelResult = false);
  };

  virtual ~DVAlgorithm( ){ }; ///< Destructor

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  StatusCode loadTools();

  /// Method to release all tools
  StatusCode releaseTools();
  
  /// Accessor for PhysDesktop Tool
  IPhysDesktop* desktop() const; 

  /// Accessor for Mass Constrained Vertex Fitter Tool
  IMassVertexFitter* massVertexFitter() const; 

  /// Accessor for Unconstrained Vertex Fitter Tool
  IVertexFitter* vertexFitter() const;

  /// Accessor for Geometrical Displacement Calculation Tool
  IGeomDispCalculator* geomDispCalculator() const;

  /// Accessor for Particle Stuffer Tool
  IParticleStuffer* particleStuffer() const;

  /// Accessor for Particle Filter Tool
  IParticleFilter* particleFilter() const;

  /// Accessor for ParticlePropertySvc
  IParticlePropertySvc* ppSvc() const;

  /// Overridden from Gaudi Algo to produce a warning if not called by user
  StatusCode setFilterPassed (bool state);  

  /// Overridden from Gaudi Algorithm
  StatusCode sysExecute ();  

  // Overridden from Gaudi Algorithm
  StatusCode sysInitialize ();  

  /// Imposes an output location for desktop different from algo name
  /// Should be avoided!
  void imposeOutputLocation (std::string outputLocationString);  

  /// Variable needed to ensure that calling getTools by user 
  /// (which is now not needed anymore) will give only one
  /// message warning, not one per event. To be removed
  /// from code when calling by user not tolerated anymore
  bool m_loadToolsWarned;
 
protected:

private:

  
  /// Concrete type Name of MassConstrained vertex to use (Property)
  std::string m_typeVertexFit;
  /// Concrete type of VertexFitter to use (Property)
  std::string m_typeLagFit;
  /// Decay description (Property)
  std::string m_decayDescriptor;
  /// Avoid writing SelResult object in TES (Property)
  bool m_avoidSelResult;
  
  /// Reference to desktop tool
  mutable IPhysDesktop* m_pDesktop;        
  /// Reference to Mass Constrained Vertex Fitter
  mutable IMassVertexFitter* m_pLagFit; 
  /// Reference to unconstrained Vertex Fitter
  mutable IVertexFitter* m_pVertexFit;    
  /// Reference to geometrical displacement Calculation.
  mutable IGeomDispCalculator* m_pGeomDispCalc;  
  /// Reference to ParticleStuffer.
  mutable IParticleStuffer* m_pStuffer;  
  /// Reference to ParticleFilter.
  mutable IParticleFilter* m_pFilter;
  /// Reference to ParticlePropertySvc
  mutable IParticlePropertySvc* m_ppSvc;
  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;
  /// Have Tools been already loaded?
  bool m_toolsLoaded;
  
};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------

//=============================================================================
// Load standard tools
//=============================================================================
inline StatusCode DVAlgorithm::loadTools() {

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
inline StatusCode DVAlgorithm::releaseTools() {

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
inline IPhysDesktop* DVAlgorithm::desktop() const {
  if(!m_toolsLoaded) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::FATAL << "Attempted to use desktop without having loaded tools"
        << endreq;
 }
  return m_pDesktop;
}  

//=============================================================================
inline IMassVertexFitter* DVAlgorithm::massVertexFitter() const {   
  return m_pLagFit;
}  

//=============================================================================
inline IVertexFitter* DVAlgorithm::vertexFitter() const {
  return m_pVertexFit;
}  

//=============================================================================
inline IGeomDispCalculator* DVAlgorithm::geomDispCalculator() const {  
  return m_pGeomDispCalc;
}  

//=============================================================================
inline IParticleStuffer* DVAlgorithm::particleStuffer() const {
  return m_pStuffer;
} 
 
//=============================================================================
inline IParticleFilter* DVAlgorithm::particleFilter() const {
  return m_pFilter;
}

//=============================================================================
inline IParticlePropertySvc* DVAlgorithm::ppSvc() const {
  return m_ppSvc;
}

//=============================================================================
inline StatusCode DVAlgorithm::setFilterPassed  (  bool    state  ) {
  
  StatusCode sc = this->Algorithm::setFilterPassed(state);
  
  m_setFilterCalled = true;

  return sc;

}

//=============================================================================
inline StatusCode DVAlgorithm::sysExecute () {

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
    
    
    existingSelRess->insert(myResult);
    msg << MSG::DEBUG << "Number of objects in existingSelRes: "
        << existingSelRess->size() << endreq;
    
  }
  
  // Reset for next event
  m_setFilterCalled = false;

  return sc;

}

//=============================================================================
inline StatusCode DVAlgorithm::sysInitialize () {
  
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
inline void DVAlgorithm::imposeOutputLocation 
(std::string outputLocationString){
  MsgStream          msg( msgSvc(), name() );
  if( !desktop() ) {
    msg << MSG::FATAL << "Desktop has not been created yet" << endreq;
  }
  desktop()->imposeOutputLocation(outputLocationString);  
  return;
  
}

#endif // DAVINCITOOLS_DVALGORITHM_H
