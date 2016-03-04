#ifndef STClustersToRawBankAlg_H
#define STClustersToRawBankAlg_H 1

// from STL
#include <string>
#include <vector>
#include <map>

// gaudi
#include "Kernel/STAlgBase.h"

// event
#include "Event/STCluster.h"
#include "STClustersOnBoard.h"

#include "Kernel/STDAQDefinitions.h"
#include "Event/RawBank.h"
#include "Event/BankWriter.h"

/** @class STClustersToRawBankAlg STClustersToRawBankAlg.h
 *
 *  Algorithm to fill the Raw buffer with ST information from STClusters
 *
 *  @author M Needham
 *  @date   2004-08-01
 */

class STTell1ID;
class ISTDAQDataSvc;

#include "STBoardToBankMap.h"
namespace LHCb {class RawBank;}

class ISTReadoutTool;


class STClustersToRawBankAlg : public ST::AlgBase {

public:

  /// Standard constructor
  STClustersToRawBankAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~STClustersToRawBankAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  /// convert string to enum
  StatusCode configureBankType();

  /// initialize event
  void initEvent();

  /// fill the banks
  StatusCode groupByBoard(const LHCb::STClusters* clusCont);

  unsigned int bankSize(STClustersOnBoard::ClusterVector& clusCont) const;

  unsigned int getPCN() const;

  // create a new bank
  void writeBank(const STClustersOnBoard::ClusterVector& clusCont ,
                 LHCb::BankWriter& bWriter,
                 const STTell1ID aBoardID);

  std::string m_clusterLocation;
  std::string m_summaryLocation;
  std::string m_rawLocation;

  LHCb::RawBank::BankType m_bankType;

  STBoardToBankMap m_bankMapping;

  std::map<STTell1ID,STClustersOnBoard* > m_clusMap;
  std::vector<STClustersOnBoard> m_clusVectors;

  unsigned int m_overflow;
  int m_maxClustersPerPPx;
  unsigned int m_maxClusterSize;
  unsigned int m_pcn;

};
#endif // STClustersToRawBankAlg
