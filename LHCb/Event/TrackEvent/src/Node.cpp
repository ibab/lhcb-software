// $Id: Node.cpp,v 1.2 2006-09-21 16:05:09 mneedham Exp $

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

LHCb::Node* LHCb::Node::clone() const {
  return new LHCb::Node(*this);;
}

