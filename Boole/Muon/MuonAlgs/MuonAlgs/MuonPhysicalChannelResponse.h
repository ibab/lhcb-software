#ifndef MUONALGS_MUONPHYSICALCHANNELRESPONSE_H
#define MUONALGS_MUONPHYSICALCHANNELRESPONSE_H 1
#include <cmath>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  
#include <GaudiKernel/ObjectVector.h>  
#include <GaudiKernel/IDataProviderSvc.h>
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiKernel/RndmGenerators.h"
#include "AIDA/IHistogram1D.h"
#include "CLHEP/Units/PhysicalConstants.h"
 
/// Detector description classes
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElementException.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBox.h"  

#include "MuonTools/IMuonTileXYZTool.h"
 #include "MuonAlgs/MuonDigitizationParameters.h"
 
#include "MuonKernel/MuonTile.h"
 #include "MuonAlgs/MuonPhChID.h"    
 #include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/DeMuonGasGap.h"
 
#include "Event/MuonOriginFlag.h"
 

 
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
float m_meadDeadtime ;
float m_rmsDeadtime ;
float m_chamberEfficiency ;
float m_timeAdjustmentImprecision ;
MuonReadoutCond*  p_detectorResponse ;
 };

#endif
