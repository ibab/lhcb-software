#ifndef VPDIGIT2MCPARTICLELINKER_H
#define VPDIGIT2MCPARTICLELINKER_H 1
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from MC
#include "Event/MCParticle.h"

/** @class VPDigit2MCParticleLinker VPDigit2MCParticleLinker.h
 *  @Based on ST code
 *  
 *  @author Marcin Kucharczyk
 *  @date   06/11/2009
 */

class VPDigit2MCParticleLinker : public GaudiAlgorithm {

      friend class AlgFactory<VPDigit2MCParticleLinker>;

public:
  /// Standard constructor
  VPDigit2MCParticleLinker(const std::string& name,
                                ISvcLocator* pSvcLocator);
  virtual ~VPDigit2MCParticleLinker(); ///< Destructor
  virtual StatusCode initialize();          ///< Algorithm initialization
  virtual StatusCode execute   ();          ///< Algorithm execution
  typedef std::pair<const LHCb::MCParticle*,double> PartPair;
  typedef std::map<const LHCb::MCParticle*,double> ParticleMap;
  // Path to put table
  std::string outputData() const;

protected:

private:

  double totalCharge(const ParticleMap& partMap) const;
  void refsToRelate(std::vector<PartPair>& selectedRefs,
                    const ParticleMap& hitMap, const double totCharge,
                    LHCb::MCParticles* particles) const;
  std::string m_outputData;   ///< Location of the STDigits linker table
  std::string m_inputData;    ///< Location of the STDigits
  bool m_addSpillOverHits;    ///< Flag to add spill-over to linker table
  double m_minFrac;           ///< Minimal charge fract to link to MCParticle
  bool m_oneRef;              ///< Flag to allow only 1 link for each digit

};

inline std::string VPDigit2MCParticleLinker::outputData() const {
  return  m_outputData;
}

#endif // VPDIGIT2MCPARTICLELINKER_H
