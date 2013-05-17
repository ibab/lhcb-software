#ifndef  MUONALGS_MUONPHYCHANNELINPUT_H
#define  MUONALGS_MUONPHYCHANNELINPUT_H 1

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "MuonPhChID.h"
#include "MuonHitTraceBack.h"

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
