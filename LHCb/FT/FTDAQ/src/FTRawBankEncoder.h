// $Id: $
#ifndef FTRAWBANKENCODER_H 
#define FTRAWBANKENCODER_H 1

// Include files
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

  virtual ~FTRawBankEncoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_nbBanks;
  int m_nbSipmPerTELL40;
  std::vector<std::vector<std::vector<unsigned int> > > m_sipmData;

  std::string m_outputLocation; 

};
#endif // FTRAWBANKENCODER_H
