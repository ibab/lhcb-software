// $Id: ProtoParticle.cpp,v 1.2 2002-07-18 17:40:40 gcorti Exp $
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
double ProtoParticle::probOfBestPID() const {

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

//============================================================================
// Return if specific detectorID technique is available
//============================================================================
bool ProtoParticle::detPID(const ProtoParticle::detectorPID& det) const {

  bool found = false;
  for( PIDDetVector::const_iterator id = pIDDetectors().begin();
       pIDDetectors().end() != id; ++id ) {
    if( det == (*id).first ) {
      found = true;
      break;
    }
  }
  return found;
}

//=============================================================================
// Value for specific technique if it is available (-1.0 if not)
//=============================================================================
double ProtoParticle::detPIDvalue(const ProtoParticle::detectorPID& det) const{
 
  double value = -1.0;
  for( PIDDetVector::const_iterator id = pIDDetectors().begin();
       pIDDetectors().end() != id; ++id ) {
    if( det == (*id).first ) {
      value = (*id).second;
      break;
    }
  }
  return value;
}

//=============================================================================
// Return TrStateP at measure point closest to IP
//=============================================================================
TrStateP* ProtoParticle::trStateP() {

  TrStateP* trackState = NULL;
  if( !track() ) return trackState;
  if( track()->velo() ) return trackState;
  
  SmartRefVector<TrState>& states = track()->states();
  double firstZ=1000000;
  for ( SmartRefVector<TrState>::iterator it = states.begin() ;
        states.end() != it; it++ ) {
    TrState* temp = *it;
    if ( firstZ > temp->z() )  {
      TrState* temp = *it;
      if ( firstZ > temp->z() )  {
        firstZ = temp->z();
        trackState = dynamic_cast<TrStateP*>(temp);
      }
    }
  }
  return trackState;
}

//=============================================================================
// Return TrStateP at measure point closest to IP
//=============================================================================
const TrStateP* ProtoParticle::trStateP() const {

  const TrStateP* trackState = NULL;
  if( !track() ) return trackState;
  if( track()->velo() ) return trackState;
  
  const SmartRefVector<TrState>& states = track()->states();
  double firstZ=1000000;
  for ( SmartRefVector<TrState>::const_iterator it = states.begin() ;
        states.end() != it; it++ ) {
    const TrState* temp = *it;
    if ( firstZ > temp->z() )  {
      const TrState* temp = *it;
      if ( firstZ > temp->z() )  {
        firstZ = temp->z();
        trackState = dynamic_cast<const TrStateP*>(temp);
      }
    }
  }
  return trackState;


}

//=============================================================================
// Return charge of ProtoParticle
//=============================================================================
double ProtoParticle::charge() const {
 
  double tkcharge = 0.0;
  
  const TrStateP* tkstate = trStateP();
  if( !tkstate ) return tkcharge;
  tkcharge = tkstate->qDivP() > 0.0 ? 1.0 : -1.0;
  return tkcharge;

}
//=============================================================================

