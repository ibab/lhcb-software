#ifndef  MUONALGS_MUONHITTRACEBACK_H
#define  MUONALGS_MUONHITTRACEBACK_H 1

// Include files
#include "Event/MCHit.h"
#include "Event/MCMuonHitHistory.h"
#include "GaudiKernel/SmartRef.h"
#include "MuonHitOrigin.h"

class MuonHitTraceBack
{
public:
MuonHitTraceBack(); 
~MuonHitTraceBack(){};

void setMCHit(SmartRef<LHCb::MCHit> value); 
const SmartRef<LHCb::MCHit> getMCHit() const;
SmartRef<LHCb::MCHit> getMCHit();

void setHitArrivalTime(double value);
double hitArrivalTime() const;

void setMCMuonHistory(LHCb::MCMuonHitHistory& value);
LHCb::MCMuonHitHistory& getMCMuonHistory();

MuonHitOrigin& getMCMuonHitOrigin();
void setCordinate(double value[4]);
double* getCordinate();
private:
  SmartRef<LHCb::MCHit> m_MCMuonHit;
	double m_hitArrivalTime ;
	LHCb::MCMuonHitHistory m_history ;
	MuonHitOrigin m_origin ;
  double m_cordinate[4] ;
};

inline  void MuonHitTraceBack::setMCHit(SmartRef<LHCb::MCHit> value){
  m_MCMuonHit=value ;
}

inline  SmartRef<LHCb::MCHit> MuonHitTraceBack::getMCHit(){
  return m_MCMuonHit ;
}

inline  const SmartRef<LHCb::MCHit> MuonHitTraceBack::getMCHit() const {
  return m_MCMuonHit ;
}

inline void MuonHitTraceBack::setHitArrivalTime(double value){
 m_hitArrivalTime = value ;
}

inline double MuonHitTraceBack::hitArrivalTime() const{
 return m_hitArrivalTime ;
}

inline void MuonHitTraceBack::setMCMuonHistory(LHCb::MCMuonHitHistory& 
value){
 m_history=value ;
}

inline LHCb::MCMuonHitHistory& MuonHitTraceBack::getMCMuonHistory(){
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
