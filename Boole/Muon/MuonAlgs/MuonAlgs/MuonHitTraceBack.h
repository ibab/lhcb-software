#ifndef  MUONALGS_MUONHITTRACEBACK_H
#define  MUONALGS_MUONHITTRACEBACK_H 1

// Include files
#include "Event/MCMuonHit.h"
#include "Event/MCMuonHitHistory.h"
#include "GaudiKernel/SmartRef.h"
#include "MuonAlgs/MuonHitOrigin.h"

class MuonHitTraceBack
{
public:
MuonHitTraceBack(); 
~MuonHitTraceBack(){};

void setMCMuonHit(SmartRef<MCMuonHit> value); 
const SmartRef<MCMuonHit> getMCMuonHit() const;
SmartRef<MCMuonHit> getMCMuonHit();

void setHitArrivalTime(double value);
double hitArrivalTime() const;

void setMCMuonHistory(MCMuonHitHistory& value);
MCMuonHitHistory& getMCMuonHistory();

MuonHitOrigin& getMCMuonHitOrigin();
void setCordinate(double value[4]);
double* getCordinate();
private:
  SmartRef<MCMuonHit> m_MCMuonHit;
	double m_hitArrivalTime ;
	MCMuonHitHistory m_history ;
	MuonHitOrigin m_origin ;
  double m_cordinate[4] ;
};

inline  void MuonHitTraceBack::setMCMuonHit(SmartRef<MCMuonHit> value){
  m_MCMuonHit=value ;
}

inline  SmartRef<MCMuonHit> MuonHitTraceBack::getMCMuonHit(){
  return m_MCMuonHit ;
}

inline  const SmartRef<MCMuonHit> MuonHitTraceBack::getMCMuonHit() const {
  return m_MCMuonHit ;
}

inline void MuonHitTraceBack::setHitArrivalTime(double value){
 m_hitArrivalTime = value ;
}

inline double MuonHitTraceBack::hitArrivalTime() const{
 return m_hitArrivalTime ;
}

inline void MuonHitTraceBack::setMCMuonHistory(MCMuonHitHistory& value){
 m_history=value ;
}

inline MCMuonHitHistory& MuonHitTraceBack::getMCMuonHistory(){
  return m_history;
}

inline MuonHitOrigin & MuonHitTraceBack::getMCMuonHitOrigin(){
  return m_origin;
}
inline void MuonHitTraceBack::setCordinate(double value[4]){
  for (int i=0;i<4;i++){
	  m_cordinate[i]=value[i];
	}
}

inline double* MuonHitTraceBack::getCordinate(){
  return m_cordinate;
}

#endif 
