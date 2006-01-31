#ifndef MUONALGS_MUONPHYSICALCHANNELRESPONSE_CPP 
#define MUONALGS_MUONPHYSICALCHANNELRESPONSE_CPP 1

#include "MuonDet/MuonReadoutCond.h"

#include "MuonPhysicalChannelResponse.h"

MuonPhysicalChannelResponse::MuonPhysicalChannelResponse(Rndm::Numbers* flat,
                                                         Rndm::Numbers* gauss,
                                                         Rndm::Numbers* 
                                                         genericPDF, 
                                                         Rndm::Numbers* 
                                                         electronicNoise,
                                                         double min, 
                                                         double max,
                                                         MuonReadoutCond*  
                                                         detectorCondition, 
                                                         int readoutType ){
  p_flat=flat;
  p_gauss=gauss;
  p_timeJitter=genericPDF;
  p_electronicNoise=electronicNoise;
  m_readoutIndex=readoutType;
  m_minOfTimeJitter=min;
  m_maxOfTimeJitter=max;
  m_meadDeadtime=detectorCondition->meanDeadTime(readoutType);
  m_rmsDeadtime=detectorCondition->rmsDeadTime(readoutType);
  m_chamberEfficiency=detectorCondition->efficiency(readoutType);
  m_timeAdjustmentImprecision=detectorCondition->syncDrift(readoutType);
  p_detectorResponse = detectorCondition ;  
}
void MuonPhysicalChannelResponse::extractDeadtime(double& deadtime){  
  deadtime=m_meadDeadtime+m_rmsDeadtime*(*p_gauss)();
}				

bool MuonPhysicalChannelResponse::surviveInefficiency(){
  bool survive=true;
  if(m_chamberEfficiency<(*p_flat)())survive=false;
  return survive;
}				

double MuonPhysicalChannelResponse::extractTimeAdjustmentImprecision(){
  return m_timeAdjustmentImprecision*((*p_flat)());
}

double MuonPhysicalChannelResponse::extractTimeJitter(){
  double offset=m_minOfTimeJitter;
  double expansion=m_maxOfTimeJitter-m_minOfTimeJitter;
  double time=(*p_timeJitter)();
  return time*expansion+offset;
}
int MuonPhysicalChannelResponse::extractXTalkX(double distanceFromBoundary){
  double randomNumber=(*p_flat)();
  return p_detectorResponse-> singleGapClusterX(randomNumber,
                                                distanceFromBoundary,
                                                m_readoutIndex);
}
int MuonPhysicalChannelResponse::extractXTalkY(double distanceFromBoundary){
  double randomNumber=(*p_flat)();
  return p_detectorResponse-> singleGapClusterY(randomNumber,
                                                distanceFromBoundary,
                                                m_readoutIndex);
}
int MuonPhysicalChannelResponse::electronicNoise(){
  return (int) (*p_electronicNoise)() ;
}
#endif
