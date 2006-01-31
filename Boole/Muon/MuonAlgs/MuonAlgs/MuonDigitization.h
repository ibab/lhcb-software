#ifndef MUONALGS_MUONDIGITIZATION_H
#define MUONALGS_MUONDIGITIZATION_H 1
#include <cmath>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  
#include <GaudiKernel/ObjectVector.h>  
#include <GaudiKernel/IDataProviderSvc.h>
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiAlg/GaudiAlgorithm.h"
//#include "CLHEP/Units/PhysicalConstants.h"
 
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
//#include "MuonTools/IMuonTileXYZTool.h"
//#include "MuonTools/IMuonGetInfoTool.h"
#include "MuonAlgs/MuonPhyChannelInput.h"
#include "MuonAlgs/MuonPhPreInput.h"
#include "MuonAlgs/MuonDigitizationData.h"
#include "MuonAlgs/MuonPhysicalChannel.h"
#include "MuonKernel/MuonTile.h"
#include "Event/MCHit.h"   
#include "Event/MCMuonDigit.h"   
#include "Event/MuonDigit.h"   
#include "MuonAlgs/MuonPhChID.h"    
#include "MuonAlgs/SortPhChID.h" 
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonReadoutCond.h"  
#include "MuonAlgs/MuonPhysicalChannelOutput.h" 
#include "MuonAlgs/MuonPhysicalChannelResponse.h" 
#include "MuonAlgs/MuonDetectorResponse.h" 
#include "Event/MuonOriginFlag.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonChamberGrid.h"


 
class MuonDigitization : public GaudiAlgorithm {

 
public:
  // Constructor: A constructor of this form must be provided.
	MuonDigitization(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
	StatusCode createInput(MuonDigitizationData<MuonPhyChannelInput> &
                         PhyChaInput);
  StatusCode 
  elaborateMuonPhyChannelInputs(MuonDigitizationData<MuonPhyChannelInput> & 
                                PhyChaInput,
                                MuonDigitizationData<MuonPhysicalChannel>& 
                                PhysicalChannel); 
  StatusCode applyPhysicalEffects(MuonDigitizationData<MuonPhysicalChannel>& 
                                  PhysicalChannel);
  StatusCode  fillPhysicalChannel(MuonDigitizationData<MuonPhysicalChannel>& 
                                  PhysicalChannel,
                                  MuonDigitizationData
                                  <MuonPhysicalChannelOutput>&
                                  PhysicalChannelOutput);
  StatusCode createLogicalChannel(MuonDigitizationData
                                  <MuonPhysicalChannelOutput>& PhyChaOutput,
                                  LHCb::MCMuonDigits & mcDigitContainer);
  StatusCode createRAWFormat(LHCb::MCMuonDigits & mcDigitContainer, 
                                 LHCb::MuonDigits & digitContainer);
  StatusCode addChamberNoise();
  StatusCode addElectronicNoise(MuonDigitizationData<MuonPhysicalChannel>& 
                                PhysicalChannel);
  
private:
  int m_numberOfSpilloverEvents;
  int m_numberOfEvents;
  int m_numberOfEventsNeed;
  double m_BXTime;
  double m_gate;
  bool m_applyTimeJitter;
  bool m_applyChamberNoise;	 
  bool m_applyElectronicNoise;	 
  bool m_applyXTalk;	 
  bool m_applyEfficiency;	 
  bool m_applyDeadtime;	 
  bool m_applyTimeAdjustment;	 
  bool m_registerPhysicalChannelOutput;
;  
  bool m_verboseDebug;
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;
  int m_spill;
  int m_container;
  unsigned int m_TimeBits;
  double m_timeBin;
  
  
  
  
  MuonDetectorResponse detectorResponse;
  DeMuonDetector* m_muonDetector;

  static std::string spill[6];
  static std::string numreg[4];
  static std::string numsta[5];
  static std::string TESPathOfHitsContainer[4];
  static const int OriginOfHitsContainer[5];
  
  Rndm::Numbers m_flatDist;
  };
#endif
