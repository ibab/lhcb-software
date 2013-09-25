#pragma once
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"
// Event
#include "Event/RawEvent.h"
#include "Event/VPDigit.h"
class DeVP;

bool sortLessBySensor(const LHCb::VPDigit* first,
                      const LHCb::VPDigit* second) {
  return first->channelID().module() < second->channelID().module();
}

class VPDigitRawBankEncoder : public GaudiAlgorithm {
public:
  /// Standard constructor
  VPDigitRawBankEncoder(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VPDigitRawBankEncoder();     ///< Destructor
  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute   ();      ///< Algorithm execution

private:

  void makeBank(std::vector<const LHCb::VPDigit*>::const_iterator begin, 
                std::vector<const LHCb::VPDigit*>::const_iterator end);

  void storeBank(int sensor,
	         std::vector<const LHCb::VPDigit*>::const_iterator begin, 
	         std::vector<const LHCb::VPDigit*>::const_iterator end,
	         LHCb::RawEvent* rawEvent);

  std::string m_digitLocation;
  std::string m_rawEventLocation;
  unsigned int m_bankVersion;
  unsigned int m_bankSizeInBytes;
  std::vector<const LHCb::VPDigit*> m_sortedDigits; 
  std::vector<SiDAQ::buffer_word> m_rawData;
  std::vector<unsigned int> m_sensorNumbers;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVP* m_vPelDet;
};
