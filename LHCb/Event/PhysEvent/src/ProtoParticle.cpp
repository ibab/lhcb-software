// $Id: ProtoParticle.cpp,v 1.1 2002-07-10 15:39:55 gcorti Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticle
//                                 derived from class ContainedObject
//
// 2002-07-08 : Gloria Corti
//-----------------------------------------------------------------------------

//=============================================================================
// Return probability for specified PDG id code
//=============================================================================
double ProtoParticle::probOfPID(int pid) const {

  double prob = 0.0;
  for( PIDInfoVector::const_iterator id=pIDInfo().begin(); 
       pIDInfo().end()!=id; ++id ) {
    if( pid == (*id).first ) {
      prob = (*id).second;
    }
  }
  return prob;
}

//=============================================================================
// PDG code of most likely particleID
//=============================================================================
double ProtoParticle::ProbOfBestPID() const {

  double probmax = 0.0;
  for( PIDInfoVector::const_iterator id=pIDInfo().begin(); 
       pIDInfo().end()!=id; ++id ) {
    if( probmax < (*id).second ) {
      probmax = (*id).second;
    }
  }
  return probmax;
}

//=============================================================================
// PDG code of most likely particleID
//=============================================================================
int ProtoParticle::bestPID() const {

  int pid = 0;
  double probmax = 0.0;
  for( PIDInfoVector::const_iterator id=pIDInfo().begin(); 
       pIDInfo().end()!=id; ++id ) {
    if( probmax < (*id).second ) {
      pid = (*id).first;
    }
  }
  return pid;
}

//=============================================================================
// List of compatible particleIDs
//=============================================================================
std::vector<int> ProtoParticle::pIDList() const {
  
  std::vector<int> pids;
  pids.clear();

  for( PIDInfoVector::const_iterator id=pIDInfo().begin();
       pIDInfo().end()!=id; ++id ) { 
    pids.push_back((*id).first);
  }
  return pids;
} 

//=============================================================================

