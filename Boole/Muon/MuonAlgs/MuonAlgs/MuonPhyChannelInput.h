#ifndef  MUONALGS_MUONPHYCHANNELINPUT_H
#define  MUONALGS_MUONPHYCHANNELINPUT_H 1

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

class MuonPhyChannelInput: public KeyedObject<int>
{
public:  
  MuonPhyChannelInput(unsigned int phID, MuonHitTraceBack value):m_ID(phID),
	m_hitTraceBack(value){};
	MuonPhyChannelInput() {};
	~MuonPhyChannelInput(){};  
     
	MuonPhChID* phChID(); 	

  void setPhChID(MuonPhChID& value);
	MuonHitTraceBack* getHitTraceBack();
 
 
private:
  MuonPhChID m_ID;
  MuonHitTraceBack m_hitTraceBack ;
   
 }        ;
 
 
inline   MuonPhChID* MuonPhyChannelInput::phChID(){
return &(m_ID);
} 

inline   void  MuonPhyChannelInput::setPhChID(MuonPhChID& value){
m_ID=value;
}

inline  MuonHitTraceBack*  MuonPhyChannelInput::getHitTraceBack(){
  return &(m_hitTraceBack) ;
}

 

typedef KeyedContainer<MuonPhyChannelInput, Containers::HashMap>
MuonPhyChannelInputs ;

#endif      
