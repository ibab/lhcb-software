#ifndef  MUONALGS_MuonPhPreInput_H
#define  MUONALGS_MuonPhPreInput_H 1

// Include files
#include "MuonPhChID.h"
#include "MuonHitTraceBack.h"


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

