// $Id: OTTime2MCParticleAlg.h,v 1.1 2004-09-03 12:08:10 jnardull Exp $
#ifndef OTASSOCIATORS_OTTIME2MCPARTICLEALG_H
#define OTASSOCIATORS_OTTIME2MCPARTICLEALG_H 1

// Event
#include "Event/OTTime.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTTime2MCHitAsct.h"

class OTTime;
class MCParticle;

/** @class OTTime2MCParticleAlg OTTime2MCParticleAlg.h
 *  
 *  Algorithm which makes the association from OTTimes to MCParticles. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  from OTTimes to MCHits.
 *
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

class OTTime2MCParticleAlg : public Algorithm {

  friend class AlgFactory<OTTime2MCParticleAlg>;
  
public:

  /// Standard constructor
  OTTime2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTTime2MCParticleAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTTime* aTime,
                                      std::vector<MCParticle*>& partVector);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData;          ///< path to put relation table
  std::string m_nameAsct;            ///< Associator from OTTimes to MCHits

  // pointer to associator
  OTTime2MCHitAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTTime2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTTIME2MCPARTICLEALG_H







