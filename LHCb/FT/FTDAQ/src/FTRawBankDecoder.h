// $Id: $
#ifndef FTRAWBANKDECODER_H 
#define FTRAWBANKDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "DAQKernel/DecoderAlgBase.h"


/** @class FTRawBankDecoder FTRawBankDecoder.h
 *  Decode the FT raw bank into FTLiteClusters
 *
 *  @author Olivier Callot
 *  @date   2012-05-11
 */
class FTRawBankDecoder : public Decoder::AlgBase {
public: 
  /// Standard constructor
  FTRawBankDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  ~FTRawBankDecoder( ) override = default; ///< Destructor

  StatusCode execute() override; ///< Algorithm execution


private:
  
  std::string m_outputClusterLocation;

  std::unique_ptr<FastClusterContainer<LHCb::FTLiteCluster,int>>
  operator()(const std::vector<LHCb::RawBank*>& banks) const;

};
#endif // FTRAWBANKDECODER_H
