#ifndef MuonAlgs_MuonPhysicalChannelOutput_H
#define MuonAlgs_MuonPhysicalChannelOutput_H  1
// Include files
#include <vector>
#include <algorithm>
#include "MuonKernel/MuonTileID.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "MuonAlgs/MuonPhChID.h"
#include "MuonAlgs/MuonPhysicalChannel.h"
#include "Event/MCMuonHit.h"
#include "Event/Kernel.h"  
#include "MuonAlgs/MuonHitTraceBack.h"
#include "Event/MCMuonDigitInfo.h"
#include "MuonTools/IMuonGetInfoTool.h"



class MuonPhysicalChannelOutput: public KeyedObject<int>{
public:
MuonPhysicalChannelOutput(){};
MuonPhysicalChannelOutput(MuonPhysicalChannel& orig);
//MuonPhysicalChannelOutput(const MuonPhysicalChannel orig){};
MuonPhysicalChannelOutput(MuonPhChID origID, std::vector<MuonHitTraceBack>
origHits):m_ID(origID),m_Hits(origHits){};

~MuonPhysicalChannelOutput(){};


  /// Retrieve Stored GEANT hits connected to the ph. (const)
  const std::vector<MuonHitTraceBack>& hitsTraceBack() const ;

  /// Retrieve Stored GEANT hits connected to the ph (non-const)
  std::vector<MuonHitTraceBack>& hitsTraceBack() ;

  void setPhChID(MuonPhChID value); 
	MuonPhChID* phChID(); 	

	
  void setMCMuonDigitInfo(MCMuonDigitInfo value);	
  MCMuonDigitInfo& phChInfo();
	
	void setFiringTime( double value);
	double firingTime();
  void calculateTileID(int & numberOfTileInOutput,MuonTileID phChTileID[2],
                       IMuonGetInfoTool* usefullPointer);
private:
 MuonPhChID m_ID;
 std::vector<MuonHitTraceBack> m_Hits;
 MCMuonDigitInfo m_phChInfo;
 double m_FiringTime;
};	

inline   const std::vector<MuonHitTraceBack>& 
         MuonPhysicalChannelOutput::hitsTraceBack() const {
				 return m_Hits;
}
inline   std::vector<MuonHitTraceBack>& 
         MuonPhysicalChannelOutput::hitsTraceBack(){
				 return m_Hits;
}

inline   void MuonPhysicalChannelOutput::setPhChID(MuonPhChID value){
         m_ID=value; 
}
inline   MuonPhChID* 
         MuonPhysicalChannelOutput::phChID(){
				 return &m_ID;
}

inline   void MuonPhysicalChannelOutput::
         setMCMuonDigitInfo(MCMuonDigitInfo value){
				 m_phChInfo=value ;
}	 

inline   MCMuonDigitInfo& 
         MuonPhysicalChannelOutput::phChInfo(){
				 return m_phChInfo;
}

inline   void MuonPhysicalChannelOutput::setFiringTime(double value){
         m_FiringTime=value;
}

inline  double MuonPhysicalChannelOutput::firingTime(){
         return m_FiringTime;
}

typedef KeyedContainer<MuonPhysicalChannelOutput, Containers::HashMap>
MuonPhysicalChannelOutputs ;

#endif
