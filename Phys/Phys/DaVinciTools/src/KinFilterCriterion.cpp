// $Id: KinFilterCriterion.cpp,v 1.1 2002-03-27 20:35:00 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "KinFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : KinFilterCriterion
//
// 19/03/2002 : Paul Colrain
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<KinFilterCriterion>          s_factory ;
const        IToolFactory& KinFilterCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
KinFilterCriterion::KinFilterCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "MinMomentum", m_minMom = 0. );  
  declareProperty( "MinPt", m_minPt = 0. );  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode KinFilterCriterion::initialize() {

  MsgStream          log( msgSvc(), name() );

  

  log << MSG::DEBUG << ">>>   KinFilterCriterion::initialize() " 
      << endreq;
  log << MSG::DEBUG << ">>>   Cuts are " << endreq;
  log << MSG::DEBUG << ">>>   Min. momentum    " << m_minMom << endreq;    
  log << MSG::DEBUG << ">>>   Min. Pt          " << m_minPt << endreq;    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool KinFilterCriterion::isSatisfied( const Particle* const & part ) {
return ( part->momentum().vect().mag()  > m_minMom &&
         part->momentum().vect().perp() > m_minPt );
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool KinFilterCriterion::operator()( const Particle* const & part ) {
return ( part->momentum().vect().mag()  > m_minMom &&
         part->momentum().vect().perp() > m_minPt );
}

//=============================================================================
