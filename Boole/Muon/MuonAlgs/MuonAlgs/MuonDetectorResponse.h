#ifndef MUONALGS_MUONDETECTORRESPONSE_H
#define MUONALGS_MUONDETECTORRESPONSE_H 1
#include <cmath>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  
#include <GaudiKernel/ObjectVector.h>  
#include <GaudiKernel/IDataProviderSvc.h>
#include "GaudiKernel/IHistogramSvc.h"
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
#include "DetDesc/IReadOut.h"
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonAlgs/MuonPhyChannelInput.h"
#include "MuonAlgs/MuonPhPreInput.h"
#include "MuonAlgs/MuonDigitizationParameters.h"
#include "MuonAlgs/MuonDigitizationData.h"
 
#include "MuonKernel/MuonTile.h"
#include "Event/MCMuonHit.h"   
#include "Event/MCMuonDigit.h"   
#include "Event/MuonDigit.h"   
#include "MuonAlgs/MuonPhChID.h"    
#include "MuonAlgs/SortPhChID.h" 
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonReadoutCond.h"  


#include "Event/MuonOriginFlag.h"
#include "MuonAlgs/MuonPhysicalChannelResponse.h" 
#include "MuonAlgs/MuonChamberResponse.h" 
#include "GaudiKernel/RndmGenerators.h"
#include "MuonAlgs/MuonPhChID.h"
 
class MuonDetectorResponse  {
public:
//constructor
MuonDetectorResponse(){};
 void initialize(IRndmGenSvc * randSvc,
                      IDataProviderSvc* detSvc, IMessageSvc * msgSvc);
~MuonDetectorResponse();
MuonPhysicalChannelResponse* getResponse(MuonPhChID& phChID);
MuonPhysicalChannelResponse* getResponse(int partition,int readout);
MuonChamberResponse* getChamberResponse(int partition);
private:	
 Rndm::Numbers m_gaussDist;
 Rndm::Numbers m_flatDist;
 std::vector<Rndm::Numbers*> m_poissonDist;
 std::vector<Rndm::Numbers*> m_electronicNoise;
 std::vector<Rndm::Numbers*> m_timeJitter;
MuonDigitizationParameters::Parameters usefull;
MuonPhysicalChannelResponse* responseVector[2][20];
MuonChamberResponse* responseChamber[20];
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;  
 };


inline MuonChamberResponse* MuonDetectorResponse::getChamberResponse(int
                            partition){
       return    responseChamber[partition];					
}
#endif
