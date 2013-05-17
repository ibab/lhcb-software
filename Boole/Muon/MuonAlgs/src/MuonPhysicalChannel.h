#ifndef MuonAlgs_MuonPhysicalChannel_H
#define MuonAlgs_MuonPhysicalChannel_H  1

// Include files
#include <vector>
#include <algorithm>
#include "GaudiKernel/KeyedObject.h"
#include "MuonPhChID.h"
#include "MuonHitTraceBack.h"
#include "MuonPhysicalChannelResponse.h"

class MuonPhysicalChannel : public KeyedObject<int> {

public:
  MuonPhysicalChannel(double time, double bx)
    :response(0)
    {
      m_timeGate=time;
      m_timeBX=bx;
    }
    
  MuonPhysicalChannel(unsigned int FETile, double time, double bx)
    :m_ID(FETile),response(0)
    {
      m_timeGate=time;
      m_timeBX=bx;
    }
  
  ~MuonPhysicalChannel(){};
    
      
  /// Retrieve Stored GEANT hits connected to the ph. (const)
  const std::vector<MuonHitTraceBack>& hitsTraceBack() const ;
  
  /// Retrieve Stored GEANT hits connected to the ph (non-const)
  std::vector<MuonHitTraceBack>& hitsTraceBack() ;  
  MuonPhChID* phChID(); 	
  void setPhChID(MuonPhChID& value);
  void applyTimeJitter();
  void applyGeoInefficiency();
  void applyChamberInefficiency();
  void applyXTalk(int phChInX[2], int phChInY[2],std::vector<MuonPhysicalChannel*>& hitDueToXTalk);
  void addToPC(MuonPhysicalChannel* pointerOfPCToAdd);
  void sortInTimeHits();
  void applyTimeAdjustment();
  void applyDeadtime(int BX);
  void setResponse(MuonPhysicalChannelResponse* value);
  
  void createXTalkChannel(MuonHitTraceBack* iter,unsigned int newChannel,
			  int axis, MuonPhysicalChannel*	newPhCh);
  void fireXTalkChannels(int axis, int numberOfXTalkHits,MuonHitTraceBack* iter,unsigned int
			 indexOfChannel, int direction, int maxChannel, 
			 std::vector<MuonPhysicalChannel*>& hitDueToXTalk);
  MuonPhysicalChannelResponse* getResponse();
 private:
  MuonPhChID m_ID;
  std::vector<MuonHitTraceBack> m_Hits;
  MuonPhysicalChannelResponse* response; 
  static double m_timeGate;
  static double m_timeBX;
};	


inline   MuonPhChID* MuonPhysicalChannel::phChID(){
  return &(m_ID);
}

inline  void MuonPhysicalChannel::setPhChID(MuonPhChID& value)
{
  m_ID=value ;
}
inline  const std::vector<MuonHitTraceBack>&
MuonPhysicalChannel::hitsTraceBack() const 
{
  return m_Hits  ;
}

/// Retrieve Stored GEANT hits connected to the ph (non-const)
inline  std::vector<MuonHitTraceBack>& MuonPhysicalChannel::hitsTraceBack()
{
  return  m_Hits;
}

inline 	void MuonPhysicalChannel::setResponse(MuonPhysicalChannelResponse*
					      value)
{
  response=value;
}
inline  MuonPhysicalChannelResponse* MuonPhysicalChannel::getResponse()
{
  return response;
}


typedef KeyedContainer<MuonPhysicalChannel, Containers::HashMap>
  MuonPhysicalChannels;

#endif
