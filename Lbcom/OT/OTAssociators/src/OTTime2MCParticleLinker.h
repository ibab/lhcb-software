#ifndef OTASSOCIATORS_OTTIME2MCPARTICLELINKER_H
#define OTASSOCIATORS_OTTIME2MCPARTICLELINKER_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class OTTime2MCParticleLinker OTTime2MCParticleLinker.h
 *  
 *  Algorithm which makes the association from OTTimes to MCParticles. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  from OTTimes to MCHits.
 *
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

namespace LHCb 
{
  class MCParticle;
  class MCOTTime;
  //class OTTime;
}

class OTTime2MCParticleLinker : public GaudiAlgorithm {
  
  friend class AlgFactory<OTTime2MCParticleLinker>;
  
 public:

  /// Standard constructor
  OTTime2MCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Destructor
  virtual ~OTTime2MCParticleLinker(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;
 
 protected:
  
 private:
  
  // associator function
  /* virtual StatusCode associateToTruth( const LHCb::OTTime* aTime, */
/* 				       std::vector<const LHCb::MCParticle*>& partVec ); */
  virtual StatusCode associateToTruth( const LHCb::MCOTTime* aTime,
				       std::vector<const LHCb::MCParticle*>& partVec );
  // job options:
  std::string m_outputData;          ///< path to put relation table
};

inline std::string OTTime2MCParticleLinker::outputData() const {
  return  m_outputData;
}

#endif // OTASSOCIATORS_OTTIME2MCPARTICLELINKER_H







