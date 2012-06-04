// $Id$
#ifndef FTRAWBANKDECODER_H 
#define FTRAWBANKDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FTRawBankDecoder FTRawBankDecoder.h
 *  Decode the FT raw bank into FTRawClusters
 *
 *  @author Olivier Callot
 *  @date   2012-05-11
 */
class FTRawBankDecoder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FTRawBankDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FTRawBankDecoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // FTRAWBANKDECODER_H
