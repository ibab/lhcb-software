#ifndef HCRAWBANKDECODER_H
#define HCRAWBANKDECODER_H 1

#include "DAQKernel/DecoderAlgBase.h"

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

  virtual StatusCode initialize();       ///< Algorithm initialization
  virtual StatusCode execute();          ///< Algorithm execution

private:

  std::string m_digitLocation;
  
};

#endif
