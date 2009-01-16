// $Id: STDigit2MCParticleLinker.h,v 1.5 2009-01-16 08:39:37 mneedham Exp $
#ifndef STDigit2MCPARTICLELINKER_H
#define STDigit2MCPARTICLELINKER_H 1

#include "Kernel/STAlgBase.h"

#include "Event/MCParticle.h"

/** @class STDigit2MCParticleLinker STDigit2MCParticleLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/04/2002
 */

class STDigit2MCParticleLinker : public ST::AlgBase {

  friend class AlgFactory<STDigit2MCParticleLinker>;
  
public:

  typedef std::pair<const LHCb::MCParticle*,double> PartPair;
  typedef std::map<const LHCb::MCParticle*,double> ParticleMap;

  /// Standard constructor
  STDigit2MCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~STDigit2MCParticleLinker(); 


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

};

inline std::string STDigit2MCParticleLinker::outputData() const {
  return  m_outputData;
}

#endif // STDIGIT2MCPARTICLELINKER_H
