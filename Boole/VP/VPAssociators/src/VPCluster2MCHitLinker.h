#ifndef VPCLUSTER2MCHITLINKER_H
#define VPCLUSTER2MCHITLINKER_H 1
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/MCHit.h"
#include "Event/VPDigit.h"

/** @class VPCluster2MCHitLinker VPCluster2MCHitLinker.h
 *  @Based on ST code 
 * 
 *  @author Marcin Kucharczyk
 *  @date   01/12/2009
 */

class VPCluster2MCHitLinker : public GaudiAlgorithm {

      friend class AlgFactory<VPCluster2MCHitLinker>;
  
public:
  /// Standard constructor
  VPCluster2MCHitLinker(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VPCluster2MCHitLinker(); ///< Destructor
  virtual StatusCode initialize();       ///< Algorithm initialization
  virtual StatusCode execute   ();       ///< Algorithm execution
  typedef std::pair<const LHCb::MCHit*,double> HitPair;
  typedef std::map<const LHCb::MCHit*,double> HitMap;
  // Path to put table
  std::string outputData() const;
 
protected:

private:

  void refsToRelate(std::vector<HitPair>& selectedRefs, 
                    const HitMap& hitMap,
                    LHCb::MCHits* hits) const;
  StatusCode associateToTruth(const LHCb::VPCluster* aCluster,
                              HitMap& hitMap);
  LHCb::VPDigits* m_digitCont;
  std::string m_inputData;
  std::string m_hitLocation;
  std::string m_asctLocation;
  std::string m_outputData;
  bool m_addSpillOverHits;
  double m_minFrac;
  bool m_oneRef;
  std::string m_digitLocation;
  double m_conversion;
  double m_scaleFactor;
};

inline std::string VPCluster2MCHitLinker::outputData() const {
  return  m_outputData;
}

#endif // VPCLUSTER2MCHITLINKER_H
