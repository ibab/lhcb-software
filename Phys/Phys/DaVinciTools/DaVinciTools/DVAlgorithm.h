#ifndef DAVINCITOOLS_DVALGORITHM_H
#define DAVINCITOOLS_DVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IHistogram1D;
class IPhysDesktop;
class IMassVertexFitter;
class IVertexFitter;
class IGeomDispCalculator;
class IParticleStuffer;
class IParticleFilter;

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
  DVAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DVAlgorithm( ){ }; ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization (DUMMY)
  virtual StatusCode execute   ();    ///< Algorithm execution (DUMMY)
  virtual StatusCode finalize  ();    ///< Algorithm finalization (DUMMY)

  /// Method to load all tools. 
  /// The base class provides an instance of all type of tools
  virtual StatusCode loadTools();
  
  /// Acessor for PhysDesktop Tool
  IPhysDesktop* desktop() const; 

  /// Acessor for Mass Constrained Vertex Fitter Tool
  IMassVertexFitter* massVertexFitter() const; 

  /// Acessor for Unconstrained Vertex Fitter Tool
  IVertexFitter* vertexFitter() const;

  /// Acessor for Geometrical Displacement Calculation Tool
  IGeomDispCalculator* geomDispCalculator() const;

  /// Acessor for Particle Stuffer Tool
  IParticleStuffer* particleStuffer() const;

  /// Acessor for Particle Filter Tool
  IParticleFilter* particleFilter() const;
  
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

  
};
// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------
// Include files 

// from Gaudi
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"

// from DaVinciTools
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IParticleStuffer.h"
#include "DaVinciTools/IParticleFilter.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DVAlgorithm::DVAlgorithm( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ),
    m_pDesktop(0),
    m_pLagFit(0),
    m_pVertexFit(0),
    m_pGeomDispCalc(0),
    m_pStuffer(0),
    m_pFilter(0) {  

  declareProperty("VertexFitter", m_typeVertexFit="UnconstVertexFitter");
  declareProperty("MassVertexFitter", m_typeLagFit="LagrangeMassVertexFitter");
  
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DVAlgorithm::initialize() {
  return StatusCode::SUCCESS;
}

//=============================================================================
// Load standard tools
//=============================================================================
StatusCode DVAlgorithm::loadTools() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::INFO << ">>> Retriving tools" << endreq;
  
  log << MSG::DEBUG << ">>> Retriving PhysDesktop" << endreq;
  StatusCode sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[PhysDesktop] not found" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << ">>> Retriving " << m_typeLagFit 
      << " as IMassVertexFitter" << endreq;
  sc = toolSvc()->retrieveTool(m_typeLagFit, m_pLagFit, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[" << m_typeLagFit << "] not found"
        << endreq;
    return StatusCode::FAILURE;
  }
     
  log << MSG::DEBUG << ">>> Retriving " << m_typeVertexFit 
      << " as IVertexFitter" << endreq;
  sc = toolSvc()->retrieveTool(m_typeVertexFit, m_pVertexFit, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[" << m_typeVertexFit 
        << "] not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << ">>> Retriving GeomDispCalculator" << endreq;
  sc = toolSvc()->retrieveTool("GeomDispCalculator", m_pGeomDispCalc, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[GeomDispCalculator] not found" 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << ">>> Retriving ParticleStuffer" << endreq;
  sc = toolSvc()->retrieveTool("ParticleStuffer", m_pStuffer);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[ParticleStuffer] not found" 
        << endreq;
    return StatusCode::FAILURE;

  }
   
  log << MSG::DEBUG << ">>> Retriving one ParticleFilter" << endreq;
  sc = toolSvc()->retrieveTool("ParticleFilter", m_pFilter, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << ">>> DVAlgorithm[ParticleFilter] not found" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DVAlgorithm::execute() {

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DVAlgorithm::finalize() {

  return StatusCode::SUCCESS;
}
 

//=============================================================================
IPhysDesktop* DVAlgorithm::desktop() const {
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

#endif // DAVINCITOOLS_DVALGORITHM_H
