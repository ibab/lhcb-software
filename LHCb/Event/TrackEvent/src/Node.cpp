// $Id: Node.cpp,v 1.3 2007-09-17 09:21:36 jonrob Exp $

// local
#include "Event/Node.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Node
//
// 2006-06-10 : M. Needham
//-----------------------------------------------------------------------------

LHCb::Node::Node( const LHCb::Node& rhs ):
  m_measurement(rhs.m_measurement),
  m_residual(rhs.m_residual),
  m_errResidual(rhs.m_errResidual),
  m_errMeasure(rhs.m_errMeasure),
  m_projectionMatrix(rhs.m_projectionMatrix)
{
  //Node copy constructer
  // Clone the state !
  m_state = rhs.state().clone();
}

LHCb::Node::~Node(){
  // destructer
  if (m_state != 0) delete m_state;
}

void LHCb::Node::reset() 
{

  if (m_state != 0 ) delete m_state;
  m_state = 0;
  m_residual = 0.;
  m_errResidual = 0.;
  m_errMeasure = 0.;
        
}

LHCb::Node* LHCb::Node::clone() const 
{

  return new LHCb::Node(*this);
        
}
