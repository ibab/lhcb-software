// $Id: VtxFilterCriterion.cpp,v 1.1 2003-04-29 18:38:55 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "VtxFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VtxFilterCriterion
//
// 22/06/2002 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<VtxFilterCriterion>          s_factory ;
const        IToolFactory& VtxFilterCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VtxFilterCriterion::VtxFilterCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "MaxChi2", m_maxChi2 = 0. );  
  declareProperty( "MaxZPos", m_maxZPos = 100000. );  
  declareProperty( "MinZPos", m_minZPos = -100000. );  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode VtxFilterCriterion::initialize() {

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>>   VtxFilterCriterion::initialize() " 
      << endreq;
  log << MSG::DEBUG << ">>>   Cuts are " << endreq;
  log << MSG::DEBUG << ">>>   Max. Chi2        " << m_maxChi2 << endreq;    
  log << MSG::DEBUG << ">>>   Max. Z position  " << m_maxZPos << endreq;    
  log << MSG::DEBUG << ">>>   Min. Z position  " << m_minZPos << endreq;    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool VtxFilterCriterion::isSatisfied( const Particle* const & part ) {

  const Vertex *vtx = part->endVertex();
  if ( vtx == 0 ) return false;
  double z = vtx->position().z();
  return ( vtx->chi2() < m_maxChi2 && 
           m_minZPos < z && z < m_maxZPos  );
}

//=============================================================================
