// Include files 

// from STL
#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"
// from Event 
#include "Event/Vertex.h"
#include "Event/Particle.h"

// from DaVinci
// Tools
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IParticleStuffer.h"
#include "DaVinciTools/IParticleFilter.h"


// local
#include "DaVinciTools/DVAlgorithm.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DVAlgorithm
//
// 19/04/2002 : Claudia Pereira Nunes
//-----------------------------------------------------------------------------


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

  
}

//=============================================================================
// Destructor
//=============================================================================
DVAlgorithm::~DVAlgorithm() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DVAlgorithm::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DVAlgorithm::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DVAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;


  return StatusCode::SUCCESS;
}
 

//=============================================================================
IPhysDesktop* DVAlgorithm::desktop() const {
  
  if (0 == m_pDesktop) {
    StatusCode sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
    if( sc.isFailure() ) {
      throw GaudiException ("ToolService[PhysDesktop] not found", name(), sc);
    }
  }
  
  return m_pDesktop;
}  

//=============================================================================
IMassVertexFitter* DVAlgorithm::massVertexFitter() const {
   
 if (0 == m_pLagFit) {
    StatusCode sc = toolSvc()->retrieveTool("LagrangeMassVertexFitter", 
    m_pLagFit, this);
    if( sc.isFailure() ) {
      throw GaudiException 
      ("ToolService[LagrangeMassVertexFitter] not found", name(), sc);
    }
  }
  
  return m_pLagFit;
}  

//=============================================================================
IVertexFitter* DVAlgorithm::uncostVertexFitter() const {
    
if (0 == m_pVertexFit) {
    StatusCode sc = toolSvc()->retrieveTool("UnconstVertexFitter", 
                                              m_pVertexFit,this);
    if( sc.isFailure() ) {
      throw GaudiException 
      ("ToolService[UnconstVertexFitter] not found", name(), sc);
    }
  }
  
  return m_pVertexFit;
}  

//=============================================================================
IGeomDispCalculator* DVAlgorithm::geomDispCalculator() const {
  
  if (0 == m_pGeomDispCalc) {
    StatusCode sc = toolSvc()->retrieveTool("GeomDispCalculator", 
                                              m_pGeomDispCalc, this);
    if( sc.isFailure() ) {
      throw GaudiException 
      ("ToolService[GeomDispCalculator] not found", name(), sc);
    }
  }
  
  return m_pGeomDispCalc;
}  

//=============================================================================
IParticleStuffer* DVAlgorithm::particleStuffer() const {
  
  if (0 == m_pStuffer) {
    StatusCode sc = toolSvc()->retrieveTool("ParticleStuffer", 
                                              m_pStuffer);
    if( sc.isFailure() ) {
      throw GaudiException 
      ("ToolService[ParticleStuffer] not found", name(), sc);
    }
  }
  
  return m_pStuffer;
}  
//=============================================================================
IParticleFilter* DVAlgorithm::particleFilter() const {
  
  if (0 == m_pFilter) {
    StatusCode sc = toolSvc()->retrieveTool("ParticleFilter", 
                                              m_pFilter, this);
    if( sc.isFailure() ) {
      throw GaudiException 
      ("ToolService[ParticleFilter] not found", name(), sc);
    }
  }
  
  return m_pFilter;
}  
