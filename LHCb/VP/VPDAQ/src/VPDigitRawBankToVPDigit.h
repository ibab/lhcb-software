#ifndef VPDigitRawBankToVPDigit_H 
#define VPDigitRawBankToVPDigit_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"

// Forward declarations
class DeVP;
namespace LHCb {
  class RawEvent ;
}

class VPDigitRawBankToVPDigit : public GaudiAlgorithm {
public:
  /// Standard constructor
  VPDigitRawBankToVPDigit(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VPDigitRawBankToVPDigit();   ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution

private:

StatusCode decodeRawBanks(LHCb::RawEvent* rawEvt, LHCb::VPDigits* digiCont) const;

void createVPDigit( VPDigitWord aWord, LHCb::VPDigits* digitCont) const;

private:

  std::string m_rawEventLocation;
	std::string m_digitLocation;
  bool m_isDebug; 
};

#endif // VPDigitRawBankToVPDigit_H
