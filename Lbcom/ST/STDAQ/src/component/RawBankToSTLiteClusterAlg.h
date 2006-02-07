// $Id: RawBankToSTLiteClusterAlg.h,v 1.1 2006-02-07 08:47:36 mneedham Exp $
#ifndef RAWBANKTOSTLITECLUSTERALG_H 
#define RAWBANKTOSTLITECLUSTERALG_H 1

#include "STDecodingBaseAlg.h"
#include "Event/RawBank.h"
#include "STDAQ/STDAQDefinitions.h"

#include "Event/STCluster.h"

#include <vector>
#include <string>

/** @class RawBankToSTLiteClusterAlg RawBankToSTLiteClusterAlg.h
 *  
 *  Algorithm to create STClusters from RawEvent object
 * 
 *  @author M. Needham
 *  @date   2004-01-07
 */


#include "STDAQ/STClusterWord.h"

class SiADCWord;
class STTell1Board;

namespace LHCb{
 class STChannelID;
 class STLiteCluster;
};

class RawBankToSTLiteClusterAlg : public STDecodingBaseAlg {

public:

  /// Standard constructor
  RawBankToSTLiteClusterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankToSTLiteClusterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:

  // create Clusters from this type

  StatusCode decodeBanks(LHCb::RawEvent* rawEvt) const;

  /// Output location for STClusters
  std::string m_clusterLocation;
 
};

#endif //  RAWBANKTOSTLITECLUSTERALG_H 
