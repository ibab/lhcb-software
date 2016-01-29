#ifndef RAWBANKTOSTLITECLUSTERALG_H 
#define RAWBANKTOSTLITECLUSTERALG_H 1

#include "STDecodingBaseAlg.h"
#include "Event/RawBank.h"
#include "Kernel/STDAQDefinitions.h"

#include "Event/STLiteCluster.h"

#include <vector>
#include <string>
#include <utility>

/** @class RawBankToSTLiteClusterAlg RawBankToSTLiteClusterAlg.h
 *  
 *  Algorithm to create STClusters from RawEvent object
 * 
 *  @author M. Needham
 *  @date   2004-01-07
 */


#include "Kernel/STClusterWord.h"

class SiADCWord;
class STTell1Board;

namespace LHCb{
 class STChannelID;
 class STLiteCluster;
}

class RawBankToSTLiteClusterAlg : public STDecodingBaseAlg {

public:

  /// Standard constructor
  RawBankToSTLiteClusterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankToSTLiteClusterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize(); ///< finalize

private:

  // create Clusters from this type
  StatusCode decodeBanks(LHCb::RawEvent* rawEvt, LHCb::STLiteCluster::STLiteClusters* fCont) const;

  // add a single cluster to the output container
  void createCluster(const STTell1Board* aBoard,  const STDAQ::version& bankVersion, 
                     const STClusterWord& aWord, LHCb::STLiteCluster::STLiteClusters* fCont, const bool isUT) const;


  std::string m_clusterLocation;  
  
  class Less_by_Channel : public std::binary_function<LHCb::STLiteCluster,LHCb::STLiteCluster ,bool>{
  public:

    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    //
    inline bool operator() ( LHCb::STLiteCluster obj1 , LHCb::STLiteCluster obj2 ) const
    {
      return obj1.channelID() < obj2.channelID() ;
    }
  };

   
  class Equal_Channel : public std::binary_function<LHCb::STLiteCluster,LHCb::STLiteCluster ,bool>{
  public:

    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    //
    inline bool operator() ( LHCb::STLiteCluster obj1 , LHCb::STLiteCluster obj2 ) const
    {
      return obj1.channelID() == obj2.channelID() ;
    }
  };

  
};

#include "Kernel/STTell1Board.h"
#include "Kernel/ISTReadoutTool.h"

inline void RawBankToSTLiteClusterAlg::createCluster(const STTell1Board* aBoard,  const STDAQ::version& bankVersion,
                                                     const STClusterWord& aWord, LHCb::STLiteCluster::STLiteClusters* fCont, 
                                                     const bool isUT) const{
   
  const unsigned int fracStrip = aWord.fracStripBits();     
  const STTell1Board::chanPair chan = aBoard->DAQToOffline(fracStrip, bankVersion, STDAQ::StripRepresentation(aWord.channelID()));
  LHCb::STLiteCluster liteCluster(chan.second,
                            aWord.pseudoSizeBits(),
                            aWord.hasHighThreshold(),
                            chan.first,
                            isUT);
  fCont->push_back(std::move(liteCluster));
}

#endif //  RAWBANKTOSTLITECLUSTERALG_H 
