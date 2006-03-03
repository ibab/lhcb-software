// $Id: STDigit2MCParticleLinker.h,v 1.2 2006-03-03 17:01:11 mneedham Exp $
#ifndef STDigit2MCPARTICLELINKER_H
#define STDigit2MCPARTICLELINKER_H 1

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MCParticle.h"

/** @class STDigit2MCParticleLinker STDigit2MCParticleLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/04/2002
 */
class STDigit2MCParticleLinker : public GaudiAlgorithm {

  friend class AlgFactory<STDigit2MCParticleLinker>;
  
public:

  typedef std::pair<const LHCb::MCParticle*,double> partPair;

  /// Standard constructor
  STDigit2MCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~STDigit2MCParticleLinker(); 

  /// initialize
  virtual StatusCode initialize();

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;


private:

  double totalCharge(const std::map<const LHCb::MCParticle*,double>& partMap) const;
  StatusCode refsToRelate(std::vector<partPair>& selectedRefs,
                          const std::map<const LHCb::MCParticle*,double>& hitMap,
                          const double totCharge,
                          LHCb::MCParticles* particles) const;

  std::string m_hitLocation;
  std::string m_outputData;
  std::string m_inputData;
  
  std::string m_detType;
  bool m_addSpillOverHits;
  double m_minFrac;  
  bool m_oneRef;
  
};

inline std::string STDigit2MCParticleLinker::outputData() const {
  return  m_outputData;
}


#endif // STDIGIT2MCPARTICLEALG_H







