// $Id: RawBankToSTClusterAlg.h,v 1.1.1.1 2005-12-20 12:47:27 mneedham Exp $
#ifndef RAWBANKTOSTCLUSTERALG_H 
#define RAWBANKTOSTCLUSTERALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
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

#include "Event/RawEvent.h"
#include "Event/ByteStream.h"

#include "STDAQ/STClusterWord.h"

class SiADCWord;
class STTell1Board;

class ISTReadoutTool;

class DeSTDetector;

class RawBankToSTClusterAlg : public GaudiAlgorithm {

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

  bool goodData(unsigned int version, unsigned int bSize, STDAQ::rawInt* theData) const;

  /// Output location for STDigits
  std::string m_clusterLocation;
  std::string m_readoutToolName;

  /// bank type
  std::string m_detType;

  LHCb::RawBank::BankType m_bankType;

  ISTReadoutTool* m_readoutTool;

  DeSTDetector* m_tracker;

};

#endif // RAWBUFFERTOSTDIGITALG_H 
