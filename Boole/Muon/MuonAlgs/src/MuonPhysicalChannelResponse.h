#ifndef MUONALGS_MUONPHYSICALCHANNELRESPONSE_H
#define MUONALGS_MUONPHYSICALCHANNELRESPONSE_H 1

#include "GaudiKernel/RndmGenerators.h"
 
class MuonReadoutCond;

 
class MuonPhysicalChannelResponse  {
public:
//constructor
MuonPhysicalChannelResponse(Rndm::Numbers* flat,Rndm::Numbers* gauss, Rndm::Numbers* genericPDF,Rndm::Numbers* electronicNoise, double min, double max, MuonReadoutCond*
                            detectorCondition, int readoutIndex );
 MuonPhysicalChannelResponse(){};
~MuonPhysicalChannelResponse(){};
void extractDeadtime(double& deadtime);
bool surviveInefficiency();
double extractTimeAdjustmentImprecision();
double extractTimeJitter();
int extractXTalkX(double distanceFromBoundary);
int extractXTalkY(double distanceFromBoundary);
int electronicNoise();

private:
Rndm::Numbers* p_flat;
Rndm::Numbers* p_gauss;
Rndm::Numbers* p_timeJitter;
Rndm::Numbers* p_electronicNoise;
int m_readoutIndex ;
double m_minOfTimeJitter ;
double m_maxOfTimeJitter ;
double m_meadDeadtime ;
double m_rmsDeadtime ;
double m_chamberEfficiency ;
double m_timeAdjustmentImprecision ;
MuonReadoutCond*  p_detectorResponse ;
 };

#endif
