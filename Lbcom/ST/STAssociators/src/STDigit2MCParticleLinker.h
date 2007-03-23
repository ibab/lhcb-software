// $Id: STDigit2MCParticleLinker.h,v 1.4 2007-03-23 08:59:13 jvantilb Exp $
#ifndef STDigit2MCPARTICLELINKER_H
#define STDigit2MCPARTICLELINKER_H 1

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

  typedef std::pair<const LHCb::MCParticle*,double> PartPair;
  typedef std::map<const LHCb::MCParticle*,double> ParticleMap;

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

  double totalCharge(const ParticleMap& partMap) const;

  void refsToRelate(std::vector<PartPair>& selectedRefs,
                    const ParticleMap& hitMap, const double totCharge,
                    LHCb::MCParticles* particles) const;

  // job options
  std::string m_outputData;   ///< Location of the STDigits linker table
  std::string m_inputData;    ///< Location of the STDigits
  bool m_addSpillOverHits;    ///< Flag to add spill-over to linker table
  double m_minFrac;           ///< Minimal charge fraction to link to MCParticle
  bool m_oneRef;              ///< Flag to allow only 1 link for each digit
  std::string m_detType;      ///< Detector type (IT or TT)
};

inline std::string STDigit2MCParticleLinker::outputData() const {
  return  m_outputData;
}

#endif // STDIGIT2MCPARTICLELINKER_H
