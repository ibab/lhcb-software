// $Id: ProtoParticle.cpp,v 1.9 2004-03-05 13:08:13 cattanem Exp $
// Include files 

// STD and STL
#include <iostream> 
// from Gaudi
#include "GaudiKernel/Kernel.h"
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
// Copy constructor
//=============================================================================
ProtoParticle::ProtoParticle(const ProtoParticle& proto)
  : KeyedObject<int>()
  , m_pIDDetectors( proto.pIDDetectors() )
  , m_pIDInfo( proto.pIDInfo() )
  , m_pIDhistory( proto.pIDhistory() )
  , m_calo( proto.calo() )
  , m_track( proto.track() )
  , m_richPID( proto.richPID() )
  , m_muonPID( proto.muonPID() )
{

}

//=============================================================================
// Clone 
//=============================================================================
ProtoParticle* ProtoParticle::clone() const
{
  return new ProtoParticle(*this);
}

//=============================================================================
// Assignment operator
//=============================================================================
ProtoParticle& ProtoParticle::operator=(const ProtoParticle& orig) {
  
  // protect against self assignement
  if( this != &orig ) {
    m_pIDDetectors = orig.pIDDetectors();
    m_pIDInfo = orig.pIDInfo();
    m_pIDhistory = orig.pIDhistory();
    m_calo = orig.calo(); 
    m_track = orig.track();
    m_richPID = orig.richPID();
    m_muonPID = orig.muonPID();
  }
  return *this;
}    
    
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
      probmax = (*id).second;
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
 
  double value = -999.0;
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
      firstZ = temp->z();
      trackState = dynamic_cast<TrStateP*>(temp);
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
      firstZ = temp->z();
      trackState = dynamic_cast<const TrStateP*>(temp);
    }
  }
  return trackState;
}
//=============================================================================
// First Measured Point
//=============================================================================
const int ProtoParticle::minimalStates() const {
  // State 0 is the extrapolation to beam axis if available, else it is
  // the first measured point. This function gives the minimal number of
  // states per track.

  // forward and match have 5 (1%) or 6 (99%).
  if ( track()->isLong() || 
  // downstream have always 5 states (no extrapolation)
       track()->isDownstream() ){
    return 5 ;
  // veloTT have 3 (5%) or 4 (95%).
  } else if ( track()->veloTT() ){ 
    return 3 ;
  }
  return -1;

}
//=============================================================================
// Return TrStateP at first measured point
//=============================================================================
TrStateP* ProtoParticle::firstMeasured() {

  TrStateP* trackState = NULL;
  if( !track() ) return trackState;
  if( track()->velo() ) return trackState;
  
  SmartRefVector<TrState>& states = track()->states();
  const int MS =  minimalStates();
  // if the number of states is minimal, the first is also the first measured.
  // if there is one more, the first state is the extrapolation to the
  // beam axis -> take the next one.
  const int FirstState = states.size() - MS;
  
  TrState* temp = *(states.begin()+FirstState);
  trackState = dynamic_cast<TrStateP*>(temp);
  
  return trackState;
}
//=============================================================================
// Return TrStateP at first measured point
//=============================================================================
const TrStateP* ProtoParticle::firstMeasured() const {

  const TrStateP* trackState = NULL;
  if( !track() ) return trackState;
  if( track()->velo() ) return trackState;
  
  const SmartRefVector<TrState>& states = track()->states();
  const int MS =  minimalStates();
  const int FirstState = states.size() - MS;
  
  const TrState* temp = *(states.begin()+FirstState);
  trackState = dynamic_cast<const TrStateP*>(temp);
  
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

