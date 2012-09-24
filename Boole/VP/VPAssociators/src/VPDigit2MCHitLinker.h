// $Id: VPDigit2MCHitLinker.h,v 1.1.1.1 2009-12-04 14:34:46 marcin Exp $
#ifndef VPDIGIT2MCHITLINKER_H
#define VPDIGIT2MCHITLINKER_H 1
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from Event
#include "Event/MCHit.h"

/** @class VPDigit2MCHitLinker VPDigit2MCHitLinker.h
 *  @Based on ST code
 *  
 *  @author Marcin Kucharczyk
 *  @date   06/11/2009
 */

class VPDigit2MCHitLinker : public GaudiAlgorithm {

      friend class AlgFactory<VPDigit2MCHitLinker>;

public:
  /// Standard constructor
  VPDigit2MCHitLinker(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VPDigit2MCHitLinker(); ///< Destructor
  virtual StatusCode initialize();     ///< Algorithm initialization
  virtual StatusCode execute   ();     ///< Algorithm execution
  virtual StatusCode finalize  ();     ///< Algorithm finalise
  typedef std::pair<const LHCb::MCHit*,double> HitPair;
  typedef std::map<const LHCb::MCHit*,double> HitMap;
  // Path to put table
  std::string outputData() const;

protected:

private:

  double totalCharge(const HitMap& hitMap) const;
  void refsToRelate(std::vector<HitPair>& selectedRefs, 
                    const HitMap& hitMap,
                    const double& totCharge, 
                    LHCb::MCHits* hits) const;
  std::string m_hitLocation;
  std::string m_outputData;
  std::string m_inputData; 
  bool m_addSpillOverHits; 
  double m_minFrac;           ///< Minimal charge fract to link to MCParticle
  bool m_oneRef;              ///< Flag to allow only 1 link for each digit
  double m_conversion;
};

inline std::string VPDigit2MCHitLinker::outputData() const {
  return m_outputData;
}

#endif // VPDIGIT2MCHITLINKER_H
