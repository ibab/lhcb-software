// $Id: Node.cpp,v 1.4 2007-11-30 13:12:29 wouter Exp $

// local
#include "Event/Node.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Node
//
// 2006-06-10 : M. Needham
//-----------------------------------------------------------------------------

LHCb::Node* LHCb::Node::clone() const 
{

  return new LHCb::Node(*this);
        
}
