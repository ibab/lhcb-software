// $Id: $
#ifndef FTRAWBANKENCODER_H 
#define FTRAWBANKENCODER_H 1

// Include files
#include <cstdint>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FTRawBankEncoder FTRawBankEncoder.h
 *  Encode the FTCLusters into raw banks
 *
 *  @author Olivier Callot
 *  @date   2012-05-11
 */
class FTRawBankEncoder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FTRawBankEncoder( const std::string& name, ISvcLocator* pSvcLocator );

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution
  StatusCode finalize  () override;    ///< Algorithm finalization

private:
  std::vector<std::vector<std::vector<uint16_t> > > m_sipmData;
  std::string m_outputLocation; 

};
#endif // FTRAWBANKENCODER_H
