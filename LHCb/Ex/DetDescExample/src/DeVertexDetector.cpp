// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/DeVertexDetector.cpp,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#define DDEXAMPLE_DEVERTEXDETECTOR_CPP

// Include files
#include "DeVertexDetector.h"



//------------------------------------------------------------------------------
//
// Implementation of class :  DeVertexDetector
//
//------------------------------------------------------------------------------


const CLID& CLID_DEVertex = 9999;  // User defined

// Standard Constructors
DeVertexDetector::DeVertexDetector()
: m_stations( 0 )
{
}
  
DeVertexDetector::DeVertexDetector( unsigned int stations )
: m_stations( stations )
{
}

// Standard Destructor
DeVertexDetector::~DeVertexDetector()
{
}
  
// Retrieve Pointer to class defininition structure
const CLID& DeVertexDetector::classID()
{
  return CLID_DEVertex;
}

// Get/Set thickness
unsigned int DeVertexDetector::stations()
{
  return m_stations;
}

void   DeVertexDetector::setStations( unsigned int stations )
{
  m_stations = stations;
}

