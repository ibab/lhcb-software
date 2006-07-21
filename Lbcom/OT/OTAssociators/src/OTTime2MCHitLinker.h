#ifndef OTASSOCIATORS_OTTIME2MCHITLINKER_H
#define OTASSOCIATORS_OTTIME2MCHITLINKER_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class OTTime2MCHitLinker OTTime2MCHitLinker.h
 *  
 *  Algorithm which makes the association from OTTimes to MCHits. This
 *  is used by the associator tool. There is a flag to make relations with 
 *  spillover hits as well. By default those are not associated.
 *
 *  @author Jacopo Nardulli and J. van Tilburg
 *  @date   15/06/2004
 */

namespace LHCb
{
  class MCHit;
  class OTTime;
}

class OTTime2MCHitLinker : public GaudiAlgorithm {

  friend class AlgFactory<OTTime2MCHitLinker>;
  
 public:

  /// Standard constructor
  OTTime2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Destructor
  virtual ~OTTime2MCHitLinker(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();
  
  /// path to put relation table
  std::string outputData() const;
 
 protected:
  
 private:

  // associator function
  virtual StatusCode associateToTruth( const LHCb::OTTime* aTime ,
				       std::vector<const LHCb::MCHit*>& hitVec , 
				       LHCb::MCHits* mcHits );
 
  // job options:
  bool m_spillOver;          ///< True = Add spillover
  std::string m_outputData;  ///< path to put relation table

};

inline std::string OTTime2MCHitLinker::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTTIME2MCHITLINKER_H
