#ifndef MuonAlgs_MuonCardiacChannelOutput_H
#define MuonAlgs_MuonCardiacChannelOutput_H  1
// Include files
#include <vector>
#include <algorithm>
#include "GaudiKernel/KeyedObject.h"
//#include "MuonPhChID.h"
#include "MuonCardiacChannelOutput.h"
#include "MuonPhysicalChannelOutput.h"
//#include "MuonCardiacChID.h"
#include "MuonHitTraceBack.h"
#include "MuonCardiacTraceBack.h"
#include "Event/MCMuonDigitInfo.h"

class DeMuonDetector;

class MuonCardiacChannelOutput: public KeyedObject<int>{
public:
MuonCardiacChannelOutput(){};
  MuonCardiacChannelOutput(LHCb::MuonTileID 
origID):m_ID(origID),m_FiringTime(0),m_fired(false){};

~MuonCardiacChannelOutput(){};


  void setChID(LHCb::MuonTileID value); 
  LHCb::MuonTileID chID(); 	
  void addPhyChannel(MuonPhysicalChannelOutput* value);
  // std::vector<MuonPhysicalChannelOutput*>& retrievePhyChannel();
  
  void setMCMuonDigitInfo(LHCb::MCMuonDigitInfo value);	
  LHCb::MCMuonDigitInfo& chInfo();
	
	void setFiringTime( double value);
	void setFiringTime();
	double firingTime();
  bool hasFired(){return m_fired;};
  
  
  void processForDeadTime(float value, float timeGate);
  // void fillTimeList();
  
    void setNotFiringDigit();
  std::vector<MuonHitTraceBack*>& hitsTraceBack() {
    return  m_Hits;
  };
  void fillhitsTraceBack();
  
  void setChInfoFiring(MuonCardiacTraceBack pointer);
  //  void  setChInfoNotFiringDueDeadtime(MuonCardiacTraceBack pointer);
  // void  setChInfoNotFiring();
  MuonPhysicalChannelOutput* getPhCh(MuonCardiacTraceBack pointer);
  void UpdateMCInfo();
  
  //  std::vector<MuonHitTraceBack*>& hitsTraceBack() ;
private:
  LHCb::MuonTileID m_ID;
  std::vector<MuonPhysicalChannelOutput*> m_phChannel;
  std::vector<std::pair<float,MuonCardiacTraceBack> > m_timeList;
  LHCb::MCMuonDigitInfo m_phChInfo;
  double m_FiringTime; 
  std::vector<MuonHitTraceBack*>  m_Hits;
  bool m_fired;
  
};	

inline   void MuonCardiacChannelOutput::setChID(LHCb::MuonTileID value){
         m_ID=value; 
}
inline   LHCb::MuonTileID 
         MuonCardiacChannelOutput::chID(){
				 return m_ID;
}

inline   void MuonCardiacChannelOutput::
         setMCMuonDigitInfo(LHCb::MCMuonDigitInfo value){
				 m_phChInfo=value ;
}	 

inline   LHCb::MCMuonDigitInfo& 
         MuonCardiacChannelOutput::chInfo(){
				 return m_phChInfo;
}

inline   void MuonCardiacChannelOutput::setFiringTime(double value){
         m_FiringTime=value;
}

inline  double MuonCardiacChannelOutput::firingTime(){
         return m_FiringTime;
}

typedef KeyedContainer<MuonCardiacChannelOutput, Containers::HashMap>
MuonCardiacChannelOutputs ;

#endif
