#ifndef OTASSOCIATORS_OTTIME2MCDEPOSITLINKER_H
#define OTASSOCIATORS_OTTIME2MCDEPOSITLINKER_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class OTTime2MCDepositLinker OTTime2MCDepositLinker.h
 *  
 *  Algorithm which makes the association from OTTimes to MCOTDeposits. 
 *  This is used by the associator tool. The relations with deposits from 
 *  spillover are made as well.
 *  If other MCOTDeposits on the same channel but killed by dead time, are
 *  recorded within a certain (small) time-window, they can be associated as 
 *  well.
 *
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

namespace LHCb 
{
  class MCOTDeposit;
  //class MCOTTime;
  class OTTime;
}

class OTTime2MCDepositLinker : public GaudiAlgorithm {

  friend class AlgFactory<OTTime2MCDepositLinker>;
  
 public:

  /// Standard constructor
  OTTime2MCDepositLinker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTTime2MCDepositLinker(); 

  /// initialize
  virtual StatusCode initialize();

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;
 
 protected:

 private:

  // associator function
  /*   virtual StatusCode associateToTruth( const LHCb::MCOTTime* aTime,  */
  /* 				       std::vector<LHCb::MCOTDeposit*>& depVec ); */
  virtual StatusCode associateToTruth( const LHCb::OTTime* aTime, 
				       std::vector<LHCb::MCOTDeposit*>& depVec );

  // job options:
  std::string m_outputData; ///< path to put relation table
  double m_acceptTime;      ///< Time window for deposits killed by dead time

};

inline std::string OTTime2MCDepositLinker::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTTIME2MCDEPOSITLINKER_H
