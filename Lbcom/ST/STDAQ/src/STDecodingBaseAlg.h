// $Id: STDecodingBaseAlg.h,v 1.1 2006-02-10 08:59:31 mneedham Exp $
#ifndef STDECODINGBASEALG_H 
#define STDECODINGBASEALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/RawBank.h"
#include "STDAQDefinitions.h"

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

#include "STClusterWord.h"

#include <string>

class ISTReadoutTool;
class DeSTDetector;

class STDecodingBaseAlg : public GaudiAlgorithm {

public:

  /// Standard constructor
  STDecodingBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STDecodingBaseAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

protected:

 ISTReadoutTool* readoutTool() const;

 LHCb::RawBank::BankType bankType() const;

 DeSTDetector* tracker() const;

 std::string detType() const;

private:

  /// bank type
  std::string m_detType;
  LHCb::RawBank::BankType m_bankType;
  std::string m_readoutToolName;
  ISTReadoutTool* m_readoutTool;
  DeSTDetector* m_tracker;
 
};

inline ISTReadoutTool* STDecodingBaseAlg::readoutTool() const{
  return m_readoutTool;
}

inline LHCb::RawBank::BankType STDecodingBaseAlg::bankType() const {
  return m_bankType;
}

inline DeSTDetector* STDecodingBaseAlg::tracker() const{
  return m_tracker;
}

inline std::string STDecodingBaseAlg::detType() const{
  return m_detType;
}


#endif // STDECODINGBASEALG_H 
