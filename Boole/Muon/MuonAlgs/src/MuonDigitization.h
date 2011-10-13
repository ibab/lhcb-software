#ifndef MUONALGS_MUONDIGITIZATION_H
#define MUONALGS_MUONDIGITIZATION_H 1

#include <string>

// From Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// local
#include "MuonDigitizationData.h"
#include "MuonPhyChannelInput.h"
#include "MuonPhysicalChannel.h"
#include "MuonPhysicalChannelOutput.h" 
#include "MuonCardiacChannelOutput.h" 
#include "MuonDetectorResponse.h" 

// Event model
#include "Event/MCMuonDigit.h"   
#include "Event/MuonDigit.h"   

// Forward declaration
class DeMuonDetector;

 
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
  StatusCode  fillCardiacChannel(MuonDigitizationData<MuonPhysicalChannelOutput>& 
                                  PhysicalChannel,
                                  MuonDigitizationData
                                  <MuonCardiacChannelOutput>&
                                  PhysicalChannelOutput);
  StatusCode createLogicalChannel(MuonDigitizationData
                                  <MuonPhysicalChannelOutput>& PhyChaOutput,
                                  LHCb::MCMuonDigits & mcDigitContainer);
  StatusCode createLogicalChannel(MuonDigitizationData
                                  <MuonCardiacChannelOutput>& PhyChaOutput,
                                  LHCb::MCMuonDigits & mcDigitContainer);
  StatusCode createRAWFormat(LHCb::MCMuonDigits & mcDigitContainer, 
                                 LHCb::MuonDigits & digitContainer);
  StatusCode addChamberNoise();
  StatusCode addElectronicNoise(MuonDigitizationData<MuonPhysicalChannel>& 
                                PhysicalChannel);
  
private:
  int m_numberOfEvents;
  int m_numberOfEventsNeed;
  int m_numberOfSpilloverEvents; ///< Option to set max no. of spillover events
  double m_BXTime;
  double m_gate;
  bool m_applyTimeJitter;
  bool m_applyChamberNoise;	 
  bool m_applyElectronicNoise;	 
  bool m_applyXTalk;	 
  bool m_applyEfficiency;	 
  bool m_applyDeadtime;	  
  bool m_applyDialogDeadtime;	 
  bool m_applyTimeAdjustment;	 
  bool m_registerPhysicalChannelOutput;

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
  static const int spillTimeOffset[6];  
  Rndm::Numbers m_flatDist;
int m_hitNotInGap;
  };
#endif
