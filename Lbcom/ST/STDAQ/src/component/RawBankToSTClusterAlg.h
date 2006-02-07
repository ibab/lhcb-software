// $Id: RawBankToSTClusterAlg.h,v 1.2 2006-02-07 08:47:36 mneedham Exp $
#ifndef RAWBANKTOSTCLUSTERALG_H 
#define RAWBANKTOSTCLUSTERALG_H 1

#include "STDecodingBaseAlg.h"
#include "Event/RawBank.h"
#include "STDAQ/STDAQDefinitions.h"

#include "Event/STCluster.h"

#include <vector>
#include <string>

/** @class RawBankToSTClusterAlg RawBankToSTClusterAlg.h
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

class RawBankToSTClusterAlg : public STDecodingBaseAlg {

public:

  /// Standard constructor
  RawBankToSTClusterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankToSTClusterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:

  StatusCode decodeBanks(LHCb::RawEvent* rawEvt, LHCb::STClusters* digitCont ) const;

  StatusCode createCluster(const STClusterWord& aWord,
                           const STTell1Board* aBoard,
                           LHCb::ByteStream& stream,
		 	   LHCb::STClusters* clusCont) const;
 
  unsigned int mean(const std::vector<SiADCWord>& adcValues) const;
 
  LHCb::STLiteCluster word2LiteCluster(STClusterWord aWord, LHCb::STChannelID chan) const;

  /// Output location for STClusters
  std::string m_clusterLocation;
 
};

#endif // RAWBUFFERTOSTDIGITALG_H 
