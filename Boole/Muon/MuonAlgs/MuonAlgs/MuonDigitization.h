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
#include "MuonAlgs/MuonPhysicalChannel.h"
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
#include "MuonAlgs/MuonPhysicalChannelOutput.h" 
#include "MuonAlgs/MuonPhysicalChannelResponse.h" 
#include "MuonAlgs/MuonDetectorResponse.h" 
#include "Event/MuonOriginFlag.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h"

 
class MuonDigitization : public Algorithm {

 
public:
  // Constructor: A constructor of this form must be provided.
	MuonDigitization(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
	StatusCode createInput(MuonDigitizationData<MuonPhyChannelInput> & PhyChaInput);
  StatusCode elaborateMuonPhyChannelInputs(MuonDigitizationData<MuonPhyChannelInput> & PhyChaInput,
	           MuonDigitizationData<MuonPhysicalChannel>& PhysicalChannel); 
  StatusCode applyPhysicalEffects(MuonDigitizationData<MuonPhysicalChannel>& PhysicalChannel);
  StatusCode  fillPhysicalChannel(MuonDigitizationData<MuonPhysicalChannel>& PhysicalChannel,MuonDigitizationData<MuonPhysicalChannelOutput>&
	PhysicalChannelOutput);
 StatusCode createLogicalChannel(MuonDigitizationData<MuonPhysicalChannelOutput>& PhyChaOutput,MCMuonDigits & mcDigitContainer);
 StatusCode createRAWFormat(MCMuonDigits & mcDigitContainer, MuonDigits & digitContainer);
 StatusCode addChamberNoise();
 StatusCode addElectronicNoise(MuonDigitizationData<MuonPhysicalChannel>& PhysicalChannel);
 
private:
   int m_numberOfSpilloverEvents;
   int m_numberOfEvents;
	 double m_BXTime;
	 bool m_verboseDebug;
	 MuonDetectorResponse detectorResponse;
	 
	 static const std::string spill[5];
	 static const std::string numreg[4];
	 static const std::string numsta[5];
	 static const std::string TESPathOfHitsContainer[4];
   static const int OriginOfHitsContainer[4];
	 
   Rndm::Numbers m_flatDist;
	 
//// move them to monitoring
  IMuonTileXYZTool* m_pMuonTileXYZ ;
/*   IHistogram1D* m_histoTiming ;     
   IHistogram1D* m_histoTimingOne ;     
 
   NTuple::Tuple*                  m_ntuple;
  /// N-tuple items to be written away: Number of tracks
   NTuple::Item<long>              m_hit1;
   NTuple::Item<long>              m_hit2;
   NTuple::Item<long>              m_hit3;
   NTuple::Item<long>              m_hit4;
   NTuple::Item<long>              m_hit5;
	 NTuple::Array<float>            m_x1; 
	 NTuple::Array<float>            m_y1; 
	 NTuple::Array<float>            m_z1; 
	 NTuple::Array<float>            m_x2; 
	 NTuple::Array<float>            m_y2; 
	 NTuple::Array<float>            m_z2; 
	 NTuple::Array<float>            m_x3; 
	 NTuple::Array<float>            m_y3; 
	 NTuple::Array<float>            m_z3; 
	 NTuple::Array<float>            m_x4; 
	 NTuple::Array<float>            m_y4; 
	 NTuple::Array<float>            m_z4; 
	 NTuple::Array<float>            m_x5; 
	 NTuple::Array<float>            m_y5; 
	 NTuple::Array<float>            m_z5; */
 };
#endif
