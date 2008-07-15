// $Id: STDecodingBaseAlg.h,v 1.10 2008-07-15 11:20:26 mneedham Exp $
#ifndef STDECODINGBASEALG_H 
#define STDECODINGBASEALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/RawBank.h"
#include "Kernel/STDAQDefinitions.h"

#include "Event/STCluster.h"

#include <vector>
#include <string>

/** @class STDecodingBaseAlg STDecodingBaseAlg.h
 *  
 *  Algorithm to create STClusters from RawEvent object
 * 
 *  @author M. Needham
 *  @date   2004-01-07
 */

#include "Event/RawEvent.h"
#include "Event/ByteStream.h"

#include "Kernel/STClusterWord.h"
#include "Kernel/STDecoder.h"

#include <string>

class ISTReadoutTool;
class STTell1Board;

class STDecodingBaseAlg : public GaudiAlgorithm {

public:

  /// Standard constructor
  STDecodingBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STDecodingBaseAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
   
  virtual StatusCode finalize();

protected:

 ISTReadoutTool* readoutTool() const;

 LHCb::RawBank::BankType bankType() const;

 std::string detType() const;

 bool forceVersion() const;  

 void createSummaryBlock(const unsigned int nclus, const unsigned int pcn, 
                         const bool pcnsync, const std::vector<unsigned int>& bankList,
                         const std::vector<unsigned int>& missing ) const;

 unsigned int pcnVote(const std::vector<LHCb::RawBank* >& banks) const;

 bool checkDataIntegrity(STDecoder& decoder, const STTell1Board* aBoard, 
                         const unsigned int bankSize, const int version) const;

 /** list of boards missing in action */
 std::vector<unsigned int> missingInAction(const std::vector<LHCb::RawBank*>& banks) const; 

 bool m_skipErrors;
 std::string m_bankTypeString;
 std::string m_rawEventLocation;
 
 int m_forcedVersion;
 
private:

  /// bank type
  std::string m_detType;
 
  std::string m_summaryLocation;
  LHCb::RawBank::BankType m_bankType;
  std::string m_readoutToolName;
  ISTReadoutTool* m_readoutTool;
  
};

inline ISTReadoutTool* STDecodingBaseAlg::readoutTool() const{
  return m_readoutTool;
}

inline LHCb::RawBank::BankType STDecodingBaseAlg::bankType() const {
  return m_bankType;
}

inline std::string STDecodingBaseAlg::detType() const{
  return m_detType;
}

inline bool STDecodingBaseAlg::forceVersion() const{
  return m_forcedVersion >= 0;
}


#endif // STDECODINGBASEALG_H 
