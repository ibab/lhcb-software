#ifndef  MUONALGS_MuonPhPreInput_H
#define  MUONALGS_MuonPhPreInput_H 1

// Include files
#include <vector>
#include <algorithm>
#include "Kernel/CLHEPStreams.h"
#include "Event/MuonEventBase.h"
#include "MuonKernel/MuonTileID.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "MuonAlgs/MuonPhChID.h"
#include "Event/MCMuonHit.h"
#include "MuonAlgs/MuonHitTraceBack.h"


class MuonPhPreInput  
{
public:  
  MuonPhPreInput() {};
	~MuonPhPreInput(){};   
 
  MuonHitTraceBack* getHitTraceBack();
	 
  MuonPhChID* phChID(); 	
  const MuonPhChID* phChID() const; 	
  void setPhChID(MuonPhChID& value);
 private: 
  MuonPhChID m_ID; 
  MuonHitTraceBack m_hitTraceBack;
  }        ;
 
  

inline MuonPhChID*  MuonPhPreInput::phChID(){
  return &(m_ID);
} 

inline   const MuonPhChID* MuonPhPreInput::phChID() const{
  return &(m_ID);
} 

inline   void  MuonPhPreInput::setPhChID(MuonPhChID& value){
  m_ID=value;
}
 
inline  MuonHitTraceBack* MuonPhPreInput::getHitTraceBack(){
  return &(m_hitTraceBack) ;
}


 
#endif   

