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

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"

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
    , m_ppSvc(0) {  

    declareProperty("VertexFitter", m_typeVertexFit="UnconstVertexFitter");
    declareProperty("MassVertexFitter", 
                    m_typeLagFit="LagrangeMassVertexFitter");
  
  };

  virtual ~DVAlgorithm( ){ }; ///< Destructor

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  StatusCode loadTools();
  
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
  
protected:

private:

  /// Concrete type Name of MassConstrained vertex to use (Property)
  std::string m_typeVertexFit;
  /// Concrete type of VertexFitter to use (Property)
  std::string m_typeLagFit;

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
  
};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------

//=============================================================================
// Load standard tools
//=============================================================================
inline StatusCode DVAlgorithm::loadTools() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::INFO << ">>> Retreiving tools" << endreq;
  
  log << MSG::DEBUG << ">>> Retreiving PhysDesktop" << endreq;
  StatusCode sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[PhysDesktop] not found" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << ">>> Retreiving " << m_typeLagFit 
      << " as IMassVertexFitter" << endreq;
  sc = toolSvc()->retrieveTool(m_typeLagFit, m_pLagFit, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[" << m_typeLagFit << "] not found"
        << endreq;
    return StatusCode::FAILURE;
  }
     
  log << MSG::DEBUG << ">>> Retreiving " << m_typeVertexFit 
      << " as IVertexFitter" << endreq;
  sc = toolSvc()->retrieveTool(m_typeVertexFit, m_pVertexFit, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[" << m_typeVertexFit 
        << "] not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << ">>> Retreiving GeomDispCalculator" << endreq;
  sc = toolSvc()->retrieveTool("GeomDispCalculator", m_pGeomDispCalc, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[GeomDispCalculator] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << ">>> Retreiving ParticleStuffer" << endreq;
  sc = toolSvc()->retrieveTool("ParticleStuffer", m_pStuffer);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[ParticleStuffer] not found" 
        << endreq;
    return StatusCode::FAILURE;

  }
   
  log << MSG::DEBUG << ">>> Retreiving one ParticleFilter" << endreq;
  sc = toolSvc()->retrieveTool("ParticleFilter", m_pFilter, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[ParticleFilter] not found" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << ">>> Retrieving ParticlePropertySvc" << endreq;
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return StatusCode::FAILURE;
  }  

  return StatusCode::SUCCESS;
}


//=============================================================================
inline IPhysDesktop* DVAlgorithm::desktop() const {
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

#endif // DAVINCITOOLS_DVALGORITHM_H
