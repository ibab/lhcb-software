#ifndef HCRAWBANKDECODER_H
#define HCRAWBANKDECODER_H 1

#include "DAQKernel/DecoderAlgBase.h"

#include "Event/HCDigit.h"

namespace LHCb {
class RawEvent;
}

/** @class HCRawBankDecoder.h
 *
 * Decoder for Herschel raw banks.
 *
 */

class HCRawBankDecoder : public Decoder::AlgBase {
 public:
  /// Standard constructor
  HCRawBankDecoder(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCRawBankDecoder();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  LHCb::HCDigits* m_digits;
  LHCb::HCDigits* m_l0digits;

  std::string m_digitLocation;
  std::string m_l0digitLocation;

  bool m_skipTrigger;
  bool m_skipAdc;

  bool decodeV1(LHCb::RawBank* bank);
  bool decodeV2(LHCb::RawBank* bank);
  bool decodeV3(LHCb::RawBank* bank);
  bool decodeErrorBank(LHCb::RawBank* bank);

};

#endif
