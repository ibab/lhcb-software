#ifndef HCRAWBANKDECODER_H
#define HCRAWBANKDECODER_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// LHCb
#include "DAQKernel/DecoderHistoAlgBase.h"

#include "Event/HCDigit.h"

namespace LHCb {
class RawEvent;
}

/** @class HCRawBankDecoder.h
 *
 * Decoder for Herschel raw banks.
 *
 */

class HCRawBankDecoder : public Decoder::HistoAlgBase {
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
  bool m_skipErrorBanks;

  bool m_monitoring;

  IHistogram2D* m_hLinkErrors;
  IHistogram2D* m_hTell1Errors;
  std::vector<IHistogram1D*> m_hBxDiff;

  bool decode(LHCb::RawBank* bank);
  bool decodeErrorBank(LHCb::RawBank* bank, const int bxid);

};

#endif
